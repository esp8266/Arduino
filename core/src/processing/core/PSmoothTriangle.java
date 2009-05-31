/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-08 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General
  Public License along with this library; if not, write to the
  Free Software Foundation, Inc., 59 Temple Place, Suite 330,
  Boston, MA  02111-1307  USA
*/

package processing.core;


/**
 * Smoothed triangle renderer for P3D.
 *
 * Based off of the PPolygon class in old versions of Processing.
 * Name and location of this class will change in a future release.
 */
public class PSmoothTriangle implements PConstants {

  // really this is "debug" but..
  private static final boolean EWJORDAN = false;
  private static final boolean FRY = false;

  // identical to the constants from PGraphics

  static final int X = 0; // transformed xyzw
  static final int Y = 1; // formerly SX SY SZ
  static final int Z = 2;

  static final int R = 3;  // actual rgb, after lighting
  static final int G = 4;  // fill stored here, transform in place
  static final int B = 5;
  static final int A = 6;

  static final int U = 7; // texture
  static final int V = 8;

  static final int DEFAULT_SIZE = 64; // this is needed for spheres
  float vertices[][] = new float[DEFAULT_SIZE][PGraphics.VERTEX_FIELD_COUNT];
  int vertexCount;


  // after some fiddling, this seems to produce the best results
  static final int ZBUFFER_MIN_COVERAGE = 204;

  float r[]   = new float[DEFAULT_SIZE]; // storage used by incrementalize
  float dr[]  = new float[DEFAULT_SIZE];
  float l[]   = new float[DEFAULT_SIZE]; // more storage for incrementalize
  float dl[]  = new float[DEFAULT_SIZE];
  float sp[]  = new float[DEFAULT_SIZE]; // temporary storage for scanline
  float sdp[] = new float[DEFAULT_SIZE];

  // color and xyz are always interpolated
  boolean interpX;
  boolean interpZ;
  boolean interpUV; // is this necessary? could just check timage != null
  boolean interpARGB;

  int rgba;
  int r2, g2, b2, a2, a2orig;

  boolean noDepthTest;

  PGraphics3D parent;
  int pixels[];
  float[] zbuffer;

  // the parent's width/height,
  // or if smooth is enabled, parent's w/h scaled
  // up by the smooth dimension
  int width, height;
  int width1, height1;

  PImage timage;
  int tpixels[];
  int theight, twidth;
  int theight1, twidth1;
  int tformat;

  // temp fix to behave like SMOOTH_IMAGES
  // TODO ewjordan: can probably remove this variable
  boolean texture_smooth;

  // for anti-aliasing
  static final int SUBXRES  = 8;
  static final int SUBXRES1 = 7;
  static final int SUBYRES  = 8;
  static final int SUBYRES1 = 7;
  static final int MAX_COVERAGE = SUBXRES * SUBYRES;

  boolean smooth;
  int firstModY;
  int lastModY;
  int lastY;
  int aaleft[] = new int[SUBYRES];
  int aaright[] = new int[SUBYRES];
  int aaleftmin, aarightmin;
  int aaleftmax, aarightmax;
  int aaleftfull, aarightfull;

  /* Variables needed for accurate texturing. */
  //private PMatrix textureMatrix = new PMatrix3D();
  private float[] camX = new float[3];
  private float[] camY = new float[3];
  private float[] camZ = new float[3];
  private float ax,ay,az;
  private float bx,by,bz;
  private float cx,cy,cz;
  private float nearPlaneWidth, nearPlaneHeight, nearPlaneDepth;
  //private float newax, newbx, newcx;
  private float xmult, ymult;


  final private int MODYRES(int y) {
    return (y & SUBYRES1);
  }


  public PSmoothTriangle(PGraphics3D iparent) {
    parent = iparent;
    reset(0);
  }


  public void reset(int count) {
    vertexCount = count;
    interpX = true;
    interpZ = true;
    interpUV = false;
    interpARGB = true;
    timage = null;
  }


  public float[] nextVertex() {
    if (vertexCount == vertices.length) {
      //parent.message(CHATTER, "re-allocating for " +
      //             (vertexCount*2) + " vertices");
      float temp[][] = new float[vertexCount<<1][PGraphics.VERTEX_FIELD_COUNT];
      System.arraycopy(vertices, 0, temp, 0, vertexCount);
      vertices = temp;

      r   = new float[vertices.length];
      dr  = new float[vertices.length];
      l   = new float[vertices.length];
      dl  = new float[vertices.length];
      sp  = new float[vertices.length];
      sdp = new float[vertices.length];
    }
    return vertices[vertexCount++];  // returns v[0], sets vc to 1
  }


