/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2006 Ben Fry and Casey Reas

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

import java.awt.Toolkit;
import java.awt.image.DirectColorModel;
import java.awt.image.MemoryImageSource;


/**
 * Subclass of PGraphics that handles fast 2D rendering,
 * more commonly referred to as P2D. This class uses no Java2D
 * and will run with Java 1.1.
 */
public class PGraphics2D extends PGraphics {

  PPolygon polygon;     // general polygon to use for shape
  PPolygon fpolygon;    // used to fill polys for tri or quad strips
  PPolygon spolygon;    // stroke/line polygon
  float svertices[][];  // temp vertices used for stroking end of poly

  // polygon that handles tesselation
  private PPolygon tpolygon;
  private int TPOLYGON_MAX_VERTICES = 512;
  private int tpolygon_vertex_order[]; // = new int[MAX_VERTICES];

  PLine line;

  //boolean untransformed;
  boolean strokeChanged = true;
  boolean fillChanged = true;

  static final int CVERTEX_ALLOC = 128;
  float cvertex[][] = new float[CVERTEX_ALLOC][VERTEX_FIELD_COUNT];
  int cvertexIndex;


  //////////////////////////////////////////////////////////////


  //protected PGraphics2D() { }


  /*
  public PGraphics2D(int iwidth, int iheight) {
    this(iwidth, iheight, null);
  }
  */


  public PGraphics2D(int iwidth, int iheight, PApplet applet) {
    super(iwidth, iheight, applet);
    /*
    if (applet != null) {
      this.parent = applet;
      applet.addListeners();
    }
    resize(iwidth, iheight);
    */
  }


  //resize handled by superclass


  //requestDisplay handled by superclass


  protected void allocate() {
    pixelCount = width * height;
    pixels = new int[pixelCount];

    // because of a java 1.1 bug, pixels must be registered as
    // opaque before their first run, the memimgsrc will flicker
    // and run very slowly.
    backgroundColor |= 0xff000000;  // just for good measure
    for (int i = 0; i < pixelCount; i++) pixels[i] = backgroundColor;
    //for (int i = 0; i < pixelCount; i++) pixels[i] = 0xffffffff;

    //if (parent != null) {
    if (mainDrawingSurface) {
      cm = new DirectColorModel(32, 0x00ff0000, 0x0000ff00, 0x000000ff);;
      mis = new MemoryImageSource(width, height, pixels, 0, width);
      mis.setFullBufferUpdates(true);
      mis.setAnimated(true);
      image = Toolkit.getDefaultToolkit().createImage(mis);
    }

    // can't un-set this because this may be only a resize (Bug #463)
    //defaultsInited = false;
  }


  //////////////////////////////////////////////////////////////


  public void beginDraw() {
    insideResizeWait();
    insideDraw = true;

    // need to call defaults(), but can only be done when it's ok
    // to draw (i.e. for opengl, no drawing can be done outside
    // beginDraw/endDraw).
    if (!defaultsInited) {
      defaults();

      polygon  = new PPolygon(this);
      fpolygon = new PPolygon(this);
      spolygon = new PPolygon(this);
      spolygon.vertexCount = 4;
      svertices = new float[2][];
    }

    resetMatrix(); // reset model matrix

    // reset vertices
    vertexCount = 0;
  }


  public void endDraw() {
    // moving this back here (post-68) because of macosx thread problem
    if (mis != null) {
      mis.newPixels(pixels, cm, 0, width);
    }
    // mark pixels as having been updated, so that they'll work properly
    // when this PGraphics is drawn using image().
    updatePixels();

    insideDraw = false;
  }


  //////////////////////////////////////////////////////////////


  public void beginShape(int kind) {
    shape = kind;
    vertexCount = 0;
    splineVertexCount = 0;

    polygon.reset(0);
    fpolygon.reset(4);
    spolygon.reset(4);

    polygon.interpUV = false;
  }


  // PGraphics will throw a depthError
  //public void normal(float nx, float ny, float nz)

  // PGraphics will handle setting these
  //public void textureMode(int mode)
  //public void texture(PImage image)
  //protected void textureVertex(float u, float v)


  public void vertex(float x, float y) {
    float vertex[] = polygon.nextVertex();
    cvertexIndex = 0; // reset curves to start

    vertex[MX] = x;
    vertex[MY] = y;

    if (fill) {
      vertex[R] = fillR;
      vertex[G] = fillG;
      vertex[B] = fillB;
      vertex[A] = fillA;
    }

    if (stroke) {
      vertex[SR] = strokeR;
      vertex[SG] = strokeG;
      vertex[SB] = strokeB;
      vertex[SA] = strokeA;
      vertex[SW] = strokeWeight;
    }

    // this complicated if construct may defeat the purpose
    if (textureImage != null) {
      vertex[U] = textureU;
      vertex[V] = textureV;
    }
  }


  public void vertex(float x, float y, float u, float v) {
    textureVertex(u, v);
    vertex(x, y);
  }


  public void vertex(float x, float y, float z) {
    depthErrorXYZ("vertex");
  }


  public void vertex(float x, float y, float z, float u, float v) {
    depthErrorXYZ("vertex");
  }


