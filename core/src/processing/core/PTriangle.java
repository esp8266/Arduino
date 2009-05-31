/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-07 Ben Fry and Casey Reas
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
 * Handles rendering of single (tesselated) triangles in 3D.
 * <P>
 * Originally written by sami (www.sumea.com)
 */
public class PTriangle implements PConstants
{
  static final float PIXEL_CENTER = 0.5f;  // for polygon aa

  static final int R_GOURAUD = 0x1;
  static final int R_TEXTURE8 = 0x2;
  static final int R_TEXTURE24 = 0x4;
  static final int R_TEXTURE32 = 0x8;
  static final int R_ALPHA = 0x10;

  private int[] m_pixels;
  private int[] m_texture;
  //private int[] m_stencil;
  private float[] m_zbuffer;

  private int SCREEN_WIDTH;
  private int SCREEN_HEIGHT;
  //private int SCREEN_WIDTH1;
  //private int SCREEN_HEIGHT1;

  private int TEX_WIDTH;
  private int TEX_HEIGHT;
  private float F_TEX_WIDTH;
  private float F_TEX_HEIGHT;

  public boolean INTERPOLATE_UV;
  public boolean INTERPOLATE_RGB;
  public boolean INTERPOLATE_ALPHA;

  // the power of 2 that tells how many pixels to interpolate
  // for between  exactly computed texture coordinates
  private static final int DEFAULT_INTERP_POWER = 3;
  private static int TEX_INTERP_POWER = DEFAULT_INTERP_POWER;

  // Vertex coordinates
  private float[] x_array;
  private float[] y_array;
  private float[] z_array;

  private float[] camX;
  private float[] camY;
  private float[] camZ;

  // U,V coordinates
  private float[] u_array;
  private float[] v_array;

  // Vertex Intensity
  private float[] r_array;
  private float[] g_array;
  private float[] b_array;
  private float[] a_array;

  // vertex offsets
  private int o0;
  private int o1;
  private int o2;

  /* rgb & a */
  private float r0;
  private float r1;
  private float r2;
  private float g0;
  private float g1;
  private float g2;
  private float b0;
  private float b1;
  private float b2;
  private float a0;
  private float a1;
  private float a2;

  /* accurate texture uv coordinates */
  private float u0;
  private float u1;
  private float u2;
  private float v0;
  private float v1;
  private float v2;

  /* deltas */
  //private float dx0;
  //private float dx1;
  private float dx2;
  private float dy0;
  private float dy1;
  private float dy2;
  private float dz0;
  //private float dz1;
  private float dz2;

  /* texture deltas */
  private float du0;
  //private float du1;
  private float du2;
  private float dv0;
  //private float dv1;
  private float dv2;

  /* rgba deltas */
  private float dr0;
  //private float dr1;
  private float dr2;
  private float dg0;
  //private float dg1;
  private float dg2;
  private float db0;
  //private float db1;
  private float db2;
  private float da0;
  //private float da1;
  private float da2;

  /* */
  private float uleft;
  private float vleft;
  private float uleftadd;
  private float vleftadd;

  /* polyedge positions & adds */
  private float xleft;
  private float xrght;
  private float xadd1;
  private float xadd2;
  private float zleft;
  private float zleftadd;

  /* rgba positions & adds */
  private float rleft;
  private float gleft;
  private float bleft;
  private float aleft;
  private float rleftadd;
  private float gleftadd;
  private float bleftadd;
  private float aleftadd;

  /* other somewhat useful variables :) */
  private float dta;
  //private float dta2;
  private float temp;
  private float width;

  /* integer poly UV adds */
  private int iuadd;
  private int ivadd;
  private int iradd;
  private int igadd;
  private int ibadd;
  private int iaadd;
  private float izadd;

  /* fill color */
  private int m_fill;

  /* draw flags */
  public int  m_drawFlags;

  /* current poly number */
//  private int m_index;

  /** */
  private PGraphics3D parent;

  private boolean noDepthTest;
  //private boolean argbSurface;

  /** */
  private boolean m_culling;

  /** */
  private boolean m_singleRight;

  /** 
   * True if using bilinear interpolation for textures.
   * Always set to true. If this is ever changed (maybe with a hint()?)
   * will need to write code for texture8/24/32 et al that will handle mixing
   * the m_fill color in with the texture color.  
   */
  private boolean m_bilinear = true;  // always set to true


  // Vectors needed in accurate texture code
  // We store them as class members to avoid too much code duplication
  private float ax,ay,az;
  private float bx,by,bz;
  private float cx,cy,cz;
  private float nearPlaneWidth;
  private float nearPlaneHeight;
  private float nearPlaneDepth;
  private float xmult;
  private float ymult;
  // optimization vars...not pretty, but save a couple mults per pixel
  private float newax,newbx,newcx;
  // are we currently drawing the first piece of the triangle,
  // or have we already done so?
  private boolean firstSegment;


  public PTriangle(PGraphics3D g) {
    x_array = new float[3];
    y_array = new float[3];
    z_array = new float[3];
    u_array = new float[3];
    v_array = new float[3];
    r_array = new float[3];
    g_array = new float[3];
    b_array = new float[3];
    a_array = new float[3];

    camX = new float[3];
    camY = new float[3];
    camZ = new float[3];

    this.parent = g;
    reset();
  }


  /**
   * Resets polygon attributes
   */
  public void reset() {
    // reset these in case PGraphics was resized

    SCREEN_WIDTH = parent.width;
    SCREEN_HEIGHT = parent.height;
    //SCREEN_WIDTH1 = SCREEN_WIDTH-1;
    //SCREEN_HEIGHT1 = SCREEN_HEIGHT-1;

    m_pixels = parent.pixels;
//    m_stencil = parent.stencil;
    m_zbuffer = parent.zbuffer;

    noDepthTest = parent.hints[DISABLE_DEPTH_TEST];
    //argbSurface = parent.format == PConstants.ARGB;

    // other things to reset

    INTERPOLATE_UV = false;
    INTERPOLATE_RGB = false;
    INTERPOLATE_ALPHA = false;
    //m_tImage = null;
    m_texture = null;
    m_drawFlags = 0;
  }


  /**
   * Sets backface culling on/off
   */
  public void setCulling(boolean tf) {
    m_culling = tf;
  }