  public void texture(PImage image) {
    this.timage = image;
    this.tpixels = image.pixels;
    this.twidth = image.width;
    this.theight = image.height;
    this.tformat = image.format;

    twidth1 = twidth - 1;
    theight1 = theight - 1;
    interpUV = true;
  }

  public void render() {
    if (vertexCount < 3) return;

    smooth = true;//TODO
    // these may have changed due to a resize()
    // so they should be refreshed here
    pixels = parent.pixels;
    zbuffer = parent.zbuffer;

    noDepthTest = false;//parent.hints[DISABLE_DEPTH_TEST];

    // In 0148+, should always be true if this code is called at all
    //smooth = parent.smooth;

    // by default, text turns on smooth for the textures
    // themselves. but this should be shut off if the hint
    // for DISABLE_TEXT_SMOOTH is set.
    texture_smooth = true;

    width = smooth ? parent.width*SUBXRES : parent.width;
    height = smooth ? parent.height*SUBYRES : parent.height;

    width1 = width - 1;
    height1 = height - 1;

    if (!interpARGB) {
      r2 = (int) (vertices[0][R] * 255);
      g2 = (int) (vertices[0][G] * 255);
      b2 = (int) (vertices[0][B] * 255);
      a2 = (int) (vertices[0][A] * 255);
      a2orig = a2; // save an extra copy
      rgba = 0xff000000 | (r2 << 16) | (g2 << 8) | b2;
    }

    for (int i = 0; i < vertexCount; i++) {
      r[i] = 0; dr[i] = 0; l[i] = 0; dl[i] = 0;
    }

    if (smooth) {
      for (int i = 0; i < vertexCount; i++) {
        vertices[i][X] *= SUBXRES;
        vertices[i][Y] *= SUBYRES;
      }
      firstModY = -1;
    }

    // find top vertex (y is zero at top, higher downwards)
    int topi = 0;
    float ymin = vertices[0][Y];
    float ymax = vertices[0][Y]; // fry 031001
    for (int i = 1; i < vertexCount; i++) {
      if (vertices[i][Y] < ymin) {
        ymin = vertices[i][Y];
        topi = i;
      }
      if (vertices[i][Y] > ymax) ymax = vertices[i][Y];
    }

    // the last row is an exceptional case, because there won't
    // necessarily be 8 rows of subpixel lines that will force
    // the final line to render. so instead, the algo keeps track
    // of the lastY (in subpixel resolution) that will be rendered
    // and that will force a scanline to happen the same as
    // every eighth in the other situations
    //lastY = -1;  // fry 031001
    lastY = (int) (ymax - 0.5f);  // global to class bc used by other fxns

    int lefti = topi;             // li, index of left vertex
    int righti = topi;            // ri, index of right vertex
    int y = (int) (ymin + 0.5f);  // current scan line
    int lefty = y - 1;            // lower end of left edge
    int righty = y - 1;           // lower end of right edge

    interpX = true;

    int remaining = vertexCount;

    // scan in y, activating new edges on left & right
    // as scan line passes over new vertices
    while (remaining > 0) {
      // advance left edge?
      while ((lefty <= y) && (remaining > 0)) {
        remaining--;
        // step ccw down left side
        int i = (lefti != 0) ? (lefti-1) : (vertexCount-1);
        incrementalize_y(vertices[lefti], vertices[i], l, dl, y);
        lefty = (int) (vertices[i][Y] + 0.5f);
        lefti = i;
      }

      // advance right edge?
      while ((righty <= y) && (remaining > 0)) {
        remaining--;
        // step cw down right edge
        int i = (righti != vertexCount-1) ? (righti + 1) : 0;
        incrementalize_y(vertices[righti], vertices[i], r, dr, y);
        righty = (int) (vertices[i][Y] + 0.5f);
        righti = i;
      }

      // do scanlines till end of l or r edge
      while (y < lefty && y < righty) {
        // this doesn't work because it's not always set here
        //if (remaining == 0) {
        //lastY = (lefty < righty) ? lefty-1 : righty-1;
        //System.out.println("lastY is " + lastY);
        //}

        if ((y >= 0) && (y < height)) {
          //try {  // hopefully this bug is fixed
          if (l[X] <= r[X]) scanline(y, l, r);
          else scanline(y, r, l);
          //} catch (ArrayIndexOutOfBoundsException e) {
          //e.printStackTrace();
          //}
        }
        y++;
        // this increment probably needs to be different
        // UV and RGB shouldn't be incremented until line is emitted
        increment(l, dl);
        increment(r, dr);
      }
    }
    //if (smooth) {
    //System.out.println("y/lasty/lastmody = " + y + " " + lastY + " " + lastModY);
    //}
  }