  public void endShape(int mode) {
    // clear the 'shape drawing' flag in case of early exit
    //shape = 0;
    // hm can't do anymore..

    int polyVertexCount = polygon.vertexCount;
    float polyVertices[][] = polygon.vertices;

    if (untransformed()) {
      for (int i = 0; i < polyVertexCount; i++) {
        polyVertices[i][X] = polyVertices[i][MX];
        polyVertices[i][Y] = polyVertices[i][MY];
      }
    } else {
      for (int i = 0; i < polyVertexCount; i++) {
        polyVertices[i][X] = m00*polyVertices[i][MX] + m01*polyVertices[i][MY] + m03;
        polyVertices[i][Y] = m10*polyVertices[i][MX] + m11*polyVertices[i][MY] + m13;
      }
    }

    // ------------------------------------------------------------------
    // TEXTURES

    if (polygon.interpUV) {
      fpolygon.texture(textureImage); //polygon.timage);
    }


    // ------------------------------------------------------------------
    // COLORS
    // calculate RGB for each vertex

    spolygon.interpARGB = strokeChanged; //false;
    fpolygon.interpARGB = fillChanged; //false;

    // all the values for r, g, b have been set with calls to vertex()
    // (no need to re-calculate anything here)


    // ------------------------------------------------------------------
    // RENDER SHAPES

    int increment;

    switch (shape) {
    case POINTS:
      if (untransformed() && (strokeWeight == 1)) {
        if (!strokeChanged) {
          for (int i = 0; i < polyVertexCount; i++) {
            thin_point((int) polyVertices[i][X], (int) polyVertices[i][Y],
                0, strokeColor);
          }
        } else {
          for (int i = 0; i < polyVertexCount; i++) {
            thin_point((int) polyVertices[i][X], (int) polyVertices[i][Y],
                0, float_color(polyVertices[i][SR],
                    polyVertices[i][SG],
                    polyVertices[i][SB]));
          }
          //strokei = strokeiSaved;
        }
      } else {
        float f[] = polyVertices[0];

        for (int i = 0; i < polyVertexCount; i++) {
          float v[] = polyVertices[i];

          // if this is the first time (i == 0)
          // or if lighting is enabled
          // or the stroke color has changed inside beginShape/endShape
          // then re-calculate the color at this vertex
          if ((i == 0) || strokeChanged) {
            // push calculated color into 'f' (this way, f is always valid)
            calc_lighting(v[SR], v[SG], v[SB],
                v[X],  v[Y],  v[Z],
                v[NX], v[NY], v[NZ],  f, R);
          }
          // uses [SA], since stroke alpha isn't moved into [A] the
          // way that [SR] goes to [R] etc on the calc_lighting call
          // (there's no sense in copying it to [A], except consistency
          // in the code.. but why the extra slowness?)
          thick_point(v[X], v[Y], v[Z],  f[R], f[G], f[B], f[SA]);
        }
      }
      break;

    case LINES:
    //case LINE_STRIP:
    //case LINE_LOOP:
      if (!stroke) return;

      // if it's a line loop, copy the vertex data to the last element
      //if (shape == LINE_LOOP) {
      if (mode == CLOSE) {
        float v0[] = polygon.vertices[0];
        float v1[] = polygon.nextVertex();
        polyVertexCount++; // since it had already been read above

        v1[X] = v0[X]; v1[Y] = v0[Y]; v1[Z] = v0[Z];
        v1[SR] = v0[SR]; v1[SG] = v0[SG]; v1[SB] = v0[SB];
      }

      // increment by two for individual lines
      increment = (shape == LINES) ? 2 : 1;
      draw_lines(polyVertices, polyVertexCount-1, 1, increment, 0);
      break;

    case TRIANGLES:
    case TRIANGLE_STRIP:
      increment = (shape == TRIANGLES) ? 3 : 1;
      // do fill and stroke separately because otherwise
      // the lines will be stroked more than necessary
      if (fill) {
        fpolygon.vertexCount = 3;
        for (int i = 0; i < polyVertexCount-2; i += increment) {
          for (int j = 0; j < 3; j++) {
            fpolygon.vertices[j][R] = polyVertices[i+j][R];
            fpolygon.vertices[j][G] = polyVertices[i+j][G];
            fpolygon.vertices[j][B] = polyVertices[i+j][B];
            fpolygon.vertices[j][A] = polyVertices[i+j][A];

            fpolygon.vertices[j][X] = polyVertices[i+j][X];
            fpolygon.vertices[j][Y] = polyVertices[i+j][Y];
            fpolygon.vertices[j][Z] = polyVertices[i+j][Z];

            if (polygon.interpUV) {
              fpolygon.vertices[j][U] = polyVertices[i+j][U];
              fpolygon.vertices[j][V] = polyVertices[i+j][V];
            }
          }
          fpolygon.render();
        }
      }
      if (stroke) {
        // first draw all vertices as a line strip
        if (shape == TRIANGLE_STRIP) {
          draw_lines(polyVertices, polyVertexCount-1, 1, 1, 0);
        } else {
          draw_lines(polyVertices, polyVertexCount-1, 1, 1, 3);
        }
        // then draw from vertex (n) to (n+2)
        // incrementing n using the same as above
        draw_lines(polyVertices, polyVertexCount-2, 2, increment, 0);
        // changed this to vertexCount-2, because it seemed
        // to be adding an extra (nonexistant) line
      }
      break;

    case QUADS:
    case QUAD_STRIP:
      //System.out.println("pooping out a quad");
      increment = (shape == QUADS) ? 4 : 2;
      if (fill) {
        fpolygon.vertexCount = 4;
        for (int i = 0; i < polyVertexCount-3; i += increment) {
          for (int j = 0; j < 4; j++) {
            fpolygon.vertices[j][R] = polyVertices[i+j][R];
            fpolygon.vertices[j][G] = polyVertices[i+j][G];
            fpolygon.vertices[j][B] = polyVertices[i+j][B];
            fpolygon.vertices[j][A] = polyVertices[i+j][A];

            fpolygon.vertices[j][X] = polyVertices[i+j][X];
            fpolygon.vertices[j][Y] = polyVertices[i+j][Y];
            fpolygon.vertices[j][Z] = polyVertices[i+j][Z];

            if (polygon.interpUV) {
              fpolygon.vertices[j][U] = polyVertices[i+j][U];
              fpolygon.vertices[j][V] = polyVertices[i+j][V];
            }
          }
          fpolygon.render();
        }
      }
      if (stroke) {
        // first draw all vertices as a line strip
        if (shape == QUAD_STRIP) {
          draw_lines(polyVertices, polyVertexCount-1, 1, 1, 0);
        } else {  // skip every few for quads
          draw_lines(polyVertices, polyVertexCount, 1, 1, 4);
        }
        // then draw from vertex (n) to (n+3)
        // incrementing n by the same increment as above
        draw_lines(polyVertices, polyVertexCount-2, 3, increment, 0);
      }
      break;

    case POLYGON:
      if (isConvex()) {
        if (fill) {
          polygon.render();
          if (stroke) polygon.unexpand();
        }

        if (stroke) {
          draw_lines(polyVertices, polyVertexCount-1, 1, 1, 0);
          // draw the last line connecting back to the first point in poly
          svertices[0] = polyVertices[polyVertexCount-1];
          svertices[1] = polyVertices[0];
          draw_lines(svertices, 1, 1, 1, 0);
        }
      } else {
        if (fill) {
          // the triangulator produces polygons that don't align
          // when smoothing is enabled. but if there is a stroke around
          // the polygon, then smoothing can be temporarily disabled.
          boolean smoov = smooth;
          //if (stroke && !hints[DISABLE_SMOOTH_HACK]) smooth = false;
          if (stroke) smooth = false;
          concaveRender();
          //if (stroke && !hints[DISABLE_SMOOTH_HACK]) smooth = smoov;
          if (stroke) smooth = smoov;
        }

        if (stroke) {
          draw_lines(polyVertices, polyVertexCount-1, 1, 1, 0);
          // draw the last line connecting back
          // to the first point in poly
          svertices[0] = polyVertices[polyVertexCount-1];
          svertices[1] = polyVertices[0];
          draw_lines(svertices, 1, 1, 1, 0);
        }
      }
      break;
    }

    // to signify no shape being drawn
    shape = 0;
  }