  /**
   * Sets vertex coordinates for the triangle
   */
  public void setVertices(float x0, float y0, float z0,
                          float x1, float y1, float z1,
                          float x2, float y2, float z2) {
    x_array[0] = x0;
    x_array[1] = x1;
    x_array[2] = x2;

    y_array[0] = y0;
    y_array[1] = y1;
    y_array[2] = y2;

    z_array[0] = z0;
    z_array[1] = z1;
    z_array[2] = z2;
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


  /**
   * Sets the UV coordinates of the texture
   */
  public void setUV(float u0, float v0,
                    float u1, float v1,
                    float u2, float v2) {
    // sets & scales uv texture coordinates to center of the pixel
    u_array[0] = (u0 * F_TEX_WIDTH + 0.5f) * 65536f;
    u_array[1] = (u1 * F_TEX_WIDTH + 0.5f) * 65536f;
    u_array[2] = (u2 * F_TEX_WIDTH + 0.5f) * 65536f;
    v_array[0] = (v0 * F_TEX_HEIGHT + 0.5f) * 65536f;
    v_array[1] = (v1 * F_TEX_HEIGHT + 0.5f) * 65536f;
    v_array[2] = (v2 * F_TEX_HEIGHT + 0.5f) * 65536f;
  }


  /**
   * Sets vertex intensities in 0xRRGGBBAA format
   */
  public void setIntensities(float r0, float g0, float b0, float a0,
                             float r1, float g1, float b1, float a1,
                             float r2, float g2, float b2, float a2) {
    // Check if we need alpha or not?
    if ((a0 != 1.0f) || (a1 != 1.0f) || (a2 != 1.0f)) {
      INTERPOLATE_ALPHA = true;
      a_array[0] = (a0 * 253f + 1.0f) * 65536f;
      a_array[1] = (a1 * 253f + 1.0f) * 65536f;
      a_array[2] = (a2 * 253f + 1.0f) * 65536f;
      m_drawFlags|=R_ALPHA;
    } else {
      INTERPOLATE_ALPHA = false;
      m_drawFlags&=~R_ALPHA;
    }

    // Check if we need to interpolate the intensity values
    if ((r0 != r1) || (r1 != r2)) {
      INTERPOLATE_RGB = true;
      m_drawFlags |= R_GOURAUD;
    } else if ((g0 != g1) || (g1 != g2)) {
      INTERPOLATE_RGB = true;
      m_drawFlags |= R_GOURAUD;
    } else if ((b0 != b1) || (b1 != b2)) {
      INTERPOLATE_RGB = true;
      m_drawFlags |= R_GOURAUD;
    } else {
      //m_fill = parent.filli;
      m_drawFlags &=~ R_GOURAUD;
    }

    // push values to arrays.. some extra scaling is added
    // to prevent possible color "overflood" due to rounding errors
    r_array[0] = (r0 * 253f + 1.0f) * 65536f;
    r_array[1] = (r1 * 253f + 1.0f) * 65536f;
    r_array[2] = (r2 * 253f + 1.0f) * 65536f;

    g_array[0] = (g0 * 253f + 1.0f) * 65536f;
    g_array[1] = (g1 * 253f + 1.0f) * 65536f;
    g_array[2] = (g2 * 253f + 1.0f) * 65536f;

    b_array[0] = (b0 * 253f + 1.0f) * 65536f;
    b_array[1] = (b1 * 253f + 1.0f) * 65536f;
    b_array[2] = (b2 * 253f + 1.0f) * 65536f;

    // for plain triangles
    m_fill = 0xFF000000 | 
      ((int)(255*r0) << 16) | ((int)(255*g0) << 8) | (int)(255*b0);
  }


  /**
   * Sets texture image used for the polygon
   */
  public void setTexture(PImage image) {
    //m_tImage = image;
    m_texture = image.pixels;
    TEX_WIDTH = image.width;
    TEX_HEIGHT = image.height;
    F_TEX_WIDTH = TEX_WIDTH-1;
    F_TEX_HEIGHT = TEX_HEIGHT-1;
    INTERPOLATE_UV = true;

    if (image.format == ARGB) {
      m_drawFlags |= R_TEXTURE32;
    } else if (image.format == RGB) {
      m_drawFlags |= R_TEXTURE24;
    } else if (image.format == ALPHA) {
      m_drawFlags |= R_TEXTURE8;
    }
  }


  /**
   *
   */
  public void setUV(float[] u, float[] v) {
    if (m_bilinear) {
      // sets & scales uv texture coordinates to edges of pixels
      u_array[0] = (u[0] * F_TEX_WIDTH) * 65500f;
      u_array[1] = (u[1] * F_TEX_WIDTH) * 65500f;
      u_array[2] = (u[2] * F_TEX_WIDTH) * 65500f;
      v_array[0] = (v[0] * F_TEX_HEIGHT) * 65500f;
      v_array[1] = (v[1] * F_TEX_HEIGHT) * 65500f;
      v_array[2] = (v[2] * F_TEX_HEIGHT) * 65500f;
    } else {
      // sets & scales uv texture coordinates to center of the pixel
      u_array[0] = (u[0] * TEX_WIDTH) * 65500f;
      u_array[1] = (u[1] * TEX_WIDTH) * 65500f;
      u_array[2] = (u[2] * TEX_WIDTH) * 65500f;
      v_array[0] = (v[0] * TEX_HEIGHT) * 65500f;
      v_array[1] = (v[1] * TEX_HEIGHT) * 65500f;
      v_array[2] = (v[2] * TEX_HEIGHT) * 65500f;
    }
  }


//  public void setIndex(int index) {
//    m_index = index;
//  }


  /**
   * Renders the polygon
   */
  public void render() {
    float x0, x1, x2;
    float z0, z1, z2;

    float y0 = y_array[0];
    float y1 = y_array[1];
    float y2 = y_array[2];

    //System.out.println(PApplet.hex(m_drawFlags));

    // For accurate texture interpolation, need to mark whether
    // we've already pre-calculated for the triangle
    firstSegment = true;

    // do backface culling?
    if (m_culling) {
      x0 = x_array[0];
      if ((x_array[2]-x0)*(y1-y0) < (x_array[1]-x0)*(y2-y0))
        return;
    }

    /* get vertex order from top -> down */
    if (y0 < y1) {
      if (y2 < y1) {
        if (y2 < y0) { // 2,0,1
          o0 = 2;
          o1 = 0;
          o2 = 1;
        } else {  // 0,2,1
          o0 = 0;
          o1 = 2;
          o2 = 1;
        }
      } else {  // 0,1,2
        o0 = 0;
        o1 = 1;
        o2 = 2;
      }
    } else {
      if (y2 > y1) {
        if (y2 < y0) { // 1,2,0
          o0 = 1;
          o1 = 2;
          o2 = 0;
        } else {  // 1,0,2
          o0 = 1;
          o1 = 0;
          o2 = 2;
        }
      } else {  // 2,1,0
        o0 = 2;
        o1 = 1;
        o2 = 0;
      }
    }

    /**
      * o0 = "top" vertex offset
      * o1 = "mid" vertex offset
      * o2 = "bot" vertex offset
     */

    y0 = y_array[o0];
    int yi0 = (int) (y0 + PIXEL_CENTER);
    if (yi0 > SCREEN_HEIGHT) {
      return;
    } else if (yi0 < 0) {
      yi0 = 0;
    }

    y2 = y_array[o2];
    int yi2 = (int) (y2 + PIXEL_CENTER);
    if (yi2 < 0) {
      return;
    } else if (yi2 > SCREEN_HEIGHT) {
      yi2 = SCREEN_HEIGHT;
    }

    // Does the poly actually cross a scanline?
    if (yi2 > yi0) {
      x0 = x_array[o0];
      x1 = x_array[o1];
      x2 = x_array[o2];

      // get mid Y and clip it
      y1 = y_array[o1];
      int yi1 = (int) (y1 + PIXEL_CENTER);
      if (yi1 < 0)
        yi1 = 0;
      if (yi1 > SCREEN_HEIGHT)
        yi1 = SCREEN_HEIGHT;

      // calculate deltas etc.
      dx2 = x2 - x0;
      dy0 = y1 - y0;
      dy2 = y2 - y0;
      xadd2 = dx2 / dy2;  // xadd for "single" edge
      temp = dy0 / dy2;
      width = temp * dx2 + x0 - x1;

      // calculate alpha blend interpolation
      if (INTERPOLATE_ALPHA) {
        a0 = a_array[o0];
        a1 = a_array[o1];
        a2 = a_array[o2];
        da0 = a1-a0;
        da2 = a2-a0;
        iaadd = (int) ((temp * da2 - da0) / width); // alpha add
      }

      // calculate intensity interpolation
      if (INTERPOLATE_RGB) {
        r0 = r_array[o0];
        r1 = r_array[o1];
        r2 = r_array[o2];

        g0 = g_array[o0];
        g1 = g_array[o1];
        g2 = g_array[o2];

        b0 = b_array[o0];
        b1 = b_array[o1];
        b2 = b_array[o2];

        dr0 = r1-r0;
        dg0 = g1-g0;
        db0 = b1-b0;

        dr2 = r2-r0;
        dg2 = g2-g0;
        db2 = b2-b0;

        iradd = (int) ((temp * dr2 - dr0) / width); // r add
        igadd = (int) ((temp * dg2 - dg0) / width); // g add
        ibadd = (int) ((temp * db2 - db0) / width); // b add
      }

      // calculate UV interpolation
      if (INTERPOLATE_UV) {
        u0 = u_array[o0];
        u1 = u_array[o1];
        u2 = u_array[o2];
        v0 = v_array[o0];
        v1 = v_array[o1];
        v2 = v_array[o2];
        du0 = u1-u0;
        dv0 = v1-v0;
        du2 = u2-u0;
        dv2 = v2-v0;
        iuadd = (int) ((temp * du2 - du0) / width); // u add
        ivadd = (int) ((temp * dv2 - dv0) / width); // v add
      }

      z0 = z_array[o0];
      z1 = z_array[o1];
      z2 = z_array[o2];
      dz0 = z1-z0;
      dz2 = z2-z0;
      izadd = (temp * dz2 - dz0) / width;

      // draw the upper poly segment if it's visible
      if (yi1 > yi0) {
        dta = (yi0 + PIXEL_CENTER) - y0;
        xadd1 = (x1 - x0) / dy0;

        // we can determine which side is "single" side
        // by comparing left/right edge adds
        if (xadd2 > xadd1) {
          xleft = x0 + dta * xadd1;
          xrght = x0 + dta * xadd2;
          zleftadd = dz0 / dy0;
          zleft = dta*zleftadd+z0;

          if (INTERPOLATE_UV) {
            uleftadd = du0 / dy0;
            vleftadd = dv0 / dy0;
            uleft = dta*uleftadd+u0;
            vleft = dta*vleftadd+v0;
          }

          if (INTERPOLATE_RGB) {
            rleftadd = dr0 / dy0;
            gleftadd = dg0 / dy0;
            bleftadd = db0 / dy0;
            rleft = dta*rleftadd+r0;
            gleft = dta*gleftadd+g0;
            bleft = dta*bleftadd+b0;
          }

          if (INTERPOLATE_ALPHA) {
            aleftadd = da0 / dy0;
            aleft = dta*aleftadd+a0;

            if (m_drawFlags == R_ALPHA) {
              drawsegment_plain_alpha(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_ALPHA)) {
              drawsegment_gouraud_alpha(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_TEXTURE8 + R_ALPHA)) {
              drawsegment_texture8_alpha(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_TEXTURE24 + R_ALPHA)) {
              drawsegment_texture24_alpha(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_TEXTURE32 + R_ALPHA)) {
              drawsegment_texture32_alpha(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE8 + R_ALPHA)) {
              drawsegment_gouraud_texture8_alpha(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE24 + R_ALPHA)) {
              drawsegment_gouraud_texture24_alpha(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE32 + R_ALPHA)) {
              drawsegment_gouraud_texture32_alpha(xadd1,xadd2, yi0,yi1);
            }
          } else {
            if (m_drawFlags == 0) {
              drawsegment_plain(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == R_GOURAUD) {
              drawsegment_gouraud(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == R_TEXTURE8) {
              drawsegment_texture8(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == R_TEXTURE24) {
              drawsegment_texture24(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == R_TEXTURE32) {
              drawsegment_texture32(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE8)) {
              drawsegment_gouraud_texture8(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE24)) {
              drawsegment_gouraud_texture24(xadd1,xadd2, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE32)) {
              drawsegment_gouraud_texture32(xadd1,xadd2, yi0,yi1);
            }
          }
          m_singleRight = true;
        } else {
          xleft = x0 + dta * xadd2;
          xrght = x0 + dta * xadd1;
          zleftadd = dz2 / dy2;
          zleft = dta*zleftadd+z0;
          //
          if (INTERPOLATE_UV) {
            uleftadd = du2 / dy2;
            vleftadd = dv2 / dy2;
            uleft = dta*uleftadd+u0;
            vleft = dta*vleftadd+v0;
          }

          //
          if (INTERPOLATE_RGB) {
            rleftadd = dr2 / dy2;
            gleftadd = dg2 / dy2;
            bleftadd = db2 / dy2;
            rleft = dta*rleftadd+r0;
            gleft = dta*gleftadd+g0;
            bleft = dta*bleftadd+b0;
          }


          if (INTERPOLATE_ALPHA) {
            aleftadd = da2 / dy2;
            aleft = dta*aleftadd+a0;

            if (m_drawFlags == R_ALPHA) {
              drawsegment_plain_alpha(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_ALPHA)) {
              drawsegment_gouraud_alpha(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_TEXTURE8 + R_ALPHA)) {
              drawsegment_texture8_alpha(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_TEXTURE24 + R_ALPHA)) {
              drawsegment_texture24_alpha(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_TEXTURE32 + R_ALPHA)) {
              drawsegment_texture32_alpha(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE8 + R_ALPHA)) {
              drawsegment_gouraud_texture8_alpha(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE24 + R_ALPHA)) {
              drawsegment_gouraud_texture24_alpha(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE32 + R_ALPHA)) {
              drawsegment_gouraud_texture32_alpha(xadd2, xadd1, yi0,yi1);
            }
          } else {
            if (m_drawFlags == 0) {
              drawsegment_plain(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == R_GOURAUD) {
              drawsegment_gouraud(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == R_TEXTURE8) {
              drawsegment_texture8(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == R_TEXTURE24) {
              drawsegment_texture24(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == R_TEXTURE32) {
              drawsegment_texture32(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE8)) {
              drawsegment_gouraud_texture8(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE24)) {
              drawsegment_gouraud_texture24(xadd2, xadd1, yi0,yi1);
            } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE32)) {
              drawsegment_gouraud_texture32(xadd2, xadd1, yi0,yi1);
            }
          }
          m_singleRight = false;
        }

        // if bottom segment height is zero, return
        if (yi2 == yi1) return;

        // calculate xadd 1
        dy1 = y2 - y1;
        xadd1 = (x2 - x1) / dy1;

      } else {
        // top seg height was zero, calculate & clip single edge X
        dy1 = y2 - y1;
        xadd1 = (x2 - x1) / dy1;

        // which edge is left?
        if (xadd2 < xadd1) {
          xrght = ((yi1 + PIXEL_CENTER) - y0) * xadd2 + x0;
          m_singleRight = true;
        } else {
          dta = (yi1 + PIXEL_CENTER) - y0;
          xleft = dta * xadd2 + x0;
          zleftadd = dz2 / dy2;
          zleft = dta * zleftadd + z0;

          if (INTERPOLATE_UV) {
            uleftadd = du2 / dy2;
            vleftadd = dv2 / dy2;
            uleft = dta * uleftadd + u0;
            vleft = dta * vleftadd + v0;
          }

          if (INTERPOLATE_RGB) {
            rleftadd = dr2 / dy2;
            gleftadd = dg2 / dy2;
            bleftadd = db2 / dy2;
            rleft = dta * rleftadd + r0;
            gleft = dta * gleftadd + g0;
            bleft = dta * bleftadd + b0;
          }

          //
          if (INTERPOLATE_ALPHA) {
            aleftadd = da2 / dy2;
            aleft = dta * aleftadd + a0;
          }
          m_singleRight = false;
        }
      }

      // draw the lower segment
      if (m_singleRight) {
        dta = (yi1 + PIXEL_CENTER) - y1;
        xleft = dta * xadd1 + x1;
        zleftadd = (z2 - z1) / dy1;
        zleft = dta * zleftadd + z1;

        if (INTERPOLATE_UV) {
          uleftadd = (u2 - u1) / dy1;
          vleftadd = (v2 - v1) / dy1;
          uleft = dta * uleftadd + u1;
          vleft = dta * vleftadd + v1;
        }

        if (INTERPOLATE_RGB) {
          rleftadd = (r2 - r1) / dy1;
          gleftadd = (g2 - g1) / dy1;
          bleftadd = (b2 - b1) / dy1;
          rleft = dta * rleftadd + r1;
          gleft = dta * gleftadd + g1;
          bleft = dta * bleftadd + b1;
        }

        if (INTERPOLATE_ALPHA) {
          aleftadd = (a2 - a1) / dy1;
          aleft = dta * aleftadd + a1;

          if (m_drawFlags == R_ALPHA) {
            drawsegment_plain_alpha(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_ALPHA)) {
            drawsegment_gouraud_alpha(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_TEXTURE8 + R_ALPHA)) {
            drawsegment_texture8_alpha(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_TEXTURE24 + R_ALPHA)) {
            drawsegment_texture24_alpha(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_TEXTURE32 + R_ALPHA)) {
            drawsegment_texture32_alpha(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE8 + R_ALPHA)) {
            drawsegment_gouraud_texture8_alpha(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE24 + R_ALPHA)) {
            drawsegment_gouraud_texture24_alpha(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE32 + R_ALPHA)) {
            drawsegment_gouraud_texture32_alpha(xadd1, xadd2, yi1,yi2);
          }
        } else {
          if (m_drawFlags == 0) {
            drawsegment_plain(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == R_GOURAUD) {
            drawsegment_gouraud(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == R_TEXTURE8) {
            drawsegment_texture8(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == R_TEXTURE24) {
            drawsegment_texture24(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == R_TEXTURE32) {
            drawsegment_texture32(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE8)) {
            drawsegment_gouraud_texture8(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE24)) {
            drawsegment_gouraud_texture24(xadd1, xadd2, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE32)) {
            drawsegment_gouraud_texture32(xadd1, xadd2, yi1,yi2);
          }
        }
      } else {
        xrght = ((yi1 + PIXEL_CENTER)- y1) * xadd1 + x1;

        if (INTERPOLATE_ALPHA) {
          if (m_drawFlags == R_ALPHA) {
            drawsegment_plain_alpha(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_ALPHA)) {
            drawsegment_gouraud_alpha(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_TEXTURE8 + R_ALPHA)) {
            drawsegment_texture8_alpha(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_TEXTURE24 + R_ALPHA)) {
            drawsegment_texture24_alpha(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_TEXTURE32 + R_ALPHA)) {
            drawsegment_texture32_alpha(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE8 + R_ALPHA)) {
            drawsegment_gouraud_texture8_alpha(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE24 + R_ALPHA)) {
            drawsegment_gouraud_texture24_alpha(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE32 + R_ALPHA)) {
            drawsegment_gouraud_texture32_alpha(xadd2, xadd1, yi1,yi2);
          }
        } else {
          if (m_drawFlags == 0) {
            drawsegment_plain(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == R_GOURAUD) {
            drawsegment_gouraud(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == R_TEXTURE8) {
            drawsegment_texture8(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == R_TEXTURE24) {
            drawsegment_texture24(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == R_TEXTURE32) {
            drawsegment_texture32(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE8)) {
            drawsegment_gouraud_texture8(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE24)) {
            drawsegment_gouraud_texture24(xadd2, xadd1, yi1,yi2);
          } else if (m_drawFlags == (R_GOURAUD + R_TEXTURE32)) {
            drawsegment_gouraud_texture32(xadd2, xadd1, yi1,yi2);
          }
        }
      }
    }
  }


  /**
   * Accurate texturing code by ewjordan@gmail.com, April 14, 2007
   * The getColorFromTexture() function should be inlined and optimized
   * so that most of the heavy lifting happens outside the per-pixel loop.
   * The unoptimized generic algorithm looks like this (unless noted,
   * all of these variables are vectors, so the actual code will look messier):
   *
   * p = camera space vector where u == 0, v == 0;
   * m = vector from p to location where u == TEX_WIDTH;
   * n = vector from p to location where v == TEX_HEIGHT;
   *
   * A = p cross n;
   * B = m cross p;
   * C = n cross m;
   * A *= texture.width;
   * B *= texture.height;
   *
   * for (scanlines in triangle){
   *   float a = S * A;
   *   float b = S * B;
   *   float c = S * C;
   *   for (pixels in scanline){
   *     int u = a/c;
   *     int v = b/c;
   *     color = texture[v * texture.width + u];
   *     a += A.x;
   *     b += B.x;
   *     c += C.x;
   *   }
   * }
   *
   * We don't use this exact algorithm here, however, because of the extra
   * overhead from the divides. Instead we compute the exact u and v (labelled
   * iu and iv in the code) at the start of each scanline and we perform a
   * linear interpolation for every linearInterpLength = 1 << TEX_INTERP_POWER
   * pixels. This means that we only perform the true calculation once in a
   * while, and the rest of the time the algorithm functions exactly as in the
   * fast inaccurate case, at least in theory.  In practice, even if we set
   * linearInterpLength very high we still incur some speed penalty due to the
   * preprocessing that must take place per-scanline.  A similar method could
   * be applied per scanline to avoid this, but it would only be worthwhile in
   * the case that we never compute more than one exact calculation per
   * scanline.  If we want something like this, however, it would be best to
   * create another mode of calculation called "constant-z" interpolation,
   * which could be used for things like floors and ceilings where the
   * distance from the camera plane never changes over the course of a
   * scanline.  We could also add the vertical analogue for drawing vertical
   * walls.  In any case, these are not critical as the current algorithm runs
   * fairly well, perhaps ~10% slower than the default perspective-less one.
   */

  /**
   * Solve for camera space coordinates of critical texture points and
   * set up per-triangle variables for accurate texturing.
   * Sets all class variables relevant to accurate texture computation
   * Should be called once per triangle - checks firstSegment to see if
   * we've already called.
   */
  private boolean precomputeAccurateTexturing() {
    // rescale u/v_array values when inverting matrix and performing other calcs
    float myFact = 65500.0f;
    float myFact2 = 65500.0f;

    //Matrix inversion to find affine transform between (u,v,(1)) -> (x,y,z)

    // OPTIMIZE: There should be a way to avoid the inversion here, which is
    // quite expensive (~150 mults).  Also it might crap out due to loss of
    // precision depending on the scaling of the u/v_arrays.  Nothing clever
    // currently happens if the inversion fails, since this means the
    // transformation is degenerate - we just pass false back to the caller
    // and let it handle the situation. [There is no good solution to this
    // case from within this function, since the way the calculation proceeds
    // presumes a non-degenerate transformation matrix between camera space
    // and uv space]

    // Obvious optimization: if the vertices are actually at the appropriate
    // texture coordinates (e.g. (0,0), (TEX_WIDTH,0), and (0,TEX_HEIGHT))
    // then we can immediately return the right solution without the inversion.
    // This is fairly common, so could speed up many cases of drawing.
    // [not implemented]

    // Furthermore, we could cache the p,resultT0,result0T vectors in the
    // triangle's basis, since we could then do a look-up and generate the
    // resulting coordinates very simply. This would include the above
    // optimization as a special case - we could pre-populate the cache with
    // special cases like that and dynamically add more. The idea here is that
    // most people simply paste textures onto triangles and move the triangles
    // from frame to frame, so any per-triangle-per-frame code is likely
    // wasted effort. [not implemented]

    // Note: o0, o1, and o2 vary depending on view angle to triangle,
    // but p, n, and m should not depend on ordering differences

    if (firstSegment){
      PMatrix3D myMatrix = 
        new PMatrix3D(u_array[o0]/myFact, v_array[o0]/myFact2, 1, 0,
                    u_array[o1]/myFact, v_array[o1]/myFact2, 1, 0,
                    u_array[o2]/myFact, v_array[o2]/myFact2, 1, 0,
                    0,           0,           0, 1);
      // A 3x3 inversion would be more efficient here, 
      // given that the fourth r/c are unity
      myMatrix.invert();
      // if the matrix inversion had trouble, let the caller know
      if (myMatrix == null) return false;

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
      // Bugfix: possibly we should use F_TEX_WIDTH/HEIGHT instead?
      // Seems to read off end of array in that case, though...
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
    }

    nearPlaneWidth = parent.rightScreen-parent.leftScreen;
    nearPlaneHeight = parent.topScreen-parent.bottomScreen;
    nearPlaneDepth = parent.nearPlane;
    // one pixel width in nearPlane coordinates
    xmult = nearPlaneWidth / SCREEN_WIDTH;
    ymult = nearPlaneHeight / SCREEN_HEIGHT;
    // Extra scalings to map screen plane units to pixel units
    newax = ax*xmult;
    newbx = bx*xmult;
    newcx = cx*xmult;
    return true;
  }


  /**
   * Set the power of two used for linear interpolation of texture coordinates.
   * A true texture coordinate is computed every 2^pwr pixels along a scanline.
   */
  static public void setInterpPower(int pwr) {
    //Currently must be invoked from P5 as PTriangle.setInterpPower(...)
    TEX_INTERP_POWER = pwr;
  }


  /**
   * Plain color
   */
  private void drawsegment_plain(float leftadd,
                                 float rghtadd,
                                 int ytop,
                                 int ybottom) {
    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int f = m_fill;
//    int p = m_index;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;

      float xdiff = (xstart + PIXEL_CENTER) - xleft;
      float iz = izadd * xdiff + zleft;
      xstart+=ytop;
      xend+=ytop;

      for ( ; xstart < xend; xstart++ ) {
        if (noDepthTest || (iz <= m_zbuffer[xstart])) {
          m_zbuffer[xstart] = iz;
          m_pixels[xstart] = m_fill;
//          m_stencil[xstart] = p;
        }
        iz+=izadd;
      }

      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      zleft+=zleftadd;
    }
  }

  
  /**
   * Plain color, interpolated alpha
   */
  private void drawsegment_plain_alpha(float leftadd,
                                       float rghtadd,
                                       int ytop,
                                       int ybottom) {
    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;

    int pr = m_fill & 0xFF0000;
    int pg = m_fill & 0xFF00;
    int pb = m_fill & 0xFF;

//    int p = m_index;
    float iaf = iaadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;

      float xdiff = (xstart + PIXEL_CENTER) - xleft;
      float iz = izadd * xdiff + zleft;
      int ia = (int) (iaf * xdiff + aleft);
      xstart += ytop;
      xend += ytop;

      //int ma0 = 0xFF000000;
      
      for ( ; xstart < xend; xstart++ ) {
        if (noDepthTest || (iz <= m_zbuffer[xstart])) {
          // don't set zbuffer when not fully opaque
          //m_zbuffer[xstart] = iz;

          int alpha = ia >> 16;
          int mr0 = m_pixels[xstart];
          /*
          if (argbSurface) {
            ma0 = (((mr0 >>> 24) * alpha) << 16) & 0xFF000000;
            if (ma0 == 0) ma0 = alpha << 24;
          }
          */
          int mg0 = mr0 & 0xFF00;
          int mb0 = mr0 & 0xFF;
          mr0 &= 0xFF0000;
          
          mr0 = mr0 + (((pr - mr0) * alpha) >> 8);
          mg0 = mg0 + (((pg - mg0) * alpha) >> 8);
          mb0 = mb0 + (((pb - mb0) * alpha) >> 8);
          
          m_pixels[xstart] = 0xFF000000 | 
            (mr0 & 0xFF0000) | (mg0 & 0xFF00) | (mb0 & 0xFF);

//          m_stencil[xstart] = p;
        }
        iz += izadd;
        ia += iaadd;
      }
      ytop += SCREEN_WIDTH;
      xleft += leftadd;
      xrght += rghtadd;
      zleft += zleftadd;
    }
  }


  /**
   * RGB gouraud
   */
  private void drawsegment_gouraud(float leftadd,
                                   float rghtadd,
                                   int ytop,
                                   int ybottom) {
    float irf = iradd;
    float igf = igadd;
    float ibf = ibadd;

    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;

      float xdiff = (xstart + PIXEL_CENTER) - xleft;
      int ir = (int) (irf * xdiff + rleft);
      int ig = (int) (igf * xdiff + gleft);
      int ib = (int) (ibf * xdiff + bleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      for ( ; xstart < xend; xstart++ ) {
        if (noDepthTest || (iz <= m_zbuffer[xstart])) {
          m_zbuffer[xstart] = iz;
          m_pixels[xstart] = 0xFF000000 | 
            ((ir & 0xFF0000) | ((ig >> 8) & 0xFF00) | (ib >> 16));
//          m_stencil[xstart] = p;
        }

        ir+=iradd;
        ig+=igadd;
        ib+=ibadd;
        iz+=izadd;
      }

      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      rleft+=rleftadd;
      gleft+=gleftadd;
      bleft+=bleftadd;
      zleft+=zleftadd;
    }
  }


  /**
   * RGB gouraud + interpolated alpha
   */
  private void drawsegment_gouraud_alpha(float leftadd,
                                         float rghtadd,
                                         int ytop,
                                         int ybottom) {
    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    float irf = iradd;
    float igf = igadd;
    float ibf = ibadd;
    float iaf = iaadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;
      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;
      float xdiff = (xstart + PIXEL_CENTER) - xleft;

      int ir = (int) (irf * xdiff + rleft);
      int ig = (int) (igf * xdiff + gleft);
      int ib = (int) (ibf * xdiff + bleft);
      int ia = (int) (iaf * xdiff + aleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      for ( ; xstart < xend; xstart++ ) {
        if (noDepthTest || (iz <= m_zbuffer[xstart])) {
          //m_zbuffer[xstart] = iz;

          //
          int red = (ir & 0xFF0000);
          int grn = (ig >> 8) & 0xFF00;
          int blu = (ib >> 16);

          // get buffer pixels
          int bb = m_pixels[xstart];
          int br = (bb & 0xFF0000);  // 0x00FF0000
          int bg = (bb & 0xFF00);    // 0x0000FF00
          bb = (bb & 0xFF);          // 0x000000FF

          // blend alpha
          int al = ia >> 16;

          //
          m_pixels[xstart] = 0xFF000000 | 
            ((br + (((red - br) * al) >> 8)) & 0xFF0000) | 
            ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) | 
            ((bb + (((blu - bb) * al) >> 8)) & 0xFF);
//          m_stencil[xstart] = p;
        }

        //
        ir+=iradd;
        ig+=igadd;
        ib+=ibadd;
        ia+=iaadd;
        iz+=izadd;
      }

      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      rleft+=rleftadd;
      gleft+=gleftadd;
      bleft+=bleftadd;
      aleft+=aleftadd;
      zleft+=zleftadd;
    }
  }


  /**
   * 8-bit plain texture
   */

   //THIS IS MESSED UP, NEED TO GRAB ORIGINAL VERSION!!!
  private void drawsegment_texture8(float leftadd,
                                    float rghtadd,
                                    int ytop,
                                    int ybottom) {    
    // Accurate texture mode added - comments stripped from dupe code, 
    // see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode) {
      // see if the precomputation goes well, if so finish the setup
      if (precomputeAccurateTexturing()) { 
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        // if the matrix inversion screwed up, revert to normal rendering 
        // (something is degenerate)
        accurateMode = false; 
      }
    }
    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    float iuf = iuadd;
    float ivf = ivadd;

    int red = m_fill & 0xFF0000;
    int grn = m_fill & 0xFF00;
    int blu = m_fill & 0xFF;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

          int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;

      float xdiff = (xstart + PIXEL_CENTER) - xleft;
      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode && goingIn) {
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU;
        iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else {
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }

      for ( ; xstart < xend; xstart++ ) {
        if (accurateMode) {
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else {
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }
        // try-catch just in case pixel offset it out of range
        try {
          if (noDepthTest || (iz <= m_zbuffer[xstart])) {
            //m_zbuffer[xstart] = iz;

            int al0;
            if (m_bilinear) {
              int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
              int iui = iu & 0xFFFF;
              al0 = m_texture[ofs] & 0xFF;
              int al1 = m_texture[ofs + 1] & 0xFF;
              if (ofs < lastRowStart) ofs+=TEX_WIDTH;
              int al2 = m_texture[ofs] & 0xFF;
              int al3 = m_texture[ofs + 1] & 0xFF;
              al0 = al0 + (((al1-al0) * iui) >> 16);
              al2 = al2 + (((al3-al2) * iui) >> 16);
              al0 = al0 + (((al2-al0) * (iv & 0xFFFF)) >> 16);
            } else {
              al0 = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)] & 0xFF;
            }

            int br = m_pixels[xstart];
            int bg = (br & 0xFF00);
            int bb = (br & 0xFF);
            br = (br & 0xFF0000);
            m_pixels[xstart] = 0xFF000000 |  
              ((br + (((red - br) * al0) >> 8)) & 0xFF0000) | 
              ((bg + (((grn - bg) * al0) >> 8)) & 0xFF00) | 
              ((bb + (((blu - bb) * al0) >> 8)) & 0xFF);
//            m_stencil[xstart] = p;
          }
        }
        catch (Exception e) {
        }
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        iz+=izadd;
      }
      ypixel++;//accurate mode
      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      zleft+=zleftadd;
    }
  }



  /**
   * 8-bit texutre + alpha
   */
  private void drawsegment_texture8_alpha(float leftadd,
                                          float rghtadd,
                                          int ytop,
                                          int ybottom) {    
    // Accurate texture mode added - comments stripped from dupe code, 
    // see drawsegment_texture24() for details
    
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode) {
      // see if the precomputation goes well, if so finish the setup
      if (precomputeAccurateTexturing()) { 
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else {
        // if the matrix inversion screwed up, 
        // revert to normal rendering (something is degenerate)
        accurateMode = false; 
      }
    }
    ytop*=SCREEN_WIDTH;
    ybottom*=SCREEN_WIDTH;
//    int p = m_index;

    float iuf = iuadd;
    float ivf = ivadd;
    float iaf = iaadd;

    int red = m_fill & 0xFF0000;
    int grn = m_fill & 0xFF00;
    int blu = m_fill & 0xFF;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;

      float xdiff = (xstart + PIXEL_CENTER) - xleft;
      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      int ia = (int) (iaf * xdiff + aleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }


      for ( ; xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }
        // try-catch just in case pixel offset it out of range
        try
          {
            if (noDepthTest || (iz <= m_zbuffer[xstart])) {
              //m_zbuffer[xstart] = iz;

              int al0;
              if (m_bilinear) {
                int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
                int iui = iu & 0xFFFF;
                al0 = m_texture[ofs] & 0xFF;
                int al1 = m_texture[ofs + 1] & 0xFF;
                if (ofs < lastRowStart) ofs+=TEX_WIDTH;
                int al2 = m_texture[ofs] & 0xFF;
                int al3 = m_texture[ofs + 1] & 0xFF;
                al0 = al0 + (((al1-al0) * iui) >> 16);
                al2 = al2 + (((al3-al2) * iui) >> 16);
                al0 = al0 + (((al2-al0) * (iv & 0xFFFF)) >> 16);
              } else {
                al0 = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)] & 0xFF;
              }
              al0 = (al0 * (ia >> 16)) >> 8;

              int br = m_pixels[xstart];
              int bg = (br & 0xFF00);
              int bb = (br & 0xFF);
              br = (br & 0xFF0000);
              m_pixels[xstart] = 0xFF000000 | 
                ((br + (((red - br) * al0) >> 8)) & 0xFF0000) | 
                ((bg + (((grn - bg) * al0) >> 8)) & 0xFF00) | 
                ((bb + (((blu - bb) * al0) >> 8)) & 0xFF);
//              m_stencil[xstart] = p;
            }
          }
        catch (Exception e) {
        }
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        iz+=izadd;
        ia+=iaadd;
      }
      ypixel++;//accurate mode
      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      zleft+=zleftadd;
      aleft+=aleftadd;
    }
  }

  /**
   * Plain 24-bit texture
   */
  private void drawsegment_texture24(float leftadd,
                                     float rghtadd,
                                     int ytop,
                                     int ybottom) {
    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;
    float iuf = iuadd;
    float ivf = ivadd;
    
    boolean tint = (m_fill & 0xFFFFFF) != 0xFFFFFF;
    int rtint = (m_fill >> 16) & 0xff;
    int gtint = (m_fill >> 8) & 0xff;
    int btint = m_fill & 0xFF;

    int ypixel = ytop/SCREEN_WIDTH;//ACCTEX
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;//If we're past this index, we can't shift down a row w/o throwing an exception
    //      int exCount = 0;//counter for exceptions caught
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES]; //bring this local since it will be accessed often
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;

    //Interpolation length of 16 tends to look good except at a small angle; 8 looks okay then, except for the
    //above issue.  When viewing close to flat, as high as 32 is often acceptable.  Could add dynamic interpolation
    //settings based on triangle angle - currently we just pick a value and leave it (by default I have the
    //power set at 3, so every 8 pixels a true coordinate is calculated, which seems a decent compromise).

    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode){
      if(precomputeAccurateTexturing()){ //see if the precomputation goes well, if so finish the setup
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        accurateMode = false; //if the matrix inversion gave us garbage, revert to normal rendering (something is degenerate)
      }
    }


    while (ytop < ybottom) {//scanline loop
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0){ xstart = 0; }

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH){ xend = SCREEN_WIDTH; }
      float xdiff = (xstart + PIXEL_CENTER) - xleft;
      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      float iz = izadd * xdiff + zleft;
      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        //off by one (half, I guess) hack, w/o it the first rows are outside the texture - maybe a mistake somewhere?
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;//OPT - some of this could be brought out of the y-loop since
        b = screenx*bx+screeny*by+screenz*bz;//xpixel and ypixel just increment by the same numbers each iteration.
        c = screenx*cx+screeny*cy+screenz*cz;//Probably not a big bottleneck, though.
      }

      //Figure out whether triangle is going further into the screen or not as we move along scanline
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;

      //Set up linear interpolation between calculated texture points
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      //float fdeltaU = 0; float fdeltaV = 0;//vars for floating point interpolating version of algorithm
      //float fiu = 0; float fiv = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;


      //Bugfix (done): it's a Really Bad Thing to interpolate along a scanline when the triangle goes deeper into the screen,
      //because if the angle is severe enough the last control point for interpolation may cross the vanishing
      //point.  This leads to some pretty nasty artifacts, and ideally we should scan from right to left if the
      //triangle is better served that way, or (what we do now) precompute the offset that we'll need so that we end up
      //with a control point exactly at the furthest edge of the triangle.

      if (accurateMode&&goingIn){
        //IMPORTANT!!!  Results are horrid without this hack!
        //If polygon goes into the screen along scan line, we want to match the control point to the furthest point drawn
        //since the control points are less meaningful the closer you are to the vanishing point.
        //We'll do this by making the first control point lie before the start of the scanline (safe since it's closer to us)

        int rightOffset = (xend-xstart-1)%linearInterpLength; //"off by one" hack...probably means there's a small bug somewhere
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;

        //Take step to control point to the left of start pixel
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);

        //Now step to right control point
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;

        //Get deltas for interpolation
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        //Otherwise the left edge is further, and we pin the first control point to it
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }

      for ( ; xstart < xend; xstart++ ) {//pixel loop - keep trim, can execute thousands of times per frame
        //boolean drawBlack = false; //used to display control points
        if(accurateMode){
          /*  //Non-interpolating algorithm - slowest version, calculates exact coordinate for each pixel,
          //and casts from float->int
          float oneoverc = 65536.0f/c;  //a bit faster to pre-divide for next two steps
          iu = (int)(a*oneoverc);
          iv = (int)(b*oneoverc);
          a += newax;
          b += newbx;
          c += newcx;
          */

          //Float while calculating, int while interpolating
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            //drawBlack = true;
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv; //ints are used for interpolation, not actual computation
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{        //race through using linear interpolation if we're not at a control point
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;

          /* //Floating point interpolating version - slower than int thanks to casts during interpolation steps
             if (interpCounter == 0) {
             interpCounter = linearInterpLength;
             a += newax;
             b += newbx;
             c += newcx;
             float oneoverc = 65536.0f/c;
             oldfu = fu;
             oldfv = fv;
             fu = (a*oneoverc);
             fv = (b*oneoverc);
             //oldu = u; oldv = v;
             fiu = oldfu;
             fiv = oldfv;
             fdeltaU = (fu-oldfu)/linearInterpLength;
             fdeltaV = (fv-oldfv)/linearInterpLength;
             }
             else{
             fiu += fdeltaU;
             fiv += fdeltaV;
             }
             interpCounter--;
             iu = (int)(fiu); iv = (int)(fiv);*/

        }

        // try-catch just in case pixel offset is out of range
        try{
          if (noDepthTest || (iz <= m_zbuffer[xstart])) {
            m_zbuffer[xstart] = iz;
            if (m_bilinear) {
              //We could (should?) add bounds checking on iu and iv here (keep in mind the 16 bit shift!).
              //This would also be the place to add looping texture mode (bounds check == clamped).
              //For looping/clamped textures, we'd also need to change PGraphics.textureVertex() to remove
              //the texture range check there (it constrains normalized texture coordinates from 0->1).

              int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
              int iui = (iu & 0xFFFF) >> 9;
              int ivi = (iv & 0xFFFF) >> 9;

              //if(ofs < 0) { ofs += TEX_WIDTH; }
              //if(ofs > m_texture.length-2) {ofs -= TEX_WIDTH; }

              // get texture pixels
              int pix0 = m_texture[ofs];
              int pix1 = m_texture[ofs + 1];
              if (ofs < lastRowStart) ofs+=TEX_WIDTH; //quick hack to thwart exceptions
              int pix2 = m_texture[ofs];
              int pix3 = m_texture[ofs + 1];

              // red
              int red0 = (pix0 & 0xFF0000);
              int red2 = (pix2 & 0xFF0000);
              int up = red0 + ((((pix1 & 0xFF0000) - red0) * iui) >> 7);
              int dn = red2 + ((((pix3 & 0xFF0000) - red2) * iui) >> 7);
              int red = up + (((dn-up) * ivi) >> 7);
              if (tint) red = ((red * rtint) >> 8) & 0xFF0000;

              // grn
              red0 = (pix0 & 0xFF00);
              red2 = (pix2 & 0xFF00);
              up = red0 + ((((pix1 & 0xFF00) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF00) - red2) * iui) >> 7);
              int grn = up + (((dn-up) * ivi) >> 7);
              if (tint) grn = ((grn * gtint) >> 8) & 0xFF00;

              // blu
              red0 = (pix0 & 0xFF);
              red2 = (pix2 & 0xFF);
              up = red0 + ((((pix1 & 0xFF) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF) - red2) * iui) >> 7);
              int blu = up + (((dn-up) * ivi) >> 7);
              if (tint) blu = ((blu * btint) >> 8) & 0xFF;

              //m_pixels[xstart] = (red & 0xFF0000) | (grn & 0xFF00) | (blu & 0xFF);
              m_pixels[xstart] = 0xFF000000 | 
                (red & 0xFF0000) | (grn & 0xFF00) | (blu & 0xFF);

            } else {
              m_pixels[xstart] = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)];
            }
//            m_stencil[xstart] = p;
          }
        } catch (Exception e) {/*exCount++;*/}
        iz+=izadd;
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
      }
      ypixel++; //accurate mode
      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      zleft+=zleftadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
    }
    //if (exCount>0) System.out.println(exCount+" exceptions in this segment");
  }


  /**
   * Alpha 24-bit texture
   */
  private void drawsegment_texture24_alpha(float leftadd,
                                           float rghtadd,
                                           int ytop,
                                           int ybottom) {
    //Accurate texture mode added - comments stripped from dupe code, see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode){
      if(precomputeAccurateTexturing()){ //see if the precomputation goes well, if so finish the setup
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        accurateMode = false; //if the matrix inversion screwed up, revert to normal rendering (something is degenerate)
      }
    }

    boolean tint = (m_fill & 0xFFFFFF) != 0xFFFFFF;
    int rtint = (m_fill >> 16) & 0xff;
    int gtint = (m_fill >> 8) & 0xff;
    int btint = m_fill & 0xFF;
    
    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    float iuf = iuadd;
    float ivf = ivadd;
    float iaf = iaadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;

      float xdiff = (xstart + PIXEL_CENTER) - xleft;
      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      int ia = (int) (iaf * xdiff + aleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }


      for ( ; xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }

        try {
          if (noDepthTest || (iz <= m_zbuffer[xstart])) {
            //m_zbuffer[xstart] = iz;

            // get alpha
            int al = ia >> 16;

            if (m_bilinear) {
              int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
              int iui = (iu & 0xFFFF) >> 9;
              int ivi = (iv & 0xFFFF) >> 9;

              // get texture pixels
              int pix0 = m_texture[ofs];
              int pix1 = m_texture[ofs + 1];
              if (ofs < lastRowStart) ofs+=TEX_WIDTH;
              int pix2 = m_texture[ofs];
              int pix3 = m_texture[ofs + 1];

              // red
              int red0 = (pix0 & 0xFF0000);
              int red2 = (pix2 & 0xFF0000);
              int up = red0 + ((((pix1 & 0xFF0000) - red0) * iui) >> 7);
              int dn = red2 + ((((pix3 & 0xFF0000) - red2) * iui) >> 7);
              int red = up + (((dn-up) * ivi) >> 7);
              if (tint) red = ((red * rtint) >> 8) & 0xFF0000;

              // grn
              red0 = (pix0 & 0xFF00);
              red2 = (pix2 & 0xFF00);
              up = red0 + ((((pix1 & 0xFF00) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF00) - red2) * iui) >> 7);
              int grn = up + (((dn-up) * ivi) >> 7);
              if (tint) grn = ((grn * gtint) >> 8) & 0xFF00;

              // blu
              red0 = (pix0 & 0xFF);
              red2 = (pix2 & 0xFF);
              up = red0 + ((((pix1 & 0xFF) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF) - red2) * iui) >> 7);
              int blu = up + (((dn-up) * ivi) >> 7);
              if (tint) blu = ((blu * btint) >> 8) & 0xFF;

              // get buffer pixels
              int bb = m_pixels[xstart];
              int br = (bb & 0xFF0000);  // 0x00FF0000
              int bg = (bb & 0xFF00);    // 0x0000FF00
              bb = (bb & 0xFF);          // 0x000000FF
              m_pixels[xstart] = 0xFF000000 | 
                ((br + (((red - br) * al) >> 8)) & 0xFF0000) |
                ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) | 
                ((bb + (((blu - bb) * al) >> 8)) & 0xFF);

            } else {
              int red = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)];
              int grn = red & 0xFF00;
              int blu = red & 0xFF;
              red&=0xFF0000;

              // get buffer pixels
              int bb = m_pixels[xstart];
              int br = (bb & 0xFF0000);  // 0x00FF0000
              int bg = (bb & 0xFF00);    // 0x0000FF00
              bb = (bb & 0xFF);          // 0x000000FF
              m_pixels[xstart] = 0xFF000000 | 
                ((br + (((red - br) * al) >> 8)) & 0xFF0000) |
                ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) |
                ((bb + (((blu - bb) * al) >> 8)) & 0xFF);
            }
