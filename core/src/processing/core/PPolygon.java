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
 * Z-buffer polygon rendering object used by PGraphics2D.
 */
public class PPolygon implements PConstants {

  static final int DEFAULT_SIZE = 64; // this is needed for spheres
  float vertices[][] = new float[DEFAULT_SIZE][VERTEX_FIELD_COUNT];
  int vertexCount;

  float r[]   = new float[DEFAULT_SIZE]; // storage used by incrementalize
  float dr[]  = new float[DEFAULT_SIZE];
  float l[]   = new float[DEFAULT_SIZE]; // more storage for incrementalize
  float dl[]  = new float[DEFAULT_SIZE];
  float sp[]  = new float[DEFAULT_SIZE]; // temporary storage for scanline
  float sdp[] = new float[DEFAULT_SIZE];

  protected boolean interpX;
  protected boolean interpUV; // is this necessary? could just check timage != null
  protected boolean interpARGB;

  private int rgba;
  private int r2, g2, b2, a2, a2orig;

  PGraphics parent;
  int[] pixels;
  // the parent's width/height,
  // or if smooth is enabled, parent's w/h scaled
  // up by the smooth dimension
  int width, height;
  int width1, height1;

  PImage timage;
  int[] tpixels;
  int theight, twidth;
  int theight1, twidth1;
  int tformat;

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

  final private int MODYRES(int y) {
    return (y & SUBYRES1);
  }


  public PPolygon(PGraphics iparent) {
    parent = iparent;
    reset(0);
  }


  protected void reset(int count) {
    vertexCount = count;
    interpX = true;
//    interpZ = true;
    interpUV = false;
    interpARGB = true;
    timage = null;
  }