  //////////////////////////////////////////////////////////////

  // CONCAVE/CONVEX POLYGONS


  private boolean isConvex() {
    float v[][] = polygon.vertices;
    int n = polygon.vertexCount;
    int j,k;
    int flag = 0;
    float z;
    //float tol = 0.001f;

    if (n < 3)
      // ERROR: this is a line or a point, render with CONVEX
      return true;

    // iterate along border doing dot product.
    // if the sign of the result changes, then is concave
    for (int i=0;i<n;i++) {
      j = (i + 1) % n;
      k = (i + 2) % n;
      z  = (v[j][X] - v[i][X]) * (v[k][Y] - v[j][Y]);
      z -= (v[j][Y] - v[i][Y]) * (v[k][X] - v[j][X]);
      if (z < 0)
        flag |= 1;
      else if (z > 0)
        flag |= 2;
      if (flag == 3)
        return false;  // CONCAVE
    }
    if (flag != 0)
      return true;    // CONVEX
    else
      // ERROR: colinear points, self intersection
      // treat as CONVEX
      return true;
  }


  // triangulate the current polygon
  private void concaveRender() {
    // WARNING: code is not in optimum form
    // local initiations of some variables are made to
    // keep the code modular and easy to integrate
    // restet triangle
    float polyVertices[][] = polygon.vertices;

    if (tpolygon == null) {
      // allocate on first use, rather than slowing
      // the startup of the class.
      tpolygon = new PPolygon(this);
      tpolygon_vertex_order = new int[TPOLYGON_MAX_VERTICES];
    }
    tpolygon.reset(3);

    // copy render parameters

    if (textureImage != null) {
      tpolygon.texture(textureImage); //polygon.timage);
    }

    tpolygon.interpX = polygon.interpX;
    tpolygon.interpZ = polygon.interpZ;
    tpolygon.interpUV = polygon.interpUV;
    tpolygon.interpARGB = polygon.interpARGB;

    // simple ear clipping polygon triangulation
    // addapted from code by john w. ratcliff (jratcliff@verant.com)

    // 1 - first we check if the polygon goes CW or CCW
    // CW-CCW ordering adapted from code by
    //        Joseph O'Rourke orourke@cs.smith.edu
    // 1A - we start by finding the lowest-right most vertex

    boolean ccw = false; // clockwise

    int n = polygon.vertexCount;
    int mm; // postion for LR vertex
    float min[] = new float[2];

    min[X] = polyVertices[0][X];
    min[Y] = polyVertices[0][Y];
    mm = 0;

    for(int i = 0; i < n; i++ ) {
      if( (polyVertices[i][Y] < min[Y]) ||
          ( (polyVertices[i][Y] == min[Y]) && (polyVertices[i][X] > min[X]) )
      ) {
        mm = i;
        min[X] = polyVertices[mm][X];
        min[Y] = polyVertices[mm][Y];
      }
    }

    // 1B - now we compute the cross product of the edges of this vertex
    float cp;
    int mm1;

    // just for renaming
    float a[] = new float[2];
    float b[] = new float[2];
    float c[] = new float[2];

    mm1 = (mm + (n-1)) % n;

    // assign a[0] to point to poly[m1][0] etc.
    for(int i = 0; i < 2; i++ ) {
      a[i] = polyVertices[mm1][i];
      b[i] = polyVertices[mm][i];
      c[i] = polyVertices[(mm+1)%n][i];
    }

    cp = a[0] * b[1] - a[1] * b[0] +
    a[1] * c[0] - a[0] * c[1] +
    b[0] * c[1] - c[0] * b[1];

    if ( cp > 0 )
      ccw = true;   // CCW
    else
      ccw = false;  // CW

    // 1C - then we sort the vertices so they
    // are always in a counterclockwise order
    //int j = 0;
    if (!ccw) {
      // keep the same order
      for (int i = 0; i < n; i++) {
        tpolygon_vertex_order[i] = i;
      }

    } else {
      // invert the order
      for (int i = 0; i < n; i++) {
        tpolygon_vertex_order[i] = (n - 1) - i;
      }
    }

    // 2 - begin triangulation
    // resulting triangles are stored in the triangle array
    // remove vc-2 Vertices, creating 1 triangle every time
    int vc = n;
    int count = 2*vc;  // complex polygon detection

    for (int m = 0, v = vc - 1; vc > 2; ) {
      boolean snip = true;

      // if we start over again, is a complex polygon
      if (0 >= (count--)) {
        break; // triangulation failed
      }

      // get 3 consecutive vertices <u,v,w>
      int u = v ; if (vc <= u) u = 0; // previous
      v = u+1; if (vc <= v) v = 0;    // current
      int w = v+1; if (vc <= w) w = 0;  // next

      // triangle A B C
      float Ax, Ay, Bx, By, Cx, Cy, Px, Py;

      Ax =  -polyVertices[tpolygon_vertex_order[u]][X];
      Ay =   polyVertices[tpolygon_vertex_order[u]][Y];
      Bx =  -polyVertices[tpolygon_vertex_order[v]][X];
      By =   polyVertices[tpolygon_vertex_order[v]][Y];
      Cx =  -polyVertices[tpolygon_vertex_order[w]][X];
      Cy =   polyVertices[tpolygon_vertex_order[w]][Y];

      if ( EPSILON > (((Bx-Ax) * (Cy-Ay)) - ((By-Ay) * (Cx-Ax)))) {
        continue;
      }

      for (int p = 0; p < vc; p++) {

        // this part is a bit osbscure, basically what it does
        // is test if this tree vertices are and ear or not, looking for
        // intersections with the remaining vertices using a cross product
        float ax, ay, bx, by, cx, cy, apx, apy, bpx, bpy, cpx, cpy;
        float cCROSSap, bCROSScp, aCROSSbp;

        if( (p == u) || (p == v) || (p == w) ) {
          continue;
        }

        Px = -polyVertices[tpolygon_vertex_order[p]][X];
        Py =  polyVertices[tpolygon_vertex_order[p]][Y];

        ax = Cx - Bx; ay = Cy - By;
        bx = Ax - Cx; by = Ay - Cy;
        cx = Bx - Ax; cy = By - Ay;
        apx= Px - Ax; apy= Py - Ay;
        bpx= Px - Bx; bpy= Py - By;
        cpx= Px - Cx; cpy= Py - Cy;

        aCROSSbp = ax * bpy - ay * bpx;
        cCROSSap = cx * apy - cy * apx;
        bCROSScp = bx * cpy - by * cpx;

        if ((aCROSSbp >= 0.0f) && (bCROSScp >= 0.0f) && (cCROSSap >= 0.0f)) {
          snip = false;
        }
      }

      if (snip) {
        // yes, the trio is an ear, render it and cut it

        int triangle_vertices[] = new int[3];
        int s,t;

        // true names of the vertices
        triangle_vertices[0] = tpolygon_vertex_order[u];
        triangle_vertices[1] = tpolygon_vertex_order[v];
        triangle_vertices[2] = tpolygon_vertex_order[w];

        // create triangle
        //render_triangle(triangle_vertices);
        //private final void render_triangle(int[] triangle_vertices) {
        // copy all fields of the triangle vertices
        for (int i = 0; i < 3; i++) {
          float[] src = polygon.vertices[triangle_vertices[i]];
          float[] dest = tpolygon.vertices[i];
          for (int k = 0; k < VERTEX_FIELD_COUNT; k++) {
            dest[k] = src[k];
          }
        }
        // render triangle
        tpolygon.render();
        //}

        m++;

        // remove v from remaining polygon
        for( s = v, t = v + 1; t < vc; s++, t++) {
          tpolygon_vertex_order[s] = tpolygon_vertex_order[t];
        }

        vc--;

        // resest error detection counter
        count = 2 * vc;
      }
    }
  }