//            m_stencil[xstart] = p;
          }
        } catch (Exception e) { }
        
        xpixel++; // accurate mode
        if (!accurateMode){
          iu += iuadd;
          iv += ivadd;
        }
        ia+=iaadd;
        iz+=izadd;
      }
      ypixel++; // accurate mode

      ytop+=SCREEN_WIDTH;

      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      zleft+=zleftadd;
      aleft+=aleftadd;
    }
  }

  /**
   * Plain 32-bit texutre
   */
  private void drawsegment_texture32(float leftadd,
                                     float rghtadd,
                                     int ytop,
                                     int ybottom) {
    //Accurate texture mode added - comments stripped from dupe code, see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode){
      if(precomputeAccurateTexturing()){ //see if the precomputation goes well, if so finish the setup
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        accurateMode = false; //if the matrix inversion screwed up, revert to normal rendering (something is degenerate)
      }
    }

    ytop*=SCREEN_WIDTH;
    ybottom*=SCREEN_WIDTH;
//    int p = m_index;

    boolean tint = m_fill != 0xFFFFFFFF;
    int rtint = (m_fill >> 16) & 0xff;
    int gtint = (m_fill >> 8) & 0xff;
    int btint = m_fill & 0xFF;
    
    float iuf = iuadd;
    float ivf = ivadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;

      float xdiff = (xstart + PIXEL_CENTER) - xleft;
      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }


      for ( ; xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }

        // try-catch just in case pixel offset it out of range
        try {
          if (noDepthTest || (iz <= m_zbuffer[xstart])) {
            //m_zbuffer[xstart] = iz;

            if (m_bilinear) {
              int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
              int iui = (iu & 0xFFFF) >> 9;
              int ivi = (iv & 0xFFFF) >> 9;

              // get texture pixels
              int pix0 = m_texture[ofs];
              int pix1 = m_texture[ofs + 1];
              if (ofs < lastRowStart) ofs+=TEX_WIDTH;
              int pix2 = m_texture[ofs];
              int pix3 = m_texture[ofs + 1];

              // red
              int red0 = (pix0 & 0xFF0000);
              int red2 = (pix2 & 0xFF0000);
              int up = red0 + ((((pix1 & 0xFF0000) - red0) * iui) >> 7);
              int dn = red2 + ((((pix3 & 0xFF0000) - red2) * iui) >> 7);
              int red = up + (((dn-up) * ivi) >> 7);
              if (tint) red = ((red * rtint) >> 8) & 0xFF0000;

              // grn
              red0 = (pix0 & 0xFF00);
              red2 = (pix2 & 0xFF00);
              up = red0 + ((((pix1 & 0xFF00) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF00) - red2) * iui) >> 7);
              int grn = up + (((dn-up) * ivi) >> 7);
              if (tint) grn = ((grn * gtint) >> 8) & 0xFF00;

              // blu
              red0 = (pix0 & 0xFF);
              red2 = (pix2 & 0xFF);
              up = red0 + ((((pix1 & 0xFF) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF) - red2) * iui) >> 7);
              int blu = up + (((dn-up) * ivi) >> 7);
              if (tint) blu = ((blu * btint) >> 8) & 0xFF;

              // alpha
              pix0>>>=24;
              pix2>>>=24;
              up = pix0 + ((((pix1 >>> 24) - pix0) * iui) >> 7);
              dn = pix2 + ((((pix3 >>> 24) - pix2) * iui) >> 7);
              int al = up + (((dn-up) * ivi) >> 7);

              // get buffer pixels
              int bb = m_pixels[xstart];
              int br = (bb & 0xFF0000);  // 0x00FF0000
              int bg = (bb & 0xFF00);    // 0x0000FF00
              bb = (bb & 0xFF);          // 0x000000FF
              m_pixels[xstart] = 0xFF000000 | 
                ((br + (((red - br) * al) >> 8)) & 0xFF0000) |
                ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) | 
                ((bb + (((blu - bb) * al) >> 8)) & 0xFF);
            } else {
              int red = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)];
              int al = red >>> 24;
              int grn = red & 0xFF00;
              int blu = red & 0xFF;
              red&=0xFF0000;

              // get buffer pixels
              int bb = m_pixels[xstart];
              int br = (bb & 0xFF0000);  // 0x00FF0000
              int bg = (bb & 0xFF00);    // 0x0000FF00
              bb = (bb & 0xFF);          // 0x000000FF
              m_pixels[xstart] = 0xFF000000 | 
              ((br + (((red - br) * al) >> 8)) & 0xFF0000) |
              ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) | 
              ((bb + (((blu - bb) * al) >> 8)) & 0xFF);
            }
