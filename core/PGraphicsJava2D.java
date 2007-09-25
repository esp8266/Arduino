/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2005-06 Ben Fry and Casey Reas

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

import java.awt.*;
import java.awt.geom.*;
import java.awt.image.*;


/**
 * Subclass for PGraphics that implements the graphics API
 * in Java 1.3+ using Java 2D.
 *
 * <p>Pixel operations too slow? As of release 0085 (the first beta),
 * the default renderer uses Java2D. It's more accurate than the renderer
 * used in alpha releases of Processing (it handles stroke caps and joins,
 * and has better polygon tessellation), but it's super slow for handling
 * pixels. At least until we get a chance to get the old 2D renderer
 * (now called P2D) working in a similar fashion, you can use
 * <TT>size(w, h, P3D)</TT> instead of <TT>size(w, h)</TT> which will
 * be faster for general pixel flipping madness. </p>
 *
 * <p>To get access to the Java 2D "Graphics2D" object for the default
 * renderer, use:
 * <PRE>Graphics2D g2 = ((PGraphicsJava2D)g).g2;</PRE>
 * This will let you do Java 2D stuff directly, but is not supported in
 * any way shape or form. Which just means "have fun, but don't complain
 * if it breaks."</p>
 */
public class PGraphicsJava2D extends PGraphics {

  public Graphics2D g2;
  GeneralPath gpath;

  int transformCount;
  AffineTransform transformStack[] =
    new AffineTransform[MATRIX_STACK_DEPTH];
  double transform[] = new double[6];

  Line2D.Float line = new Line2D.Float();
  Ellipse2D.Float ellipse = new Ellipse2D.Float();
  Rectangle2D.Float rect = new Rectangle2D.Float();
  Arc2D.Float arc = new Arc2D.Float();

  protected Color tintColorObject;

  protected Color fillColorObject;
  public boolean fillGradient;
  public Paint fillGradientObject;

  protected Color strokeColorObject;
  public boolean strokeGradient;
  public Paint strokeGradientObject;



  //////////////////////////////////////////////////////////////

  // INTERNAL


  /**
   * Constructor for the PGraphicsJava object.
   * This prototype only exists because of annoying
   * java compilers, and should not be used.
   */
  //public PGraphicsJava2D() { }


  /**
   * Constructor for the PGraphics object. Use this to ensure that
   * the defaults get set properly. In a subclass, use this(w, h)
   * as the first line of a subclass' constructor to properly set
   * the internal fields and defaults.
   *
   * @param iwidth  viewport width
   * @param iheight viewport height
   */
  public PGraphicsJava2D(int iwidth, int iheight, PApplet parent) {
    super(iwidth, iheight, parent);
    //resize(iwidth, iheight);
  }


  /**
   * Called in repsonse to a resize event, handles setting the
   * new width and height internally, as well as re-allocating
   * the pixel buffer for the new size.
   *
   * Note that this will nuke any cameraMode() settings.
   */
  public void resize(int iwidth, int iheight) {  // ignore
    //System.out.println("resize " + iwidth + " " + iheight);
    insideDrawWait();
    insideResize = true;

    width = iwidth;
    height = iheight;
    width1 = width - 1;
    height1 = height - 1;

    allocate();

    // ok to draw again
    insideResize = false;
  }


  // broken out because of subclassing for opengl
  protected void allocate() {
    image = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
    g2 = (Graphics2D) image.getGraphics();
    // can't un-set this because this may be only a resize (Bug #463)
    //defaultsInited = false;
  }



  //////////////////////////////////////////////////////////////

  // FRAME


  public void beginDraw() {
    insideResizeWait();
    insideDraw = true;

    // need to call defaults(), but can only be done when it's ok
    // to draw (i.e. for opengl, no drawing can be done outside
    // beginDraw/endDraw).
    if (!defaultsInited) defaults();

    resetMatrix(); // reset model matrix

    // reset vertices
    vertexCount = 0;
  }


  public void endDraw() {
    // hm, mark pixels as changed, because this will instantly do a full
    // copy of all the pixels to the surface.. so that's kind of a mess.
    //updatePixels();

    if (!mainDrawingSurface) {
      loadPixels();
    }
    modified = true;
    insideDraw = false;
  }



  //////////////////////////////////////////////////////////////

  // SHAPES


  public void beginShape(int kind) {
    //super.beginShape(kind);
    shape = kind;
    vertexCount = 0;
    splineVertexCount = 0;

    // set gpath to null, because when mixing curves and straight
    // lines, vertexCount will be set back to zero, so vertexCount == 1
    // is no longer a good indicator of whether the shape is new.
    // this way, just check to see if gpath is null, and if it isn't
    // then just use it to continue the shape.
    gpath = null;
  }


  public void textureMode(int mode) {
    unavailableError("textureMode(mode)");
  }


