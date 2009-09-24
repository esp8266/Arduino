/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2006-08 Ben Fry and Casey Reas

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
import java.util.Arrays;


/**
 * Subclass of PGraphics that handles fast 2D rendering using a
 * MemoryImageSource. The renderer found in this class is not as accurate as
 * PGraphicsJava2D, but offers certain speed tradeoffs, particular when
 * messing with the pixels array, or displaying image or video data.
 */
public class PGraphics2D extends PGraphics {

  PMatrix2D ctm = new PMatrix2D();

  //PPolygon polygon;     // general polygon to use for shape
  PPolygon fpolygon;    // used to fill polys for tri or quad strips
  PPolygon spolygon;    // stroke/line polygon
  float svertices[][];  // temp vertices used for stroking end of poly

  PPolygon tpolygon;
  int[] vertexOrder;

  PLine line;

  float[][] matrixStack = new float[MATRIX_STACK_DEPTH][6];
  int matrixStackDepth;

  DirectColorModel cm;
  MemoryImageSource mis;


  //////////////////////////////////////////////////////////////


  public PGraphics2D() { }


  //public void setParent(PApplet parent)


  //public void setPrimary(boolean primary)


  //public void setPath(String path)


  //public void setSize(int iwidth, int iheight)


  protected void allocate() {
    pixelCount = width * height;
    pixels = new int[pixelCount];

    if (primarySurface) {
      cm = new DirectColorModel(32, 0x00ff0000, 0x0000ff00, 0x000000ff);;
      mis = new MemoryImageSource(width, height, pixels, 0, width);
      mis.setFullBufferUpdates(true);
      mis.setAnimated(true);
      image = Toolkit.getDefaultToolkit().createImage(mis);
    }
  }


  //public void dispose()



  //////////////////////////////////////////////////////////////


  public boolean canDraw() {
    return true;
  }