//            m_stencil[xstart] = p;
          }
        } catch (Exception e) { }
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        iz+=izadd;
      }
      ypixel++;//accurate mode

      ytop+=SCREEN_WIDTH;

      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      zleft+=zleftadd;
      aleft+=aleftadd;
    }


  }

  /**
   * Alpha 32-bit texutre
   */
  private void drawsegment_texture32_alpha(float leftadd,
                                           float rghtadd,
                                           int ytop,
                                           int ybottom) {
    //Accurate texture mode added - comments stripped from dupe code, see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode){
      if(precomputeAccurateTexturing()){ //see if the precomputation goes well, if so finish the setup
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        accurateMode = false; //if the matrix inversion screwed up, revert to normal rendering (something is degenerate)
      }
    }

    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    boolean tint = (m_fill & 0xFFFFFF) != 0xFFFFFF;
    int rtint = (m_fill >> 16) & 0xff;
    int gtint = (m_fill >> 8) & 0xff;
    int btint = m_fill & 0xFF;

    float iuf = iuadd;
    float ivf = ivadd;
    float iaf = iaadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;

      float xdiff = (xstart + PIXEL_CENTER) - xleft;
      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      int ia = (int) (iaf * xdiff + aleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }

      for ( ; xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }

        // try-catch just in case pixel offset it out of range
        try {
          if (noDepthTest || (iz <= m_zbuffer[xstart])) {
            //m_zbuffer[xstart] = iz;

            // get alpha
            int al = ia >> 16;

            if (m_bilinear) {
              int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
              int iui = (iu & 0xFFFF) >> 9;
              int ivi = (iv & 0xFFFF) >> 9;

              // get texture pixels
              int pix0 = m_texture[ofs];
              int pix1 = m_texture[ofs + 1];
              if (ofs < lastRowStart) ofs+=TEX_WIDTH;
              int pix2 = m_texture[ofs];
              int pix3 = m_texture[ofs + 1];

              // red
              int red0 = (pix0 & 0xFF0000);
              int red2 = (pix2 & 0xFF0000);
              int up = red0 + ((((pix1 & 0xFF0000) - red0) * iui) >> 7);
              int dn = red2 + ((((pix3 & 0xFF0000) - red2) * iui) >> 7);
              int red = up + (((dn-up) * ivi) >> 7);
              if (tint) red = ((red * rtint) >> 8) & 0xFF0000;

              // grn
              red0 = (pix0 & 0xFF00);
              red2 = (pix2 & 0xFF00);
              up = red0 + ((((pix1 & 0xFF00) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF00) - red2) * iui) >> 7);
              int grn = up + (((dn-up) * ivi) >> 7);
              if (tint) grn = ((grn * gtint) >> 8) & 0xFF00;

              // blu
              red0 = (pix0 & 0xFF);
              red2 = (pix2 & 0xFF);
              up = red0 + ((((pix1 & 0xFF) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF) - red2) * iui) >> 7);
              int blu = up + (((dn-up) * ivi) >> 7);
              if (tint) blu = ((blu * btint) >> 8) & 0xFF;

              // alpha
              pix0>>>=24;
              pix2>>>=24;
              up = pix0 + ((((pix1 >>> 24) - pix0) * iui) >> 7);
              dn = pix2 + ((((pix3 >>> 24) - pix2) * iui) >> 7);
              al = al * (up + (((dn-up) * ivi) >> 7)) >> 8;

              // get buffer pixels
              int bb = m_pixels[xstart];
              int br = (bb & 0xFF0000);  // 0x00FF0000
              int bg = (bb & 0xFF00);    // 0x0000FF00
              bb = (bb & 0xFF);          // 0x000000FF
              m_pixels[xstart] = 0xFF000000 | 
              ((br + (((red - br) * al) >> 8)) & 0xFF0000) |
              ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) | 
              ((bb + (((blu - bb) * al) >> 8)) & 0xFF);
            } else {
              int red = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)];
              al = al * (red >>> 24) >> 8;
              int grn = red & 0xFF00;
              int blu = red & 0xFF;
              red&=0xFF0000;

              // get buffer pixels
              int bb = m_pixels[xstart];
              int br = (bb & 0xFF0000);  // 0x00FF0000
              int bg = (bb & 0xFF00);    // 0x0000FF00
              bb = (bb & 0xFF);          // 0x000000FF
              m_pixels[xstart] = 0xFF000000 | 
              ((br + (((red - br) * al) >> 8)) & 0xFF0000) |
              ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) | 
              ((bb + (((blu - bb) * al) >> 8)) & 0xFF);
            }