  public void texture(PImage image) {
    unavailableError("texture(image)");
  }


  public void vertex(float x, float y) {
    splineVertexCount = 0;
    //float vertex[];

    if (vertexCount == vertices.length) {
      float temp[][] = new float[vertexCount<<1][VERTEX_FIELD_COUNT];
      System.arraycopy(vertices, 0, temp, 0, vertexCount);
      vertices = temp;
      //message(CHATTER, "allocating more vertices " + vertices.length);
    }
    // not everyone needs this, but just easier to store rather
    // than adding another moving part to the code...
    vertices[vertexCount][MX] = x;
    vertices[vertexCount][MY] = y;
    vertexCount++;

    switch (shape) {

    case POINTS:
      point(x, y);
      break;

    case LINES:
      if ((vertexCount % 2) == 0) {
        line(vertices[vertexCount-2][MX],
             vertices[vertexCount-2][MY], x, y);
      }
      break;

/*
    case LINE_STRIP:
    case LINE_LOOP:
      if (gpath == null) {
        gpath = new GeneralPath();
        gpath.moveTo(x, y);
      } else {
        gpath.lineTo(x, y);
      }
      break;
*/

    case TRIANGLES:
      if ((vertexCount % 3) == 0) {
        triangle(vertices[vertexCount - 3][MX],
                 vertices[vertexCount - 3][MY],
                 vertices[vertexCount - 2][MX],
                 vertices[vertexCount - 2][MY],
                 x, y);
      }
      break;

    case TRIANGLE_STRIP:
      if (vertexCount >= 3) {
        triangle(vertices[vertexCount - 2][MX],
                 vertices[vertexCount - 2][MY],
                 vertices[vertexCount - 1][MX],
                 vertices[vertexCount - 1][MY],
                 vertices[vertexCount - 3][MX],
                 vertices[vertexCount - 3][MY]);
      }
      break;

    case TRIANGLE_FAN:
      if (vertexCount == 3) {
        triangle(vertices[0][MX], vertices[0][MY],
                 vertices[1][MX], vertices[1][MY],
                 x, y);
      } else if (vertexCount > 3) {
        gpath = new GeneralPath();
        // when vertexCount > 3, draw an un-closed triangle
        // for indices 0 (center), previous, current
        gpath.moveTo(vertices[0][MX],
                     vertices[0][MY]);
        gpath.lineTo(vertices[vertexCount - 2][MX],
                    vertices[vertexCount - 2][MY]);
        gpath.lineTo(x, y);
        draw_shape(gpath);
      }
      break;

    case QUADS:
      if ((vertexCount % 4) == 0) {
        quad(vertices[vertexCount - 4][MX],
             vertices[vertexCount - 4][MY],
             vertices[vertexCount - 3][MX],
             vertices[vertexCount - 3][MY],
             vertices[vertexCount - 2][MX],
             vertices[vertexCount - 2][MY],
             x, y);
      }
      break;

    case QUAD_STRIP:
      // 0---2---4
      // |   |   |
      // 1---3---5
      if ((vertexCount >= 4) && ((vertexCount % 2) == 0)) {
        quad(vertices[vertexCount - 4][MX],
             vertices[vertexCount - 4][MY],
             vertices[vertexCount - 2][MX],
             vertices[vertexCount - 2][MY],
             x, y,
             vertices[vertexCount - 3][MX],
             vertices[vertexCount - 3][MY]);
      }
      break;

    case POLYGON:
      if (gpath == null) {
        gpath = new GeneralPath();
        gpath.moveTo(x, y);
      } else if (breakShape) {
        gpath.moveTo(x, y);
        breakShape = false;
      } else {
        gpath.lineTo(x, y);
      }
      break;
    }
  }


  public void vertex(float x, float y, float u, float v) {
    unavailableError("vertex(x, y, u, v");
  }


  public void vertex(float x, float y, float z) {
    depthErrorXYZ("vertex");
  }


  public void vertex(float x, float y, float z, float u, float v) {
    depthErrorXYZ("vertex");
  }


  public void bezierVertex(float x1, float y1,
                           float x2, float y2,
                           float x3, float y3) {
    if (gpath == null) {
      throw new RuntimeException("Must call vertex() at least once " +
                                 "before using bezierVertex()");
    }

    switch (shape) {
      //case LINE_LOOP:
      //case LINE_STRIP:
      case POLYGON:
        gpath.curveTo(x1, y1, x2, y2, x3, y3);
        break;

    default:
      throw new RuntimeException("bezierVertex() can only be used with " +
                                 "LINE_STRIP, LINE_LOOP, or POLYGON");
    }
  }


  float curveX[] = new float[4];
  float curveY[] = new float[4];