  public void beginDraw() {
    // need to call defaults(), but can only be done when it's ok to draw
    // (i.e. for OpenGL, no drawing can be done outside beginDraw/endDraw).
    if (!settingsInited) {
      defaultSettings();

//      polygon  = new PPolygon(this);
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
    if (mis != null) {
      mis.newPixels(pixels, cm, 0, width);
    }
    // mark pixels as having been updated, so that they'll work properly
    // when this PGraphics is drawn using image().
    updatePixels();
  }


  // public void flush()



  //////////////////////////////////////////////////////////////


  //protected void checkSettings()


  //protected void defaultSettings()


  //protected void reapplySettings()



  //////////////////////////////////////////////////////////////


  //public void hint(int which)



  //////////////////////////////////////////////////////////////


  //public void beginShape()


  public void beginShape(int kind) {
    shape = kind;
    vertexCount = 0;
    curveVertexCount = 0;

//    polygon.reset(0);
    fpolygon.reset(4);
    spolygon.reset(4);

    textureImage = null;
//    polygon.interpUV = false;
  }


  //public void edge(boolean e)


  //public void normal(float nx, float ny, float nz)


  //public void textureMode(int mode)


  //public void texture(PImage image)


  /*
  public void vertex(float x, float y) {
    if (shape == POINTS) {
      point(x, y);
    } else {
      super.vertex(x, y);
    }
  }
  */


  public void vertex(float x, float y, float z) {
    showDepthWarningXYZ("vertex");
  }


  //public void vertex(float x, float y, float u, float v)


  public void vertex(float x, float y, float z, float u, float v) {
    showDepthWarningXYZ("vertex");
  }


  //protected void vertexTexture(float u, float v);


  public void breakShape() {
    showWarning("This renderer cannot handle concave shapes " +
                "or shapes with holes.");
  }


  //public void endShape()


  public void endShape(int mode) {
    if (ctm.isIdentity()) {
      for (int i = 0; i < vertexCount; i++) {
        vertices[i][TX] = vertices[i][X];
        vertices[i][TY] = vertices[i][Y];
      }
    } else {
      for (int i = 0; i < vertexCount; i++) {
        vertices[i][TX] = ctm.multX(vertices[i][X], vertices[i][Y]);
        vertices[i][TY] = ctm.multY(vertices[i][X], vertices[i][Y]);
      }
    }

    // ------------------------------------------------------------------
    // TEXTURES

    fpolygon.texture(textureImage);

    // ------------------------------------------------------------------
    // COLORS
    // calculate RGB for each vertex

    spolygon.interpARGB = true; //strokeChanged; //false;
    fpolygon.interpARGB = true; //fillChanged; //false;

    // all the values for r, g, b have been set with calls to vertex()
    // (no need to re-calculate anything here)

    // ------------------------------------------------------------------
    // RENDER SHAPES

    int increment;

    switch (shape) {
    case POINTS:
      // stroke cannot change inside beginShape(POINTS);
      if (stroke) {
        if ((ctm.m00 == ctm.m11) && (strokeWeight == 1)) {
          for (int i = 0; i < vertexCount; i++) {
            thin_point(vertices[i][TX], vertices[i][TY], strokeColor);
          }
        } else {
          for (int i = 0; i < vertexCount; i++) {
            float[] v = vertices[i];
            thick_point(v[TX], v[TY], v[TZ],  v[SR], v[SG], v[SB], v[SA]);
          }
        }
      }
      break;

    case LINES:
      if (stroke) {
        // increment by two for individual lines
        increment = (shape == LINES) ? 2 : 1;
        draw_lines(vertices, vertexCount-1, 1, increment, 0);
      }
      break;

    case TRIANGLE_FAN:
      // do fill and stroke separately because otherwise
      // the lines will be stroked more than necessary
      if (fill || textureImage != null) {
        fpolygon.vertexCount = 3;

        for (int i = 1; i < vertexCount-1; i++) {
//          System.out.println(i + " of " + vertexCount);

          fpolygon.vertices[2][R] = vertices[0][R];
          fpolygon.vertices[2][G] = vertices[0][G];
          fpolygon.vertices[2][B] = vertices[0][B];
          fpolygon.vertices[2][A] = vertices[0][A];

          fpolygon.vertices[2][TX] = vertices[0][TX];
          fpolygon.vertices[2][TY] = vertices[0][TY];

          if (textureImage != null) {
            fpolygon.vertices[2][U] = vertices[0][U];
            fpolygon.vertices[2][V] = vertices[0][V];
          }
//          System.out.println(fpolygon.vertices[2][TX] + " " + fpolygon.vertices[2][TY]);

          for (int j = 0; j < 2; j++) {
            fpolygon.vertices[j][R] = vertices[i+j][R];
            fpolygon.vertices[j][G] = vertices[i+j][G];
            fpolygon.vertices[j][B] = vertices[i+j][B];
            fpolygon.vertices[j][A] = vertices[i+j][A];

            fpolygon.vertices[j][TX] = vertices[i+j][TX];
            fpolygon.vertices[j][TY] = vertices[i+j][TY];

//            System.out.println(fpolygon.vertices[j][TX] + " " + fpolygon.vertices[j][TY]);

            if (textureImage != null) {
              fpolygon.vertices[j][U] = vertices[i+j][U];
              fpolygon.vertices[j][V] = vertices[i+j][V];
            }
          }
//          System.out.println();
          fpolygon.render();
        }
      }
      if (stroke) {
        // draw internal lines
        for (int i = 1; i < vertexCount; i++) {
          draw_line(vertices[0], vertices[i]);
        }
        // draw a ring around the outside
        for (int i = 1; i < vertexCount-1; i++) {
          draw_line(vertices[i], vertices[i+1]);
        }
        // close the shape
        draw_line(vertices[vertexCount-1], vertices[1]);
      }
      break;

    case TRIANGLES:
    case TRIANGLE_STRIP:
      increment = (shape == TRIANGLES) ? 3 : 1;
      // do fill and stroke separately because otherwise
      // the lines will be stroked more than necessary
      if (fill || textureImage != null) {
        fpolygon.vertexCount = 3;
        for (int i = 0; i < vertexCount-2; i += increment) {
          for (int j = 0; j < 3; j++) {
            fpolygon.vertices[j][R] = vertices[i+j][R];
            fpolygon.vertices[j][G] = vertices[i+j][G];
            fpolygon.vertices[j][B] = vertices[i+j][B];
            fpolygon.vertices[j][A] = vertices[i+j][A];

            fpolygon.vertices[j][TX] = vertices[i+j][TX];
            fpolygon.vertices[j][TY] = vertices[i+j][TY];
            fpolygon.vertices[j][TZ] = vertices[i+j][TZ];

            if (textureImage != null) {
              fpolygon.vertices[j][U] = vertices[i+j][U];
              fpolygon.vertices[j][V] = vertices[i+j][V];
            }
          }
          fpolygon.render();
        }
      }
      if (stroke) {
        // first draw all vertices as a line strip
        if (shape == TRIANGLE_STRIP) {
          draw_lines(vertices, vertexCount-1, 1, 1, 0);
        } else {
          draw_lines(vertices, vertexCount-1, 1, 1, 3);
        }
        // then draw from vertex (n) to (n+2)
        // incrementing n using the same as above
        draw_lines(vertices, vertexCount-2, 2, increment, 0);
        // changed this to vertexCount-2, because it seemed
        // to be adding an extra (nonexistant) line
      }
      break;

    case QUADS:
      if (fill || textureImage != null) {
        fpolygon.vertexCount = 4;
        for (int i = 0; i < vertexCount-3; i += 4) {
          for (int j = 0; j < 4; j++) {
            int jj = i+j;
            fpolygon.vertices[j][R] = vertices[jj][R];
            fpolygon.vertices[j][G] = vertices[jj][G];
            fpolygon.vertices[j][B] = vertices[jj][B];
            fpolygon.vertices[j][A] = vertices[jj][A];

            fpolygon.vertices[j][TX] = vertices[jj][TX];
            fpolygon.vertices[j][TY] = vertices[jj][TY];
            fpolygon.vertices[j][TZ] = vertices[jj][TZ];

            if (textureImage != null) {
              fpolygon.vertices[j][U] = vertices[jj][U];
              fpolygon.vertices[j][V] = vertices[jj][V];
            }
          }
          fpolygon.render();
        }
      }
      if (stroke) {
        for (int i = 0; i < vertexCount-3; i += 4) {
          draw_line(vertices[i+0], vertices[i+1]);
          draw_line(vertices[i+1], vertices[i+2]);
          draw_line(vertices[i+2], vertices[i+3]);
          draw_line(vertices[i+3], vertices[i+0]);
        }
      }
      break;

    case QUAD_STRIP:
      if (fill || textureImage != null) {
        fpolygon.vertexCount = 4;
        for (int i = 0; i < vertexCount-3; i += 2) {
          for (int j = 0; j < 4; j++) {
            int jj = i+j;
            if (j == 2) jj = i+3;  // swap 2nd and 3rd vertex
            if (j == 3) jj = i+2;

            fpolygon.vertices[j][R] = vertices[jj][R];
            fpolygon.vertices[j][G] = vertices[jj][G];
            fpolygon.vertices[j][B] = vertices[jj][B];
            fpolygon.vertices[j][A] = vertices[jj][A];

            fpolygon.vertices[j][TX] = vertices[jj][TX];
            fpolygon.vertices[j][TY] = vertices[jj][TY];
            fpolygon.vertices[j][TZ] = vertices[jj][TZ];

            if (textureImage != null) {
              fpolygon.vertices[j][U] = vertices[jj][U];
              fpolygon.vertices[j][V] = vertices[jj][V];
            }
          }
          fpolygon.render();
        }
      }
      if (stroke) {
        draw_lines(vertices, vertexCount-1, 1, 2, 0);  // inner lines
        draw_lines(vertices, vertexCount-2, 2, 1, 0);  // outer lines
      }
      break;

    case POLYGON:
      if (isConvex()) {
        if (fill || textureImage != null) {
          //System.out.println("convex");
          fpolygon.renderPolygon(vertices, vertexCount);
          //if (stroke) polygon.unexpand();
        }

        if (stroke) {
          draw_lines(vertices, vertexCount-1, 1, 1, 0);
          if (mode == CLOSE) {
            // draw the last line connecting back to the first point in poly
            //svertices[0] = vertices[vertexCount-1];
            //svertices[1] = vertices[0];
            //draw_lines(svertices, 1, 1, 1, 0);
            draw_line(vertices[vertexCount-1], vertices[0]);
          }
        }
      } else {  // not convex
        //System.out.println("concave");
        if (fill || textureImage != null) {
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
          draw_lines(vertices, vertexCount-1, 1, 1, 0);
          if (mode == CLOSE) {
            // draw the last line connecting back
            // to the first point in poly
//            svertices[0] = vertices[vertexCount-1];
//            svertices[1] = vertices[0];
//            draw_lines(svertices, 1, 1, 1, 0);
            draw_line(vertices[vertexCount-1], vertices[0]);
          }
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
    //float v[][] = polygon.vertices;
    //int n = polygon.vertexCount;
    //int j,k;
    //float tol = 0.001f;

    if (vertexCount < 3) {
      // ERROR: this is a line or a point, render as convex
      return true;
    }

    int flag = 0;
    // iterate along border doing dot product.
    // if the sign of the result changes, then is concave
    for (int i = 0; i < vertexCount; i++) {
      float[] vi = vertices[i];
      float[] vj = vertices[(i + 1) % vertexCount];
      float[] vk = vertices[(i + 2) % vertexCount];
      float calc = ((vj[TX] - vi[TX]) * (vk[TY] - vj[TY]) -
                    (vj[TY] - vi[TY]) * (vk[TX] - vj[TX]));
      if (calc < 0) {
        flag |= 1;
      } else if (calc > 0) {
        flag |= 2;
      }
      if (flag == 3) {
        return false;  // CONCAVE
      }
    }
    if (flag != 0) {
      return true;    // CONVEX
    } else {
      // ERROR: colinear points, self intersection
      // treat as CONVEX
      return true;
    }
  }


  /**
   * Triangulate the current polygon.
   * <BR> <BR>
   * Simple ear clipping polygon triangulation adapted from code by
   * John W. Ratcliff (jratcliff at verant.com). Presumably
   * <A HREF="http://www.flipcode.org/cgi-bin/fcarticles.cgi?show=63943">this</A>
   * bit of code from the web.
   */
  protected void concaveRender() {
    if (vertexOrder == null || vertexOrder.length != vertices.length) {
      vertexOrder = new int[vertices.length];
//      int[] temp = new int[vertices.length];
//      // since vertex_start may not be zero, might need to keep old stuff around
//      PApplet.arrayCopy(vertexOrder, temp, vertexCount);
//      vertexOrder = temp;
    }

    if (tpolygon == null) {
      tpolygon = new PPolygon(this);
    }
    tpolygon.reset(3);

    // first we check if the polygon goes clockwise or counterclockwise
    float area = 0;
    for (int p = vertexCount - 1, q = 0; q < vertexCount; p = q++) {
      area += (vertices[q][X] * vertices[p][Y] -
               vertices[p][X] * vertices[q][Y]);
    }
    // ain't nuthin there
    if (area == 0) return;

    // don't allow polygons to come back and meet themselves,
    // otherwise it will anger the triangulator
    // http://dev.processing.org/bugs/show_bug.cgi?id=97
    float vfirst[] = vertices[0];
    float vlast[] = vertices[vertexCount-1];
    if ((Math.abs(vfirst[X] - vlast[X]) < EPSILON) &&
        (Math.abs(vfirst[Y] - vlast[Y]) < EPSILON) &&
        (Math.abs(vfirst[Z] - vlast[Z]) < EPSILON)) {
      vertexCount--;
    }

    // then sort the vertices so they are always in a counterclockwise order
    for (int i = 0; i < vertexCount; i++) {
      vertexOrder[i] = (area > 0) ? i : (vertexCount-1 - i);
    }

    // remove vc-2 Vertices, creating 1 triangle every time
    int vc = vertexCount;  // vc will be decremented while working
    int count = 2*vc;  // complex polygon detection

    for (int m = 0, v = vc - 1; vc > 2; ) {
      boolean snip = true;

      // if we start over again, is a complex polygon
      if (0 >= (count--)) {
        break; // triangulation failed
      }

      // get 3 consecutive vertices <u,v,w>
      int u = v ; if (vc <= u) u = 0;    // previous
      v = u + 1; if (vc <= v) v = 0;     // current
      int w = v + 1; if (vc <= w) w = 0; // next

      // Upgrade values to doubles, and multiply by 10 so that we can have
      // some better accuracy as we tessellate. This seems to have negligible
      // speed differences on Windows and Intel Macs, but causes a 50% speed
      // drop for PPC Macs with the bug's example code that draws ~200 points
      // in a concave polygon. Apple has abandoned PPC so we may as well too.
      // http://dev.processing.org/bugs/show_bug.cgi?id=774

      // triangle A B C
      double Ax = -10 * vertices[vertexOrder[u]][X];
      double Ay =  10 * vertices[vertexOrder[u]][Y];
      double Bx = -10 * vertices[vertexOrder[v]][X];
      double By =  10 * vertices[vertexOrder[v]][Y];
      double Cx = -10 * vertices[vertexOrder[w]][X];
      double Cy =  10 * vertices[vertexOrder[w]][Y];

      // first we check if <u,v,w> continues going ccw
      if (EPSILON > (((Bx-Ax) * (Cy-Ay)) - ((By-Ay) * (Cx-Ax)))) {
        continue;
      }

      for (int p = 0; p < vc; p++) {
        if ((p == u) || (p == v) || (p == w)) {
          continue;
        }

        double Px = -10 * vertices[vertexOrder[p]][X];
        double Py =  10 * vertices[vertexOrder[p]][Y];

        double ax  = Cx - Bx;  double ay  = Cy - By;
        double bx  = Ax - Cx;  double by  = Ay - Cy;
        double cx  = Bx - Ax;  double cy  = By - Ay;
        double apx = Px - Ax;  double apy = Py - Ay;
        double bpx = Px - Bx;  double bpy = Py - By;
        double cpx = Px - Cx;  double cpy = Py - Cy;

        double aCROSSbp = ax * bpy - ay * bpx;
        double cCROSSap = cx * apy - cy * apx;
        double bCROSScp = bx * cpy - by * cpx;

        if ((aCROSSbp >= 0.0) && (bCROSScp >= 0.0) && (cCROSSap >= 0.0)) {
          snip = false;
        }
      }

      if (snip) {
        tpolygon.renderTriangle(vertices[vertexOrder[u]],
                                vertices[vertexOrder[v]],
                                vertices[vertexOrder[w]]);
        m++;

        // remove v from remaining polygon
        for (int s = v, t = v + 1; t < vc; s++, t++) {
          vertexOrder[s] = vertexOrder[t];
        }
        vc--;

        // reset error detection counter
        count = 2 * vc;
      }
    }
  }


  /*
  // triangulate the current polygon
  private void concaveRender() {
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
    //float min[] = new float[2];
    float minX = polyVertices[0][TX];
    float minY = polyVertices[0][TY];
    mm = 0;

    for(int i = 0; i < n; i++ ) {
      if ((polyVertices[i][TY] < minY) ||
          ((polyVertices[i][TY] == minY) && (polyVertices[i][TX] > minX) )
      ) {
        mm = i;
        minX = polyVertices[mm][TX];
        minY = polyVertices[mm][TY];
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

      Ax =  -polyVertices[tpolygon_vertex_order[u]][TX];
      Ay =   polyVertices[tpolygon_vertex_order[u]][TY];
      Bx =  -polyVertices[tpolygon_vertex_order[v]][TX];
      By =   polyVertices[tpolygon_vertex_order[v]][TY];
      Cx =  -polyVertices[tpolygon_vertex_order[w]][TX];
      Cy =   polyVertices[tpolygon_vertex_order[w]][TY];

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

        Px = -polyVertices[tpolygon_vertex_order[p]][TX];
        Py =  polyVertices[tpolygon_vertex_order[p]][TY];

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
  */



  //////////////////////////////////////////////////////////////

  // BEZIER VERTICES


  //public void bezierVertex(float x2, float y2,
  //                         float x3, float y3,
  //                         float x4, float y4)


  //public void bezierVertex(float x2, float y2, float z2,
  //                         float x3, float y3, float z3,
  //                         float x4, float y4, float z4)



  //////////////////////////////////////////////////////////////

  // CURVE VERTICES


  //public void curveVertex(float x, float y)


  //public void curveVertex(float x, float y, float z)



  //////////////////////////////////////////////////////////////

  // FLUSH


  //public void flush()



  //////////////////////////////////////////////////////////////

  // PRIMITIVES


  //public void point(float x, float y)


  public void point(float x, float y, float z) {
    showDepthWarningXYZ("point");
  }


  //public void line(float x1, float y1, float x2, float y2)


  //public void line(float x1, float y1, float z1,
  //                 float x2, float y2, float z2)


  //public void triangle(float x1, float y1,
  //                     float x2, float y2,
  //                     float x3, float y3)


  //public void quad(float x1, float y1, float x2, float y2,
  //                 float x3, float y3, float x4, float y4)



  //////////////////////////////////////////////////////////////

  // RECT


  protected void rectImpl(float x1f, float y1f, float x2f, float y2f) {
    if (smooth || strokeAlpha || ctm.isWarped()) {
      // screw the efficiency, send this off to beginShape().
      super.rectImpl(x1f, y1f, x2f, y2f);

    } else {
      int x1 = (int) (x1f + ctm.m02);
      int y1 = (int) (y1f + ctm.m12);
      int x2 = (int) (x2f + ctm.m02);
      int y2 = (int) (y2f + ctm.m12);

      if (fill) {
        simple_rect_fill(x1, y1, x2, y2);
      }

      if (stroke) {
        if (strokeWeight == 1) {
          thin_flat_line(x1, y1, x2, y1);
          thin_flat_line(x2, y1, x2, y2);
          thin_flat_line(x2, y2, x1, y2);
          thin_flat_line(x1, y2, x1, y1);

        } else {
          thick_flat_line(x1, y1, strokeR, strokeG, strokeB, strokeA,
                          x2, y1, strokeR, strokeG, strokeB, strokeA);
          thick_flat_line(x2, y1, strokeR, strokeG, strokeB, strokeA,
                          x2, y2, strokeR, strokeG, strokeB, strokeA);
          thick_flat_line(x2, y2, strokeR, strokeG, strokeB, strokeA,
                          x1, y2, strokeR, strokeG, strokeB, strokeA);
          thick_flat_line(x1, y2, strokeR, strokeG, strokeB, strokeA,
                          x1, y1, strokeR, strokeG, strokeB, strokeA);
        }
      }
    }
  }


  /**
   * Draw a rectangle that hasn't been warped by the CTM (though it may be
   * translated). Just fill a bunch of pixels, or blend them if there's alpha.
   */
  private void simple_rect_fill(int x1, int y1, int x2, int y2) {
    if (y2 < y1) {
      int temp = y1; y1 = y2; y2 = temp;
    }
    if (x2 < x1) {
      int temp = x1; x1 = x2; x2 = temp;
    }
    // check to see if completely off-screen (e.g. if the left edge of the
    // rectangle is bigger than the width, and so on.)
    if ((x1 > width1) || (x2 < 0) ||
        (y1 > height1) || (y2 < 0)) return;

    // these only affect the fill, not the stroke
    // (otherwise strange boogers at edges b/c frame changes shape)
    if (x1 < 0) x1 = 0;
    if (x2 > width) x2 = width;
    if (y1 < 0) y1 = 0;
    if (y2 > height) y2 = height;

    int ww = x2 - x1;

    if (fillAlpha) {
      for (int y = y1; y < y2; y++) {
        int index = y*width + x1;
        for (int x = 0; x < ww; x++) {
          pixels[index] = blend_fill(pixels[index]);
          index++;
        }
      }

    } else {
      // on avg. 20-25% faster fill routine using System.arraycopy() [toxi 031223]
      // removed temporary row[] array for (hopefully) better performance [fry 081117]
      int hh = y2 - y1;
      //    int[] row = new int[ww];
      //    for (int i = 0; i < ww; i++) row[i] = fillColor;
      int index = y1 * width + x1;
      int rowIndex = index;
      for (int i = 0; i < ww; i++) {
        pixels[index + i] = fillColor;
      }
      for (int y = 0; y < hh; y++) {
        //      System.arraycopy(row, 0, pixels, idx, ww);
        System.arraycopy(pixels, rowIndex, pixels, index, ww);
        index += width;
      }
      //    row = null;
    }
  }



  //////////////////////////////////////////////////////////////

  // ELLIPSE AND ARC


  protected void ellipseImpl(float x, float y, float w, float h) {
    if (smooth || (strokeWeight != 1) ||
        fillAlpha || strokeAlpha || ctm.isWarped()) {
      // identical to PGraphics version, but uses POLYGON
      // for the fill instead of a TRIANGLE_FAN
      float radiusH = w / 2;
      float radiusV = h / 2;

      float centerX = x + radiusH;
      float centerY = y + radiusV;

      float sx1 = screenX(x, y);
      float sy1 = screenY(x, y);
      float sx2 = screenX(x+w, y+h);
      float sy2 = screenY(x+w, y+h);
      int accuracy = (int) (TWO_PI * PApplet.dist(sx1, sy1, sx2, sy2) / 8);
      if (accuracy < 4) return;  // don't bother?
      //System.out.println("diameter " + w + " " + h + " -> " + accuracy);

      float inc = (float)SINCOS_LENGTH / accuracy;

      float val = 0;

      if (fill) {
        boolean savedStroke = stroke;
        stroke = false;

        beginShape();
        for (int i = 0; i < accuracy; i++) {
          vertex(centerX + cosLUT[(int) val] * radiusH,
                 centerY + sinLUT[(int) val] * radiusV);
          val += inc;
        }
        endShape(CLOSE);

        stroke = savedStroke;
      }

      if (stroke) {
        boolean savedFill = fill;
        fill = false;

        val = 0;
        beginShape();
        for (int i = 0; i < accuracy; i++) {
          vertex(centerX + cosLUT[(int) val] * radiusH,
                 centerY + sinLUT[(int) val] * radiusV);
          val += inc;
        }
        endShape(CLOSE);

        fill = savedFill;
      }
    } else {
      float hradius = w / 2f;
      float vradius = h / 2f;

      int centerX = (int) (x + hradius + ctm.m02);
      int centerY = (int) (y + vradius + ctm.m12);

      int hradiusi = (int) hradius;
      int vradiusi = (int) vradius;

      if (hradiusi == vradiusi) {
        if (fill) flat_circle_fill(centerX, centerY, hradiusi);
        if (stroke) flat_circle_stroke(centerX, centerY, hradiusi);

      } else {
        if (fill) flat_ellipse_internal(centerX, centerY, hradiusi, vradiusi, true);
        if (stroke) flat_ellipse_internal(centerX, centerY, hradiusi, vradiusi, false);
      }
    }
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
      thin_point(xC + x, yC + y, strokeColor); // NNE
      thin_point(xC + y, yC - x, strokeColor); // ESE
      thin_point(xC - x, yC - y, strokeColor); // SSW
      thin_point(xC - y, yC + x, strokeColor); // WNW

      x++; E += u; u += 2;
      if (v < 2 * E) {
        y--; E -= v; v -= 2;
      }
      if (x > y) break;

      thin_point(xC + y, yC + x, strokeColor); // ENE
      thin_point(xC + x, yC - y, strokeColor); // SSE
      thin_point(xC - y, yC - x, strokeColor); // WSW
      thin_point(xC - x, yC + y, strokeColor); // NNW
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
        thin_point(xx, yc + y, fillColor);
      }
      for (int xx = xc; xx < xc + y; xx++) {  // ESE
        thin_point(xx, yc - x, fillColor);
      }
      for (int xx = xc - x; xx < xc; xx++) {  // SSW
        thin_point(xx, yc - y, fillColor);
      }
      for (int xx = xc - y; xx < xc; xx++) {  // WNW
        thin_point(xx, yc + x, fillColor);
      }

      x++; E += u; u += 2;
      if (v < 2 * E) {
        y--; E -= v; v -= 2;
      }
      if (x > y) break;

      for (int xx = xc; xx < xc + y; xx++) {  // ENE
        thin_point(xx, yc + x, fillColor);
      }
      for (int xx = xc; xx < xc + x; xx++) {  // SSE
        thin_point(xx, yc - y, fillColor);
      }
      for (int xx = xc - y; xx < xc; xx++) {  // WSW
        thin_point(xx, yc - x, fillColor);
      }
      for (int xx = xc - x; xx < xc; xx++) {  // NNW
        thin_point(xx, yc + y, fillColor);
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
        thin_point(i, centerY - ellipseY, fillColor);
        thin_point(i, centerY + ellipseY, fillColor);
      }
    } else {
      thin_point(centerX - ellipseX, centerY + ellipseY, strokeColor);
      thin_point(centerX + ellipseX, centerY + ellipseY, strokeColor);
      thin_point(centerX - ellipseX, centerY - ellipseY, strokeColor);
      thin_point(centerX + ellipseX, centerY - ellipseY, strokeColor);
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


  // TODO really need a decent arc function in here..

  protected void arcImpl(float x, float y, float w, float h,
                         float start, float stop) {
    float hr = w / 2f;
    float vr = h / 2f;

    float centerX = x + hr;
    float centerY = y + vr;

    if (fill) {
      // shut off stroke for a minute
      boolean savedStroke = stroke;
      stroke = false;

      int startLUT = (int) (-0.5f + (start / TWO_PI) * SINCOS_LENGTH);
      int stopLUT = (int) (0.5f + (stop / TWO_PI) * SINCOS_LENGTH);

      beginShape();
      vertex(centerX, centerY);
      for (int i = startLUT; i < stopLUT; i++) {
        int ii = i % SINCOS_LENGTH;
        // modulo won't make the value positive
        if (ii < 0) ii += SINCOS_LENGTH;
        vertex(centerX + cosLUT[ii] * hr,
               centerY + sinLUT[ii] * vr);
      }
      endShape(CLOSE);

      stroke = savedStroke;
    }

    if (stroke) {
      // Almost identical to above, but this uses a LINE_STRIP
      // and doesn't include the first (center) vertex.

      boolean savedFill = fill;
      fill = false;

      int startLUT = (int) (0.5f + (start / TWO_PI) * SINCOS_LENGTH);
      int stopLUT = (int) (0.5f + (stop / TWO_PI) * SINCOS_LENGTH);

      beginShape(); //LINE_STRIP);
      int increment = 1; // what's a good algorithm? stopLUT - startLUT;
      for (int i = startLUT; i < stopLUT; i += increment) {
        int ii = i % SINCOS_LENGTH;
        if (ii < 0) ii += SINCOS_LENGTH;
        vertex(centerX + cosLUT[ii] * hr,
               centerY + sinLUT[ii] * vr);
      }
      // draw last point explicitly for accuracy
      vertex(centerX + cosLUT[stopLUT % SINCOS_LENGTH] * hr,
             centerY + sinLUT[stopLUT % SINCOS_LENGTH] * vr);
      endShape();

      fill = savedFill;
    }
  }



  //////////////////////////////////////////////////////////////

  // BOX


  public void box(float size) {
    showDepthWarning("box");
  }

  public void box(float w, float h, float d) {
    showDepthWarning("box");
  }



  //////////////////////////////////////////////////////////////

  // SPHERE


  public void sphereDetail(int res) {
    showDepthWarning("sphereDetail");
  }

  public void sphereDetail(int ures, int vres) {
    showDepthWarning("sphereDetail");
  }

  public void sphere(float r) {
    showDepthWarning("sphere");
  }



  //////////////////////////////////////////////////////////////

  // BEZIER & CURVE


  public void bezier(float x1, float y1, float z1,
                     float x2, float y2, float z2,
                     float x3, float y3, float z3,
                     float x4, float y4, float z4) {
    showDepthWarningXYZ("bezier");
  }


  public void curve(float x1, float y1, float z1,
                    float x2, float y2, float z2,
                    float x3, float y3, float z3,
                    float x4, float y4, float z4) {
    showDepthWarningXYZ("curve");
  }



  //////////////////////////////////////////////////////////////

  // IMAGE


  protected void imageImpl(PImage image,
                           float x1, float y1, float x2, float y2,
                           int u1, int v1, int u2, int v2) {
    if ((x2 - x1 == image.width) &&
        (y2 - y1 == image.height) &&
        !tint && !ctm.isWarped()) {
      simple_image(image, (int) (x1 + ctm.m02), (int) (y1 + ctm.m12), u1, v1, u2, v2);

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
  private void simple_image(PImage image, int sx1, int sy1,
                            int ix1, int iy1, int ix2, int iy2) {
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
//            _blend(pixels[target + x],
//                   image.pixels[source + tx],
//                   image.pixels[source + tx++] >>> 24);
          blend_color(pixels[target + x],
                      image.pixels[source + tx++]);
        }
        source += image.width;
        target += width;
      }
    } else if (image.format == ALPHA) {
      for (int y = sy1; y < sy2; y++) {
        int tx = 0;

        for (int x = sx1; x < sx2; x++) {
          pixels[target + x] =
            blend_color_alpha(pixels[target + x],
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

  // UGLY RENDERING SHITE


  // expects properly clipped coords, hence does
    // NOT check if x/y are in bounds [toxi]
    private void thin_point_at(int x, int y, float z, int color) {
      int index = y*width+x; // offset values are pre-calced in constructor
      pixels[index] = color;
    }

    // expects offset/index in pixelbuffer array instead of x/y coords
    // used by optimized parts of thin_flat_line() [toxi]
    private void thin_point_at_index(int offset, float z, int color) {
      pixels[offset] = color;
    }


    private void thick_point(float x, float y, float z, // note floats
                             float r, float g, float b, float a) {
      spolygon.reset(4);
      spolygon.interpARGB = false;  // no changes for vertices of a point

      float strokeWidth2 = strokeWeight/2.0f;

      float svertex[] = spolygon.vertices[0];
      svertex[TX] = x - strokeWidth2;
      svertex[TY] = y - strokeWidth2;
      svertex[TZ] = z;

      svertex[R] = r;
      svertex[G] = g;
      svertex[B] = b;
      svertex[A] = a;

      svertex = spolygon.vertices[1];
      svertex[TX] = x + strokeWidth2;
      svertex[TY] = y - strokeWidth2;
      svertex[TZ] = z;

      svertex = spolygon.vertices[2];
      svertex[TX] = x + strokeWidth2;
      svertex[TY] = y + strokeWidth2;
      svertex[TZ] = z;

      svertex = spolygon.vertices[3];
      svertex[TX] = x - strokeWidth2;
      svertex[TY] = y + strokeWidth2;
      svertex[TZ] = z;

      spolygon.render();
    }


    // new bresenham clipping code, as old one was buggy [toxi]
    private void thin_flat_line(int x1, int y1, int x2, int y2) {
      int nx1,ny1,nx2,ny2;

      // get the "dips" for the points to clip
      int code1 = thin_flat_line_clip_code(x1, y1);
      int code2 = thin_flat_line_clip_code(x2, y2);

      if ((code1 & code2)!=0) {
        return;
      } else {
        int dip = code1 | code2;
        if (dip != 0) {
          // now calculate the clipped points
          float a1 = 0, a2 = 1, a = 0;
          for (int i=0;i<4;i++) {
            if (((dip>>i)%2)==1) {
              a = thin_flat_line_slope((float)x1, (float)y1,
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
          thin_point_at_index(offset,0,strokeColor);
          offset+=width;
        }
        return;
      } else if (ny1==ny2) {
        // special case: horizontal line
        if (nx1>nx2) { int tx=nx1; nx1=nx2; nx2=tx; }
        int offset=ny1*width+nx1;
        for(int j=nx1; j<=nx2; j++) thin_point_at_index(offset++,0,strokeColor);
        return;
      } else if (yLonger) {
        if (longLen>0) {
          longLen+=ny1;
          for (int j=0x8000+(nx1<<16);ny1<=longLen;++ny1) {
            thin_point_at(j>>16, ny1, 0, strokeColor);
            j+=decInc;
          }
          return;
        }
        longLen+=ny1;
        for (int j=0x8000+(nx1<<16);ny1>=longLen;--ny1) {
          thin_point_at(j>>16, ny1, 0, strokeColor);
          j-=decInc;
        }
        return;
      } else if (longLen>0) {
        longLen+=nx1;
        for (int j=0x8000+(ny1<<16);nx1<=longLen;++nx1) {
          thin_point_at(nx1, j>>16, 0, strokeColor);
          j+=decInc;
        }
        return;
      }
      longLen+=nx1;
      for (int j=0x8000+(ny1<<16);nx1>=longLen;--nx1) {
        thin_point_at(nx1, j>>16, 0, strokeColor);
        j-=decInc;
      }
    }


    private int thin_flat_line_clip_code(float x, float y) {
      return ((y < 0 ? 8 : 0) | (y > height1 ? 4 : 0) |
              (x < 0 ? 2 : 0) | (x > width1 ? 1 : 0));
    }


    private float thin_flat_line_slope(float x1, float y1,
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


    private void thick_flat_line(float ox1, float oy1,
                                 float r1, float g1, float b1, float a1,
                                 float ox2, float oy2,
                                 float r2, float g2, float b2, float a2) {
      spolygon.interpARGB = (r1 != r2) || (g1 != g2) || (b1 != b2) || (a1 != a2);
//      spolygon.interpZ = false;

      float dX = ox2-ox1 + EPSILON;
      float dY = oy2-oy1 + EPSILON;
      float len = (float) Math.sqrt(dX*dX + dY*dY);

      // TODO stroke width should be transformed!
      float rh = (strokeWeight / len) / 2;

      float dx0 = rh * dY;
      float dy0 = rh * dX;
      float dx1 = rh * dY;
      float dy1 = rh * dX;

      spolygon.reset(4);

      float svertex[] = spolygon.vertices[0];
      svertex[TX] = ox1+dx0;
      svertex[TY] = oy1-dy0;
      svertex[R] = r1;
      svertex[G] = g1;
      svertex[B] = b1;
      svertex[A] = a1;

      svertex = spolygon.vertices[1];
      svertex[TX] = ox1-dx0;
      svertex[TY] = oy1+dy0;
      svertex[R] = r1;
      svertex[G] = g1;
      svertex[B] = b1;
      svertex[A] = a1;

      svertex = spolygon.vertices[2];
      svertex[TX] = ox2-dx1;
      svertex[TY] = oy2+dy1;
      svertex[R] = r2;
      svertex[G] = g2;
      svertex[B] = b2;
      svertex[A] = a2;

      svertex = spolygon.vertices[3];
      svertex[TX] = ox2+dx1;
      svertex[TY] = oy2-dy1;
      svertex[R] = r2;
      svertex[G] = g2;
      svertex[B] = b2;
      svertex[A] = a2;

      spolygon.render();
    }


    private void draw_line(float[] v1, float[] v2) {
      if (strokeWeight == 1) {
        if (line == null) line = new PLine(this);

        line.reset();
        line.setIntensities(v1[SR], v1[SG], v1[SB], v1[SA],
                            v2[SR], v2[SG], v2[SB], v2[SA]);
        line.setVertices(v1[TX], v1[TY], v1[TZ],
                         v2[TX], v2[TY], v2[TZ]);
        line.draw();

      } else {  // use old line code for thickness != 1
        thick_flat_line(v1[TX], v1[TY],  v1[SR], v1[SG], v1[SB], v1[SA],
                        v2[TX], v2[TY],  v2[SR], v2[SG], v2[SB], v2[SA]);
      }
    }


    /**
     * @param max is what to count to
     * @param offset is offset to the 'next' vertex
     * @param increment is how much to increment in the loop
     */
    private void draw_lines(float vertices[][], int max,
                            int offset, int increment, int skip) {

      if (strokeWeight == 1) {
        for (int i = 0; i < max; i += increment) {
          if ((skip != 0) && (((i+offset) % skip) == 0)) continue;

          float a[] = vertices[i];
          float b[] = vertices[i+offset];

          if (line == null) line = new PLine(this);

          line.reset();
          line.setIntensities(a[SR], a[SG], a[SB], a[SA],
                              b[SR], b[SG], b[SB], b[SA]);
          line.setVertices(a[TX], a[TY], a[TZ],
                           b[TX], b[TY], b[TZ]);
          line.draw();
        }

      } else {  // use old line code for thickness != 1
        for (int i = 0; i < max; i += increment) {
          if ((skip != 0) && (((i+offset) % skip) == 0)) continue;

          float v1[] = vertices[i];
          float v2[] = vertices[i+offset];
          thick_flat_line(v1[TX], v1[TY],  v1[SR], v1[SG], v1[SB], v1[SA],
                          v2[TX], v2[TY],  v2[SR], v2[SG], v2[SB], v2[SA]);
        }
      }
    }


    private void thin_point(float fx, float fy, int color) {
      int x = (int) (fx + 0.4999f);
      int y = (int) (fy + 0.4999f);
      if (x < 0 || x > width1 || y < 0 || y > height1) return;

      int index = y*width + x;
      if ((color & 0xff000000) == 0xff000000) {  // opaque
        pixels[index] = color;

      } else {  // transparent
        // a1 is how much of the orig pixel
        int a2 = (color >> 24) & 0xff;
        int a1 = a2 ^ 0xff;

        int p2 = strokeColor;
        int p1 = pixels[index];

        int r = (a1 * ((p1 >> 16) & 0xff) + a2 * ((p2 >> 16) & 0xff)) & 0xff00;
        int g = (a1 * ((p1 >>  8) & 0xff) + a2 * ((p2 >>  8) & 0xff)) & 0xff00;
        int b = (a1 * ( p1        & 0xff) + a2 * ( p2        & 0xff)) >> 8;

        pixels[index] =  0xff000000 | (r << 8) | g | b;
      }
    }



    //////////////////////////////////////////////////////////////

    // MATRIX TRANSFORMATIONS


    public void translate(float tx, float ty) {
      ctm.translate(tx, ty);
    }


    public void translate(float tx, float ty, float tz) {
      showDepthWarningXYZ("translate");
    }


    public void rotate(float angle) {
      ctm.rotate(angle);
//      float c = (float) Math.cos(angle);
//      float s = (float) Math.sin(angle);
//      applyMatrix(c, -s, 0,  s, c, 0);
    }


    public void rotateX(float angle) {
      showDepthWarning("rotateX");
    }

    public void rotateY(float angle) {
      showDepthWarning("rotateY");
    }


    public void rotateZ(float angle) {
      showDepthWarning("rotateZ");
    }


    public void rotate(float angle, float vx, float vy, float vz) {
      showVariationWarning("rotate(angle, x, y, z)");
    }


    public void scale(float s) {
      ctm.scale(s);
//      applyMatrix(s, 0, 0,
//                  0, s, 0);
    }


    public void scale(float sx, float sy) {
      ctm.scale(sx, sy);
//      applyMatrix(sx, 0, 0,
//                  0, sy, 0);
    }


    public void scale(float x, float y, float z) {
      showDepthWarningXYZ("scale");
    }



    //////////////////////////////////////////////////////////////

    // TRANSFORMATION MATRIX


    public void pushMatrix() {
      if (matrixStackDepth == MATRIX_STACK_DEPTH) {
        throw new RuntimeException(ERROR_PUSHMATRIX_OVERFLOW);
      }
      ctm.get(matrixStack[matrixStackDepth]);
      matrixStackDepth++;
    }


    public void popMatrix() {
      if (matrixStackDepth == 0) {
        throw new RuntimeException(ERROR_PUSHMATRIX_UNDERFLOW);
      }
      matrixStackDepth--;
      ctm.set(matrixStack[matrixStackDepth]);
    }


    /**
     * Load identity as the transform/model matrix.
     * Same as glLoadIdentity().
     */
    public void resetMatrix() {
      ctm.reset();
//      m00 = 1; m01 = 0; m02 = 0;
//      m10 = 0; m11 = 1; m12 = 0;
    }


    /**
     * Apply a 3x2 affine transformation matrix.
     */
    public void applyMatrix(float n00, float n01, float n02,
                            float n10, float n11, float n12) {
      ctm.apply(n00, n01, n02,
                n10, n11, n12);
//
//      float r00 = m00*n00 + m01*n10;
//      float r01 = m00*n01 + m01*n11;
//      float r02 = m00*n02 + m01*n12 + m02;
//
//      float r10 = m10*n00 + m11*n10;
//      float r11 = m10*n01 + m11*n11;
//      float r12 = m10*n02 + m11*n12 + m12;
//
//      m00 = r00; m01 = r01; m02 = r02;
//      m10 = r10; m11 = r11; m12 = r12;
    }


    public void applyMatrix(float n00, float n01, float n02, float n03,
                            float n10, float n11, float n12, float n13,
                            float n20, float n21, float n22, float n23,
                            float n30, float n31, float n32, float n33) {
      showDepthWarningXYZ("applyMatrix");
    }


    /**
     * Loads the current matrix into m00, m01 etc (or modelview and
     * projection when using 3D) so that the values can be read.
     * <P/>
     * Note that there is no "updateMatrix" because that gets too
     * complicated (unnecessary) when considering the 3D matrices.
     */
//    public void loadMatrix() {
      // no-op on base PGraphics because they're used directly
//    }


    /**
     * Print the current model (or "transformation") matrix.
     */
    public void printMatrix() {
      ctm.print();

//      loadMatrix();  // just to make sure
//
//      float big = Math.abs(m00);
//      if (Math.abs(m01) > big) big = Math.abs(m01);
//      if (Math.abs(m02) > big) big = Math.abs(m02);
//      if (Math.abs(m10) > big) big = Math.abs(m10);
//      if (Math.abs(m11) > big) big = Math.abs(m11);
//      if (Math.abs(m12) > big) big = Math.abs(m12);
//
//      // avoid infinite loop
//      if (Float.isNaN(big) || Float.isInfinite(big)) {
//        big = 1000000; // set to something arbitrary
//      }
//
//      int d = 1;
//      int bigi = (int) big;
//      while ((bigi /= 10) != 0) d++;  // cheap log()
//
//      System.out.println(PApplet.nfs(m00, d, 4) + " " +
//                         PApplet.nfs(m01, d, 4) + " " +
//                         PApplet.nfs(m02, d, 4));
//
//      System.out.println(PApplet.nfs(m10, d, 4) + " " +
//                         PApplet.nfs(m11, d, 4) + " " +
//                         PApplet.nfs(m12, d, 4));
//
//      System.out.println();
    }



    //////////////////////////////////////////////////////////////

    // SCREEN TRANSFORMS


    public float screenX(float x, float y) {
      return ctm.m00 * x + ctm.m01 * y + ctm.m02;
    }


    public float screenY(float x, float y) {
      return ctm.m10 * x + ctm.m11 * y + ctm.m12;
    }



    //////////////////////////////////////////////////////////////

    // BACKGROUND AND FRIENDS


    /**
     * Clear the pixel buffer.
     */
    protected void backgroundImpl() {
      Arrays.fill(pixels, backgroundColor);
    }



    /*
    public void ambient(int rgb) {
      showDepthError("ambient");
    }

    public void ambient(float gray) {
      showDepthError("ambient");
    }

    public void ambient(float x, float y, float z) {
      // This doesn't take
      if ((x != PMaterial.DEFAULT_AMBIENT) ||
          (y != PMaterial.DEFAULT_AMBIENT) ||
          (z != PMaterial.DEFAULT_AMBIENT)) {
        showDepthError("ambient");
      }
    }

    public void specular(int rgb) {
      showDepthError("specular");
    }

    public void specular(float gray) {
      showDepthError("specular");
    }

    public void specular(float x, float y, float z) {
      showDepthError("specular");
    }

    public void shininess(float shine) {
      showDepthError("shininess");
    }


    public void emissive(int rgb) {
      showDepthError("emissive");
    }

    public void emissive(float gray) {
      showDepthError("emissive");
    }

    public void emissive(float x, float y, float z ) {
      showDepthError("emissive");
    }
    */



    //////////////////////////////////////////////////////////////

    // INTERNAL SCHIZZLE


    // TODO make this more efficient, or move into PMatrix2D
//    private boolean untransformed() {
//      return ((ctm.m00 == 1) && (ctm.m01 == 0) && (ctm.m02 == 0) &&
//              (ctm.m10 == 0) && (ctm.m11 == 1) && (ctm.m12 == 0));
//    }
//
//
//    // TODO make this more efficient, or move into PMatrix2D
//    private boolean unwarped() {
//      return ((ctm.m00 == 1) && (ctm.m01 == 0) &&
//              (ctm.m10 == 0) && (ctm.m11 == 1));
//    }


    // only call this if there's an alpha in the fill
    private final int blend_fill(int p1) {
      int a2 = fillAi;
      int a1 = a2 ^ 0xff;

      int r = (a1 * ((p1 >> 16) & 0xff)) + (a2 * fillRi) & 0xff00;
      int g = (a1 * ((p1 >>  8) & 0xff)) + (a2 * fillGi) & 0xff00;
      int b = (a1 * ( p1        & 0xff)) + (a2 * fillBi) & 0xff00;

      return 0xff000000 | (r << 8) | g | (b >> 8);
    }


    private final int blend_color(int p1, int p2) {
      int a2 = (p2 >>> 24);

      if (a2 == 0xff) {
        // full replacement
        return p2;

      } else {
        int a1 = a2 ^ 0xff;
        int r = (a1 * ((p1 >> 16) & 0xff) + a2 * ((p2 >> 16) & 0xff)) & 0xff00;
        int g = (a1 * ((p1 >>  8) & 0xff) + a2 * ((p2 >>  8) & 0xff)) & 0xff00;
        int b = (a1 * ( p1        & 0xff) + a2 * ( p2        & 0xff)) >> 8;

        return 0xff000000 | (r << 8) | g | b;
      }
    }


    private final int blend_color_alpha(int p1, int p2, int a2) {
      // scale alpha by alpha of incoming pixel
      a2 = (a2 * (p2 >>> 24)) >> 8;

      int a1 = a2 ^ 0xff;
      int r = (a1 * ((p1 >> 16) & 0xff) + a2 * ((p2 >> 16) & 0xff)) & 0xff00;
      int g = (a1 * ((p1 >>  8) & 0xff) + a2 * ((p2 >>  8) & 0xff)) & 0xff00;
      int b = (a1 * ( p1        & 0xff) + a2 * ( p2        & 0xff)) >> 8;

      return 0xff000000 | (r << 8) | g | b;
    }
}