//            m_stencil[xstart] = p;
          }
        } catch (Exception e) { }
        
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        ia+=iaadd;
        iz+=izadd;
      }
      ypixel++;//accurate mode

      ytop+=SCREEN_WIDTH;

      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      zleft+=zleftadd;
      aleft+=aleftadd;
    }


  }


  /**
   * Gouraud blended with 8-bit alpha texture
   */
  private void drawsegment_gouraud_texture8(float leftadd,
                                            float rghtadd,
                                            int ytop,
                                            int ybottom) {
    //Accurate texture mode added - comments stripped from dupe code, see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode){
      if(precomputeAccurateTexturing()){ //see if the precomputation goes well, if so finish the setup
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        accurateMode = false; //if the matrix inversion screwed up, revert to normal rendering (something is degenerate)
      }
    }

    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    float iuf = iuadd;
    float ivf = ivadd;
    float irf = iradd;
    float igf = igadd;
    float ibf = ibadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;
      float xdiff = (xstart + PIXEL_CENTER) - xleft;

      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      int ir = (int) (irf * xdiff + rleft);
      int ig = (int) (igf * xdiff + gleft);
      int ib = (int) (ibf * xdiff + bleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }


      for ( ; xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }

        try
          {
            if (noDepthTest || (iz <= m_zbuffer[xstart])) {
              //m_zbuffer[xstart] = iz;

              int al0;
              if (m_bilinear) {
                int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
                int iui = iu & 0xFFFF;
                al0 = m_texture[ofs] & 0xFF;
                int al1 = m_texture[ofs + 1] & 0xFF;
                if (ofs < lastRowStart) ofs+=TEX_WIDTH;
                int al2 = m_texture[ofs] & 0xFF;
                int al3 = m_texture[ofs + 1] & 0xFF;
                al0 = al0 + (((al1-al0) * iui) >> 16);
                al2 = al2 + (((al3-al2) * iui) >> 16);
                al0 = al0 + (((al2-al0) * (iv & 0xFFFF)) >> 16);
              } else {
                al0 = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)] & 0xFF;
              }

              // get RGB colors
              int red = ir & 0xFF0000;
              int grn = (ig >> 8) & 0xFF00;
              int blu = (ib >> 16);

              // get buffer pixels
              int bb = m_pixels[xstart];
              int br = (bb & 0xFF0000);  // 0x00FF0000
              int bg = (bb & 0xFF00);    // 0x0000FF00
              bb = (bb & 0xFF);          // 0x000000FF
              m_pixels[xstart] = 0xFF000000 |
                ((br + (((red - br) * al0) >> 8)) & 0xFF0000) |
                ((bg + (((grn - bg) * al0) >> 8)) & 0xFF00) | 
                ((bb + (((blu - bb) * al0) >> 8)) & 0xFF);

              // write stencil