  public void curveVertex(float x, float y) {
    //if ((shape != LINE_LOOP) && (shape != LINE_STRIP) && (shape != POLYGON)) {
    if (shape != POLYGON) {
      throw new RuntimeException("curveVertex() can only be used with " +
                                 "POLYGON shapes");
                                 //"LINE_LOOP, LINE_STRIP, and POLYGON shapes");
    }

    if (!curve_inited) curve_init();
    vertexCount = 0;

    if (splineVertices == null) {
      splineVertices = new float[DEFAULT_SPLINE_VERTICES][VERTEX_FIELD_COUNT];
    }

    // if more than 128 points, shift everything back to the beginning
    if (splineVertexCount == DEFAULT_SPLINE_VERTICES) {
      System.arraycopy(splineVertices[DEFAULT_SPLINE_VERTICES - 3], 0,
                       splineVertices[0], 0, VERTEX_FIELD_COUNT);
      System.arraycopy(splineVertices[DEFAULT_SPLINE_VERTICES - 2], 0,
                       splineVertices[1], 0, VERTEX_FIELD_COUNT);
      System.arraycopy(splineVertices[DEFAULT_SPLINE_VERTICES - 1], 0,
                       splineVertices[2], 0, VERTEX_FIELD_COUNT);
      splineVertexCount = 3;
    }

    // this new guy will be the fourth point (or higher),
    // which means it's time to draw segments of the curve
    if (splineVertexCount >= 3) {
      curveX[0] = splineVertices[splineVertexCount-3][MX];
      curveY[0] = splineVertices[splineVertexCount-3][MY];

      curveX[1] = splineVertices[splineVertexCount-2][MX];
      curveY[1] = splineVertices[splineVertexCount-2][MY];

      curveX[2] = splineVertices[splineVertexCount-1][MX];
      curveY[2] = splineVertices[splineVertexCount-1][MY];

      curveX[3] = x;
      curveY[3] = y;

      curveToBezierMatrix.mult(curveX, curveX);
      curveToBezierMatrix.mult(curveY, curveY);

      // since the paths are continuous,
      // only the first point needs the actual moveto
      if (gpath == null) {
        gpath = new GeneralPath();
        gpath.moveTo(curveX[0], curveY[0]);
      }

      gpath.curveTo(curveX[1], curveY[1],
                    curveX[2], curveY[2],
                    curveX[3], curveY[3]);
    }

    // add the current point to the list
    splineVertices[splineVertexCount][MX] = x;
    splineVertices[splineVertexCount][MY] = y;
    splineVertexCount++;
  }


  boolean breakShape;
  public void breakShape() {
    breakShape = true;
  }


  public void endShape(int mode) {
    if (gpath != null) {  // make sure something has been drawn
      if (shape == POLYGON) {
        if (mode == CLOSE) {
          gpath.closePath();
        }
        draw_shape(gpath);
      }
    }
    shape = 0;
  }



  //////////////////////////////////////////////////////////////


  /*
  protected void fillGradient(Paint paint) {
    fillGradient = true;
    fillGradientObject = paint;
  }


  protected void noFillGradient() {
    fillGradient = false;
  }
  */


  //////////////////////////////////////////////////////////////


  protected void fill_shape(Shape s) {
    if (fillGradient) {
      g2.setPaint(fillGradientObject);
      g2.fill(s);
    } else if (fill) {
      g2.setColor(fillColorObject);
      g2.fill(s);
    }
  }

  protected void stroke_shape(Shape s) {
    if (strokeGradient) {
      g2.setPaint(strokeGradientObject);
      g2.draw(s);
    } else if (stroke) {
      g2.setColor(strokeColorObject);
      g2.draw(s);
    }
  }

  protected void draw_shape(Shape s) {
    if (fillGradient) {
      g2.setPaint(fillGradientObject);
      g2.fill(s);
    } else if (fill) {
      g2.setColor(fillColorObject);
      g2.fill(s);
    }
    if (strokeGradient) {
      g2.setPaint(strokeGradientObject);
      g2.draw(s);
    } else if (stroke) {
      g2.setColor(strokeColorObject);
      g2.draw(s);
    }
  }


  //////////////////////////////////////////////////////////////


  public void point(float x, float y) {
    line(x, y, x, y);
  }


  public void line(float x1, float y1, float x2, float y2) {
    //graphics.setColor(strokeColorObject);
    //graphics.drawLine(x1, y1, x2, y2);
    line.setLine(x1, y1, x2, y2);
    stroke_shape(line);
  }


  public void triangle(float x1, float y1, float x2, float y2,
                       float x3, float y3) {
    gpath = new GeneralPath();
    gpath.moveTo(x1, y1);
    gpath.lineTo(x2, y2);
    gpath.lineTo(x3, y3);
    gpath.closePath();

    draw_shape(gpath);
  }