  protected float[] nextVertex() {
    if (vertexCount == vertices.length) {
      float temp[][] = new float[vertexCount<<1][VERTEX_FIELD_COUNT];
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


  /**
   * Return true if this vertex is redundant. If so, will also
   * decrement the vertex count.
   */
  /*
  public boolean redundantVertex(float x, float y, float z) {
    // because vertexCount will be 2 when setting vertex[1]
    if (vertexCount < 2) return false;

    // vertexCount-1 is the current vertex that would be used
    // vertexCount-2 would be the previous feller
    if ((Math.abs(vertices[vertexCount-2][MX] - x) < EPSILON) &&
        (Math.abs(vertices[vertexCount-2][MY] - y) < EPSILON) &&
        (Math.abs(vertices[vertexCount-2][MZ] - z) < EPSILON)) {
      vertexCount--;
      return true;
    }
    return false;
  }
  */


  protected void texture(PImage image) {
    this.timage = image;

    if (image != null) {
      this.tpixels = image.pixels;
      this.twidth = image.width;
      this.theight = image.height;
      this.tformat = image.format;

      twidth1 = twidth - 1;
      theight1 = theight - 1;
      interpUV = true;
      
    } else {
      interpUV = false;      
    }
  }


  protected void renderPolygon(float[][] v, int count) {
    vertices = v;
    vertexCount = count;

    if (r.length < vertexCount) {
      r   = new float[vertexCount]; // storage used by incrementalize
      dr  = new float[vertexCount];
      l   = new float[vertexCount]; // more storage for incrementalize
      dl  = new float[vertexCount];
      sp  = new float[vertexCount]; // temporary storage for scanline
      sdp = new float[vertexCount];
    }

    render();
    checkExpand();
  }


  protected void renderTriangle(float[] v1, float[] v2, float[] v3) {
    // Calling code will have already done reset(3).
    // Can't do it here otherwise would nuke any texture settings.

    vertices[0] = v1;
    vertices[1] = v2;
    vertices[2] = v3;
    
    render();
    checkExpand();
  }
  
  
  protected void checkExpand() {
    if (smooth) {
      for (int i = 0; i < vertexCount; i++) {
        vertices[i][TX] /= SUBXRES;
        vertices[i][TY] /= SUBYRES;
      }
    }
  }
  
  
  protected void render() {
    if (vertexCount < 3) return;

    // these may have changed due to a resize()
    // so they should be refreshed here
    pixels = parent.pixels;
    //zbuffer = parent.zbuffer;

//    noDepthTest = parent.hints[DISABLE_DEPTH_TEST];
    smooth = parent.smooth;

    // by default, text turns on smooth for the textures
    // themselves. but this should be shut off if the hint
    // for DISABLE_TEXT_SMOOTH is set.
//    texture_smooth = true;

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

    /*
    // hack to not make polygons fly into the screen
    if (parent.hints[DISABLE_FLYING_POO]) {
      float nwidth2 = -width * 2;
      float nheight2 = -height * 2;
      float width2 = width * 2;
      float height2 = height * 2;
      for (int i = 0; i < vertexCount; i++) {
        if ((vertices[i][TX] < nwidth2) ||
            (vertices[i][TX] > width2) ||
            (vertices[i][TY] < nheight2) ||
            (vertices[i][TY] > height2)) {
          return;  // this is a bad poly
        }
      }
    }
    */

//    for (int i = 0; i < 4; i++) {
//      System.out.println(vertices[i][R] + " " + vertices[i][G] + " " + vertices[i][B]);
//    }
//    System.out.println();
    
    if (smooth) {
      for (int i = 0; i < vertexCount; i++) {
        vertices[i][TX] *= SUBXRES;
        vertices[i][TY] *= SUBYRES;
      }
      firstModY = -1;
    }

    // find top vertex (y is zero at top, higher downwards)
    int topi = 0;
    float ymin = vertices[0][TY];
    float ymax = vertices[0][TY]; // fry 031001
    for (int i = 1; i < vertexCount; i++) {
      if (vertices[i][TY] < ymin) {
        ymin = vertices[i][TY];
        topi = i;
      }
      if (vertices[i][TY] > ymax) {
        ymax = vertices[i][TY];
      }
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
        incrementalizeY(vertices[lefti], vertices[i], l, dl, y);
        lefty = (int) (vertices[i][TY] + 0.5f);
        lefti = i;
      }

      // advance right edge?
      while ((righty <= y) && (remaining > 0)) {
        remaining--;
        // step cw down right edge
        int i = (righti != vertexCount-1) ? (righti + 1) : 0;
        incrementalizeY(vertices[righti], vertices[i], r, dr, y);
        righty = (int) (vertices[i][TY] + 0.5f);
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
          if (l[TX] <= r[TX]) scanline(y, l, r);
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


  private void scanline(int y, float l[], float r[]) {
    //System.out.println("scanline " + y);
    for (int i = 0; i < vertexCount; i++) {  // should be moved later
      sp[i] = 0; sdp[i] = 0;
    }

    // this rounding doesn't seem to be relevant with smooth
    int lx = (int) (l[TX] + 0.49999f);  // ceil(l[TX]-.5);
    if (lx < 0) lx = 0;
    int rx = (int) (r[TX] - 0.5f);
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
    incrementalizeX(l, r, sp, sdp, lx);

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

    interpX = false;
    int tr, tg, tb, ta;

//    System.out.println("P2D interp uv " + interpUV + " " + 
//                       vertices[2][U] + " " + vertices[2][V]);
    for (int x = lx; x <= rx; x++) {
      // map texture based on U, V coords in sp[U] and sp[V]
      if (interpUV) {
        int tu = (int) (sp[U] * twidth);
        int tv = (int) (sp[V] * theight);

        if (tu > twidth1) tu = twidth1;
        if (tv > theight1) tv = theight1;
        if (tu < 0) tu = 0;
        if (tv < 0) tv = 0;

        int txy = tv*twidth + tu;

        int tuf1 = (int) (255f * (sp[U]*twidth - (float)tu));
        int tvf1 = (int) (255f * (sp[V]*theight - (float)tv));

        // the closer sp[U or V] is to the decimal being zero
        // the more coverage it should get of the original pixel
        int tuf = 255 - tuf1;
        int tvf = 255 - tvf1;

        // this code sucks! filled with bugs and slow as hell!
        int pixel00 = tpixels[txy];
        int pixel01 = (tv < theight1) ?
          tpixels[txy + twidth] : tpixels[txy];
        int pixel10 = (tu < twidth1) ?
          tpixels[txy + 1] : tpixels[txy];
        int pixel11 = ((tv < theight1) && (tu < twidth1)) ?
          tpixels[txy + twidth + 1] : tpixels[txy];

        int p00, p01, p10, p11;
        int px0, px1; //, pxy;

        
        // calculate alpha component (ta)
        
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
          ta = interpARGB ? ((int) (sp[A]*255)) : a2orig;
        }

        // calculate r,g,b components (tr, tg, tb)
        
        if ((tformat == RGB) || (tformat == ARGB)) {
          p00 = (pixel00 >> 16) & 0xff;  // red
          p01 = (pixel01 >> 16) & 0xff;
          p10 = (pixel10 >> 16) & 0xff;
          p11 = (pixel11 >> 16) & 0xff;

          px0 = (p00*tuf + p10*tuf1) >> 8;
          px1 = (p01*tuf + p11*tuf1) >> 8;
          tr = (((px0*tvf + px1*tvf1) >> 8) *
                (interpARGB ? ((int) (sp[R]*255)) : r2)) >> 8;
                
          p00 = (pixel00 >> 8) & 0xff;  // green
          p01 = (pixel01 >> 8) & 0xff;
          p10 = (pixel10 >> 8) & 0xff;
          p11 = (pixel11 >> 8) & 0xff;

          px0 = (p00*tuf + p10*tuf1) >> 8;
          px1 = (p01*tuf + p11*tuf1) >> 8;
          tg = (((px0*tvf + px1*tvf1) >> 8) *
                (interpARGB ? ((int) (sp[G]*255)) : g2)) >> 8;

          p00 = pixel00 & 0xff;  // blue
          p01 = pixel01 & 0xff;
          p10 = pixel10 & 0xff;
          p11 = pixel11 & 0xff;

          px0 = (p00*tuf + p10*tuf1) >> 8;
          px1 = (p01*tuf + p11*tuf1) >> 8;
          tb = (((px0*tvf + px1*tvf1) >> 8) *
                (interpARGB ? ((int) (sp[B]*255)) : b2)) >> 8;

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

        int weight = smooth ? coverage(x) : 255;
        if (weight != 255) ta = ta*weight >> 8;
        
        if ((ta == 254) || (ta == 255)) {  // if (ta & 0xf8) would be good
          // no need to blend
          pixels[offset+x] = 0xff000000 | (tr << 16) | (tg << 8) | tb;

        } else {
          // blend with pixel on screen
          int a1 = 255-ta;
          int r1 = (pixels[offset+x] >> 16) & 0xff;
          int g1 = (pixels[offset+x] >> 8) & 0xff;
          int b1 = (pixels[offset+x]) & 0xff;

          pixels[offset+x] = 0xff000000 |
            (((tr*ta + r1*a1) >> 8) << 16) |
            ((tg*ta + g1*a1) & 0xff00) |
            ((tb*ta + b1*a1) >> 8);
        }

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
          //zbuffer[offset+x] = sp[Z];

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
        }
      }

      // if smooth enabled, don't increment values
      // for the pixel in the stretch out version
      // of the scanline used to get smooth edges.
      if (!smooth || ((x >= truelx) && (x <= truerx))) {
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


  private void incrementalizeY(float p1[], float p2[],
                               float p[], float dp[], int y) {
    float delta = p2[TY] - p1[TY];
    if (delta == 0) delta = 1;
    float fraction = y + 0.5f - p1[TY];

    if (interpX) {
      dp[TX] = (p2[TX] - p1[TX]) / delta;
      p[TX] = p1[TX] + dp[TX] * fraction;
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

      p[U] = p1[U] + dp[U] * fraction;
      p[V] = p1[V] + dp[V] * fraction;
    }
  }


  private void incrementalizeX(float p1[], float p2[],
                               float p[], float dp[], int x) {
    float delta = p2[TX] - p1[TX];
    if (delta == 0) delta = 1;
    float fraction = x + 0.5f - p1[TX];
    if (smooth) {
      delta /= SUBXRES;
      fraction /= SUBXRES;
    }

    if (interpX) {
      dp[TX] = (p2[TX] - p1[TX]) / delta;
      p[TX] = p1[TX] + dp[TX] * fraction;
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

      p[U] = p1[U] + dp[U] * fraction;
      p[V] = p1[V] + dp[V] * fraction;
    }
  }


  private void increment(float p[], float dp[]) {
    if (interpX) p[TX] += dp[TX];

    if (interpARGB) {
      p[R] += dp[R];
      p[G] += dp[G];
      p[B] += dp[B];
      p[A] += dp[A];
    }

    if (interpUV) {
      p[U] += dp[U];
      p[V] += dp[V];
    }
  }
}