//              m_stencil[xstart] = p;
            }
          }
        catch (Exception e) {

        }

        //
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        ir+=iradd;
        ig+=igadd;
        ib+=ibadd;
        iz+=izadd;
      }
      ypixel++;//accurate mode
      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;

      uleft+=uleftadd;
      vleft+=vleftadd;
      rleft+=rleftadd;
      gleft+=gleftadd;
      bleft+=bleftadd;
      zleft+=zleftadd;
    }
  }


  /**
   * Texture multiplied with gouraud
   */
  private void drawsegment_gouraud_texture8_alpha(float leftadd,
                                                  float rghtadd,
                                                  int ytop,
                                                  int ybottom) {
    // Accurate texture mode added - comments stripped from dupe code, 
    // see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode) {
      // see if the precomputation goes well, if so finish the setup
      if (precomputeAccurateTexturing()) { 
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        // if the matrix inversion screwed up, 
        // revert to normal rendering (something is degenerate)
        accurateMode = false; 
      }
    }

    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    float iuf = iuadd;
    float ivf = ivadd;
    float irf = iradd;
    float igf = igadd;
    float ibf = ibadd;
    float iaf = iaadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;
      float xdiff = (xstart + PIXEL_CENTER) - xleft;

      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      int ir = (int) (irf * xdiff + rleft);
      int ig = (int) (igf * xdiff + gleft);
      int ib = (int) (ibf * xdiff + bleft);
      int ia = (int) (iaf * xdiff + aleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }


      for ( ; xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }

        try {
          if (noDepthTest || (iz <= m_zbuffer[xstart])) {
            //m_zbuffer[xstart] = iz;

            int al0;
            if (m_bilinear) {
              int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
              int iui = iu & 0xFFFF;
              al0 = m_texture[ofs] & 0xFF;
              int al1 = m_texture[ofs + 1] & 0xFF;
              if (ofs < lastRowStart) ofs+=TEX_WIDTH;
              int al2 = m_texture[ofs] & 0xFF;
              int al3 = m_texture[ofs + 1] & 0xFF;
              al0 = al0 + (((al1-al0) * iui) >> 16);
              al2 = al2 + (((al3-al2) * iui) >> 16);
              al0 = al0 + (((al2-al0) * (iv & 0xFFFF)) >> 16);
            } else {
              al0 = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)] & 0xFF;
            }
            al0 = (al0 * (ia >> 16)) >> 8;

            // get RGB colors
            int red = ir & 0xFF0000;
            int grn = (ig >> 8) & 0xFF00;
            int blu = (ib >> 16);

            // get buffer pixels
            int bb = m_pixels[xstart];
            int br = (bb & 0xFF0000);  // 0x00FF0000
            int bg = (bb & 0xFF00);    // 0x0000FF00
            bb = (bb & 0xFF);          // 0x000000FF
            
            m_pixels[xstart] = 0xFF000000 |  
              ((br + (((red - br) * al0) >> 8)) & 0xFF0000) |
              ((bg + (((grn - bg) * al0) >> 8)) & 0xFF00) | 
              ((bb + (((blu - bb) * al0) >> 8)) & 0xFF);

            // write stencil