  //////////////////////////////////////////////////////////////

  // RECT


  protected void rectImpl(float x1f, float y1f, float x2f, float y2f) {

    if (untransformed() && !fillAlpha) {
      int x1 = (int) x1f;
      int y1 = (int) y1f;
      int x2 = (int) x2f;
      int y2 = (int) y2f;

      rectImplFillUntranSolidRGB(x1, y1, x2, y2);

      if (stroke) {
        if (strokeWeight == 1) {
          thin_flat_line(x1, y1, x2, y1);
          thin_flat_line(x2, y1, x2, y2);
          thin_flat_line(x2, y2, x1, y2);
          thin_flat_line(x1, y2, x1, y1);

        } else {
          thick_flat_line(x1, y1, fillR, fillG, fillB, fillA,
                          x2, y1, fillR, fillG, fillB, fillA);
          thick_flat_line(x2, y1, fillR, fillG, fillB, fillA,
                          x2, y2, fillR, fillG, fillB, fillA);
          thick_flat_line(x2, y2, fillR, fillG, fillB, fillA,
                          x1, y2, fillR, fillG, fillB, fillA);
          thick_flat_line(x1, y2, fillR, fillG, fillB, fillA,
                          x1, y1, fillR, fillG, fillB, fillA);
        }
      }

    } else {
      beginShape(QUADS);
      vertex(x1f, y1f);
      vertex(x2f, y1f);
      vertex(x2f, y2f);
      vertex(x1f, y2f);
      endShape();
    }
  }


  /**
   * Draw an untransformed rectangle with no alpha.
   */
  private void rectImplFillUntranSolidRGB(int x1, int y1, int x2, int y2) {
    //System.out.println("flat quad");
    if (y2 < y1) {
      int temp = y1; y1 = y2; y2 = temp;
    }
    if (x2 < x1) {
      int temp = x1; x1 = x2; x2 = temp;
    }
    // checking to watch out for boogers
    if ((x1 > width1) || (x2 < 0) ||
        (y1 > height1) || (y2 < 0)) return;

    //if (fill) {
    int fx1 = x1;
    int fy1 = y1;
    int fx2 = x2;
    int fy2 = y2;

    // these only affect the fill, not the stroke
    // (otherwise strange boogers at edges b/c frame changes shape)
    if (fx1 < 0) fx1 = 0;
    if (fx2 > width) fx2 = width;
    if (fy1 < 0) fy1 = 0;
    if (fy2 > height) fy2 = height;

    // [toxi 031223]
    // on avg. 20-25% faster fill routine using System.arraycopy()
    int ww = fx2 - fx1;
    int hh = fy2 - fy1;
    int[] row = new int[ww];
    for (int i = 0; i < ww; i++) row[i] = fillColor;
    int idx = fy1 * width + fx1;
    for (int y = 0; y < hh; y++) {
      System.arraycopy(row, 0, pixels, idx, ww);
      idx += width;
    }
    row = null;
    //}
  }



  //////////////////////////////////////////////////////////////

  // ELLIPSE AND ARC


  public void ellipseImpl(float x1, float y1, float w, float h) {
    if (!smooth && (strokeWeight == 1) &&
        !fillAlpha && !strokeAlpha && untransformed()) {
      float hradius = w / 2f;
      float vradius = h / 2f;

      int centerX = (int) (x1 + hradius);
      int centerY = (int) (y1 + vradius);

      if (hradius == vradius) {
        flat_circle(centerX, centerY, (int)hradius);

      } else {
        flat_ellipse(centerX, centerY, (int)hradius, (int)vradius);
      }
    } else {
      super.ellipseImpl(x1, y1, w, h);
    }
  }


  private void flat_circle(int centerX, int centerY, int radius) {
    if (unwarped()) {
      float x = m00*centerX + m01*centerY + m02;
      float y = m10*centerX + m11*centerY + m12;
      centerX = (int)x;
      centerY = (int)y;
    }
    if (fill) flat_circle_fill(centerX, centerY, radius);
    if (stroke) flat_circle_stroke(centerX, centerY, radius);
  }


  /**
   * Draw the outline around a flat circle using a bresenham-style
   * algorithm. Adapted from drawCircle function in "Computer Graphics
   * for Java Programmers" by Leen Ammeraal, p. 110.
   * <P/>
   * This function is included because the quality is so much better,
   * and the drawing significantly faster than with adaptive ellipses
   * drawn using the sine/cosine tables.
   * <P/>
   * Circle quadrants break down like so:
   * <PRE>
   *              |
   *        \ NNW | NNE /
   *          \   |   /
   *       WNW  \ | /  ENE
   *     -------------------
   *       WSW  / | \  ESE
   *          /   |   \
   *        / SSW | SSE \
   *              |
   * </PRE>
   * @param xc x center
   * @param yc y center
   * @param r radius
   */
  private void flat_circle_stroke(int xC, int yC, int r) {
    int x = 0, y = r, u = 1, v = 2 * r - 1, E = 0;
    while (x < y) {
      thin_point(xC + x, yC + y, 0, strokeColor); // NNE
      thin_point(xC + y, yC - x, 0, strokeColor); // ESE
      thin_point(xC - x, yC - y, 0, strokeColor); // SSW
      thin_point(xC - y, yC + x, 0, strokeColor); // WNW

      x++; E += u; u += 2;
      if (v < 2 * E) {
        y--; E -= v; v -= 2;
      }
      if (x > y) break;

      thin_point(xC + y, yC + x, 0, strokeColor); // ENE
      thin_point(xC + x, yC - y, 0, strokeColor); // SSE
      thin_point(xC - y, yC - x, 0, strokeColor); // WSW
      thin_point(xC - x, yC + y, 0, strokeColor); // NNW
    }
  }

