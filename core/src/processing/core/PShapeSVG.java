package processing.core;

import java.awt.Paint;
import java.awt.PaintContext;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.geom.AffineTransform;
import java.awt.geom.Point2D;
import java.awt.geom.Rectangle2D;
import java.awt.image.ColorModel;
import java.awt.image.Raster;
import java.awt.image.WritableRaster;
import java.util.HashMap;

import processing.xml.XMLElement;


/**
 * SVG stands for Scalable Vector Graphics, a portable graphics format. It is
 * a vector format so it allows for infinite resolution and relatively small
 * file sizes. Most modern media software can view SVG files, including Adobe
 * products, Firefox, etc. Illustrator and Inkscape can edit SVG files.
 * <p>
 * We have no intention of turning this into a full-featured SVG library.
 * The goal of this project is a basic shape importer that is small enough
 * to be included with applets, meaning that its download size should be
 * in the neighborhood of 25-30k. Starting with release 0149, this library
 * has been incorporated into the core via the loadShape() command, because
 * vector shape data is just as important as the image data from loadImage().
 * <p>
 * For more sophisticated import/export, consider the
 * <A HREF="http://xmlgraphics.apache.org/batik/">Batik</A>
 * library from the Apache Software Foundation. Future improvements to this
 * library may focus on this properly supporting a specific subset of SVG,
 * for instance the simpler SVG profiles known as
 * <A HREF="http://www.w3.org/TR/SVGMobile/">SVG Tiny or Basic</A>,
 * although we still would not support the interactivity options.
 *
 * <p> <hr noshade> <p>
 *
 * A minimal example program using SVG:
 * (assuming a working moo.svg is in your data folder)
 *
 * <PRE>
 * PShape moo;
 *
 * void setup() {
 *   size(400, 400);
 *   moo = loadShape("moo.svg");
 * }
 * void draw() {
 *   background(255);
 *   shape(moo, mouseX, mouseY);
 * }
 * </PRE>
 *
 * This code is based on the Candy library written by Michael Chang, which was
 * later revised and expanded for use as a Processing core library by Ben Fry.
 * Thanks to Ricard Marxer Pinon for help with better Inkscape support in 0154.
 *
 * <p> <hr noshade> <p>
 *
 * Late October 2008 revisions from ricardmp, incorporated by fry (0154)
 * <UL>
 * <LI>Better style attribute handling, enabling better Inkscape support.
 * </UL>
 *
 * October 2008 revisions by fry (Processing 0149, pre-1.0)
 * <UL>
 * <LI> Candy is no longer a separate library, and is instead part of core.
 * <LI> Loading now works through loadShape()
 * <LI> Shapes are now drawn using the new PGraphics shape() method.
 * </UL>
 *
 * August 2008 revisions by fry (Processing 0149)
 * <UL>
 * <LI> Major changes to rework around PShape.
 * <LI> Now implementing more of the "transform" attribute.
 * </UL>
 *
 * February 2008 revisions by fry (Processing 0136)
 * <UL>
 * <LI> Added support for quadratic curves in paths (Q, q, T, and t operators)
 * <LI> Support for reading SVG font data (though not rendering it yet)
 * </UL>
 *
 * Revisions for "Candy 2" November 2006 by fry
 * <UL>
 * <LI> Switch to the new processing.xml library
 * <LI> Several bug fixes for parsing of shape data
 * <LI> Support for linear and radial gradients
 * <LI> Support for additional types of shapes
 * <LI> Added compound shapes (shapes with interior points)
 * <LI> Added methods to get shapes from an internal table
 * </UL>
 *
 * Revision 10/31/06 by flux
 * <UL>
 * <LI> Now properly supports Processing 0118
 * <LI> Fixed a bunch of things for Casey's students and general buggity.
 * <LI> Will now properly draw #FFFFFFFF colors (were being represented as -1)
 * <LI> SVGs without <g> tags are now properly caught and loaded
 * <LI> Added a method customStyle() for overriding SVG colors/styles
 * <LI> Added a method SVGStyle() to go back to using SVG colors/styles
 * </UL>
 *
 * Some SVG objects and features may not yet be supported.
 * Here is a partial list of non-included features
 * <UL>
 * <LI> Rounded rectangles
 * <LI> Drop shadow objects
 * <LI> Typography
 * <LI> <STRIKE>Layers</STRIKE> added for Candy 2
 * <LI> Patterns
 * <LI> Embedded images
 * </UL>
 *
 * For those interested, the SVG specification can be found
 * <A HREF="http://www.w3.org/TR/SVG">here</A>.
 */
public class PShapeSVG extends PShape {
  XMLElement element;

  float opacity;
  float strokeOpacity;
  float fillOpacity;


  Gradient strokeGradient;
  Paint strokeGradientPaint;
  String strokeName;  // id of another object, gradients only?

  Gradient fillGradient;
  Paint fillGradientPaint;
  String fillName;  // id of another object


  /**
   * Initializes a new SVG Object with the given filename.
   */
  public PShapeSVG(PApplet parent, String filename) {
    // this will grab the root document, starting <svg ...>
    // the xml version and initial comments are ignored
    this(new XMLElement(parent, filename));
  }


  /**
   * Initializes a new SVG Object from the given XMLElement.
   */
  public PShapeSVG(XMLElement svg) {
    this(null, svg);

    if (!svg.getName().equals("svg")) {
      throw new RuntimeException("root is not <svg>, it's <" + svg.getName() + ">");
    }

    // not proper parsing of the viewBox, but will cover us for cases where
    // the width and height of the object is not specified
    String viewBoxStr = svg.getStringAttribute("viewBox");
    if (viewBoxStr != null) {
      int[] viewBox = PApplet.parseInt(PApplet.splitTokens(viewBoxStr));
      width = viewBox[2];
      height = viewBox[3];
    }

    // TODO if viewbox is not same as width/height, then use it to scale
    // the original objects. for now, viewbox only used when width/height
    // are empty values (which by the spec means w/h of "100%"
    String unitWidth = svg.getStringAttribute("width");
    String unitHeight = svg.getStringAttribute("height");
    if (unitWidth != null) {
      width = parseUnitSize(unitWidth);
      height = parseUnitSize(unitHeight);
    } else {
      if ((width == 0) || (height == 0)) {
        //throw new RuntimeException("width/height not specified");
        PGraphics.showWarning("The width and/or height is not " +
                              "readable in the <svg> tag of this file.");
        // For the spec, the default is 100% and 100%. For purposes
        // here, insert a dummy value because this is prolly just a
        // font or something for which the w/h doesn't matter.
        width = 1;
        height = 1;
      }
    }

    //root = new Group(null, svg);
    parseChildren(svg);  // ?
  }