//            m_stencil[xstart] = p;
          }
        } catch (Exception e) { }

        //
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        ir+=iradd;
        ig+=igadd;
        ib+=ibadd;
        ia+=iaadd;
        iz+=izadd;
      }
      ypixel++;//accurate mode
      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      rleft+=rleftadd;
      gleft+=gleftadd;
      bleft+=bleftadd;
      aleft+=aleftadd;
      zleft+=zleftadd;
    }
  }


  /**
   * Texture multiplied with gouraud
   */
  private void drawsegment_gouraud_texture24(float leftadd,
                                             float rghtadd,
                                             int ytop,
                                             int ybottom) {
    //Accurate texture mode added - comments stripped from dupe code, see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode){
      if(precomputeAccurateTexturing()){ //see if the precomputation goes well, if so finish the setup
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        accurateMode = false; //if the matrix inversion screwed up, revert to normal rendering (something is degenerate)
      }
    }

    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    float iuf = iuadd;
    float ivf = ivadd;
    float irf = iradd;
    float igf = igadd;
    float ibf = ibadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;
      float xdiff = (xstart + PIXEL_CENTER) - xleft;

      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      int ir = (int) (irf * xdiff + rleft);
      int ig = (int) (igf * xdiff + gleft);
      int ib = (int) (ibf * xdiff + bleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }

      for ( ; xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }

        try {
          if (noDepthTest || (iz <= m_zbuffer[xstart])) {
            m_zbuffer[xstart] = iz;

            int red;
            int grn;
            int blu;

            if (m_bilinear) {
              int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
              int iui = (iu & 0xFFFF) >> 9;
              int ivi = (iv & 0xFFFF) >> 9;

              // get texture pixels
              int pix0 = m_texture[ofs];
              int pix1 = m_texture[ofs + 1];
              if (ofs < lastRowStart) ofs+=TEX_WIDTH;
              int pix2 = m_texture[ofs];
              int pix3 = m_texture[ofs + 1];

              // red
              int red0 = (pix0 & 0xFF0000);
              int red2 = (pix2 & 0xFF0000);
              int up = red0 + ((((pix1 & 0xFF0000) - red0) * iui) >> 7);
              int dn = red2 + ((((pix3 & 0xFF0000) - red2) * iui) >> 7);
              red = up + (((dn-up) * ivi) >> 7);

              // grn
              red0 = (pix0 & 0xFF00);
              red2 = (pix2 & 0xFF00);
              up = red0 + ((((pix1 & 0xFF00) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF00) - red2) * iui) >> 7);
              grn = up + (((dn-up) * ivi) >> 7);

              // blu
              red0 = (pix0 & 0xFF);
              red2 = (pix2 & 0xFF);
              up = red0 + ((((pix1 & 0xFF) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF) - red2) * iui) >> 7);
              blu = up + (((dn-up) * ivi) >> 7);
            } else {
              // get texture pixel color
              blu = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)];
              red = (blu & 0xFF0000);
              grn = (blu & 0xFF00);
              blu = blu & 0xFF;
            }

            //
            int r = (ir >> 16);
            int g = (ig >> 16);
            // oops, namespace collision with accurate 
            // texture vector b...sorry [ewjordan]
            int bb2 = (ib >> 16); 

            m_pixels[xstart] = 0xFF000000 | 
            ((((red * r) & 0xFF000000) | ((grn * g) & 0xFF0000) | (blu * bb2)) >> 8);
//            m_stencil[xstart] = p;
          }
        } catch (Exception e) { }

        //
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        ir+=iradd;
        ig+=igadd;
        ib+=ibadd;
        iz+=izadd;
      }
      ypixel++;//accurate mode

      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      rleft+=rleftadd;
      gleft+=gleftadd;
      bleft+=bleftadd;
      zleft+=zleftadd;
    }
  }


  /**
   * Gouraud*texture blended with interpolating alpha
   */
  private void drawsegment_gouraud_texture24_alpha
    (
     float leftadd,
     float rghtadd,
     int ytop,
     int ybottom
     ) {

    //Accurate texture mode added - comments stripped from dupe code, see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode){
      if(precomputeAccurateTexturing()){ //see if the precomputation goes well, if so finish the setup
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        accurateMode = false; //if the matrix inversion screwed up, revert to normal rendering (something is degenerate)
      }
    }

    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    float iuf = iuadd;
    float ivf = ivadd;
    float irf = iradd;
    float igf = igadd;
    float ibf = ibadd;
    float iaf = iaadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;
      float xdiff = (xstart + PIXEL_CENTER) - xleft;

      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      int ir = (int) (irf * xdiff + rleft);
      int ig = (int) (igf * xdiff + gleft);
      int ib = (int) (ibf * xdiff + bleft);
      int ia = (int) (iaf * xdiff + aleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;

      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }

      for ( ;xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }

        // get texture pixel color
        try
          {
            //if (iz < m_zbuffer[xstart]) {
            if (noDepthTest || (iz <= m_zbuffer[xstart])) {  // [fry 041114]
              //m_zbuffer[xstart] = iz;

              // blend
              int al = ia >> 16;

              int red;
              int grn;
              int blu;

              if (m_bilinear) {
                int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
                int iui = (iu & 0xFFFF) >> 9;
                int ivi = (iv & 0xFFFF) >> 9;

                // get texture pixels
                int pix0 = m_texture[ofs];
                int pix1 = m_texture[ofs + 1];
                if (ofs < lastRowStart) ofs+=TEX_WIDTH;
                int pix2 = m_texture[ofs];
                int pix3 = m_texture[ofs + 1];

                // red
                int red0 = (pix0 & 0xFF0000);
                int red2 = (pix2 & 0xFF0000);
                int up = red0 + ((((pix1 & 0xFF0000) - red0) * iui) >> 7);
                int dn = red2 + ((((pix3 & 0xFF0000) - red2) * iui) >> 7);
                red = (up + (((dn-up) * ivi) >> 7)) >> 16;

                // grn
                red0 = (pix0 & 0xFF00);
                red2 = (pix2 & 0xFF00);
                up = red0 + ((((pix1 & 0xFF00) - red0) * iui) >> 7);
                dn = red2 + ((((pix3 & 0xFF00) - red2) * iui) >> 7);
                grn = (up + (((dn-up) * ivi) >> 7)) >> 8;

                // blu
                red0 = (pix0 & 0xFF);
                red2 = (pix2 & 0xFF);
                up = red0 + ((((pix1 & 0xFF) - red0) * iui) >> 7);
                dn = red2 + ((((pix3 & 0xFF) - red2) * iui) >> 7);
                blu = up + (((dn-up) * ivi) >> 7);
              } else {
                blu = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)];
                red = (blu & 0xFF0000) >> 16; // 0 - 255
                grn = (blu & 0xFF00) >> 8;    // 0 - 255
                blu = (blu & 0xFF);       // 0 - 255
              }

              // multiply with gouraud color
              red = (red * ir) >>> 8;   // 0x00FF????
              grn = (grn * ig) >>> 16;  // 0x0000FF??
              blu = (blu * ib) >>> 24;  // 0x000000FF

              // get buffer pixels
              int bb = m_pixels[xstart];
              int br = (bb & 0xFF0000); // 0x00FF0000
              int bg = (bb & 0xFF00);   // 0x0000FF00
              bb = (bb & 0xFF);         // 0x000000FF

              //
              m_pixels[xstart] = 0xFF000000 | 
                ((br + (((red - br) * al) >> 8)) & 0xFF0000) |
                ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) | 
                ((bb + (((blu - bb) * al) >> 8)) & 0xFF);