  public void quad(float x1, float y1, float x2, float y2,
                   float x3, float y3, float x4, float y4) {
    GeneralPath gp = new GeneralPath();
    gp.moveTo(x1, y1);
    gp.lineTo(x2, y2);
    gp.lineTo(x3, y3);
    gp.lineTo(x4, y4);
    gp.closePath();

    draw_shape(gp);
  }


  //////////////////////////////////////////////////////////////


  protected void rectImpl(float x1, float y1, float x2, float y2) {
    rect.setFrame(x1, y1, x2-x1, y2-y1);
    draw_shape(rect);
  }


  protected void ellipseImpl(float x, float y, float w, float h) {
    ellipse.setFrame(x, y, w, h);
    draw_shape(ellipse);
  }


  protected void arcImpl(float x, float y, float w, float h,
                         float start, float stop) {
    // 0 to 90 in java would be 0 to -90 for p5 renderer
    // but that won't work, so -90 to 0?

    if (stop - start >= TWO_PI) {
      start = 0;
      stop = 360;

    } else {
      start = -start * RAD_TO_DEG;
      stop = -stop * RAD_TO_DEG;

      // ok to do this because already checked for NaN
      while (start < 0) {
        start += 360;
        stop += 360;
      }
      if (start > stop) {
        float temp = start;
        start = stop;
        stop = temp;
      }
    }
    float span = stop - start;

    // stroke as Arc2D.OPEN, fill as Arc2D.PIE
    if (fill) {
      //System.out.println("filla");
      arc.setArc(x, y, w, h, start, span, Arc2D.PIE);
      fill_shape(arc);
    }
    if (stroke) {
      //System.out.println("strokey");
      arc.setArc(x, y, w, h, start, span, Arc2D.OPEN);
      stroke_shape(arc);
    }
  }


  //////////////////////////////////////////////////////////////


  /** Ignored (not needed) in Java 2D. */
  public void bezierDetail(int detail) {
  }


  /** Ignored (not needed) in Java 2D. */
  public void curveDetail(int detail) {
  }


  //////////////////////////////////////////////////////////////


  /**
   * Handle renderer-specific image drawing.
   */
  protected void imageImpl(PImage who,
                           float x1, float y1, float x2, float y2,
                           int u1, int v1, int u2, int v2) {
    if (who.cache != null) {
      if (!(who.cache instanceof ImageCache)) {
        // this cache belongs to another renderer.. fix me later,
        // because this is gonna make drawing *really* inefficient
        //who.cache = null;
      }
    }

    if (who.cache == null) {
      //System.out.println("making new image cache");
      who.cache = new ImageCache(who);
      who.updatePixels();  // mark the whole thing for update
      who.modified = true;
    }

    ImageCache cash = (ImageCache) who.cache;
    // if image previously was tinted, or the color changed
    // or the image was tinted, and tint is now disabled
    if ((tint && !cash.tinted) ||
        (tint && (cash.tintedColor != tintColor)) ||
        (!tint && cash.tinted)) {
      // for tint change, mark all pixels as needing update
      who.updatePixels();
    }

    if (who.modified) {
      cash.update(tint, tintColor);
      who.modified = false;
    }

    g2.drawImage(((ImageCache) who.cache).image,
                 (int) x1, (int) y1, (int) x2, (int) y2,
                 u1, v1, u2, v2, null);
  }


  class ImageCache {
    PImage source;
    boolean tinted;
    int tintedColor;
    int tintedPixels[];
    BufferedImage image;

    public ImageCache(PImage source) {
      this.source = source;
      // even if RGB, set the image type to ARGB, because the
      // image may have an alpha value for its tint().
      int type = BufferedImage.TYPE_INT_ARGB;
      //System.out.println("making new buffered image");
      image = new BufferedImage(source.width, source.height, type);
    }