  public PShapeSVG(PShapeSVG parent, XMLElement properties) {
    //super(GROUP);

    if (parent == null) {
      // set values to their defaults according to the SVG spec
      stroke = false;
      strokeColor = 0xff000000;
      strokeWeight = 1;
      strokeCap = PConstants.SQUARE;  // equivalent to BUTT in svg spec
      strokeJoin = PConstants.MITER;
      strokeGradient = null;
      strokeGradientPaint = null;
      strokeName = null;

      fill = true;
      fillColor = 0xff000000;
      fillGradient = null;
      fillGradientPaint = null;
      fillName = null;

      //hasTransform = false;
      //transformation = null; //new float[] { 1, 0, 0, 1, 0, 0 };

      strokeOpacity = 1;
      fillOpacity = 1;
      opacity = 1;

    } else {
      stroke = parent.stroke;
      strokeColor = parent.strokeColor;
      strokeWeight = parent.strokeWeight;
      strokeCap = parent.strokeCap;
      strokeJoin = parent.strokeJoin;
      strokeGradient = parent.strokeGradient;
      strokeGradientPaint = parent.strokeGradientPaint;
      strokeName = parent.strokeName;

      fill = parent.fill;
      fillColor = parent.fillColor;
      fillGradient = parent.fillGradient;
      fillGradientPaint = parent.fillGradientPaint;
      fillName = parent.fillName;

      //hasTransform = parent.hasTransform;
      //transformation = parent.transformation;

      opacity = parent.opacity;
    }

    element = properties;
    name = properties.getStringAttribute("id");

    String displayStr = properties.getStringAttribute("display", "inline");
    visible = !displayStr.equals("none");

    String transformStr = properties.getStringAttribute("transform");
    if (transformStr != null) {
      matrix = parseMatrix(transformStr);
    }

    parseColors(properties);
    parseChildren(properties);
  }


  protected void parseChildren(XMLElement graphics) {
    XMLElement[] elements = graphics.getChildren();
    children = new PShape[elements.length];
    childCount = 0;

    for (XMLElement elem : elements) {
      PShape kid = parseChild(elem);
      if (kid != null) {
        addChild(kid);
      }
    }
  }


  /**
   * Parse a child XML element.
   * Override this method to add parsing for more SVG elements.
   */
  protected PShape parseChild(XMLElement elem) {
    String name = elem.getName();
    PShapeSVG shape = null;

    if (name.equals("g")) {
      //return new BaseObject(this, elem);
      shape = new PShapeSVG(this, elem);

    } else if (name.equals("defs")) {
      // generally this will contain gradient info, so may
      // as well just throw it into a group element for parsing
      //return new BaseObject(this, elem);
      shape = new PShapeSVG(this, elem);

    } else if (name.equals("line")) {
      //return new Line(this, elem);
      //return new BaseObject(this, elem, LINE);
      shape = new PShapeSVG(this, elem);
      shape.parseLine();

    } else if (name.equals("circle")) {
      //return new BaseObject(this, elem, ELLIPSE);
      shape = new PShapeSVG(this, elem);
      shape.parseEllipse(true);

    } else if (name.equals("ellipse")) {
      //return new BaseObject(this, elem, ELLIPSE);
      shape = new PShapeSVG(this, elem);
      shape.parseEllipse(false);

    } else if (name.equals("rect")) {
      //return new BaseObject(this, elem, RECT);
      shape = new PShapeSVG(this, elem);
      shape.parseRect();

    } else if (name.equals("polygon")) {
      //return new BaseObject(this, elem, POLYGON);
      shape = new PShapeSVG(this, elem);
      shape.parsePoly(true);

    } else if (name.equals("polyline")) {
      //return new BaseObject(this, elem, POLYGON);
      shape = new PShapeSVG(this, elem);
      shape.parsePoly(false);

    } else if (name.equals("path")) {
      //return new BaseObject(this, elem, PATH);
      shape = new PShapeSVG(this, elem);
      shape.parsePath();

    } else if (name.equals("radialGradient")) {
      return new RadialGradient(this, elem);

    } else if (name.equals("linearGradient")) {
      return new LinearGradient(this, elem);

    } else if (name.equals("text")) {
      PGraphics.showWarning("Text in SVG files is not currently supported, " +
                            "convert text to outlines instead.");

    } else if (name.equals("filter")) {
      PGraphics.showWarning("Filters are not supported.");

    } else if (name.equals("mask")) {
      PGraphics.showWarning("Masks are not supported.");

    } else {
      PGraphics.showWarning("Ignoring  <" + name + "> tag.");
    }
    return shape;
  }


  protected void parseLine() {
    kind = LINE;
    family = PRIMITIVE;
    params = new float[] {
      element.getFloatAttribute("x1"),
      element.getFloatAttribute("y1"),
      element.getFloatAttribute("x2"),
      element.getFloatAttribute("y2"),
    };
    //    x = params[0];
    //    y = params[1];
    //    width = params[2];
    //    height = params[3];
  }


  /**
   * Handles parsing ellipse and circle tags.
   * @param circle true if this is a circle and not an ellipse
   */
  protected void parseEllipse(boolean circle) {
    kind = ELLIPSE;
    family = PRIMITIVE;
    params = new float[4];

    params[0] = element.getFloatAttribute("cx");
    params[1] = element.getFloatAttribute("cy");

    float rx, ry;
    if (circle) {
      rx = ry = element.getFloatAttribute("r");
    } else {
      rx = element.getFloatAttribute("rx");
      ry = element.getFloatAttribute("ry");
    }
    params[0] -= rx;
    params[1] -= ry;

    params[2] = rx*2;
    params[3] = ry*2;
  }


  protected void parseRect() {
    kind = RECT;
    family = PRIMITIVE;
    params = new float[] {
      element.getFloatAttribute("x"),
      element.getFloatAttribute("y"),
      element.getFloatAttribute("width"),
      element.getFloatAttribute("height"),
    };
  }


