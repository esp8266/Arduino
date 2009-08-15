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
 * Code for rendering lines with P2D and P3D.
 * @author rocha
 * @author fry
 */
public class PLine implements PConstants
{
  private int[]   m_pixels;
  private float[] m_zbuffer;
  //private int[]   m_stencil;

  private int m_index;

  static final int R_COLOR   = 0x1;
  static final int R_ALPHA   = 0x2;
  static final int R_SPATIAL = 0x8;
  static final int R_THICK   = 0x4;
  static final int R_SMOOTH  = 0x10;

  private int SCREEN_WIDTH;
  private int SCREEN_HEIGHT;
  private int SCREEN_WIDTH1;
  private int SCREEN_HEIGHT1;

  public boolean INTERPOLATE_RGB;
  public boolean INTERPOLATE_ALPHA;
  public boolean INTERPOLATE_Z;
  public boolean INTERPOLATE_THICK;

  // antialias
  private boolean SMOOTH;

  // blender
  //private boolean BLENDER;

  // stroke color
  private int m_stroke;

  // draw flags
  public int  m_drawFlags;

  // vertex coordinates
  private float[] x_array;
  private float[] y_array;
  private float[] z_array;

  // vertex intensity
  private float[] r_array;
  private float[] g_array;
  private float[] b_array;
  private float[] a_array;

  // vertex offsets
  private int o0;
  private int o1;

  // start values
  private float m_r0;
  private float m_g0;
  private float m_b0;
  private float m_a0;
  private float m_z0;

  // deltas
  private float dz;

  // rgba deltas
  private float dr;
  private float dg;
  private float db;
  private float da;

  private PGraphics parent;


  public PLine(PGraphics g) {
    INTERPOLATE_Z = false;

    x_array = new float[2];
    y_array = new float[2];
    z_array = new float[2];
    r_array = new float[2];
    g_array = new float[2];
    b_array = new float[2];
    a_array = new float[2];

    this.parent = g;
  }


  public void reset() {
    // reset these in case PGraphics was resized
    SCREEN_WIDTH = parent.width;
    SCREEN_HEIGHT = parent.height;
    SCREEN_WIDTH1 = SCREEN_WIDTH-1;
    SCREEN_HEIGHT1 = SCREEN_HEIGHT-1;

    m_pixels = parent.pixels;
    //m_stencil = parent.stencil;
    if (parent instanceof PGraphics3D) {
      m_zbuffer = ((PGraphics3D) parent).zbuffer;
    }

    // other things to reset

    INTERPOLATE_RGB = false;
    INTERPOLATE_ALPHA = false;
    //INTERPOLATE_Z = false;
    m_drawFlags = 0;
    m_index = 0;
    //BLENDER = false;
  }


  public void setVertices(float x0, float y0, float z0,
                          float x1, float y1, float z1) {
    // [rocha] fixed z drawing, so whenever a line turns on
    // z interpolation, all the lines are z interpolated
    if (z0 != z1 || z0 != 0.0f || z1 != 0.0f || INTERPOLATE_Z) {
      INTERPOLATE_Z = true;
      m_drawFlags |= R_SPATIAL;
    } else {
      INTERPOLATE_Z = false;
      m_drawFlags &= ~R_SPATIAL;
    }

    z_array[0] = z0;
    z_array[1] = z1;

    x_array[0] = x0;
    x_array[1] = x1;

    y_array[0] = y0;
    y_array[1] = y1;
  }


  public void setIntensities(float r0, float g0, float b0, float a0,
                             float r1, float g1, float b1, float a1) {
    a_array[0] = (a0 * 253f + 1.0f) * 65536f;
    a_array[1] = (a1 * 253f + 1.0f) * 65536f;

    // check if we need alpha or not?
    if ((a0 != 1.0f) || (a1 != 1.0f)) {
      INTERPOLATE_ALPHA = true;
      m_drawFlags |= R_ALPHA;
    } else {
      INTERPOLATE_ALPHA = false;
      m_drawFlags &= ~R_ALPHA;
    }

    // extra scaling added to prevent color "overflood" due to rounding errors
    r_array[0] = (r0 * 253f + 1.0f) * 65536f;
    r_array[1] = (r1 * 253f + 1.0f) * 65536f;

    g_array[0] = (g0 * 253f + 1.0f) * 65536f;
    g_array[1] = (g1 * 253f + 1.0f) * 65536f;

    b_array[0] = (b0 * 253f + 1.0f) * 65536f;
    b_array[1] = (b1 * 253f + 1.0f) * 65536f;

    // check if we need to interpolate the intensity values
    if (r0 != r1) {
      INTERPOLATE_RGB = true;
      m_drawFlags |= R_COLOR;

    } else if (g0 != g1) {
      INTERPOLATE_RGB = true;
      m_drawFlags |= R_COLOR;

    } else if (b0 != b1) {
      INTERPOLATE_RGB = true;
      m_drawFlags |= R_COLOR;

    } else {
      // when plain we use the stroke color of the first vertex
      m_stroke = 0xFF000000 |
        ((int)(255*r0) << 16) | ((int)(255*g0) << 8) | (int)(255*b0);
      INTERPOLATE_RGB = false;
      m_drawFlags &= ~R_COLOR;
    }
  }