    // for rev 0124, passing the tintColor in here. the problem is that
    // the 'parent' PGraphics object of this inner class may not be
    // the same one that's used when drawing. for instance, if this
    // is a font used by the main drawing surface, then it's later
    // used in an offscreen PGraphics, the tintColor value from the
    // original PGraphics will be used.
    public void update(boolean tint, int tintColor) {
      if (tintedPixels == null) {
        //System.out.println("tinted pixels null");
        tintedPixels = new int[source.width * source.height];
      }

      if ((source.format == ARGB) || (source.format == RGB)) {
        if (tint) {
          // create tintedPixels[] if necessary
          //if (tintedPixels == null) {
          //  tintedPixels = new int[source.width * source.height];
          //}

          int a2 = (tintColor >> 24) & 0xff;
          int r2 = (tintColor >> 16) & 0xff;
          int g2 = (tintColor >> 8) & 0xff;
          int b2 = (tintColor) & 0xff;

          // multiply each of the color components into tintedPixels
          // if straight RGB image, don't bother multiplying
          // (also avoids problems if high bits not set)
          if (source.format == RGB) {
            int alpha = a2 << 24;

            for (int i = 0; i < tintedPixels.length; i++) {
              int argb1 = source.pixels[i];
              int r1 = (argb1 >> 16) & 0xff;
              int g1 = (argb1 >> 8) & 0xff;
              int b1 = (argb1) & 0xff;

              tintedPixels[i] = alpha |
                (((r2 * r1) & 0xff00) << 8) |
                ((g2 * g1) & 0xff00) |
                (((b2 * b1) & 0xff00) >> 8);
            }

          } else {
            for (int i = 0; i < tintedPixels.length; i++) {
              int argb1 = source.pixels[i];
              int a1 = (argb1 >> 24) & 0xff;
              int r1 = (argb1 >> 16) & 0xff;
              int g1 = (argb1 >> 8) & 0xff;
              int b1 = (argb1) & 0xff;

              tintedPixels[i] =
                (((a2 * a1) & 0xff00) << 16) |
                (((r2 * r1) & 0xff00) << 8) |
                ((g2 * g1) & 0xff00) |
                (((b2 * b1) & 0xff00) >> 8);
            }
          }

          tinted = true;
          tintedColor = tintColor;

          // finally, do a setRGB based on tintedPixels
          //image.setRGB(0, 0, source.width, source.height,
          //             tintedPixels, 0, source.width);
          WritableRaster raster = ((BufferedImage) image).getRaster();
          raster.setDataElements(0, 0, source.width, source.height,
                                 tintedPixels);

        } else {  // no tint
          // just do a setRGB like before
          // (and we'll just hope that the high bits are set)
          //image.setRGB(0, 0, source.width, source.height,
          //             source.pixels, 0, source.width);
          WritableRaster raster = ((BufferedImage) image).getRaster();
          raster.setDataElements(0, 0, source.width, source.height,
                                 source.pixels);
        }

      } else if (source.format == ALPHA) {
        int lowbits = tintColor & 0x00ffffff;
        if (((tintColor >> 24) & 0xff) >= 254) {
          //PApplet.println("  no alfa " + PApplet.hex(tintColor));
          // no actual alpha to the tint, set the image's alpha
          // as the high 8 bits, and use the color as the low 24 bits
          for (int i = 0; i < tintedPixels.length; i++) {
            // don't bother with the math if value is zero
            tintedPixels[i] = (source.pixels[i] == 0) ?
              0 : (source.pixels[i] << 24) | lowbits;
          }

        } else {
          //PApplet.println("  yes alfa " + PApplet.hex(tintColor));
          // multiply each image alpha by the tint alpha
          int alphabits = (tintColor >> 24) & 0xff;
          for (int i = 0; i < tintedPixels.length; i++) {
            tintedPixels[i] = (source.pixels[i] == 0) ?
              0 : (((alphabits * source.pixels[i]) & 0xFF00) << 16) | lowbits;
          }
        }

        // mark the pixels for next time
        tinted = true;
        tintedColor = tintColor;

        // finally, do a setRGB based on tintedPixels
        //image.setRGB(0, 0, source.width, source.height,
        //             tintedPixels, 0, source.width);
        WritableRaster raster = ((BufferedImage) image).getRaster();
        raster.setDataElements(0, 0, source.width, source.height, tintedPixels);
      }
    }
  }


  //////////////////////////////////////////////////////////////


  public float textAscent() {
    if (textFontNative == null) {
      return super.textAscent();
    }
    return textFontNativeMetrics.getAscent();
  }


  public float textDescent() {
    if (textFontNative == null) {
      return super.textDescent();
    }
    return textFontNativeMetrics.getDescent();
  }


  /**
   * Same as parent, but override for native version of the font.
   * <p/>
   * Also gets called by textFont, so the metrics
   * will get recorded properly.
   */
  public void textSize(float size) {
    // if a native version available, subset this font
    if (textFontNative != null) {
      textFontNative = textFontNative.deriveFont(size);
      g2.setFont(textFontNative);
      textFontNativeMetrics = g2.getFontMetrics(textFontNative);
    }

    // take care of setting the textSize and textLeading vars
    // this has to happen second, because it calls textAscent()
    // (which requires the native font metrics to be set)
    super.textSize(size);
  }


  protected float textWidthImpl(char buffer[], int start, int stop) {
    if (textFontNative == null) {
      //System.out.println("native is null");
      return super.textWidthImpl(buffer, start, stop);
    }
    // maybe should use one of the newer/fancier functions for this?
    int length = stop - start;
    return textFontNativeMetrics.charsWidth(buffer, start, length);
  }