  /**
   * Parse a polyline or polygon from an SVG file.
   * @param close true if shape is closed (polygon), false if not (polyline)
   */
  protected void parsePoly(boolean close) {
    family = PATH;
    this.close = close;

    String pointsAttr = element.getStringAttribute("points");
    if (pointsAttr != null) {
      String[] pointsBuffer = PApplet.splitTokens(pointsAttr);
      vertexCount = pointsBuffer.length;
      vertices = new float[vertexCount][2];
      for (int i = 0; i < vertexCount; i++) {
        String pb[] = PApplet.split(pointsBuffer[i], ',');
        vertices[i][X] = Float.valueOf(pb[0]).floatValue();
        vertices[i][Y] = Float.valueOf(pb[1]).floatValue();
      }
    }
  }


  protected void parsePath() {
    family = PATH;
    kind = 0;

    String pathData = element.getStringAttribute("d");
    if (pathData == null) return;
    char[] pathDataChars = pathData.toCharArray();

    StringBuffer pathBuffer = new StringBuffer();
    boolean lastSeparate = false;

    for (int i = 0; i < pathDataChars.length; i++) {
      char c = pathDataChars[i];
      boolean separate = false;

      if (c == 'M' || c == 'm' ||
          c == 'L' || c == 'l' ||
          c == 'H' || c == 'h' ||
          c == 'V' || c == 'v' ||
          c == 'C' || c == 'c' ||  // beziers
          c == 'S' || c == 's' ||
          c == 'Q' || c == 'q' ||  // quadratic beziers
          c == 'T' || c == 't' ||
          c == 'Z' || c == 'z' ||  // closepath
          c == ',') {
        separate = true;
        if (i != 0) {
          pathBuffer.append("|");
        }
      }
      if (c == 'Z' || c == 'z') {
        separate = false;
      }
      if (c == '-' && !lastSeparate) {
        // allow for 'e' notation in numbers, e.g. 2.10e-9 
        // http://dev.processing.org/bugs/show_bug.cgi?id=1408
        if (i == 0 || pathDataChars[i-1] != 'e') {
          pathBuffer.append("|");
        }
      }
      if (c != ',') {
        pathBuffer.append(c); //"" + pathDataBuffer.charAt(i));
      }
      if (separate && c != ',' && c != '-') {
        pathBuffer.append("|");
      }
      lastSeparate = separate;
    }

    // use whitespace constant to get rid of extra spaces and CR or LF
    String[] pathDataKeys =
      PApplet.splitTokens(pathBuffer.toString(), "|" + WHITESPACE);
    vertices = new float[pathDataKeys.length][2];
    vertexCodes = new int[pathDataKeys.length];

    float cx = 0;
    float cy = 0;

    int i = 0;
    while (i < pathDataKeys.length) {
      char c = pathDataKeys[i].charAt(0);
      switch (c) {

      case 'M':  // M - move to (absolute)
        cx = PApplet.parseFloat(pathDataKeys[i + 1]);
        cy = PApplet.parseFloat(pathDataKeys[i + 2]);
        parsePathMoveto(cx, cy);
        i += 3;
        break;

      case 'm':  // m - move to (relative)
        cx = cx + PApplet.parseFloat(pathDataKeys[i + 1]);
        cy = cy + PApplet.parseFloat(pathDataKeys[i + 2]);
        parsePathMoveto(cx, cy);
        i += 3;
        break;

      case 'L':
        cx = PApplet.parseFloat(pathDataKeys[i + 1]);
        cy = PApplet.parseFloat(pathDataKeys[i + 2]);
        parsePathLineto(cx, cy);
        i += 3;
        break;

      case 'l':
        cx = cx + PApplet.parseFloat(pathDataKeys[i + 1]);
        cy = cy + PApplet.parseFloat(pathDataKeys[i + 2]);
        parsePathLineto(cx, cy);
        i += 3;
        break;

        // horizontal lineto absolute
      case 'H':
        cx = PApplet.parseFloat(pathDataKeys[i + 1]);
        parsePathLineto(cx, cy);
        i += 2;
        break;

        // horizontal lineto relative
      case 'h':
        cx = cx + PApplet.parseFloat(pathDataKeys[i + 1]);
        parsePathLineto(cx, cy);
        i += 2;
        break;

      case 'V':
        cy = PApplet.parseFloat(pathDataKeys[i + 1]);
        parsePathLineto(cx, cy);
        i += 2;
        break;

      case 'v':
        cy = cy + PApplet.parseFloat(pathDataKeys[i + 1]);
        parsePathLineto(cx, cy);
        i += 2;
        break;

        // C - curve to (absolute)
      case 'C': {
        float ctrlX1 = PApplet.parseFloat(pathDataKeys[i + 1]);
        float ctrlY1 = PApplet.parseFloat(pathDataKeys[i + 2]);
        float ctrlX2 = PApplet.parseFloat(pathDataKeys[i + 3]);
        float ctrlY2 = PApplet.parseFloat(pathDataKeys[i + 4]);
        float endX = PApplet.parseFloat(pathDataKeys[i + 5]);
        float endY = PApplet.parseFloat(pathDataKeys[i + 6]);
        parsePathCurveto(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
        cx = endX;
        cy = endY;
        i += 7;
      }
        break;

        // c - curve to (relative)
      case 'c': {
        float ctrlX1 = cx + PApplet.parseFloat(pathDataKeys[i + 1]);
        float ctrlY1 = cy + PApplet.parseFloat(pathDataKeys[i + 2]);
        float ctrlX2 = cx + PApplet.parseFloat(pathDataKeys[i + 3]);
        float ctrlY2 = cy + PApplet.parseFloat(pathDataKeys[i + 4]);
        float endX = cx + PApplet.parseFloat(pathDataKeys[i + 5]);
        float endY = cy + PApplet.parseFloat(pathDataKeys[i + 6]);
        parsePathCurveto(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
        cx = endX;
        cy = endY;
        i += 7;
      }
        break;

        // S - curve to shorthand (absolute)
      case 'S': {
        float ppx = vertices[vertexCount-2][X];
        float ppy = vertices[vertexCount-2][Y];
        float px = vertices[vertexCount-1][X];
        float py = vertices[vertexCount-1][Y];
        float ctrlX1 = px + (px - ppx);
        float ctrlY1 = py + (py - ppy);
        float ctrlX2 = PApplet.parseFloat(pathDataKeys[i + 1]);
        float ctrlY2 = PApplet.parseFloat(pathDataKeys[i + 2]);
        float endX = PApplet.parseFloat(pathDataKeys[i + 3]);
        float endY = PApplet.parseFloat(pathDataKeys[i + 4]);
        parsePathCurveto(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
        cx = endX;
        cy = endY;
        i += 5;
      }
        break;

        // s - curve to shorthand (relative)
      case 's': {
        float ppx = vertices[vertexCount-2][X];
        float ppy = vertices[vertexCount-2][Y];
        float px = vertices[vertexCount-1][X];
        float py = vertices[vertexCount-1][Y];
        float ctrlX1 = px + (px - ppx);
        float ctrlY1 = py + (py - ppy);
        float ctrlX2 = cx + PApplet.parseFloat(pathDataKeys[i + 1]);
        float ctrlY2 = cy + PApplet.parseFloat(pathDataKeys[i + 2]);
        float endX = cx + PApplet.parseFloat(pathDataKeys[i + 3]);
        float endY = cy + PApplet.parseFloat(pathDataKeys[i + 4]);
        parsePathCurveto(ctrlX1, ctrlY1, ctrlX2, ctrlY2, endX, endY);
        cx = endX;
        cy = endY;
        i += 5;
      }
        break;

        // Q - quadratic curve to (absolute)
      case 'Q': {
        float ctrlX = PApplet.parseFloat(pathDataKeys[i + 1]);
        float ctrlY = PApplet.parseFloat(pathDataKeys[i + 2]);
        float endX = PApplet.parseFloat(pathDataKeys[i + 3]);
        float endY = PApplet.parseFloat(pathDataKeys[i + 4]);
        parsePathQuadto(cx, cy, ctrlX, ctrlY, endX, endY);
        cx = endX;
        cy = endY;
        i += 5;
      }
        break;

        // q - quadratic curve to (relative)
      case 'q': {
        float ctrlX = cx + PApplet.parseFloat(pathDataKeys[i + 1]);
        float ctrlY = cy + PApplet.parseFloat(pathDataKeys[i + 2]);
        float endX = cx + PApplet.parseFloat(pathDataKeys[i + 3]);
        float endY = cy + PApplet.parseFloat(pathDataKeys[i + 4]);
        parsePathQuadto(cx, cy, ctrlX, ctrlY, endX, endY);
        cx = endX;
        cy = endY;
        i += 5;
      }
        break;

        // T - quadratic curve to shorthand (absolute)
        // The control point is assumed to be the reflection of the
        // control point on the previous command relative to the
        // current point. (If there is no previous command or if the
        // previous command was not a Q, q, T or t, assume the control
        // point is coincident with the current point.)
      case 'T': {
        float ppx = vertices[vertexCount-2][X];
        float ppy = vertices[vertexCount-2][Y];
        float px = vertices[vertexCount-1][X];
        float py = vertices[vertexCount-1][Y];
        float ctrlX = px + (px - ppx);
        float ctrlY = py + (py - ppy);
        float endX = PApplet.parseFloat(pathDataKeys[i + 1]);
        float endY = PApplet.parseFloat(pathDataKeys[i + 2]);
        parsePathQuadto(cx, cy, ctrlX, ctrlY, endX, endY);
        cx = endX;
        cy = endY;
        i += 3;
      }
        break;

        // t - quadratic curve to shorthand (relative)
      case 't': {
        float ppx = vertices[vertexCount-2][X];
        float ppy = vertices[vertexCount-2][Y];
        float px = vertices[vertexCount-1][X];
        float py = vertices[vertexCount-1][Y];
        float ctrlX = px + (px - ppx);
        float ctrlY = py + (py - ppy);
        float endX = cx + PApplet.parseFloat(pathDataKeys[i + 1]);
        float endY = cy + PApplet.parseFloat(pathDataKeys[i + 2]);
        parsePathQuadto(cx, cy, ctrlX, ctrlY, endX, endY);
        cx = endX;
        cy = endY;
        i += 3;
      }
        break;

      case 'Z':
      case 'z':
        close = true;
        i++;
        break;

      default:
        String parsed =
          PApplet.join(PApplet.subset(pathDataKeys, 0, i), ",");
        String unparsed =
          PApplet.join(PApplet.subset(pathDataKeys, i), ",");
        System.err.println("parsed: " + parsed);
        System.err.println("unparsed: " + unparsed);
        if (pathDataKeys[i].equals("a") || pathDataKeys[i].equals("A")) {
          String msg = "Sorry, elliptical arc support for SVG files " +
            "is not yet implemented (See bug #996 for details)";
          throw new RuntimeException(msg);
        }
        throw new RuntimeException("shape command not handled: " + pathDataKeys[i]);
      }
    }
  }


//      private void parsePathCheck(int num) {
//        if (vertexCount + num-1 >= vertices.length) {
//          //vertices = (float[][]) PApplet.expand(vertices);
//          float[][] temp = new float[vertexCount << 1][2];
//          System.arraycopy(vertices, 0, temp, 0, vertexCount);
//          vertices = temp;
//        }
//      }

  private void parsePathVertex(float x, float y) {
    if (vertexCount == vertices.length) {
      //vertices = (float[][]) PApplet.expand(vertices);
      float[][] temp = new float[vertexCount << 1][2];
      System.arraycopy(vertices, 0, temp, 0, vertexCount);
      vertices = temp;
    }
    vertices[vertexCount][X] = x;
    vertices[vertexCount][Y] = y;
    vertexCount++;
  }


  private void parsePathCode(int what) {
    if (vertexCodeCount == vertexCodes.length) {
      vertexCodes = PApplet.expand(vertexCodes);
    }
    vertexCodes[vertexCodeCount++] = what;
  }


  private void parsePathMoveto(float px, float py) {
    if (vertexCount > 0) {
      parsePathCode(BREAK);
    }
    parsePathCode(VERTEX);
    parsePathVertex(px, py);
  }


  private void parsePathLineto(float px, float py) {
    parsePathCode(VERTEX);
    parsePathVertex(px, py);
  }


  private void parsePathCurveto(float x1, float y1,
                                float x2, float y2,
                                float x3, float y3) {
    parsePathCode(BEZIER_VERTEX);
    parsePathVertex(x1, y1);
    parsePathVertex(x2, y2);
    parsePathVertex(x3, y3);
  }

  private void parsePathQuadto(float x1, float y1, 
                               float cx, float cy,
                               float x2, float y2) {
    parsePathCode(BEZIER_VERTEX);
    // x1/y1 already covered by last moveto, lineto, or curveto
    parsePathVertex(x1 + ((cx-x1)*2/3.0f), y1 + ((cy-y1)*2/3.0f));
    parsePathVertex(x2 + ((cx-x2)*2/3.0f), y2 + ((cy-y2)*2/3.0f));
    parsePathVertex(x2, y2);
  }


  /**
   * Parse the specified SVG matrix into a PMatrix2D. Note that PMatrix2D
   * is rotated relative to the SVG definition, so parameters are rearranged
   * here. More about the transformation matrices in
   * <a href="http://www.w3.org/TR/SVG/coords.html#TransformAttribute">this section</a>
   * of the SVG documentation.
   * @param matrixStr text of the matrix param.
   * @return a good old-fashioned PMatrix2D
   */
  static protected PMatrix2D parseMatrix(String matrixStr) {
    String[] pieces = PApplet.match(matrixStr, "\\s*(\\w+)\\((.*)\\)");
    if (pieces == null) {
      System.err.println("Could not parse transform " + matrixStr);
      return null;
    }
    float[] m = PApplet.parseFloat(PApplet.splitTokens(pieces[2], ", "));
    if (pieces[1].equals("matrix")) {
      return new PMatrix2D(m[0], m[2], m[4], m[1], m[3], m[5]);

    } else if (pieces[1].equals("translate")) {
      float tx = m[0];
      float ty = (m.length == 2) ? m[1] : m[0];
      //return new float[] { 1, 0, tx,  0, 1, ty };
      return new PMatrix2D(1, 0, tx, 0, 1, ty);

    } else if (pieces[1].equals("scale")) {
      float sx = m[0];
      float sy = (m.length == 2) ? m[1] : m[0];
      //return new float[] { sx, 0, 0, 0, sy, 0 };
      return new PMatrix2D(sx, 0, 0,  0, sy, 0);

    } else if (pieces[1].equals("rotate")) {
      float angle = m[0];

      if (m.length == 1) {
        float c = PApplet.cos(angle);
        float s = PApplet.sin(angle);
        // SVG version is cos(a) sin(a) -sin(a) cos(a) 0 0
        return new PMatrix2D(c, -s, 0, s, c, 0);

      } else if (m.length == 3) {
        PMatrix2D mat = new PMatrix2D(0, 1, m[1],  1, 0, m[2]);
        mat.rotate(m[0]);
        mat.translate(-m[1], -m[2]);
        return mat; //.get(null);
      }

    } else if (pieces[1].equals("skewX")) {
      return new PMatrix2D(1, 0, 1,  PApplet.tan(m[0]), 0, 0);

    } else if (pieces[1].equals("skewY")) {
      return new PMatrix2D(1, 0, 1,  0, PApplet.tan(m[0]), 0);
    }
    return null;
  }


  protected void parseColors(XMLElement properties) {
    if (properties.hasAttribute("opacity")) {
      String opacityText = properties.getStringAttribute("opacity");
      setOpacity(opacityText);
    }

    if (properties.hasAttribute("stroke")) {
      String strokeText = properties.getStringAttribute("stroke");
      setStroke(strokeText);
    }

    if (properties.hasAttribute("stroke-width")) {
      // if NaN (i.e. if it's 'inherit') then default back to the inherit setting
      String lineweight = properties.getStringAttribute("stroke-width");
      setStrokeWeight(lineweight);
    }

    if (properties.hasAttribute("stroke-linejoin")) {
      String linejoin = properties.getStringAttribute("stroke-linejoin");
      setStrokeJoin(linejoin);
    }

    if (properties.hasAttribute("stroke-linecap")) {
      String linecap = properties.getStringAttribute("stroke-linecap");
      setStrokeCap(linecap);
    }


    // fill defaults to black (though stroke defaults to "none")
    // http://www.w3.org/TR/SVG/painting.html#FillProperties
    if (properties.hasAttribute("fill")) {
      String fillText = properties.getStringAttribute("fill");
      setFill(fillText);

    }

    if (properties.hasAttribute("style")) {
      String styleText = properties.getStringAttribute("style");
      String[] styleTokens = PApplet.splitTokens(styleText, ";");

      //PApplet.println(styleTokens);
      for (int i = 0; i < styleTokens.length; i++) {
        String[] tokens = PApplet.splitTokens(styleTokens[i], ":");
        //PApplet.println(tokens);

        tokens[0] = PApplet.trim(tokens[0]);

        if (tokens[0].equals("fill")) {
          setFill(tokens[1]);

        } else if(tokens[0].equals("fill-opacity")) {
          setFillOpacity(tokens[1]);

        } else if(tokens[0].equals("stroke")) {
          setStroke(tokens[1]);

        } else if(tokens[0].equals("stroke-width")) {
          setStrokeWeight(tokens[1]);

        } else if(tokens[0].equals("stroke-linecap")) {
          setStrokeCap(tokens[1]);

        } else if(tokens[0].equals("stroke-linejoin")) {
          setStrokeJoin(tokens[1]);

        } else if(tokens[0].equals("stroke-opacity")) {
          setStrokeOpacity(tokens[1]);

        } else if(tokens[0].equals("opacity")) {
          setOpacity(tokens[1]);

        } else {
          // Other attributes are not yet implemented
        }
      }
    }
  }

  void setOpacity(String opacityText) {
    opacity = PApplet.parseFloat(opacityText);
    strokeColor = ((int) (opacity * 255)) << 24 | strokeColor & 0xFFFFFF;
    fillColor = ((int) (opacity * 255)) << 24 | fillColor & 0xFFFFFF;
  }

  void setStrokeWeight(String lineweight) {
    strokeWeight = PApplet.parseFloat(lineweight);
  }

  void setStrokeOpacity(String opacityText) {
    strokeOpacity = PApplet.parseFloat(opacityText);
    strokeColor = ((int) (strokeOpacity * 255)) << 24 | strokeColor & 0xFFFFFF;
  }

  void setStroke(String strokeText) {
    int opacityMask = strokeColor & 0xFF000000;
    if (strokeText.equals("none")) {
      stroke = false;
    } else if (strokeText.startsWith("#")) {
      stroke = true;
      strokeColor = opacityMask |
        (Integer.parseInt(strokeText.substring(1), 16)) & 0xFFFFFF;
    } else if (strokeText.startsWith("rgb")) {
      stroke = true;
      strokeColor = opacityMask | parseRGB(strokeText);
    } else if (strokeText.startsWith("url(#")) {
      strokeName = strokeText.substring(5, strokeText.length() - 1);
      Object strokeObject = findChild(strokeName);
      if (strokeObject instanceof Gradient) {
        strokeGradient = (Gradient) strokeObject;
        strokeGradientPaint = calcGradientPaint(strokeGradient); //, opacity);
      } else {
        System.err.println("url " + strokeName + " refers to unexpected data");
      }
    }
  }

  void setStrokeJoin(String linejoin) {
    if (linejoin.equals("inherit")) {
      // do nothing, will inherit automatically

    } else if (linejoin.equals("miter")) {
      strokeJoin = PConstants.MITER;

    } else if (linejoin.equals("round")) {
      strokeJoin = PConstants.ROUND;

    } else if (linejoin.equals("bevel")) {
      strokeJoin = PConstants.BEVEL;
    }
  }

  void setStrokeCap(String linecap) {
    if (linecap.equals("inherit")) {
      // do nothing, will inherit automatically

    } else if (linecap.equals("butt")) {
      strokeCap = PConstants.SQUARE;

    } else if (linecap.equals("round")) {
      strokeCap = PConstants.ROUND;

    } else if (linecap.equals("square")) {
      strokeCap = PConstants.PROJECT;
    }
  }

  void setFillOpacity(String opacityText) {
    fillOpacity = PApplet.parseFloat(opacityText);
    fillColor = ((int) (fillOpacity * 255)) << 24 | fillColor & 0xFFFFFF;
  }

  void setFill(String fillText) {
    int opacityMask = fillColor & 0xFF000000;
    if (fillText.equals("none")) {
      fill = false;
    } else if (fillText.startsWith("#")) {
      fill = true;
      fillColor = opacityMask |
        (Integer.parseInt(fillText.substring(1), 16)) & 0xFFFFFF;
      //System.out.println("hex for fill is " + PApplet.hex(fillColor));
    } else if (fillText.startsWith("rgb")) {
      fill = true;
      fillColor = opacityMask | parseRGB(fillText);
    } else if (fillText.startsWith("url(#")) {
      fillName = fillText.substring(5, fillText.length() - 1);
      //PApplet.println("looking for " + fillName);
      Object fillObject = findChild(fillName);
      //PApplet.println("found " + fillObject);
      if (fillObject instanceof Gradient) {
        fill = true;
        fillGradient = (Gradient) fillObject;
        fillGradientPaint = calcGradientPaint(fillGradient); //, opacity);
        //PApplet.println("got filla " + fillObject);
      } else {
        System.err.println("url " + fillName + " refers to unexpected data");
      }
    }
  }


  static protected int parseRGB(String what) {
    int leftParen = what.indexOf('(') + 1;
    int rightParen = what.indexOf(')');
    String sub = what.substring(leftParen, rightParen);
    int[] values = PApplet.parseInt(PApplet.splitTokens(sub, ", "));
    return (values[0] << 16) | (values[1] << 8) | (values[2]);
  }


  static protected HashMap<String, String> parseStyleAttributes(String style) {
    HashMap<String, String> table = new HashMap<String, String>();
    String[] pieces = style.split(";");
    for (int i = 0; i < pieces.length; i++) {
      String[] parts = pieces[i].split(":");
      table.put(parts[0], parts[1]);
    }
    return table;
  }


  /**
   * Parse a size that may have a suffix for its units.
   * Ignoring cases where this could also be a percentage.
   * The <A HREF="http://www.w3.org/TR/SVG/coords.html#Units">units</A> spec:
   * <UL>
   * <LI>"1pt" equals "1.25px" (and therefore 1.25 user units)
   * <LI>"1pc" equals "15px" (and therefore 15 user units)
   * <LI>"1mm" would be "3.543307px" (3.543307 user units)
   * <LI>"1cm" equals "35.43307px" (and therefore 35.43307 user units)
   * <LI>"1in" equals "90px" (and therefore 90 user units)
   * </UL>
   */
  static protected float parseUnitSize(String text) {
    int len = text.length() - 2;

    if (text.endsWith("pt")) {
      return PApplet.parseFloat(text.substring(0, len)) * 1.25f;
    } else if (text.endsWith("pc")) {
      return PApplet.parseFloat(text.substring(0, len)) * 15;
    } else if (text.endsWith("mm")) {
      return PApplet.parseFloat(text.substring(0, len)) * 3.543307f;
    } else if (text.endsWith("cm")) {
      return PApplet.parseFloat(text.substring(0, len)) * 35.43307f;
    } else if (text.endsWith("in")) {
      return PApplet.parseFloat(text.substring(0, len)) * 90;
    } else if (text.endsWith("px")) {
      return PApplet.parseFloat(text.substring(0, len));
    } else {
      return PApplet.parseFloat(text);
    }
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  static class Gradient extends PShapeSVG {
    AffineTransform transform;

    float[] offset;
    int[] color;
    int count;

    public Gradient(PShapeSVG parent, XMLElement properties) {
      super(parent, properties);

      XMLElement elements[] = properties.getChildren();
      offset = new float[elements.length];
      color = new int[elements.length];

      // <stop  offset="0" style="stop-color:#967348"/>
      for (int i = 0; i < elements.length; i++) {
        XMLElement elem = elements[i];
        String name = elem.getName();
        if (name.equals("stop")) {
          offset[count] = elem.getFloatAttribute("offset");
          String style = elem.getStringAttribute("style");
          HashMap<String, String> styles = parseStyleAttributes(style);

          String colorStr = styles.get("stop-color");
          if (colorStr == null) colorStr = "#000000";
          String opacityStr = styles.get("stop-opacity");
          if (opacityStr == null) opacityStr = "1";
          int tupacity = (int) (PApplet.parseFloat(opacityStr) * 255);
          color[count] = (tupacity << 24) |
            Integer.parseInt(colorStr.substring(1), 16);
          count++;
        }
      }
      offset = PApplet.subset(offset, 0, count);
      color = PApplet.subset(color, 0, count);
    }
  }


  class LinearGradient extends Gradient {
    float x1, y1, x2, y2;

    public LinearGradient(PShapeSVG parent, XMLElement properties) {
      super(parent, properties);

      this.x1 = properties.getFloatAttribute("x1");
      this.y1 = properties.getFloatAttribute("y1");
      this.x2 = properties.getFloatAttribute("x2");
      this.y2 = properties.getFloatAttribute("y2");

      String transformStr =
        properties.getStringAttribute("gradientTransform");

      if (transformStr != null) {
        float t[] = parseMatrix(transformStr).get(null);
        this.transform = new AffineTransform(t[0], t[3], t[1], t[4], t[2], t[5]);

        Point2D t1 = transform.transform(new Point2D.Float(x1, y1), null);
        Point2D t2 = transform.transform(new Point2D.Float(x2, y2), null);

        this.x1 = (float) t1.getX();
        this.y1 = (float) t1.getY();
        this.x2 = (float) t2.getX();
        this.y2 = (float) t2.getY();
      }
    }
  }


  class RadialGradient extends Gradient {
    float cx, cy, r;

    public RadialGradient(PShapeSVG parent, XMLElement properties) {
      super(parent, properties);

      this.cx = properties.getFloatAttribute("cx");
      this.cy = properties.getFloatAttribute("cy");
      this.r = properties.getFloatAttribute("r");

      String transformStr =
        properties.getStringAttribute("gradientTransform");

      if (transformStr != null) {
        float t[] = parseMatrix(transformStr).get(null);
        this.transform = new AffineTransform(t[0], t[3], t[1], t[4], t[2], t[5]);

        Point2D t1 = transform.transform(new Point2D.Float(cx, cy), null);
        Point2D t2 = transform.transform(new Point2D.Float(cx + r, cy), null);

        this.cx = (float) t1.getX();
        this.cy = (float) t1.getY();
        this.r = (float) (t2.getX() - t1.getX());
      }
    }
  }



  class LinearGradientPaint implements Paint {
    float x1, y1, x2, y2;
    float[] offset;
    int[] color;
    int count;
    float opacity;

    public LinearGradientPaint(float x1, float y1, float x2, float y2,
                               float[] offset, int[] color, int count,
                               float opacity) {
      this.x1 = x1;
      this.y1 = y1;
      this.x2 = x2;
      this.y2 = y2;
      this.offset = offset;
      this.color = color;
      this.count = count;
      this.opacity = opacity;
    }

    public PaintContext createContext(ColorModel cm,
                                      Rectangle deviceBounds, Rectangle2D userBounds,
                                      AffineTransform xform, RenderingHints hints) {
      Point2D t1 = xform.transform(new Point2D.Float(x1, y1), null);
      Point2D t2 = xform.transform(new Point2D.Float(x2, y2), null);
      return new LinearGradientContext((float) t1.getX(), (float) t1.getY(),
                                       (float) t2.getX(), (float) t2.getY());
    }

    public int getTransparency() {
      return TRANSLUCENT;  // why not.. rather than checking each color
    }

    public class LinearGradientContext implements PaintContext {
      int ACCURACY = 2;
      float tx1, ty1, tx2, ty2;

      public LinearGradientContext(float tx1, float ty1, float tx2, float ty2) {
        this.tx1 = tx1;
        this.ty1 = ty1;
        this.tx2 = tx2;
        this.ty2 = ty2;
      }

      public void dispose() { }

      public ColorModel getColorModel() { return ColorModel.getRGBdefault(); }

      public Raster getRaster(int x, int y, int w, int h) {
        WritableRaster raster =
          getColorModel().createCompatibleWritableRaster(w, h);

        int[] data = new int[w * h * 4];

        // make normalized version of base vector
        float nx = tx2 - tx1;
        float ny = ty2 - ty1;
        float len = (float) Math.sqrt(nx*nx + ny*ny);
        if (len != 0) {
          nx /= len;
          ny /= len;
        }

        int span = (int) PApplet.dist(tx1, ty1, tx2, ty2) * ACCURACY;
        if (span <= 0) {
          //System.err.println("span is too small");
          // annoying edge case where the gradient isn't legit
          int index = 0;
          for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
              data[index++] = 0;
              data[index++] = 0;
              data[index++] = 0;
              data[index++] = 255;
            }
          }

        } else {
          int[][] interp = new int[span][4];
          int prev = 0;
          for (int i = 1; i < count; i++) {
            int c0 = color[i-1];
            int c1 = color[i];
            int last = (int) (offset[i] * (span-1));
            //System.out.println("last is " + last);
            for (int j = prev; j <= last; j++) {
              float btwn = PApplet.norm(j, prev, last);
              interp[j][0] = (int) PApplet.lerp((c0 >> 16) & 0xff, (c1 >> 16) & 0xff, btwn);
              interp[j][1] = (int) PApplet.lerp((c0 >> 8) & 0xff, (c1 >> 8) & 0xff, btwn);
              interp[j][2] = (int) PApplet.lerp(c0 & 0xff, c1 & 0xff, btwn);
              interp[j][3] = (int) (PApplet.lerp((c0 >> 24) & 0xff, (c1 >> 24) & 0xff, btwn) * opacity);
              //System.out.println(j + " " + interp[j][0] + " " + interp[j][1] + " " + interp[j][2]);
            }
            prev = last;
          }

          int index = 0;
          for (int j = 0; j < h; j++) {
            for (int i = 0; i < w; i++) {
              //float distance = 0; //PApplet.dist(cx, cy, x + i, y + j);
              //int which = PApplet.min((int) (distance * ACCURACY), interp.length-1);
              float px = (x + i) - tx1;
              float py = (y + j) - ty1;
              // distance up the line is the dot product of the normalized
              // vector of the gradient start/stop by the point being tested
              int which = (int) ((px*nx + py*ny) * ACCURACY);
              if (which < 0) which = 0;
              if (which > interp.length-1) which = interp.length-1;
              //if (which > 138) System.out.println("grabbing " + which);

              data[index++] = interp[which][0];
              data[index++] = interp[which][1];
              data[index++] = interp[which][2];
              data[index++] = interp[which][3];
            }
          }
        }
        raster.setPixels(0, 0, w, h, data);

        return raster;
      }
    }
  }


  class RadialGradientPaint implements Paint {
    float cx, cy, radius;
    float[] offset;
    int[] color;
    int count;
    float opacity;

    public RadialGradientPaint(float cx, float cy, float radius,
                               float[] offset, int[] color, int count,
                               float opacity) {
      this.cx = cx;
      this.cy = cy;
      this.radius = radius;
      this.offset = offset;
      this.color = color;
      this.count = count;
      this.opacity = opacity;
    }

    public PaintContext createContext(ColorModel cm,
                                      Rectangle deviceBounds, Rectangle2D userBounds,
                                      AffineTransform xform, RenderingHints hints) {
      return new RadialGradientContext();
    }

    public int getTransparency() {
      return TRANSLUCENT;
    }

    public class RadialGradientContext implements PaintContext {
      int ACCURACY = 5;

      public void dispose() {}

      public ColorModel getColorModel() { return ColorModel.getRGBdefault(); }

      public Raster getRaster(int x, int y, int w, int h) {
        WritableRaster raster =
          getColorModel().createCompatibleWritableRaster(w, h);

        int span = (int) radius * ACCURACY;
        int[][] interp = new int[span][4];
        int prev = 0;
        for (int i = 1; i < count; i++) {
          int c0 = color[i-1];
          int c1 = color[i];
          int last = (int) (offset[i] * (span - 1));
          for (int j = prev; j <= last; j++) {
            float btwn = PApplet.norm(j, prev, last);
            interp[j][0] = (int) PApplet.lerp((c0 >> 16) & 0xff, (c1 >> 16) & 0xff, btwn);
            interp[j][1] = (int) PApplet.lerp((c0 >> 8) & 0xff, (c1 >> 8) & 0xff, btwn);
            interp[j][2] = (int) PApplet.lerp(c0 & 0xff, c1 & 0xff, btwn);
            interp[j][3] = (int) (PApplet.lerp((c0 >> 24) & 0xff, (c1 >> 24) & 0xff, btwn) * opacity);
          }
          prev = last;
        }

        int[] data = new int[w * h * 4];
        int index = 0;
        for (int j = 0; j < h; j++) {
          for (int i = 0; i < w; i++) {
            float distance = PApplet.dist(cx, cy, x + i, y + j);
            int which = PApplet.min((int) (distance * ACCURACY), interp.length-1);

            data[index++] = interp[which][0];
            data[index++] = interp[which][1];
            data[index++] = interp[which][2];
            data[index++] = interp[which][3];
          }
        }
        raster.setPixels(0, 0, w, h, data);

        return raster;
      }
    }
  }


  protected Paint calcGradientPaint(Gradient gradient) {
    if (gradient instanceof LinearGradient) {
      LinearGradient grad = (LinearGradient) gradient;
      return new LinearGradientPaint(grad.x1, grad.y1, grad.x2, grad.y2,
                                     grad.offset, grad.color, grad.count,
                                     opacity);

    } else if (gradient instanceof RadialGradient) {
      RadialGradient grad = (RadialGradient) gradient;
      return new RadialGradientPaint(grad.cx, grad.cy, grad.r,
                                     grad.offset, grad.color, grad.count,
                                     opacity);
    }
    return null;
  }


//  protected Paint calcGradientPaint(Gradient gradient,
//                                    float x1, float y1, float x2, float y2) {
//    if (gradient instanceof LinearGradient) {
//      LinearGradient grad = (LinearGradient) gradient;
//      return new LinearGradientPaint(x1, y1, x2, y2,
//                                     grad.offset, grad.color, grad.count,
//                                     opacity);
//    }
//    throw new RuntimeException("Not a linear gradient.");
//  }


//  protected Paint calcGradientPaint(Gradient gradient,
//                                    float cx, float cy, float r) {
//    if (gradient instanceof RadialGradient) {
//      RadialGradient grad = (RadialGradient) gradient;
//      return new RadialGradientPaint(cx, cy, r,
//                                     grad.offset, grad.color, grad.count,
//                                     opacity);
//    }
//    throw new RuntimeException("Not a radial gradient.");
//  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  protected void styles(PGraphics g) {
    super.styles(g);

    if (g instanceof PGraphicsJava2D) {
      PGraphicsJava2D p2d = (PGraphicsJava2D) g;

      if (strokeGradient != null) {
        p2d.strokeGradient = true;
        p2d.strokeGradientObject = strokeGradientPaint;
      } else {
        // need to shut off, in case parent object has a gradient applied
        //p2d.strokeGradient = false;
      }
      if (fillGradient != null) {
        p2d.fillGradient = true;
        p2d.fillGradientObject = fillGradientPaint;
      } else {
        // need to shut off, in case parent object has a gradient applied
        //p2d.fillGradient = false;
      }
    }
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  //public void drawImpl(PGraphics g) {
  // do nothing
  //}


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Get a particular element based on its SVG ID. When editing SVG by hand,
   * this is the id="" tag on any SVG element. When editing from Illustrator,
   * these IDs can be edited by expanding the layers palette. The names used
   * in the layers palette, both for the layers or the shapes and groups
   * beneath them can be used here.
   * <PRE>
   * // This code grabs "Layer 3" and the shapes beneath it.
   * SVG layer3 = svg.getChild("Layer 3");
   * </PRE>
   */
  public PShape getChild(String name) {
    PShape found = super.getChild(name);
    if (found == null) {
      // Otherwise try with underscores instead of spaces
      // (this is how Illustrator handles spaces in the layer names).
      found = super.getChild(name.replace(' ', '_'));
    }
    // Set bounding box based on the parent bounding box
    if (found != null) {
//      found.x = this.x;
//      found.y = this.y;
      found.width = this.width;
      found.height = this.height;
    }
    return found;
  }


  /**
   * Prints out the SVG document. Useful for parsing.
   */
  public void print() {
    PApplet.println(element.toString());
  }
}