  public void setIndex(int index) {
    m_index = index;
    //BLENDER = false;
    if (m_index != -1) {
      //BLENDER = true;
    } else {
      m_index = 0;
    }
  }


  public void draw() {
    int xi;
    int yi;
    int length;
    boolean visible = true;

    if (parent.smooth) {
      SMOOTH = true;
      m_drawFlags |= R_SMOOTH;

    } else {
      SMOOTH = false;
      m_drawFlags &= ~R_SMOOTH;
    }

    /*
    // line hack
    if (parent.hints[DISABLE_FLYING_POO]) {
      float nwidth2 = -SCREEN_WIDTH;
      float nheight2 = -SCREEN_HEIGHT;
      float width2 = SCREEN_WIDTH * 2;
      float height2 = SCREEN_HEIGHT * 2;
      if ((x_array[1] < nwidth2) ||
          (x_array[1] > width2) ||
          (x_array[0] < nwidth2) ||
          (x_array[0] > width2) ||
          (y_array[1] < nheight2) ||
          (y_array[1] > height2) ||
          (y_array[0] < nheight2) ||
          (y_array[0] > height2)) {
        return;  // this is a bad line
      }
    }
    */

    ///////////////////////////////////////
    // line clipping
    visible = lineClipping();
    if (!visible) {
      return;
    }

    ///////////////////////////////////////
    // calculate line values
    int shortLen;
    int longLen;
    boolean yLonger;
    int dt;

    yLonger = false;

    // HACK for drawing lines left-to-right for rev 0069
    // some kind of bug exists with the line-stepping algorithm
    // that causes strange patterns in the anti-aliasing.
    // [040228 fry]
    //
    // swap rgba as well as the coords.. oops
    // [040712 fry]
    //
    if (x_array[1] < x_array[0]) {
      float t;

      t = x_array[1]; x_array[1] = x_array[0]; x_array[0] = t;
      t = y_array[1]; y_array[1] = y_array[0]; y_array[0] = t;
      t = z_array[1]; z_array[1] = z_array[0]; z_array[0] = t;

      t = r_array[1]; r_array[1] = r_array[0]; r_array[0] = t;
      t = g_array[1]; g_array[1] = g_array[0]; g_array[0] = t;
      t = b_array[1]; b_array[1] = b_array[0]; b_array[0] = t;
      t = a_array[1]; a_array[1] = a_array[0]; a_array[0] = t;
    }

    // important - don't change the casts
    // is needed this way for line drawing algorithm
    longLen  = (int)x_array[1] - (int)x_array[0];
    shortLen = (int)y_array[1] - (int)y_array[0];

    if (Math.abs(shortLen) > Math.abs(longLen)) {
      int swap = shortLen;
      shortLen = longLen;
      longLen = swap;
      yLonger = true;
    }

    // now we sort points so longLen is always positive
    // and we always start drawing from x[0], y[0]
    if (longLen < 0) {
      // swap order
      o0 = 1;
      o1 = 0;

      xi = (int) x_array[1];
      yi = (int) y_array[1];

      length = -longLen;

    } else {
      o0 = 0;
      o1 = 1;

      xi = (int) x_array[0];
      yi = (int) y_array[0];

      length = longLen;
    }

    // calculate dt
    if (length == 0) {
      dt = 0;
    } else {
      dt = (shortLen << 16) / longLen;
    }

    m_r0 = r_array[o0];
    m_g0 = g_array[o0];
    m_b0 = b_array[o0];

    if (INTERPOLATE_RGB) {
      dr = (r_array[o1] - r_array[o0]) / length;
      dg = (g_array[o1] - g_array[o0]) / length;
      db = (b_array[o1] - b_array[o0]) / length;
    } else {
      dr = 0;
      dg = 0;
      db = 0;
    }

    m_a0 = a_array[o0];

    if (INTERPOLATE_ALPHA) {
      da = (a_array[o1] - a_array[o0]) / length;
    } else {
      da = 0;
    }

    m_z0 = z_array[o0];
    //z0 += -0.001f; // [rocha] ugly fix for z buffer precision

    if (INTERPOLATE_Z) {
      dz = (z_array[o1] - z_array[o0]) / length;
    } else {
      dz = 0;
    }

    // draw thin points
    if (length == 0) {
      if (INTERPOLATE_ALPHA) {
        drawPoint_alpha(xi, yi);
      } else {
        drawPoint(xi, yi);
      }
      return;
    }

    /*
    // draw antialias polygon lines for non stroked polygons
    if (BLENDER && SMOOTH) {
      // fix for endpoints not being drawn
      // [rocha]
      drawPoint_alpha((int)x_array[0], (int)x_array[0]);
      drawPoint_alpha((int)x_array[1], (int)x_array[1]);

      drawline_blender(x_array[0], y_array[0], x_array[1], y_array[1]);
      return;
    }
    */

    // draw normal strokes
    if (SMOOTH) {
//      if ((m_drawFlags & R_SPATIAL) != 0) {
//        drawLine_smooth_spatial(xi, yi, dt, length, yLonger);
//      } else {
      drawLine_smooth(xi, yi, dt, length, yLonger);
//      }

    } else {
      if (m_drawFlags == 0) {
        drawLine_plain(xi, yi, dt, length, yLonger);

      } else if (m_drawFlags == R_ALPHA) {
        drawLine_plain_alpha(xi, yi, dt, length, yLonger);

      } else if (m_drawFlags == R_COLOR) {
        drawLine_color(xi, yi, dt, length, yLonger);

      } else if (m_drawFlags == (R_COLOR + R_ALPHA)) {
        drawLine_color_alpha(xi, yi, dt, length, yLonger);

      } else if (m_drawFlags == R_SPATIAL) {
        drawLine_plain_spatial(xi, yi, dt, length, yLonger);

      } else if (m_drawFlags == (R_SPATIAL + R_ALPHA)) {
        drawLine_plain_alpha_spatial(xi, yi, dt, length, yLonger);

      } else if (m_drawFlags == (R_SPATIAL + R_COLOR)) {
        drawLine_color_spatial(xi, yi, dt, length, yLonger);

      } else if (m_drawFlags == (R_SPATIAL + R_COLOR + R_ALPHA)) {
        drawLine_color_alpha_spatial(xi, yi, dt, length, yLonger);
      }
    }
  }