  /**
   * Heavily adapted version of the above algorithm that handles
   * filling the ellipse. Works by drawing from the center and
   * outwards to the points themselves. Has to be done this way
   * because the values for the points are changed halfway through
   * the function, making it impossible to just store a series of
   * left and right edges to be drawn more quickly.
   *
   * @param xc x center
   * @param yc y center
   * @param r radius
   */
  private void flat_circle_fill(int xc, int yc, int r) {
    int x = 0, y = r, u = 1, v = 2 * r - 1, E = 0;
    while (x < y) {
      for (int xx = xc; xx < xc + x; xx++) {  // NNE
        thin_point(xx, yc + y, 0, fillColor);
      }
      for (int xx = xc; xx < xc + y; xx++) {  // ESE
        thin_point(xx, yc - x, 0, fillColor);
      }
      for (int xx = xc - x; xx < xc; xx++) {  // SSW
        thin_point(xx, yc - y, 0, fillColor);
      }
      for (int xx = xc - y; xx < xc; xx++) {  // WNW
        thin_point(xx, yc + x, 0, fillColor);
      }

      x++; E += u; u += 2;
      if (v < 2 * E) {
        y--; E -= v; v -= 2;
      }
      if (x > y) break;

      for (int xx = xc; xx < xc + y; xx++) {  // ENE
        thin_point(xx, yc + x, 0, fillColor);
      }
      for (int xx = xc; xx < xc + x; xx++) {  // SSE
        thin_point(xx, yc - y, 0, fillColor);
      }
      for (int xx = xc - y; xx < xc; xx++) {  // WSW
        thin_point(xx, yc - x, 0, fillColor);
      }
      for (int xx = xc - x; xx < xc; xx++) {  // NNW
        thin_point(xx, yc + y, 0, fillColor);
      }
    }
  }

  // unfortunately this can't handle fill and stroke simultaneously,
  // because the fill will later replace some of the stroke points

  private final void flat_ellipse_symmetry(int centerX, int centerY,
                                           int ellipseX, int ellipseY,
                                           boolean filling) {
    if (filling) {
      for (int i = centerX - ellipseX + 1; i < centerX + ellipseX; i++) {
        thin_point(i, centerY - ellipseY, 0, fillColor);
        thin_point(i, centerY + ellipseY, 0, fillColor);
      }
    } else {
      thin_point(centerX - ellipseX, centerY + ellipseY, 0, strokeColor);
      thin_point(centerX + ellipseX, centerY + ellipseY, 0, strokeColor);
      thin_point(centerX - ellipseX, centerY - ellipseY, 0, strokeColor);
      thin_point(centerX + ellipseX, centerY - ellipseY, 0, strokeColor);
    }
  }


  /**
   * Bresenham-style ellipse drawing function, adapted from a posting to
   * comp.graphics.algortihms.
   *
   * This function is included because the quality is so much better,
   * and the drawing significantly faster than with adaptive ellipses
   * drawn using the sine/cosine tables.
   *
   * @param centerX x coordinate of the center
   * @param centerY y coordinate of the center
   * @param a horizontal radius
   * @param b vertical radius
   */
  private void flat_ellipse_internal(int centerX, int centerY,
                                     int a, int b, boolean filling) {
    int x, y, a2, b2, s, t;

    a2 = a*a;
    b2 = b*b;
    x = 0;
    y = b;
    s = a2*(1-2*b) + 2*b2;
    t = b2 - 2*a2*(2*b-1);
    flat_ellipse_symmetry(centerX, centerY, x, y, filling);

    do {
      if (s < 0) {
        s += 2*b2*(2*x+3);
        t += 4*b2*(x+1);
        x++;
      } else if (t < 0) {
        s += 2*b2*(2*x+3) - 4*a2*(y-1);
        t += 4*b2*(x+1) - 2*a2*(2*y-3);
        x++;
        y--;
      } else {
        s -= 4*a2*(y-1);
        t -= 2*a2*(2*y-3);
        y--;
      }
      flat_ellipse_symmetry(centerX, centerY, x, y, filling);

    } while (y > 0);
  }


  private void flat_ellipse(int centerX, int centerY, int a, int b) {
    if (unwarped()) {
      float x = m00*centerX + m01*centerY + m02;
      float y = m10*centerX + m11*centerY + m12;
      centerX = (int)x;
      centerY = (int)y;
    }
    if (fill) flat_ellipse_internal(centerX, centerY, a, b, true);
    if (stroke) flat_ellipse_internal(centerX, centerY, a, b, false);
  }


  // TODO really need a decent arc function in here..

  //protected void arcImpl(float x1, float y1, float w, float h,
  //                       float start, float stop)



  //////////////////////////////////////////////////////////////

  // BOX & SPHERE


  // The PGraphics superclass will throw errors for these fellas



  //////////////////////////////////////////////////////////////

  // BEZIER & CURVE


  public void bezier(float x1, float y1, float z1,
                     float x2, float y2, float z2,
                     float x3, float y3, float z3,
                     float x4, float y4, float z4) {
    depthErrorXYZ("bezier");
  }


  public void curve(float x1, float y1, float z1,
                    float x2, float y2, float z2,
                    float x3, float y3, float z3,
                    float x4, float y4, float z4) {
    depthErrorXYZ("curve");
  }



  //////////////////////////////////////////////////////////////

  // IMAGE


  protected void imageImpl(PImage image,
                           float x1, float y1, float x2, float y2,
                           int u1, int v1, int u2, int v2) {
    if ((x2 - x1 == image.width) &&
        (y2 - y1 == image.height) &&
        !tint && unwarped()) {
      flat_image(image, (int) (x1 + m02), (int) (y1 + m12), u1, v1, u2, v2);

    } else {
      super.imageImpl(image, x1, y1, x2, y2, u1, v1, u2, v2);
    }
  }