//              m_stencil[xstart] = p;
            }
          }
        catch (Exception e) {
        }

        //
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        ir+=iradd;
        ig+=igadd;
        ib+=ibadd;
        ia+=iaadd;
        iz+=izadd;
      }

      ypixel++;//accurate mode

      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      rleft+=rleftadd;
      gleft+=gleftadd;
      bleft+=bleftadd;
      aleft+=aleftadd;
      zleft+=zleftadd;
    }
  }


  /**
   * Gouraud*texture blended with interpolating alpha
   */
  private void drawsegment_gouraud_texture32
    (
     float leftadd,
     float rghtadd,
     int ytop,
     int ybottom
     ) {

    //Accurate texture mode added - comments stripped from dupe code, see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode){
      if(precomputeAccurateTexturing()){ //see if the precomputation goes well, if so finish the setup
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        accurateMode = false; //if the matrix inversion screwed up, revert to normal rendering (something is degenerate)
      }
    }

    ytop*=SCREEN_WIDTH;
    ybottom*=SCREEN_WIDTH;
    //int p = m_index;

    float iuf = iuadd;
    float ivf = ivadd;
    float irf = iradd;
    float igf = igadd;
    float ibf = ibadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;
      float xdiff = (xstart + PIXEL_CENTER) - xleft;

      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      int ir = (int) (irf * xdiff + rleft);
      int ig = (int) (igf * xdiff + gleft);
      int ib = (int) (ibf * xdiff + bleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;
      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }


      for ( ; xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }

        try {
          if (noDepthTest || (iz <= m_zbuffer[xstart])) {
            //m_zbuffer[xstart] = iz;

            int red;
            int grn;
            int blu;
            int al;

            if (m_bilinear) {
              int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
              int iui = (iu & 0xFFFF) >> 9;
              int ivi = (iv & 0xFFFF) >> 9;

              // get texture pixels
              int pix0 = m_texture[ofs];
              int pix1 = m_texture[ofs + 1];
              if (ofs < lastRowStart) ofs+=TEX_WIDTH;
              int pix2 = m_texture[ofs];
              int pix3 = m_texture[ofs + 1];

              // red
              int red0 = (pix0 & 0xFF0000);
              int red2 = (pix2 & 0xFF0000);
              int up = red0 + ((((pix1 & 0xFF0000) - red0) * iui) >> 7);
              int dn = red2 + ((((pix3 & 0xFF0000) - red2) * iui) >> 7);
              red = (up + (((dn-up) * ivi) >> 7)) >> 16;

              // grn
              red0 = (pix0 & 0xFF00);
              red2 = (pix2 & 0xFF00);
              up = red0 + ((((pix1 & 0xFF00) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF00) - red2) * iui) >> 7);
              grn = (up + (((dn-up) * ivi) >> 7)) >> 8;

              // blu
              red0 = (pix0 & 0xFF);
              red2 = (pix2 & 0xFF);
              up = red0 + ((((pix1 & 0xFF) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF) - red2) * iui) >> 7);
              blu = up + (((dn-up) * ivi) >> 7);

              // alpha
              pix0>>>=24;
              pix2>>>=24;
              up = pix0 + ((((pix1 >>> 24) - pix0) * iui) >> 7);
              dn = pix2 + ((((pix3 >>> 24) - pix2) * iui) >> 7);
              al = up + (((dn-up) * ivi) >> 7);
            } else {
              // get texture pixel color
              blu = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)];
              al = (blu >>> 24);
              red = (blu & 0xFF0000) >> 16;
              grn = (blu & 0xFF00) >> 8;
              blu = blu & 0xFF;
            }

            // multiply with gouraud color
            red = (red * ir) >>> 8;   // 0x00FF????
            grn = (grn * ig) >>> 16;  // 0x0000FF??
            blu = (blu * ib) >>> 24;  // 0x000000FF

            // get buffer pixels
            int bb = m_pixels[xstart];
            int br = (bb & 0xFF0000); // 0x00FF0000
            int bg = (bb & 0xFF00);   // 0x0000FF00
            bb = (bb & 0xFF);         // 0x000000FF

            //
            m_pixels[xstart] = 0xFF000000 | 
            ((br + (((red - br) * al) >> 8)) & 0xFF0000) |
            ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) | 
            ((bb + (((blu - bb) * al) >> 8)) & 0xFF);
          }
        } catch (Exception e) { }

        //
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        ir+=iradd;
        ig+=igadd;
        ib+=ibadd;
        iz+=izadd;
      }
      ypixel++;//accurate mode
      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      rleft+=rleftadd;
      gleft+=gleftadd;
      bleft+=bleftadd;
      zleft+=zleftadd;
    }
  }


  /**
   * Gouraud*texture blended with interpolating alpha
   */
  private void drawsegment_gouraud_texture32_alpha
    (
     float leftadd,
     float rghtadd,
     int ytop,
     int ybottom
     ) {
    //Accurate texture mode added - comments stripped from dupe code, see drawsegment_texture24() for details
    int ypixel = ytop;
    int lastRowStart = m_texture.length - TEX_WIDTH - 2;
    boolean accurateMode = parent.hints[ENABLE_ACCURATE_TEXTURES];
    float screenx = 0; float screeny = 0; float screenz = 0;
    float a = 0; float b = 0; float c = 0;
    int linearInterpPower = TEX_INTERP_POWER;
    int linearInterpLength = 1 << linearInterpPower;
    if (accurateMode){
      if(precomputeAccurateTexturing()){ //see if the precomputation goes well, if so finish the setup
        newax *= linearInterpLength;
        newbx *= linearInterpLength;
        newcx *= linearInterpLength;
        screenz = nearPlaneDepth;
        firstSegment = false;
      } else{
        accurateMode = false; //if the matrix inversion screwed up, revert to normal rendering (something is degenerate)
      }
    }

    ytop *= SCREEN_WIDTH;
    ybottom *= SCREEN_WIDTH;
//    int p = m_index;

    float iuf = iuadd;
    float ivf = ivadd;
    float irf = iradd;
    float igf = igadd;
    float ibf = ibadd;
    float iaf = iaadd;

    while (ytop < ybottom) {
      int xstart = (int) (xleft + PIXEL_CENTER);
      if (xstart < 0)
        xstart = 0;

      int xpixel = xstart;//accurate mode

      int xend = (int) (xrght + PIXEL_CENTER);
      if (xend > SCREEN_WIDTH)
        xend = SCREEN_WIDTH;
      float xdiff = (xstart + PIXEL_CENTER) - xleft;

      int iu = (int) (iuf * xdiff + uleft);
      int iv = (int) (ivf * xdiff + vleft);
      int ir = (int) (irf * xdiff + rleft);
      int ig = (int) (igf * xdiff + gleft);
      int ib = (int) (ibf * xdiff + bleft);
      int ia = (int) (iaf * xdiff + aleft);
      float iz = izadd * xdiff + zleft;

      xstart+=ytop;
      xend+=ytop;
      if (accurateMode){
        screenx = xmult*(xpixel+.5f-(SCREEN_WIDTH/2.0f));
        screeny = ymult*(ypixel+.5f-(SCREEN_HEIGHT/2.0f));
        a = screenx*ax+screeny*ay+screenz*az;
        b = screenx*bx+screeny*by+screenz*bz;
        c = screenx*cx+screeny*cy+screenz*cz;
      }
      boolean goingIn = ( (newcx > 0) == (c > 0) )?false:true;
      int interpCounter = 0;
      int deltaU = 0; int deltaV = 0;
      float fu = 0; float fv = 0;
      float oldfu = 0; float oldfv = 0;

      if (accurateMode&&goingIn){
        int rightOffset = (xend-xstart-1)%linearInterpLength;
        int leftOffset = linearInterpLength-rightOffset;
        float rightOffset2 = rightOffset / ((float)linearInterpLength);
        float leftOffset2 = leftOffset / ((float)linearInterpLength);
        interpCounter = leftOffset;
        float ao = a-leftOffset2*newax;
        float bo = b-leftOffset2*newbx;
        float co = c-leftOffset2*newcx;
        float oneoverc = 65536.0f/co;
        oldfu = (ao*oneoverc); oldfv = (bo*oneoverc);
        a += rightOffset2*newax;
        b += rightOffset2*newbx;
        c += rightOffset2*newcx;
        oneoverc = 65536.0f/c;
        fu = a*oneoverc; fv = b*oneoverc;
        deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
        deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
        iu = ( (int)oldfu )+(leftOffset-1)*deltaU; iv = ( (int)oldfv )+(leftOffset-1)*deltaV; //another "off-by-one" hack
      } else{
        float preoneoverc = 65536.0f/c;
        fu = (a*preoneoverc);
        fv = (b*preoneoverc);
      }

      for ( ;xstart < xend; xstart++ ) {
        if(accurateMode){
          if (interpCounter == linearInterpLength) interpCounter = 0;
          if (interpCounter == 0){
            a += newax;
            b += newbx;
            c += newcx;
            float oneoverc = 65536.0f/c;
            oldfu = fu; oldfv = fv;
            fu = (a*oneoverc); fv = (b*oneoverc);
            iu = (int)oldfu; iv = (int)oldfv;
            deltaU = ((int)(fu - oldfu)) >> linearInterpPower;
            deltaV = ((int)(fv - oldfv)) >> linearInterpPower;
          } else{
            iu += deltaU;
            iv += deltaV;
          }
          interpCounter++;
        }

        // get texture pixel color
        try {
          //if (iz < m_zbuffer[xstart]) {
          if (noDepthTest || (iz <= m_zbuffer[xstart])) {  // [fry 041114]
            //m_zbuffer[xstart] = iz;

            // blend
            int al = ia >> 16;

            int red;
            int grn;
            int blu;

            if (m_bilinear) {
              int ofs = (iv >> 16) * TEX_WIDTH + (iu >> 16);
              int iui = (iu & 0xFFFF) >> 9;
              int ivi = (iv & 0xFFFF) >> 9;

              // get texture pixels
              int pix0 = m_texture[ofs];
              int pix1 = m_texture[ofs + 1];
              if (ofs < lastRowStart) ofs+=TEX_WIDTH;
              int pix2 = m_texture[ofs];
              int pix3 = m_texture[ofs + 1];

              // red
              int red0 = (pix0 & 0xFF0000);
              int red2 = (pix2 & 0xFF0000);
              int up = red0 + ((((pix1 & 0xFF0000) - red0) * iui) >> 7);
              int dn = red2 + ((((pix3 & 0xFF0000) - red2) * iui) >> 7);
              red = (up + (((dn-up) * ivi) >> 7)) >> 16;

              // grn
              red0 = (pix0 & 0xFF00);
              red2 = (pix2 & 0xFF00);
              up = red0 + ((((pix1 & 0xFF00) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF00) - red2) * iui) >> 7);
              grn = (up + (((dn-up) * ivi) >> 7)) >> 8;

              // blu
              red0 = (pix0 & 0xFF);
              red2 = (pix2 & 0xFF);
              up = red0 + ((((pix1 & 0xFF) - red0) * iui) >> 7);
              dn = red2 + ((((pix3 & 0xFF) - red2) * iui) >> 7);
              blu = up + (((dn-up) * ivi) >> 7);

              // alpha
              pix0>>>=24;
              pix2>>>=24;
              up = pix0 + ((((pix1 >>> 24) - pix0) * iui) >> 7);
              dn = pix2 + ((((pix3 >>> 24) - pix2) * iui) >> 7);
              al = al * (up + (((dn-up) * ivi) >> 7)) >> 8;
            } else {
              blu = m_texture[(iv >> 16) * TEX_WIDTH + (iu >> 16)];
              al = al * (blu >>> 24) >> 8;
              red = (blu & 0xFF0000) >> 16; // 0 - 255
              grn = (blu & 0xFF00) >> 8;    // 0 - 255
              blu = (blu & 0xFF);       // 0 - 255
            }

            // multiply with gouraud color
            red = (red * ir) >>> 8;   // 0x00FF????
            grn = (grn * ig) >>> 16;  // 0x0000FF??
            blu = (blu * ib) >>> 24;  // 0x000000FF

            // get buffer pixels
            int bb = m_pixels[xstart];
            int br = (bb & 0xFF0000); // 0x00FF0000
            int bg = (bb & 0xFF00);   // 0x0000FF00
            bb = (bb & 0xFF);         // 0x000000FF

            //
            m_pixels[xstart] = 0xFF000000 | 
              ((br + (((red - br) * al) >> 8)) & 0xFF0000) |
              ((bg + (((grn - bg) * al) >> 8)) & 0xFF00) | 
              ((bb + (((blu - bb) * al) >> 8)) & 0xFF);
//            m_stencil[xstart] = p;
          }
        } catch (Exception e) { }

        //
        xpixel++;//accurate mode
        if (!accurateMode){
          iu+=iuadd;
          iv+=ivadd;
        }
        ir+=iradd;
        ig+=igadd;
        ib+=ibadd;
        ia+=iaadd;
        iz+=izadd;
      }
      ypixel++;//accurate mode
      ytop+=SCREEN_WIDTH;
      xleft+=leftadd;
      xrght+=rghtadd;
      uleft+=uleftadd;
      vleft+=vleftadd;
      rleft+=rleftadd;
      gleft+=gleftadd;
      bleft+=bleftadd;
      aleft+=aleftadd;
      zleft+=zleftadd;
    }
  }
}