  public boolean lineClipping() {
    // new cohen-sutherland clipping code, as old one was buggy [toxi]
    // get the "dips" for the points to clip
    int code1 = lineClipCode(x_array[0], y_array[0]);
    int code2 = lineClipCode(x_array[1], y_array[1]);
    int dip = code1 | code2;

    if ((code1 & code2)!=0) {

      return false;

    } else if (dip != 0) {

      // now calculate the clipped points
      float a0 = 0, a1 = 1, a = 0;

      for (int i = 0; i < 4; i++) {
        if (((dip>>i)%2)==1){
          a = lineSlope(x_array[0], y_array[0], x_array[1], y_array[1], i+1);
          if (((code1 >> i) % 2) == 1) {
            a0 = (a>a0)?a:a0; // max(a,a0)
          } else {
            a1 = (a<a1)?a:a1; // min(a,a1)
          }
        }
      }

      if (a0 > a1) {
        return false;
      } else {
        float xt =  x_array[0];
        float yt =  y_array[0];

        x_array[0] = xt + a0 * (x_array[1] - xt);
        y_array[0] = yt + a0 * (y_array[1] - yt);
        x_array[1] = xt + a1 * (x_array[1] - xt);
        y_array[1] = yt + a1 * (y_array[1] - yt);

        // interpolate remaining parameters
        if (INTERPOLATE_RGB) {
          float t = r_array[0];
          r_array[0] = t + a0 * (r_array[1] - t);
          r_array[1] = t + a1 * (r_array[1] - t);
          t = g_array[0];
          g_array[0] = t + a0 * (g_array[1] - t);
          g_array[1] = t + a1 * (g_array[1] - t);
          t = b_array[0];
          b_array[0] = t + a0 * (b_array[1] - t);
          b_array[1] = t + a1 * (b_array[1] - t);
        }

        if (INTERPOLATE_ALPHA) {
          float t = a_array[0];
          a_array[0] = t + a0 * (a_array[1] - t);
          a_array[1] = t + a1 * (a_array[1] - t);
        }
      }
    }
    return true;
  }


  private int lineClipCode(float xi, float yi) {
    int xmin = 0;
    int ymin = 0;
    int xmax = SCREEN_WIDTH1;
    int ymax = SCREEN_HEIGHT1;

    //return ((yi < ymin ? 8 : 0) | (yi > ymax ? 4 : 0) |
    //        (xi < xmin ? 2 : 0) | (xi > xmax ? 1 : 0));
    //(int) added by ewjordan 6/13/07 because otherwise we sometimes clip last pixel when it should actually be displayed.
        //Currently the min values are okay because values less than 0 should not be rendered; however, bear in mind that
        //(int) casts towards zero, so without this clipping, values between -1+eps and +1-eps would all be rendered as 0.
    return ((yi < ymin ? 8 : 0) | ((int)yi > ymax ? 4 : 0) |
            (xi < xmin ? 2 : 0) | ((int)xi > xmax ? 1 : 0));
  }