  /**
   * Image drawn in flat "screen space", with no scaling or warping.
   * this is so common that a special routine is included for it,
   * because the alternative is much slower.
   *
   * @param  image  image to be drawn
   * @param  sx1    x coordinate of upper-lefthand corner in screen space
   * @param  sy1    y coordinate of upper-lefthand corner in screen space
   */
  private void flat_image(PImage image, int sx1, int sy1,
                          int ix1, int iy1, int ix2, int iy2) {
    /*
    int ix1 = 0;
    int iy1 = 0;
    int ix2 = image.width;
    int iy2 = image.height;
    */

    if (imageMode == CENTER) {
      sx1 -= image.width / 2;
      sy1 -= image.height / 2;
    }

    int sx2 = sx1 + image.width;
    int sy2 = sy1 + image.height;

    // don't draw if completely offscreen
    // (without this check, ArrayIndexOutOfBoundsException)
    if ((sx1 > width1) || (sx2 < 0) ||
        (sy1 > height1) || (sy2 < 0)) return;

    if (sx1 < 0) {  // off left edge
      ix1 -= sx1;
      sx1 = 0;
    }
    if (sy1 < 0) {  // off top edge
      iy1 -= sy1;
      sy1 = 0;
    }
    if (sx2 > width) {  // off right edge
      ix2 -= sx2 - width;
      sx2 = width;
    }
    if (sy2 > height) {  // off bottom edge
      iy2 -= sy2 - height;
      sy2 = height;
    }

    int source = iy1 * image.width + ix1;
    int target = sy1 * width;

    if (image.format == ARGB) {
      for (int y = sy1; y < sy2; y++) {
        int tx = 0;

        for (int x = sx1; x < sx2; x++) {
          pixels[target + x] =
            _blend(pixels[target + x],
                   image.pixels[source + tx],
                   image.pixels[source + tx++] >>> 24);
        }
        source += image.width;
        target += width;
      }
    } else if (image.format == ALPHA) {
      for (int y = sy1; y < sy2; y++) {
        int tx = 0;

        for (int x = sx1; x < sx2; x++) {
          pixels[target + x] =
            _blend(pixels[target + x],
                   fillColor,
                   image.pixels[source + tx++]);
        }
        source += image.width;
        target += width;
      }

    } else if (image.format == RGB) {
      target += sx1;
      int tw = sx2 - sx1;
      for (int y = sy1; y < sy2; y++) {
        System.arraycopy(image.pixels, source, pixels, target, tw);
        // should set z coordinate in here
        // or maybe not, since dims=0, meaning no relevant z
        source += image.width;
        target += width;
      }
    }
  }


  //////////////////////////////////////////////////////////////

  // TEXT/FONTS


  // These will be handled entirely by PGraphics.



  //////////////////////////////////////////////////////////////


  // expects properly clipped coords, hence does
    // NOT check if x/y are in bounds [toxi]
    private void thin_pointAt(int x, int y, float z, int color) {
      int index = y*width+x; // offset values are pre-calced in constructor
      pixels[index] = color;
      zbuffer[index] = z;
    }

    // expects offset/index in pixelbuffer array instead of x/y coords
    // used by optimized parts of thin_flat_line() [toxi]
    private void thin_pointAtIndex(int offset, float z, int color) {
      pixels[offset] = color;
      zbuffer[offset] = z;
    }

    // points are inherently flat, but always tangent
    // to the screen surface. the z is only so that things
    // get scaled properly if the pt is way in back
    private void thick_point(float x, float y, float z, // note floats
                             float r, float g, float b, float a) {
      spolygon.reset(4);
      spolygon.interpARGB = false;  // no changes for vertices of a point

      float strokeWidth2 = strokeWeight/2.0f;

      float svertex[] = spolygon.vertices[0];
      svertex[X] = x - strokeWidth2;
      svertex[Y] = y - strokeWidth2;
      svertex[Z] = z;

      svertex[R] = r;
      svertex[G] = g;
      svertex[B] = b;
      svertex[A] = a;

      svertex = spolygon.vertices[1];
      svertex[X] = x + strokeWidth2;
      svertex[Y] = y - strokeWidth2;
      svertex[Z] = z;

      svertex = spolygon.vertices[2];
      svertex[X] = x + strokeWidth2;
      svertex[Y] = y + strokeWidth2;
      svertex[Z] = z;

      svertex = spolygon.vertices[3];
      svertex[X] = x - strokeWidth2;
      svertex[Y] = y + strokeWidth2;
      svertex[Z] = z;

      spolygon.render();
    }


    // new bresenham clipping code, as old one was buggy [toxi]
    private void thin_flat_line(int x1, int y1, int x2, int y2) {
      int nx1,ny1,nx2,ny2;

      // get the "dips" for the points to clip
      int code1 = thin_flat_lineClipCode(x1, y1);
      int code2 = thin_flat_lineClipCode(x2, y2);

      if ((code1 & code2)!=0) {
        return;
      } else {
        int dip = code1 | code2;
        if (dip != 0) {
          // now calculate the clipped points
          float a1 = 0, a2 = 1, a = 0;
          for (int i=0;i<4;i++) {
            if (((dip>>i)%2)==1) {
              a = thin_flat_lineSlope((float)x1, (float)y1,
                                      (float)x2, (float)y2, i+1);
              if (((code1>>i)%2)==1) {
                a1 = (float)Math.max(a, a1);
              } else {
                a2 = (float)Math.min(a, a2);
              }
            }
          }
          if (a1>a2) return;
          else {
            nx1=(int) (x1+a1*(x2-x1));
            ny1=(int) (y1+a1*(y2-y1));
            nx2=(int) (x1+a2*(x2-x1));
            ny2=(int) (y1+a2*(y2-y1));
          }
          // line is fully visible/unclipped
        } else {
          nx1=x1; nx2=x2;
          ny1=y1; ny2=y2;
        }
      }

      // new "extremely fast" line code
      // adapted from http://www.edepot.com/linee.html

      boolean yLonger=false;
      int shortLen=ny2-ny1;
      int longLen=nx2-nx1;
      if (Math.abs(shortLen)>Math.abs(longLen)) {
        int swap=shortLen;
        shortLen=longLen;
        longLen=swap;
        yLonger=true;
      }
      int decInc;
      if (longLen==0) decInc=0;
      else decInc = (shortLen << 16) / longLen;

      if (nx1==nx2) {
        // special case: vertical line
        if (ny1>ny2) { int ty=ny1; ny1=ny2; ny2=ty; }
        int offset=ny1*width+nx1;
        for(int j=ny1; j<=ny2; j++) {
          thin_pointAtIndex(offset,0,strokeColor);
          offset+=width;
        }
        return;
      } else if (ny1==ny2) {
        // special case: horizontal line
        if (nx1>nx2) { int tx=nx1; nx1=nx2; nx2=tx; }
        int offset=ny1*width+nx1;
        for(int j=nx1; j<=nx2; j++) thin_pointAtIndex(offset++,0,strokeColor);
        return;
      } else if (yLonger) {
        if (longLen>0) {
          longLen+=ny1;
          for (int j=0x8000+(nx1<<16);ny1<=longLen;++ny1) {
            thin_pointAt(j>>16, ny1, 0, strokeColor);
            j+=decInc;
          }
          return;
        }
        longLen+=ny1;
        for (int j=0x8000+(nx1<<16);ny1>=longLen;--ny1) {
          thin_pointAt(j>>16, ny1, 0, strokeColor);
          j-=decInc;
        }
        return;
      } else if (longLen>0) {
        longLen+=nx1;
        for (int j=0x8000+(ny1<<16);nx1<=longLen;++nx1) {
          thin_pointAt(nx1, j>>16, 0, strokeColor);
          j+=decInc;
        }
        return;
      }
      longLen+=nx1;
      for (int j=0x8000+(ny1<<16);nx1>=longLen;--nx1) {
        thin_pointAt(nx1, j>>16, 0, strokeColor);
        j-=decInc;
      }
    }