  public void unexpand() {
    if (smooth) {
      for (int i = 0; i < vertexCount; i++) {
        vertices[i][X] /= SUBXRES;
        vertices[i][Y] /= SUBYRES;
      }
    }
  }


  private void scanline(int y, float l[], float r[]) {
    //System.out.println("scanline " + y);
    for (int i = 0; i < vertexCount; i++) {  // should be moved later
      sp[i] = 0; sdp[i] = 0;
    }

    // this rounding doesn't seem to be relevant with smooth
    int lx = (int) (l[X] + 0.49999f);  // ceil(l[X]-.5);
    if (lx < 0) lx = 0;
    int rx = (int) (r[X] - 0.5f);
    if (rx > width1) rx = width1;

    if (lx > rx) return;

    if (smooth) {
      int mody = MODYRES(y);

      aaleft[mody] = lx;
      aaright[mody] = rx;

      if (firstModY == -1) {
        firstModY = mody;
        aaleftmin = lx; aaleftmax = lx;
        aarightmin = rx; aarightmax = rx;

      } else {
        if (aaleftmin > aaleft[mody]) aaleftmin = aaleft[mody];
        if (aaleftmax < aaleft[mody]) aaleftmax = aaleft[mody];
        if (aarightmin > aaright[mody]) aarightmin = aaright[mody];
        if (aarightmax < aaright[mody]) aarightmax = aaright[mody];
      }

      lastModY = mody;  // moved up here (before the return) 031001
      // not the eighth (or lastY) line, so not scanning this time
      if ((mody != SUBYRES1) && (y != lastY)) return;
      //lastModY = mody;  // eeK! this was missing
      //return;

      //if (y == lastY) {
      //System.out.println("y is lasty");
      //}
      //lastModY = mody;
      aaleftfull = aaleftmax/SUBXRES + 1;
      aarightfull = aarightmin/SUBXRES - 1;
    }

    // this is the setup, based on lx
    incrementalize_x(l, r, sp, sdp, lx);
    //System.out.println(l[V] + " " + r[V] + " " +sp[V] + " " +sdp[V]);

    // scan in x, generating pixels
    // using parent.width to get actual pixel index
    // rather than scaled by smooth factor
    int offset = smooth ? parent.width * (y / SUBYRES) : parent.width*y;

    int truelx = 0, truerx = 0;
    if (smooth) {
      truelx = lx / SUBXRES;
      truerx = (rx + SUBXRES1) / SUBXRES;

      lx = aaleftmin / SUBXRES;
      rx = (aarightmax + SUBXRES1) / SUBXRES;
      if (lx < 0) lx = 0;
      if (rx > parent.width1) rx = parent.width1;
    }

//    System.out.println("P3D interp uv " + interpUV + " " +
//                       vertices[2][U] + " " + vertices[2][V]);

    interpX = false;
    int tr, tg, tb, ta;
    //System.out.println("lx: "+lx + "\nrx: "+rx);
    for (int x = lx; x <= rx; x++) {

      // added == because things on same plane weren't replacing each other
      // makes for strangeness in 3D [ewj: yup!], but totally necessary for 2D
      //if (noDepthTest || (sp[Z] < zbuffer[offset+x])) {
      if (noDepthTest || (sp[Z] <= zbuffer[offset+x])) {
        //if (true) {

        // map texture based on U, V coords in sp[U] and sp[V]
        if (interpUV) {
          int tu = (int)sp[U];
          int tv = (int)sp[V];

          if (tu > twidth1) tu = twidth1;
          if (tv > theight1) tv = theight1;
          if (tu < 0) tu = 0;
          if (tv < 0) tv = 0;

          int txy = tv*twidth + tu;
          //System.out.println("tu: "+tu+" ; tv: "+tv+" ; txy: "+txy);
          float[] uv = new float[2];
          txy = getTextureIndex(x, y*1.0f/SUBYRES, uv);
          //          txy = getTextureIndex(x* 1.0f/SUBXRES, y*1.0f/SUBYRES, uv);

          tu = (int)uv[0]; tv = (int)uv[1];
          //          if (tu > twidth1) tu = twidth1;
          //          if (tv > theight1) tv = theight1;
          //          if (tu < 0) tu = 0;
          //          if (tv < 0) tv = 0;
          txy = twidth*tv + tu;
          //          if (EWJORDAN) System.out.println("x/y/txy:"+x + " " + y + " " +txy);
          //PApplet.println(sp);

          //smooth = true;
          if (smooth || texture_smooth) {
            //if (FRY) System.out.println("sp u v = " + sp[U] + " " + sp[V]);
            //System.out.println("sp u v = " + sp[U] + " " + sp[V]);
            // tuf1/tvf1 is the amount of coverage for the adjacent
            // pixel, which is the decimal percentage.
            //            int tuf1 = (int) (255f * (sp[U] - (float)tu));
            //            int tvf1 = (int) (255f * (sp[V] - (float)tv));

            int tuf1 = (int) (255f * (uv[0] - tu));
            int tvf1 = (int) (255f * (uv[1] - tv));

            // the closer sp[U or V] is to the decimal being zero
            // the more coverage it should get of the original pixel
            int tuf = 255 - tuf1;
            int tvf = 255 - tvf1;

            // this code sucks! filled with bugs and slow as hell!
            int pixel00 = tpixels[txy];
            int pixel01 = (tv < theight1) ? tpixels[txy + twidth] : tpixels[txy];
            int pixel10 = (tu < twidth1)  ? tpixels[txy + 1]      : tpixels[txy];
            int pixel11 = ((tv < theight1) && (tu < twidth1)) ? tpixels[txy + twidth + 1] : tpixels[txy];
            //System.out.println("1: "+pixel00);
            //check
            int p00, p01, p10, p11;
            int px0, px1; //, pxy;

            if (tformat == ALPHA) {
              px0 = (pixel00*tuf + pixel10*tuf1) >> 8;
              px1 = (pixel01*tuf + pixel11*tuf1) >> 8;
              ta = (((px0*tvf + px1*tvf1) >> 8) *
                    (interpARGB ? ((int) (sp[A]*255)) : a2orig)) >> 8;
            } else if (tformat == ARGB) {
              p00 = (pixel00 >> 24) & 0xff;
              p01 = (pixel01 >> 24) & 0xff;
              p10 = (pixel10 >> 24) & 0xff;
              p11 = (pixel11 >> 24) & 0xff;

              px0 = (p00*tuf + p10*tuf1) >> 8;
              px1 = (p01*tuf + p11*tuf1) >> 8;
              ta = (((px0*tvf + px1*tvf1) >> 8) *
                    (interpARGB ? ((int) (sp[A]*255)) : a2orig)) >> 8;
            } else {  // RGB image, no alpha
              //ACCTEX: Getting here when smooth is on
              ta = interpARGB ? ((int) (sp[A]*255)) : a2orig;
              //System.out.println("4: "+ta + " " +interpARGB + " " + sp[A] + " " + a2orig);
              //check
            }

            if ((tformat == RGB) || (tformat == ARGB)) {
              p00 = (pixel00 >> 16) & 0xff;  // red
              p01 = (pixel01 >> 16) & 0xff;
              p10 = (pixel10 >> 16) & 0xff;
              p11 = (pixel11 >> 16) & 0xff;

              px0 = (p00*tuf + p10*tuf1) >> 8;
              px1 = (p01*tuf + p11*tuf1) >> 8;
              tr = (((px0*tvf + px1*tvf1) >> 8) * (interpARGB ? ((int) (sp[R]*255)) : r2)) >> 8;

              p00 = (pixel00 >> 8) & 0xff;  // green
              p01 = (pixel01 >> 8) & 0xff;
              p10 = (pixel10 >> 8) & 0xff;
              p11 = (pixel11 >> 8) & 0xff;

              px0 = (p00*tuf + p10*tuf1) >> 8;
              px1 = (p01*tuf + p11*tuf1) >> 8;
              tg = (((px0*tvf + px1*tvf1) >> 8) * (interpARGB ? ((int) (sp[G]*255)) : g2)) >> 8;


              p00 = pixel00 & 0xff;  // blue
              p01 = pixel01 & 0xff;
              p10 = pixel10 & 0xff;
              p11 = pixel11 & 0xff;

              px0 = (p00*tuf + p10*tuf1) >> 8;
              px1 = (p01*tuf + p11*tuf1) >> 8;
              tb = (((px0*tvf + px1*tvf1) >> 8) * (interpARGB ? ((int) (sp[B]*255)) : b2)) >> 8;
              //System.out.println("5: "+tr + " " + tg + " " +tb);
              //check
            } else {  // alpha image, only use current fill color
              if (interpARGB) {
                tr = (int) (sp[R] * 255);
                tg = (int) (sp[G] * 255);
                tb = (int) (sp[B] * 255);
              } else {
                tr = r2;
                tg = g2;
                tb = b2;
              }
            }

            // get coverage for pixel if smooth
            // checks smooth again here because of
            // hints[SMOOTH_IMAGES] used up above
            int weight = smooth ? coverage(x) : 255;
            if (weight != 255) ta = (ta*weight) >> 8;
            //System.out.println(ta);
            //System.out.println("8");
            //check
          } else {  // no smooth, just get the pixels
            int tpixel = tpixels[txy];
            // TODO i doubt splitting these guys really gets us
            // all that much speed.. is it worth it?
            if (tformat == ALPHA) {
              ta = tpixel;
              if (interpARGB) {
                tr = (int) (sp[R]*255);
                tg = (int) (sp[G]*255);
                tb = (int) (sp[B]*255);
                if (sp[A] != 1) {
                  ta = (((int) (sp[A]*255)) * ta) >> 8;
                }
              } else {
                tr = r2;
                tg = g2;
                tb = b2;
                ta = (a2orig * ta) >> 8;
              }

            } else {  // RGB or ARGB
              ta = (tformat == RGB) ? 255 : (tpixel >> 24) & 0xff;
              if (interpARGB) {
                tr = (((int) (sp[R]*255)) * ((tpixel >> 16) & 0xff)) >> 8;
                tg = (((int) (sp[G]*255)) * ((tpixel >> 8) & 0xff)) >> 8;
                tb = (((int) (sp[B]*255)) * ((tpixel) & 0xff)) >> 8;
                ta = (((int) (sp[A]*255)) * ta) >> 8;
              } else {
                tr = (r2 * ((tpixel >> 16) & 0xff)) >> 8;
                tg = (g2 * ((tpixel >> 8) & 0xff)) >> 8;
                tb = (b2 * ((tpixel) & 0xff)) >> 8;
                ta = (a2orig * ta) >> 8;
              }
            }
          }

          if ((ta == 254) || (ta == 255)) {  // if (ta & 0xf8) would be good
            // no need to blend
            pixels[offset+x] = 0xff000000 | (tr << 16) | (tg << 8) | tb;
            zbuffer[offset+x] = sp[Z];
          } else {
            // blend with pixel on screen
            int a1 = 255-ta;
            int r1 = (pixels[offset+x] >> 16) & 0xff;
            int g1 = (pixels[offset+x] >> 8) & 0xff;
            int b1 = (pixels[offset+x]) & 0xff;


            pixels[offset+x] =
              0xff000000 |
              (((tr*ta + r1*a1) >> 8) << 16) |
              ((tg*ta + g1*a1) & 0xff00) |
              ((tb*ta + b1*a1) >> 8);

            //System.out.println("17");
            //check
            if (ta > ZBUFFER_MIN_COVERAGE) zbuffer[offset+x] = sp[Z];
          }

          //System.out.println("18");
          //check
        } else {  // no image applied
          int weight = smooth ? coverage(x) : 255;

          if (interpARGB) {
            r2 = (int) (sp[R] * 255);
            g2 = (int) (sp[G] * 255);
            b2 = (int) (sp[B] * 255);
            if (sp[A] != 1) weight = (weight * ((int) (sp[A] * 255))) >> 8;
            if (weight == 255) {
              rgba = 0xff000000 | (r2 << 16) | (g2 << 8) | b2;
            }
          } else {
            if (a2orig != 255) weight = (weight * a2orig) >> 8;
          }

          if (weight == 255) {
            // no blend, no aa, just the rgba
            pixels[offset+x] = rgba;
            zbuffer[offset+x] = sp[Z];

          } else {
            int r1 = (pixels[offset+x] >> 16) & 0xff;
            int g1 = (pixels[offset+x] >> 8) & 0xff;
            int b1 = (pixels[offset+x]) & 0xff;
            a2 = weight;

            int a1 = 255 - a2;
            pixels[offset+x] = (0xff000000 |
                                ((r1*a1 + r2*a2) >> 8) << 16 |
                                // use & instead of >> and << below
                                ((g1*a1 + g2*a2) >> 8) << 8 |
                                ((b1*a1 + b2*a2) >> 8));

            if (a2 > ZBUFFER_MIN_COVERAGE) zbuffer[offset+x] = sp[Z];
          }
        }
      }
      // if smooth enabled, don't increment values
      // for the pixel in the stretch out version
      // of the scanline used to get smooth edges.
      if (!smooth || ((x >= truelx) && (x <= truerx))) {
        //if (!smooth)
        increment(sp, sdp);
      }
    }
    firstModY = -1;
    interpX = true;
  }