  protected void textLinePlacedImpl(char buffer[], int start, int stop,
                                    float x, float y) {
    if (textFontNative == null) {
      super.textLinePlacedImpl(buffer, start, stop, x, y);
      return;
    }

    /*
    // save the current setting for text smoothing. note that this is
    // different from the smooth() function, because the font smoothing
    // is controlled when the font is created, not now as it's drawn.
    // fixed a bug in 0116 that handled this incorrectly.
    Object textAntialias =
      g2.getRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING);

    // override the current text smoothing setting based on the font
    // (don't change the global smoothing settings)
    g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
                        textFont.smooth ?
                        RenderingHints.VALUE_ANTIALIAS_ON :
                        RenderingHints.VALUE_ANTIALIAS_OFF);
    */

    Object antialias =
      g2.getRenderingHint(RenderingHints.KEY_ANTIALIASING);
    if (antialias == null) {
      // if smooth() and noSmooth() not called, this will be null (0120)
      antialias = RenderingHints.VALUE_ANTIALIAS_DEFAULT;
    }

    // override the current smoothing setting based on the font
    // also changes global setting for antialiasing, but this is because it's
    // not possible to enable/disable them independently in some situations.
    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                        textFont.smooth ?
                        RenderingHints.VALUE_ANTIALIAS_ON :
                        RenderingHints.VALUE_ANTIALIAS_OFF);


    g2.setColor(fillColorObject);
    // better to use drawString(float, float)?
    int length = stop - start;
    g2.drawChars(buffer, start, length, (int) (x + 0.5f), (int) (y + 0.5f));

    // this didn't seem to help the scaling issue
    // and creates garbage because of the new temporary object
    //java.awt.font.GlyphVector gv = textFontNative.createGlyphVector(g2.getFontRenderContext(), new String(buffer, start, stop));
    //g2.drawGlyphVector(gv, x, y);