    private int thin_flat_lineClipCode(float x, float y) {
      return ((y < 0 ? 8 : 0) | (y > height1 ? 4 : 0) |
              (x < 0 ? 2 : 0) | (x > width1 ? 1 : 0));
    }

    private float thin_flat_lineSlope(float x1, float y1,
                                      float x2, float y2, int border) {
      switch (border) {
      case 4: {
        return (-y1)/(y2-y1);
      }
      case 3: {
        return (height1-y1)/(y2-y1);
      }
      case 2: {
        return (-x1)/(x2-x1);
      }
      case 1: {
        return (width1-x1)/(x2-x1);
      }
      }
      return -1f;
    }


    private boolean flat_line_retribution(float x1, float y1,
                                          float x2, float y2,
                                          float r1, float g1, float b1) {
      /*
      // assume that if it is/isn't big in one dir, then the
      // other doesn't matter, cuz that's a weird case
      float lwidth  = m00*strokeWeight + m01*strokeWeight;
      //float lheight = m10*strokeWeight + m11*strokeWeight;
      // lines of stroke thickness 1 can be anywhere from -1.41 to 1.41
      if ((strokeWeight < TWO) && (!hints[SCALE_STROKE_WIDTH])) {
        //if (abs(lwidth) < 1.5f) {
        //System.out.println("flat line retribution " + r1 + " " + g1 + " " + b1);
        int strokeSaved = strokeColor;
        strokeColor = float_color(r1, g1, b1);
        thin_flat_line((int)x1, (int)y1, (int)x2, (int)y2);
        strokeColor = strokeSaved;
        return true;
      }
      */
      return false;
    }


    private void thick_flat_line(float ox1, float oy1,
                                 float r1, float g1, float b1, float a1,
                                 float ox2, float oy2,
                                 float r2, float g2, float b2, float a2) {
      spolygon.interpARGB = (r1 != r2) || (g1 != g2) || (b1 != b2) || (a1 != a2);
      spolygon.interpZ = false;

      if (!spolygon.interpARGB &&
          flat_line_retribution(ox1, oy1, ox2, oy2, r1, g1, b1)) {
        return;
      }

      float dX = ox2-ox1 + EPSILON;
      float dY = oy2-oy1 + EPSILON;
      float len = sqrt(dX*dX + dY*dY);

      // TODO strokeWidth should be transformed!
      float rh = strokeWeight / len;

      float dx0 = rh * dY;
      float dy0 = rh * dX;
      float dx1 = rh * dY;
      float dy1 = rh * dX;

      spolygon.reset(4);

      float svertex[] = spolygon.vertices[0];
      svertex[X] = ox1+dx0;
      svertex[Y] = oy1-dy0;
      svertex[R] = r1;
      svertex[G] = g1;
      svertex[B] = b1;
      svertex[A] = a1;

      svertex = spolygon.vertices[1];
      svertex[X] = ox1-dx0;
      svertex[Y] = oy1+dy0;
      svertex[R] = r1;
      svertex[G] = g1;
      svertex[B] = b1;
      svertex[A] = a1;

      svertex = spolygon.vertices[2];
      svertex[X] = ox2-dx1;
      svertex[Y] = oy2+dy1;
      svertex[R] = r2;
      svertex[G] = g2;
      svertex[B] = b2;
      svertex[A] = a2;

      svertex = spolygon.vertices[3];
      svertex[X] = ox2+dx1;
      svertex[Y] = oy2-dy1;
      svertex[R] = r2;
      svertex[G] = g2;
      svertex[B] = b2;
      svertex[A] = a2;

      spolygon.render();
    }


    /*
    // OPT version without z coords can save 8 multiplies and some other
    private void spatial_line(float x1, float y1,
                              float r1, float g1, float b1,
                              float x2, float y2,
                              float r2, float g2, float b2) {
      spatial_line(x1, y1, 0, r1, g1, b1,
                   x2, y2, 0, r2, g2, b2);
    }


    // the incoming values are transformed,
    // and the colors have been calculated

    private void spatial_line(float x1, float y1, float z1,
                              float r1, float g1, float b1,
                              float x2, float y2, float z2,
                              float r2, float g2, float b2) {
      spolygon.interpARGB = (r1 != r2) || (g1 != g2) || (b1 != b2);
      if (!spolygon.interpARGB &&
          flat_line_retribution(x1, y1, x2, y2, r1, g1, b1)) {
        return;
      }

      spolygon.interpZ = true;

      float ox1 = x1; float oy1 = y1; float oz1 = z1;
      float ox2 = x2; float oy2 = y2; float oz2 = z2;

      float dX = ox2-ox1 + 0.0001f;
      float dY = oy2-oy1 + 0.0001f;
      float len = sqrt(dX*dX + dY*dY);

      //float x0 = m00*0 + m01*0 + m03;

      float rh = strokeWeight / len;

      float dx0 = rh * dY;
      float dy0 = rh * dX;
      float dx1 = rh * dY;
      float dy1 = rh * dX;

      spolygon.reset(4);

      float svertex[] = spolygon.vertices[0];
      svertex[X] = ox1+dx0;
      svertex[Y] = oy1-dy0;
      svertex[Z] = oz1;
      svertex[R] = r1; //calcR1;
      svertex[G] = g1; //calcG1;
      svertex[B] = b1; //calcB1;

      svertex = spolygon.vertices[1];
      svertex[X] = ox1-dx0;
      svertex[Y] = oy1+dy0;
      svertex[Z] = oz1;
      svertex[R] = r1; //calcR1;
      svertex[G] = g1; //calcG1;
      svertex[B] = b1; //calcB1;

      svertex = spolygon.vertices[2];
      svertex[X] = ox2-dx1;
      svertex[Y] = oy2+dy1;
      svertex[Z] = oz2;
      svertex[R] = r2; //calcR2;
      svertex[G] = g2; //calcG2;
      svertex[B] = b2; //calcB2;

      svertex = spolygon.vertices[3];
      svertex[X] = ox2+dx1;
      svertex[Y] = oy2-dy1;
      svertex[Z] = oz2;
      svertex[R] = r2; //calcR2;
      svertex[G] = g2; //calcG2;
      svertex[B] = b2; //calcB2;

      spolygon.render();
    }
    */