  // x is in screen, not huge 8x coordinates
  private int coverage(int x) {
    if ((x >= aaleftfull) && (x <= aarightfull) &&
        // important since not all SUBYRES lines may have been covered
        (firstModY == 0) && (lastModY == SUBYRES1)) {
      return 255;
    }

    int pixelLeft = x*SUBXRES;  // huh?
    int pixelRight = pixelLeft + 8;

    int amt = 0;
    for (int i = firstModY; i <= lastModY; i++) {
      if ((aaleft[i] > pixelRight) || (aaright[i] < pixelLeft)) {
        continue;
      }
      // does this need a +1 ?
      amt += ((aaright[i] < pixelRight ? aaright[i] : pixelRight) -
              (aaleft[i] > pixelLeft ? aaleft[i] : pixelLeft));
    }
    amt <<= 2;
    return (amt == 256) ? 255 : amt;
  }


  private void incrementalize_y(float p1[], float p2[],
                                float p[], float dp[], int y) {
    float delta = p2[Y] - p1[Y];
    if (delta == 0) delta = 1;
    float fraction = y + 0.5f - p1[Y];

    if (interpX) {
      dp[X] = (p2[X] - p1[X]) / delta;
      p[X] = p1[X] + dp[X] * fraction;
    }
    if (interpZ) {
      dp[Z] = (p2[Z] - p1[Z]) / delta;
      p[Z] = p1[Z] + dp[Z] * fraction;
    }

    if (interpARGB) {
      dp[R] = (p2[R] - p1[R]) / delta;
      dp[G] = (p2[G] - p1[G]) / delta;
      dp[B] = (p2[B] - p1[B]) / delta;
      dp[A] = (p2[A] - p1[A]) / delta;
      p[R] = p1[R] + dp[R] * fraction;
      p[G] = p1[G] + dp[G] * fraction;
      p[B] = p1[B] + dp[B] * fraction;
      p[A] = p1[A] + dp[A] * fraction;
    }

    if (interpUV) {
      dp[U] = (p2[U] - p1[U]) / delta;
      dp[V] = (p2[V] - p1[V]) / delta;

      //if (smooth) {
      //p[U] = p1[U]; //+ dp[U] * fraction;
      //p[V] = p1[V]; //+ dp[V] * fraction;

      //} else {
      p[U] = p1[U] + dp[U] * fraction;
      p[V] = p1[V] + dp[V] * fraction;
      //}
      if (FRY) System.out.println("inc y p[U] p[V] = " + p[U] + " " + p[V]);
    }
  }