    // return to previous smoothing state if it was changed
    //g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING, textAntialias);
    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING, antialias);

    textX = x + textWidthImpl(buffer, start, stop);
    textY = y;
    textZ = 0;  // this will get set by the caller if non-zero
  }


  //////////////////////////////////////////////////////////////


  public void translate(float tx, float ty) {
    g2.translate(tx, ty);
  }


  public void rotate(float angle) {
    g2.rotate(angle);
  }


  public void scale(float s) {
    g2.scale(s, s);
  }


  public void scale(float sx, float sy) {
    g2.scale(sx, sy);
  }


  //////////////////////////////////////////////////////////////


  public void pushMatrix() {
    if (transformCount == transformStack.length) {
      throw new RuntimeException("pushMatrix() cannot use push more than " +
                                 transformStack.length + " times");
    }
    transformStack[transformCount] = g2.getTransform();
    transformCount++;
  }


  public void popMatrix() {
    if (transformCount == 0) {
      throw new RuntimeException("missing a popMatrix() " +
                                 "to go with that pushMatrix()");
    }
    transformCount--;
    g2.setTransform(transformStack[transformCount]);
  }


  public void resetMatrix() {
    g2.setTransform(new AffineTransform());
  }


  public void applyMatrix(float n00, float n01, float n02,
                          float n10, float n11, float n12) {
    g2.transform(new AffineTransform(n00, n10, n01, n11, n02, n12));
  }


  public void loadMatrix() {
    g2.getTransform().getMatrix(transform);

    m00 = (float) transform[0];
    m01 = (float) transform[2];
    m02 = (float) transform[4];

    m10 = (float) transform[1];
    m11 = (float) transform[3];
    m12 = (float) transform[5];
  }


  public float screenX(float x, float y) {
    loadMatrix();
    return super.screenX(x, y);
    //g2.getTransform().getMatrix(transform);
    //return (float)transform[0]*x + (float)transform[2]*y + (float)transform[4];
  }


  public float screenY(float x, float y) {
    loadMatrix();
    return super.screenY(x, y);
    //g2.getTransform().getMatrix(transform);
    //return (float)transform[1]*x + (float)transform[3]*y + (float)transform[5];
  }


  //////////////////////////////////////////////////////////////


  protected void tintFromCalc() {
    super.tintFromCalc();
    // TODO actually implement tinted images
    tintColorObject = new Color(tintColor, true);
  }

  protected void fillFromCalc() {
    super.fillFromCalc();
    fillColorObject = new Color(fillColor, true);
    fillGradient = false;
  }

  protected void strokeFromCalc() {
    super.strokeFromCalc();
    strokeColorObject = new Color(strokeColor, true);
    strokeGradient = false;
  }


  //////////////////////////////////////////////////////////////


  public void strokeWeight(float weight) {
    super.strokeWeight(weight);
    set_stroke();
  }


  public void strokeJoin(int join) {
    super.strokeJoin(join);
    set_stroke();
  }


  public void strokeCap(int cap) {
    super.strokeCap(cap);
    set_stroke();
  }


  protected void set_stroke() {
    int cap = BasicStroke.CAP_BUTT;
    if (strokeCap == ROUND) {
      cap = BasicStroke.CAP_ROUND;
    } else if (strokeCap == PROJECT) {
      cap = BasicStroke.CAP_SQUARE;
    }

    int join = BasicStroke.JOIN_BEVEL;
    if (strokeJoin == MITER) {
      join = BasicStroke.JOIN_MITER;
    } else if (strokeJoin == ROUND) {
      join = BasicStroke.JOIN_ROUND;
    }

    g2.setStroke(new BasicStroke(strokeWeight, cap, join));
  }


  //////////////////////////////////////////////////////////////


  public void background(PImage image) {
    if ((image.width != width) || (image.height != height)) {
      throw new RuntimeException("background image must be " +
                                 "the same size as your application");
    }
    if ((image.format != RGB) && (image.format != ARGB)) {
      throw new RuntimeException("background images should be RGB or ARGB");
    }
    // draw the image to screen without any transformations
    set(0, 0, image);
  }


  int[] clearPixels;

  public void clear() {
    // the only way to properly clear the screen is to re-allocate
    if (backgroundAlpha) {
      // clearRect() doesn't work because it just makes everything black.
      // instead, just wipe out the canvas to its transparent original
      //allocate();

      // allocate also won't work, because all the settings
      // (like smooth) will be completely reset.
      // Instead, create a small array that can be used to set the pixels
      // several times. Using a single-pixel line of length 'width' is a
      // tradeoff between speed (setting each pixel individually is too slow)
      // and memory (an array for width*height would waste lots of memory
      // if it stayed resident, and would terrify the gc if it were
      // re-created on each trip to background().
      WritableRaster raster = ((BufferedImage) image).getRaster();
      if ((clearPixels == null) || (clearPixels.length < width)) {
        clearPixels = new int[width];
      }
      for (int i = 0; i < width; i++) {
        clearPixels[i] = backgroundColor;
      }
      for (int i = 0; i < height; i++) {
        raster.setDataElements(0, i, width, 1, clearPixels);
      }
    } else {
      // in case people do transformations before background(),
      // need to handle this with a push/reset/pop
      pushMatrix();
      resetMatrix();
      g2.setColor(new Color(backgroundColor, backgroundAlpha));
      g2.fillRect(0, 0, width, height);
      popMatrix();
    }
  }



  //////////////////////////////////////////////////////////////

  // FROM PIMAGE


  public void smooth() {
    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                        RenderingHints.VALUE_ANTIALIAS_ON);
    g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION,
                        RenderingHints.VALUE_INTERPOLATION_BICUBIC);
  }


  public void noSmooth() {
    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                        RenderingHints.VALUE_ANTIALIAS_OFF);
    g2.setRenderingHint(RenderingHints.KEY_INTERPOLATION,
                        RenderingHints.VALUE_INTERPOLATION_NEAREST_NEIGHBOR);
  }



  //////////////////////////////////////////////////////////////


  public void beginRaw(PGraphics recorderRaw) {
    throw new RuntimeException("beginRaw() not available with this renderer");
  }


  public void endRaw() {
  }


  //////////////////////////////////////////////////////////////


  public void loadPixels() {
    if ((pixels == null) || (pixels.length != width * height)) {
      pixels = new int[width * height];
    }
    //((BufferedImage) image).getRGB(0, 0, width, height, pixels, 0, width);
    WritableRaster raster = ((BufferedImage) image).getRaster();
    raster.getDataElements(0, 0, width, height, pixels);
  }


  /**
   * Update the pixels[] buffer to the PGraphics image.
   * <P>
   * Unlike in PImage, where updatePixels() only requests that the
   * update happens, in PGraphicsJava2D, this will happen immediately.
   */
  public void updatePixels() {
    //updatePixels(0, 0, width, height);
    WritableRaster raster = ((BufferedImage) image).getRaster();
    raster.setDataElements(0, 0, width, height, pixels);
  }


  /**
   * Update the pixels[] buffer to the PGraphics image.
   * <P>
   * Unlike in PImage, where updatePixels() only requests that the
   * update happens, in PGraphicsJava2D, this will happen immediately.
   */
  public void updatePixels(int x, int y, int c, int d) {
    if ((x == 0) && (y == 0) && (c == width) && (d == height)) {
      updatePixels();
    } else {
      throw new RuntimeException("updatePixels(x, y, c, d) not implemented");
    }
    /*
    ((BufferedImage) image).setRGB(x, y,
                                   (imageMode == CORNER) ? c : (c - x),
                                   (imageMode == CORNER) ? d : (d - y),
                                   pixels, 0, width);
    WritableRaster raster = ((BufferedImage) image).getRaster();
    raster.setDataElements(x, y,
                           (imageMode == CORNER) ? c : (c - x),
                           (imageMode == CORNER) ? d : (d - y),
                           pixels);
  */
  }


  //////////////////////////////////////////////////////////////


  static int getset[] = new int[1];


  public int get(int x, int y) {
    if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) return 0;
    //return ((BufferedImage) image).getRGB(x, y);
    WritableRaster raster = ((BufferedImage) image).getRaster();
    raster.getDataElements(x, y, getset);
    return getset[0];
  }


  public PImage get(int x, int y, int w, int h) {
    if (imageMode == CORNERS) {  // if CORNER, do nothing
      // w/h are x2/y2 in this case, bring em down to size
      w = (w - x);
      h = (h - x);
    }

    if (x < 0) {
      w += x; // clip off the left edge
      x = 0;
    }
    if (y < 0) {
      h += y; // clip off some of the height
      y = 0;
    }

    if (x + w > width) w = width - x;
    if (y + h > height) h = height - y;

    PImage output = new PImage(w, h);
    output.parent = parent;

    // oops, the last parameter is the scan size of the *target* buffer
    //((BufferedImage) image).getRGB(x, y, w, h, output.pixels, 0, w);
    WritableRaster raster = ((BufferedImage) image).getRaster();
    raster.getDataElements(x, y, w, h, output.pixels);

    return output;
  }


  /**
   * Grab a copy of the current pixel buffer.
   */
  public PImage get() {
    /*
    PImage outgoing = new PImage(width, height);
    ((BufferedImage) image).getRGB(0, 0, width, height,
                                   outgoing.pixels, 0, width);
    return outgoing;
    */
    return get(0, 0, width, height);
  }


  public void set(int x, int y, int argb) {
    if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) return;
    //((BufferedImage) image).setRGB(x, y, argb);
    getset[0] = argb;
    WritableRaster raster = ((BufferedImage) image).getRaster();
    raster.setDataElements(x, y, getset);
  }


  protected void setImpl(int dx, int dy, int sx, int sy, int sw, int sh,
                         PImage src) {
    WritableRaster raster = ((BufferedImage) image).getRaster();
    if ((sx == 0) && (sy == 0) && (sw == src.width) && (sh == src.height)) {
      raster.setDataElements(dx, dy, src.width, src.height, src.pixels);
    } else {
      int mode = src.imageMode;
      src.imageMode = CORNER;
      // TODO Optimize, incredibly inefficient to reallocate this much memory
      PImage temp = src.get(sx, sy, sw, sh);
      src.imageMode = mode;
      raster.setDataElements(dx, dy, temp.width, temp.height, temp.pixels);
    }
  }


  //////////////////////////////////////////////////////////////


  public void mask(int alpha[]) {
    throw new RuntimeException("mask() cannot be used with JAVA2D");
  }


  public void mask(PImage alpha) {
    throw new RuntimeException("mask() cannot be used with JAVA2D");
  }


  //////////////////////////////////////////////////////////////


  public void filter(int kind) {
    loadPixels();
    super.filter(kind);
    updatePixels();
  }


  public void filter(int kind, float param) {
    loadPixels();
    super.filter(kind, param);
    updatePixels();
  }


  //////////////////////////////////////////////////////////////


  public void copy(int sx, int sy, int sw, int sh,
                   int dx, int dy, int dw, int dh) {
    if ((sw != dw) || (sh != dh)) {
      // use slow version if changing size
      copy(this, sx, sy, sw, sh, dx, dy, dw, dh);

    } else {
      if (imageMode == CORNERS) {
        sw -= sx;
        sh -= sy;
      }
      dx = dx - sx;  // java2d's "dx" is the delta, not dest
      dy = dy - sy;
      g2.copyArea(sx, sy, sw, sh, dx, dy);
    }
  }


  public void copy(PImage src,
                   int sx1, int sy1, int sx2, int sy2,
                   int dx1, int dy1, int dx2, int dy2) {
    loadPixels();
    super.copy(src, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2);
    updatePixels();
  }


  //////////////////////////////////////////////////////////////


  /*
  public void blend(PImage src, int sx, int sy, int dx, int dy, int mode) {
    loadPixels();
    super.blend(src, sx, sy, dx, dy, mode);
    updatePixels();
  }


  public void blend(int sx, int sy, int dx, int dy, int mode) {
    loadPixels();
    super.blend(sx, sy, dx, dy, mode);
    updatePixels();
  }
  */


  public void blend(int sx1, int sy1, int sx2, int sy2,
                    int dx1, int dy1, int dx2, int dy2, int mode) {
    loadPixels();
    super.blend(sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2, mode);
    updatePixels();
  }


  public void blend(PImage src, int sx1, int sy1, int sx2, int sy2,
                    int dx1, int dy1, int dx2, int dy2, int mode) {
    loadPixels();
    super.blend(src, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2, mode);
    updatePixels();
  }


  //////////////////////////////////////////////////////////////


  public void save(String filename) {
    //System.out.println("start load");
    loadPixels();
    //System.out.println("end load, start save");
    super.save(filename);
    //System.out.println("done with save");
  }
}