    // max is what to count to
    // offset is offset to the 'next' vertex
    // increment is how much to increment in the loop
    private void draw_lines(float vertices[][], int max,
                            int offset, int increment, int skip) {

      if (strokeWeight < 2) {
        for (int i = 0; i < max; i += increment) {
          if ((skip != 0) && (((i+offset) % skip) == 0)) continue;

          float a[] = vertices[i];
          float b[] = vertices[i+offset];

          if (line == null) line = new PLine(this);

          line.reset();
          line.setIntensities(a[SR], a[SG], a[SB], a[SA],
                              b[SR], b[SG], b[SB], b[SA]);
          line.setVertices(a[X], a[Y], a[Z],
                           b[X], b[Y], b[Z]);
          line.draw();
        }

      } else {  // use old line code for thickness > 1

          if ((strokeWeight < 2) && !strokeChanged) {
            // need to set color at least once?

            // THIS PARTICULAR CASE SHOULD NO LONGER BE REACHABLE

            for (int i = 0; i < max; i += increment) {
              if ((skip != 0) && (((i+offset) % skip) == 0)) continue;
              thin_flat_line((int) vertices[i][X],
                             (int) vertices[i][Y],
                             (int) vertices[i+offset][X],
                             (int) vertices[i+offset][Y]);
            }
          } else {
            for (int i = 0; i < max; i += increment) {
              if ((skip != 0) && (((i+offset) % skip) == 0)) continue;
              float v1[] = vertices[i];
              float v2[] = vertices[i+offset];
              thick_flat_line(v1[X], v1[Y],  v1[SR], v1[SG], v1[SB], v1[SA],
                              v2[X], v2[Y],  v2[SR], v2[SG], v2[SB], v2[SA]);
            }
          }
      }
    }



    //////////////////////////////////////////////////////////////

    // UGLY RENDERING SHIT


    private void thin_point(int x, int y, float z, int color) {
      // necessary? [fry] yes! [toxi]
      if (x<0 || x>width1 || y<0 || y>height1) return;

      int index = y*width + x;
      if ((color & 0xff000000) == 0xff000000) {  // opaque
        pixels[index] = color;

      } else {  // transparent
        // couldn't seem to get this working correctly

        //pixels[index] = _blend(pixels[index],
        //                     color & 0xffffff, (color >> 24) & 0xff);

        // a1 is how much of the orig pixel
        int a2 = (color >> 24) & 0xff;
        int a1 = a2 ^ 0xff;

        int p2 = strokeColor;
        int p1 = pixels[index];

        int r = (a1 * ((p1 >> 16) & 0xff) + a2 * ((p2 >> 16) & 0xff)) & 0xff00;
        int g = (a1 * ((p1 >>  8) & 0xff) + a2 * ((p2 >>  8) & 0xff)) & 0xff00;
        int b = (a1 * ( p1        & 0xff) + a2 * ( p2        & 0xff)) >> 8;

        pixels[index] =  0xff000000 | (r << 8) | g | b;

        //pixels[index] = _blend(pixels[index],
        //                     color & 0xffffff, (color >> 24) & 0xff);
        /*
        pixels[index] = 0xff000000 |
          ((((a1 * ((pixels[index] >> 16) & 0xff) +
              a2 * ((color         >> 16) & 0xff)) & 0xff00) << 24) << 8) |
          (((a1 * ((pixels[index] >>  8) & 0xff) +
             a2 * ((color         >>  8) & 0xff)) & 0xff00) << 16) |
          (((a1 * ( pixels[index]        & 0xff) +
             a2 * ( color                & 0xff)) >> 8));
        */
      }
      zbuffer[index] = z;
    }



    //////////////////////////////////////////////////////////////

    // BACKGROUND AND FRIENDS


    /**
     * Clear the pixel buffer.
     */
    protected void clear() {
      for (int i = 0; i < pixelCount; i++) {
        pixels[i] = backgroundColor;
      }
    }



    //////////////////////////////////////////////////////////////

    // INTERNAL SCHIZZLE


    private boolean untransformed() {
      return ((m00 == 1) && (m01 == 0) && (m02 == 0) &&
              (m10 == 0) && (m11 == 1) && (m12 == 0));
    }


    private boolean unwarped() {
      return ((m00 == 1) && (m01 == 0) && (m10 == 0) && (m11 == 1));
    }



    // doesn't really do lighting per se...
    private void calc_lighting(float r, float g, float b,
                               float ix, float iy, float iz,
                               float nx, float ny, float nz,
                               float target[], int toffset) {
      target[toffset + 0] = r;
      target[toffset + 1] = g;
      target[toffset + 2] = b;
    }


    static private final int float_color(float r, float g, float b) {
      return (0xff000000 |
              ((int) (255.0f * r)) << 16 |
              ((int) (255.0f * g)) << 8 |
              ((int) (255.0f * b)));
    }

    public final static int _blend(int p1, int p2, int a2) {
      // scale alpha by alpha of incoming pixel
      a2 = (a2 * (p2 >>> 24)) >> 8;

      int a1 = a2 ^ 0xff;
      int r = (a1 * ((p1 >> 16) & 0xff) + a2 * ((p2 >> 16) & 0xff)) & 0xff00;
      int g = (a1 * ((p1 >>  8) & 0xff) + a2 * ((p2 >>  8) & 0xff)) & 0xff00;
      int b = (a1 * ( p1        & 0xff) + a2 * ( p2        & 0xff)) >> 8;

      return 0xff000000 | (r << 8) | g | b;
    }
}