  //incrementalize_x(l, r, sp, sdp, lx);
  private void incrementalize_x(float p1[], float p2[],
                                float p[], float dp[], int x) {
    float delta = p2[X] - p1[X];
    if (delta == 0) delta = 1;
    float fraction = x + 0.5f - p1[X];
    if (smooth) {
      delta /= SUBXRES;
      fraction /= SUBXRES;
    }

    if (interpX) {
      dp[X] = (p2[X] - p1[X]) / delta;
      p[X] = p1[X] + dp[X] * fraction;
    }
    if (interpZ) {
      dp[Z] = (p2[Z] - p1[Z]) / delta;
      p[Z] = p1[Z] + dp[Z] * fraction;
      //System.out.println(p2[Z]+" " +p1[Z]+" " +dp[Z]);
    }

    if (interpARGB) {
      dp[R] = (p2[R] - p1[R]) / delta;
      dp[G] = (p2[G] - p1[G]) / delta;
      dp[B] = (p2[B] - p1[B]) / delta;
      dp[A] = (p2[A] - p1[A]) / delta;
      p[R] = p1[R] + dp[R] * fraction;
      p[G] = p1[G] + dp[G] * fraction;
      p[B] = p1[B] + dp[B] * fraction;
      p[A] = p1[A] + dp[A] * fraction;
    }

    if (interpUV) {
      if (FRY) System.out.println("delta, frac = " + delta + ", " + fraction);
      dp[U] = (p2[U] - p1[U]) / delta;
      dp[V] = (p2[V] - p1[V]) / delta;

      //if (smooth) {
      //p[U] = p1[U];
      // offset for the damage that will be done by the
      // 8 consecutive calls to scanline
      // agh.. this won't work b/c not always 8 calls before render
      // maybe lastModY - firstModY + 1 instead?
      if (FRY) System.out.println("before inc x p[V] = " + p[V] + " " + p1[V] + " " + p2[V]);
      //p[V] = p1[V] - SUBXRES1 * fraction;

      //} else {
      p[U] = p1[U] + dp[U] * fraction;
      p[V] = p1[V] + dp[V] * fraction;
      //}
    }
  }