  private float lineSlope(float x1, float y1, float x2, float y2, int border) {
    int xmin = 0;
    int ymin = 0;
    int xmax = SCREEN_WIDTH1;
    int ymax = SCREEN_HEIGHT1;

    switch (border) {
    case 4: return (ymin-y1)/(y2-y1);
    case 3: return (ymax-y1)/(y2-y1);
    case 2: return (xmin-x1)/(x2-x1);
    case 1: return (xmax-x1)/(x2-x1);
    }
    return -1f;
  }


  private void drawPoint(int x0, int y0) {
    float iz = m_z0;
    int offset = y0 * SCREEN_WIDTH + x0;

    if (m_zbuffer == null) {
      m_pixels[offset] = m_stroke;
      
    } else {
      if (iz <= m_zbuffer[offset]) {
        m_pixels[offset] = m_stroke;
        m_zbuffer[offset] = iz;
      }
    }
  }


  private void drawPoint_alpha(int x0, int y0) {
    int ia = (int) a_array[0];
    int pr = m_stroke & 0xFF0000;
    int pg = m_stroke & 0xFF00;
    int pb = m_stroke & 0xFF;
    float iz = m_z0;
    int offset = y0 * SCREEN_WIDTH + x0;

    if ((m_zbuffer == null) || iz <= m_zbuffer[offset]) {
      int alpha = ia >> 16;
      int r0 = m_pixels[offset];
      int g0 = r0 & 0xFF00;
      int b0 = r0 & 0xFF;
      r0 &= 0xFF0000;

      r0 = r0 + (((pr - r0) * alpha) >> 8);
      g0 = g0 + (((pg - g0) * alpha) >> 8);
      b0 = b0 + (((pb - b0) * alpha) >> 8);

      m_pixels[offset] = 0xFF000000 |
        (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
      if (m_zbuffer != null) m_zbuffer[offset] = iz;
    }
  }


  private void drawLine_plain(int x0, int y0, int dt,
                              int length, boolean vertical) {
    // new "extremely fast" line code
    // adapted from http://www.edepot.com/linee.html
    // first version modified by [toxi]
    // simplified by [rocha]
    // length must be >= 0

    //assert length>=0:length;

    int offset = 0;

    if (vertical) {
      // vertical
      length += y0;
      for (int j = 0x8000 + (x0<<16); y0 <= length; ++y0) {
        offset = y0 * SCREEN_WIDTH + (j>>16);
        m_pixels[offset] = m_stroke;
        if (m_zbuffer != null) m_zbuffer[offset] = m_z0;
        j+=dt;
      }

    } else {
      // horizontal
      length += x0;
      for (int j = 0x8000 + (y0<<16); x0 <= length; ++x0) {
        offset = (j>>16) * SCREEN_WIDTH + x0;
        m_pixels[offset] = m_stroke;
        if (m_zbuffer != null) m_zbuffer[offset] = m_z0;
        j+=dt;
      }
    }
  }


  private void drawLine_plain_alpha(int x0, int y0, int dt,
                                    int length, boolean vertical)  {
    int offset = 0;

    int pr = m_stroke & 0xFF0000;
    int pg = m_stroke & 0xFF00;
    int pb = m_stroke & 0xFF;

    int ia = (int) (m_a0);

    if (vertical) {
      length += y0;
      for (int j = 0x8000 + (x0<<16); y0 <= length; ++y0) {
        offset = y0 * SCREEN_WIDTH + (j>>16);

        int alpha = ia >> 16;
        int r0 = m_pixels[offset];
        int g0 = r0 & 0xFF00;
        int b0 = r0 & 0xFF;
        r0 &= 0xFF0000;
        r0 = r0 + (((pr - r0) * alpha) >> 8);
        g0 = g0 + (((pg - g0) * alpha) >> 8);
        b0 = b0 + (((pb - b0) * alpha) >> 8);

        m_pixels[offset] = 0xFF000000 |
          (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
        //m_zbuffer[offset] = m_z0;  // don't set zbuffer w/ alpha lines

        ia += da;
        j += dt;
      }

    } else {  // horizontal
      length += x0;
      for (int j = 0x8000 + (y0<<16); x0 <= length; ++x0) {
        offset = (j>>16) * SCREEN_WIDTH + x0;

        int alpha = ia >> 16;
        int r0 = m_pixels[offset];
        int g0 = r0 & 0xFF00;
        int b0 = r0 & 0xFF;
        r0&=0xFF0000;
        r0 = r0 + (((pr - r0) * alpha) >> 8);
        g0 = g0 + (((pg - g0) * alpha) >> 8);
        b0 = b0 + (((pb - b0) * alpha) >> 8);

        m_pixels[offset] = 0xFF000000 |
          (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
        //m_zbuffer[offset] = m_z0;  // no zbuffer w/ alpha lines

        ia += da;
        j += dt;
      }
    }
  }


  private void drawLine_color(int x0, int y0, int dt,
                              int length, boolean vertical)  {
    int offset = 0;

    int ir = (int) m_r0;
    int ig = (int) m_g0;
    int ib = (int) m_b0;

    if (vertical) {
      length += y0;
      for (int j = 0x8000 + (x0<<16); y0 <= length; ++y0) {
        offset = y0 * SCREEN_WIDTH + (j>>16);
        m_pixels[offset] = 0xFF000000 |
          ((ir & 0xFF0000) | ((ig >> 8) & 0xFF00) | (ib >> 16));
        if (m_zbuffer != null) m_zbuffer[offset] = m_z0;
        ir += dr;
        ig += dg;
        ib += db;
        j +=dt;
      }

    } else {  // horizontal
      length += x0;
      for (int j = 0x8000 + (y0<<16); x0 <= length; ++x0) {
        offset = (j>>16) * SCREEN_WIDTH + x0;
        m_pixels[offset] = 0xFF000000 |
          ((ir & 0xFF0000) | ((ig >> 8) & 0xFF00) | (ib >> 16));
        if (m_zbuffer != null) m_zbuffer[offset] = m_z0;
        ir += dr;
        ig += dg;
        ib += db;
        j += dt;
      }
    }
  }


  private void drawLine_color_alpha(int x0, int y0, int dt,
                                    int length, boolean vertical)  {
    int offset = 0;

    int ir = (int) m_r0;
    int ig = (int) m_g0;
    int ib = (int) m_b0;
    int ia = (int) m_a0;

    if (vertical) {
      length += y0;
      for (int j = 0x8000 + (x0<<16); y0 <= length; ++y0) {
        offset = y0 * SCREEN_WIDTH + (j>>16);

        int pr = ir & 0xFF0000;
        int pg = (ig >> 8) & 0xFF00;
        int pb = (ib >> 16);

        int r0 = m_pixels[offset];
        int g0 = r0 & 0xFF00;
        int b0 = r0 & 0xFF;
        r0&=0xFF0000;

        int alpha = ia >> 16;

        r0 = r0 + (((pr - r0) * alpha) >> 8);
        g0 = g0 + (((pg - g0) * alpha) >> 8);
        b0 = b0 + (((pb - b0) * alpha) >> 8);

        m_pixels[offset] = 0xFF000000 |
          (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
        if (m_zbuffer != null) m_zbuffer[offset] = m_z0;

        ir+= dr;
        ig+= dg;
        ib+= db;
        ia+= da;
        j+=dt;
      }

    } else {  // horizontal
      length += x0;
      for (int j = 0x8000 + (y0<<16); x0 <= length; ++x0) {
        offset = (j>>16) * SCREEN_WIDTH + x0;

        int pr = ir & 0xFF0000;
        int pg = (ig >> 8) & 0xFF00;
        int pb = (ib >> 16);

        int r0 = m_pixels[offset];
        int g0 = r0 & 0xFF00;
        int b0 = r0 & 0xFF;
        r0&=0xFF0000;

        int alpha = ia >> 16;

        r0 = r0 + (((pr - r0) * alpha) >> 8);
        g0 = g0 + (((pg - g0) * alpha) >> 8);
        b0 = b0 + (((pb - b0) * alpha) >> 8);

        m_pixels[offset] = 0xFF000000 |
          (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
        if (m_zbuffer != null) m_zbuffer[offset] = m_z0;

        ir+= dr;
        ig+= dg;
        ib+= db;
        ia+= da;
        j+=dt;
      }
    }
  }


  private void drawLine_plain_spatial(int x0, int y0, int dt,
                                      int length, boolean vertical)  {
    int offset = 0;
    float iz = m_z0;

    if (vertical) {
      length += y0;
      for (int j = 0x8000 + (x0<<16); y0 <= length; ++y0) {
        offset = y0 * SCREEN_WIDTH + (j>>16);
        if (offset < m_pixels.length) {
          if (iz <= m_zbuffer[offset]) {
            m_pixels[offset] = m_stroke;
            m_zbuffer[offset] = iz;
          }
        }
        iz+=dz;
        j+=dt;
      }

    } else {  // horizontal
      length += x0;
      for (int j = 0x8000 + (y0<<16); x0 <= length; ++x0) {
        offset = (j>>16) * SCREEN_WIDTH + x0;
        if (offset < m_pixels.length) {
          if (iz <= m_zbuffer[offset]) {
            m_pixels[offset] = m_stroke;
            m_zbuffer[offset] = iz;
          }
        }
        iz+=dz;
        j+=dt;
      }
    }
  }


  private void drawLine_plain_alpha_spatial(int x0, int y0, int dt,
                                            int length, boolean vertical) {
    int offset = 0;
    float iz = m_z0;

    int pr = m_stroke & 0xFF0000;
    int pg = m_stroke & 0xFF00;
    int pb = m_stroke & 0xFF;

    int ia = (int) m_a0;

    if (vertical) {
      length += y0;
      for (int j = 0x8000 + (x0<<16); y0 <= length; ++y0) {
        offset = y0 * SCREEN_WIDTH + (j>>16);
        if (offset < m_pixels.length) {
          if (iz <= m_zbuffer[offset]) {
            int alpha = ia >> 16;
            int r0 = m_pixels[offset];
            int g0 = r0 & 0xFF00;
            int b0 = r0 & 0xFF;
            r0 &= 0xFF0000;
            r0 = r0 + (((pr - r0) * alpha) >> 8);
            g0 = g0 + (((pg - g0) * alpha) >> 8);
            b0 = b0 + (((pb - b0) * alpha) >> 8);

            m_pixels[offset] = 0xFF000000 |
              (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
            m_zbuffer[offset] = iz;
          }
        }
        iz +=dz;
        ia += da;
        j += dt;
      }

    } else {  // horizontal
      length += x0;
      for (int j = 0x8000 + (y0<<16); x0 <= length; ++x0) {
        offset = (j>>16) * SCREEN_WIDTH + x0;

        if (offset < m_pixels.length) {
          if (iz <= m_zbuffer[offset]) {
            int alpha = ia >> 16;
            int r0 = m_pixels[offset];
            int g0 = r0 & 0xFF00;
            int b0 = r0 & 0xFF;
            r0&=0xFF0000;
            r0 = r0 + (((pr - r0) * alpha) >> 8);
            g0 = g0 + (((pg - g0) * alpha) >> 8);
            b0 = b0 + (((pb - b0) * alpha) >> 8);

            m_pixels[offset] = 0xFF000000 |
              (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
            m_zbuffer[offset] = iz;
          }
        }
        iz += dz;
        ia += da;
        j += dt;
      }
    }
  }


  private void drawLine_color_spatial(int x0, int y0, int dt,
                                      int length, boolean vertical)  {
    int offset = 0;
    float iz = m_z0;

    int ir = (int) m_r0;
    int ig = (int) m_g0;
    int ib = (int) m_b0;

    if (vertical) {
      length += y0;
      for (int j = 0x8000 + (x0<<16); y0 <= length; ++y0) {
        offset = y0 * SCREEN_WIDTH + (j>>16);

        if (iz <= m_zbuffer[offset]) {
          m_pixels[offset] = 0xFF000000 |
            ((ir & 0xFF0000) | ((ig >> 8) & 0xFF00) | (ib >> 16));
          m_zbuffer[offset] = iz;
        }
        iz +=dz;
        ir += dr;
        ig += dg;
        ib += db;
        j += dt;
      }
    } else {  // horizontal
      length += x0;
      for (int j = 0x8000 + (y0<<16); x0 <= length; ++x0) {
        offset = (j>>16) * SCREEN_WIDTH + x0;
        if (iz <= m_zbuffer[offset]) {
          m_pixels[offset] = 0xFF000000 |
            ((ir & 0xFF0000) | ((ig >> 8) & 0xFF00) | (ib >> 16));
          m_zbuffer[offset] = iz;
        }
        iz += dz;
        ir += dr;
        ig += dg;
        ib += db;
        j += dt;
      }
      return;
    }
  }


  private void drawLine_color_alpha_spatial(int x0, int y0, int dt,
                                            int length, boolean vertical)  {
    int offset = 0;
    float iz = m_z0;

    int ir = (int) m_r0;
    int ig = (int) m_g0;
    int ib = (int) m_b0;
    int ia = (int) m_a0;

    if (vertical) {
      length += y0;
      for (int j = 0x8000 + (x0<<16); y0 <= length; ++y0) {
        offset = y0 * SCREEN_WIDTH + (j>>16);

        if (iz <= m_zbuffer[offset]) {
          int pr = ir & 0xFF0000;
          int pg = (ig >> 8) & 0xFF00;
          int pb = (ib >> 16);

          int r0 = m_pixels[offset];
          int g0 = r0 & 0xFF00;
          int b0 = r0 & 0xFF;
          r0&=0xFF0000;

          int alpha = ia >> 16;

          r0 = r0 + (((pr - r0) * alpha) >> 8);
          g0 = g0 + (((pg - g0) * alpha) >> 8);
          b0 = b0 + (((pb - b0) * alpha) >> 8);

          m_pixels[offset] = 0xFF000000 |
            (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
          m_zbuffer[offset] = iz;
        }
        iz+=dz;
        ir+= dr;
        ig+= dg;
        ib+= db;
        ia+= da;
        j+=dt;
      }

    } else {  // horizontal
      length += x0;
      for (int j = 0x8000 + (y0<<16); x0 <= length; ++x0) {
        offset = (j>>16) * SCREEN_WIDTH + x0;

        if (iz <= m_zbuffer[offset]) {
          int pr = ir & 0xFF0000;
          int pg = (ig >> 8) & 0xFF00;
          int pb = (ib >> 16);

          int r0 = m_pixels[offset];
          int g0 = r0 & 0xFF00;
          int b0 = r0 & 0xFF;
          r0 &= 0xFF0000;

          int alpha = ia >> 16;

          r0 = r0 + (((pr - r0) * alpha) >> 8);
          g0 = g0 + (((pg - g0) * alpha) >> 8);
          b0 = b0 + (((pb - b0) * alpha) >> 8);

          m_pixels[offset] = 0xFF000000 |
            (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
          m_zbuffer[offset] = iz;
        }
        iz += dz;
        ir += dr;
        ig += dg;
        ib += db;
        ia += da;
        j += dt;
      }
    }
  }


  private void drawLine_smooth(int x0, int y0, int dt,
                               int length, boolean vertical) {
    int xi, yi; // these must be >=32 bits
    int offset = 0;
    int temp;
    int end;

    float iz = m_z0;

    int ir = (int) m_r0;
    int ig = (int) m_g0;
    int ib = (int) m_b0;
    int ia = (int) m_a0;

    if (vertical) {
      xi = x0 << 16;
      yi = y0 << 16;

      end = length + y0;

      while ((yi >> 16) < end) {

        offset = (yi>>16) * SCREEN_WIDTH + (xi>>16);

        int pr = ir & 0xFF0000;
        int pg = (ig >> 8) & 0xFF00;
        int pb = (ib >> 16);

        if ((m_zbuffer == null) || (iz <= m_zbuffer[offset])) {
          int alpha = (((~xi >> 8) & 0xFF) * (ia >> 16)) >> 8;

          int r0 = m_pixels[offset];
          int g0 = r0 & 0xFF00;
          int b0 = r0 & 0xFF;
          r0&=0xFF0000;

          r0 = r0 + (((pr - r0) * alpha) >> 8);
          g0 = g0 + (((pg - g0) * alpha) >> 8);
          b0 = b0 + (((pb - b0) * alpha) >> 8);

          m_pixels[offset] = 0xFF000000 |
            (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
          if (m_zbuffer != null) m_zbuffer[offset] = iz;
        }

        // this if() makes things slow. there should be a better way to check 
        // if the second pixel is within the image array [rocha]
        temp = ((xi>>16)+1);
        if (temp >= SCREEN_WIDTH) {
          xi += dt;
          yi += (1 << 16);
          continue;
        }

        offset = (yi>>16) * SCREEN_WIDTH + temp;

        if ((m_zbuffer == null) || (iz <= m_zbuffer[offset])) {
          int alpha = (((xi >> 8) & 0xFF) * (ia >> 16)) >> 8;

          int r0 = m_pixels[offset];
          int g0 = r0 & 0xFF00;
          int b0 = r0 & 0xFF;
          r0 &= 0xFF0000;

          r0 = r0 + (((pr - r0) * alpha) >> 8);
          g0 = g0 + (((pg - g0) * alpha) >> 8);
          b0 = b0 + (((pb - b0) * alpha) >> 8);

          m_pixels[offset] = 0xFF000000 |
            (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
          if (m_zbuffer != null) m_zbuffer[offset] = iz;
        }

        xi += dt;
        yi += (1 << 16);

        iz+=dz;
        ir+= dr;
        ig+= dg;
        ib+= db;
        ia+= da;
      }

    } else {  // horizontal
      xi = x0 << 16;
      yi = y0 << 16;
      end = length + x0;

      while ((xi >> 16) < end) {
        offset = (yi>>16) * SCREEN_WIDTH + (xi>>16);

        int pr = ir & 0xFF0000;
        int pg = (ig >> 8) & 0xFF00;
        int pb = (ib >> 16);

        if ((m_zbuffer == null) || (iz <= m_zbuffer[offset])) {
          int alpha = (((~yi >> 8) & 0xFF) * (ia >> 16)) >> 8;

          int r0 = m_pixels[offset];
          int g0 = r0 & 0xFF00;
          int b0 = r0 & 0xFF;
          r0 &= 0xFF0000;

          r0 = r0 + (((pr - r0) * alpha) >> 8);
          g0 = g0 + (((pg - g0) * alpha) >> 8);
          b0 = b0 + (((pb - b0) * alpha) >> 8);

          m_pixels[offset] = 0xFF000000 |
            (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
          if (m_zbuffer != null) m_zbuffer[offset] = iz;
        }

        // see above [rocha]
        temp = ((yi>>16)+1);
        if (temp >= SCREEN_HEIGHT) {
          xi += (1 << 16);
          yi += dt;
          continue;
        }

        offset = temp * SCREEN_WIDTH + (xi>>16);

        if ((m_zbuffer == null) || (iz <= m_zbuffer[offset])) {
          int alpha = (((yi >> 8) & 0xFF) * (ia >> 16)) >> 8;

          int r0 = m_pixels[offset];
          int g0 = r0 & 0xFF00;
          int b0 = r0 & 0xFF;
          r0&=0xFF0000;

          r0 = r0 + (((pr - r0) * alpha) >> 8);
          g0 = g0 + (((pg - g0) * alpha) >> 8);
          b0 = b0 + (((pb - b0) * alpha) >> 8);

          m_pixels[offset] = 0xFF000000 |
            (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);
          if (m_zbuffer != null) m_zbuffer[offset] = iz;
        }

        xi += (1 << 16);
        yi += dt;

        iz += dz;
        ir += dr;
        ig += dg;
        ib += db;
        ia += da;
      }
    }
  }

  
  /*
  void drawLine_smooth(int x0, int y0, int dt,
                       int length, boolean vertical) {
    int xi, yi; // these must be >=32 bits
    int offset = 0;
    int temp;
    int end;

    int ir = (int) m_r0;
    int ig = (int) m_g0;
    int ib = (int) m_b0;
    int ia = (int) m_a0;

    if (vertical) {
      xi = x0 << 16;
      yi = y0 << 16;

      end = length + y0;

      while ((yi >> 16) < end) {
        offset = (yi>>16) * SCREEN_WIDTH + (xi>>16);

        int pr = ir & 0xFF0000;
        int pg = (ig >> 8) & 0xFF00;
        int pb = (ib >> 16);

        int alpha = (((~xi >> 8) & 0xFF) * (ia >> 16)) >> 8;

        int r0 = m_pixels[offset];
        int g0 = r0 & 0xFF00;
        int b0 = r0 & 0xFF;
        r0 &= 0xFF0000;

        r0 = r0 + (((pr - r0) * alpha) >> 8);
        g0 = g0 + (((pg - g0) * alpha) >> 8);
        b0 = b0 + (((pb - b0) * alpha) >> 8);

        m_pixels[offset] = 0xFF000000 |
          (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);

        // this if() makes things slow. there should be a better way to check 
        // if the second pixel is within the image array [rocha]
        temp = ((xi>>16)+1);
        if (temp >= SCREEN_WIDTH) {
          xi += dt;
          yi += (1 << 16);
          continue;
        }

        offset = (yi>>16) * SCREEN_WIDTH + temp;

        alpha = (((xi >> 8) & 0xFF) * (ia >> 16)) >> 8;

        r0 = m_pixels[offset];
        g0 = r0 & 0xFF00;
        b0 = r0 & 0xFF;
        r0 &= 0xFF0000;

        r0 = r0 + (((pr - r0) * alpha) >> 8);
        g0 = g0 + (((pg - g0) * alpha) >> 8);
        b0 = b0 + (((pb - b0) * alpha) >> 8);

        m_pixels[offset] = 0xFF000000 |
          (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);

        xi += dt;
        yi += (1 << 16);

        ir += dr;
        ig += dg;
        ib += db;
        ia += da;
      }

    } else {  // horizontal
      xi = x0 << 16;
      yi = y0 << 16;
      end = length + x0;

      while ((xi >> 16) < end) {
        offset = (yi>>16) * SCREEN_WIDTH + (xi>>16);

        int pr = ir & 0xFF0000;
        int pg = (ig >> 8) & 0xFF00;
        int pb = (ib >> 16);

        int alpha = (((~yi >> 8) & 0xFF) * (ia >> 16)) >> 8;

        int r0 = m_pixels[offset];
        int g0 = r0 & 0xFF00;
        int b0 = r0 & 0xFF;
        r0 &= 0xFF0000;

        r0 = r0 + (((pr - r0) * alpha) >> 8);
        g0 = g0 + (((pg - g0) * alpha) >> 8);
        b0 = b0 + (((pb - b0) * alpha) >> 8);

        m_pixels[offset] = 0xFF000000 |
          (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);

        // see above [rocha]
        temp = ((yi>>16)+1);
        if (temp >= SCREEN_HEIGHT) {
          xi += (1 << 16);
          yi += dt;
          continue;
        }

        offset = temp * SCREEN_WIDTH + (xi>>16);

        alpha = (((yi >> 8) & 0xFF) * (ia >> 16)) >> 8;

        r0 = m_pixels[offset];
        g0 = r0 & 0xFF00;
        b0 = r0 & 0xFF;
        r0 &= 0xFF0000;

        r0 = r0 + (((pr - r0) * alpha) >> 8);
        g0 = g0 + (((pg - g0) * alpha) >> 8);
        b0 = b0 + (((pb - b0) * alpha) >> 8);

        m_pixels[offset] = 0xFF000000 |
          (r0 & 0xFF0000) | (g0 & 0xFF00) | (b0 & 0xFF);

        xi += (1 << 16);
        yi += dt;

        ir+= dr;
        ig+= dg;
        ib+= db;
        ia+= da;
      }
    }
  }
  */
}