  private void increment(float p[], float dp[]) {
    if (interpX) p[X] += dp[X];
    if (interpZ) p[Z] += dp[Z];

    if (interpARGB) {
      p[R] += dp[R];
      p[G] += dp[G];
      p[B] += dp[B];
      p[A] += dp[A];
    }

    if (interpUV) {
      if (FRY) System.out.println("increment() " + p[V] + " " + dp[V]);
      p[U] += dp[U];
      p[V] += dp[V];
    }
  }


  /**
   * Pass camera-space coordinates for the triangle.
   * Needed to render if hint(ENABLE_ACCURATE_TEXTURES) enabled.
   * Generally this will not need to be called manually,
   * currently called from PGraphics3D.render_triangles()
   */
  public void setCamVertices(float x0, float y0, float z0,
                             float x1, float y1, float z1,
                             float x2, float y2, float z2) {
    camX[0] = x0;
    camX[1] = x1;
    camX[2] = x2;

    camY[0] = y0;
    camY[1] = y1;
    camY[2] = y2;

    camZ[0] = z0;
    camZ[1] = z1;
    camZ[2] = z2;
  }

  public void setVertices(float x0, float y0, float z0,
                          float x1, float y1, float z1,
                          float x2, float y2, float z2) {
    vertices[0][X] = x0;
    vertices[1][X] = x1;
    vertices[2][X] = x2;

    vertices[0][Y] = y0;
    vertices[1][Y] = y1;
    vertices[2][Y] = y2;

    vertices[0][Z] = z0;
    vertices[1][Z] = z1;
    vertices[2][Z] = z2;
  }



  /**
   * Precompute a bunch of variables needed to perform
   * texture mapping.
   * @return True unless texture mapping is degenerate
   */
  boolean precomputeAccurateTexturing() {
    int o0 = 0;
    int o1 = 1;
    int o2 = 2;

    PMatrix3D myMatrix = new PMatrix3D(vertices[o0][U], vertices[o0][V], 1, 0,
                                       vertices[o1][U], vertices[o1][V], 1, 0,
                                       vertices[o2][U], vertices[o2][V], 1, 0,
                                       0,               0,               0, 1);

    // A 3x3 inversion would be more efficient here,
    // given that the fourth r/c are unity
    boolean invertSuccess = myMatrix.invert();// = myMatrix.invert();

    // If the matrix inversion had trouble, let the caller know.
    // Note that this does not catch everything that could go wrong
    // here, like if the renderer is in ortho() mode (which really
    // must be caught in PGraphics3D instead of here).
    if (!invertSuccess) return false;

    float m00, m01, m02, m10, m11, m12, m20, m21, m22;
    m00 = myMatrix.m00*camX[o0]+myMatrix.m01*camX[o1]+myMatrix.m02*camX[o2];
    m01 = myMatrix.m10*camX[o0]+myMatrix.m11*camX[o1]+myMatrix.m12*camX[o2];
    m02 = myMatrix.m20*camX[o0]+myMatrix.m21*camX[o1]+myMatrix.m22*camX[o2];
    m10 = myMatrix.m00*camY[o0]+myMatrix.m01*camY[o1]+myMatrix.m02*camY[o2];
    m11 = myMatrix.m10*camY[o0]+myMatrix.m11*camY[o1]+myMatrix.m12*camY[o2];
    m12 = myMatrix.m20*camY[o0]+myMatrix.m21*camY[o1]+myMatrix.m22*camY[o2];
    m20 = -(myMatrix.m00*camZ[o0]+myMatrix.m01*camZ[o1]+myMatrix.m02*camZ[o2]);
    m21 = -(myMatrix.m10*camZ[o0]+myMatrix.m11*camZ[o1]+myMatrix.m12*camZ[o2]);
    m22 = -(myMatrix.m20*camZ[o0]+myMatrix.m21*camZ[o1]+myMatrix.m22*camZ[o2]);

    float px = m02;
    float py = m12;
    float pz = m22;

    float TEX_WIDTH = this.twidth;
    float TEX_HEIGHT = this.theight;

    float resultT0x = m00*TEX_WIDTH+m02;
    float resultT0y = m10*TEX_WIDTH+m12;
    float resultT0z = m20*TEX_WIDTH+m22;
    float result0Tx = m01*TEX_HEIGHT+m02;
    float result0Ty = m11*TEX_HEIGHT+m12;
    float result0Tz = m21*TEX_HEIGHT+m22;
    float mx = resultT0x-m02;
    float my = resultT0y-m12;
    float mz = resultT0z-m22;
    float nx = result0Tx-m02;
    float ny = result0Ty-m12;
    float nz = result0Tz-m22;

    //avec = p x n
    ax = (py*nz-pz*ny)*TEX_WIDTH; //F_TEX_WIDTH/HEIGHT?
    ay = (pz*nx-px*nz)*TEX_WIDTH;
    az = (px*ny-py*nx)*TEX_WIDTH;
    //bvec = m x p
    bx = (my*pz-mz*py)*TEX_HEIGHT;
    by = (mz*px-mx*pz)*TEX_HEIGHT;
    bz = (mx*py-my*px)*TEX_HEIGHT;
    //cvec = n x m
    cx = ny*mz-nz*my;
    cy = nz*mx-nx*mz;
    cz = nx*my-ny*mx;

    //System.out.println("a/b/c: "+ax+" " + ay + " " + az + " " + bx + " " + by + " " + bz + " " + cx + " " + cy + " " + cz);

    nearPlaneWidth = (parent.rightScreen-parent.leftScreen);
    nearPlaneHeight = (parent.topScreen-parent.bottomScreen);
    nearPlaneDepth = parent.nearPlane;

    // one pixel width in nearPlane coordinates
    xmult = nearPlaneWidth / parent.width;
    ymult = nearPlaneHeight / parent.height;
    // Extra scalings to map screen plane units to pixel units
//    newax = ax*xmult;
//    newbx = bx*xmult;
//    newcx = cx*xmult;


    //          System.out.println("nearplane: "+ nearPlaneWidth + " " + nearPlaneHeight + " " + nearPlaneDepth);
    //          System.out.println("mults: "+ xmult + " " + ymult);
    //          System.out.println("news: "+ newax + " " + newbx + " " + newcx);
    return true;
  }

  /**
   * Get the texture map location based on the current screen
   * coordinates.  Assumes precomputeAccurateTexturing() has
   * been called already for this texture mapping.
   * @param sx
   * @param sy
   * @return
   */
  private int getTextureIndex(float sx, float sy, float[] uv) {
    if (EWJORDAN) System.out.println("Getting texel at "+sx + ", "+sy);
    //System.out.println("Screen: "+ sx + " " + sy);
    sx = xmult*(sx-(parent.width/2.0f) +.5f);//+.5f)
    sy = ymult*(sy-(parent.height/2.0f)+.5f);//+.5f)
    //sx /= SUBXRES;
    //sy /= SUBYRES;
    float sz = nearPlaneDepth;
    float a = sx * ax + sy * ay + sz * az;
    float b = sx * bx + sy * by + sz * bz;
    float c = sx * cx + sy * cy + sz * cz;
    int u = (int)(a / c);
    int v = (int)(b / c);
    uv[0] = a / c;
    uv[1] = b / c;
    if (uv[0] < 0) {
      uv[0] = u = 0;
    }
    if (uv[1] < 0) {
      uv[1] = v = 0;
    }
    if (uv[0] >= twidth) {
      uv[0] = twidth-1;
      u = twidth-1;
    }
    if (uv[1] >= theight) {
      uv[1] = theight-1;
      v = theight-1;
    }
    int result = v*twidth + u;
    //System.out.println("a/b/c: "+a + " " + b + " " + c);
    //System.out.println("cx/y/z: "+cx + " " + cy + " " + cz);
    //if (result < 0) result = 0;
    //if (result >= timage.pixels.length-2) result = timage.pixels.length - 2;
    if (EWJORDAN) System.out.println("Got texel "+result);
    return result;
  }


  public void setIntensities(float ar, float ag, float ab, float aa,
                             float br, float bg, float bb, float ba,
                             float cr, float cg, float cb, float ca) {
    vertices[0][R] = ar;
    vertices[0][G] = ag;
    vertices[0][B] = ab;
    vertices[0][A] = aa;
    vertices[1][R] = br;
    vertices[1][G] = bg;
    vertices[1][B] = bb;
    vertices[1][A] = ba;
    vertices[2][R] = cr;
    vertices[2][G] = cg;
    vertices[2][B] = cb;
    vertices[2][A] = ca;
  }
}
