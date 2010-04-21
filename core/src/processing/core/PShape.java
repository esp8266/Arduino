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

import java.util.HashMap;

import processing.core.PApplet;


/**
 * Datatype for storing shapes. Processing can currently load and display SVG (Scalable Vector Graphics) shapes.
 * Before a shape is used, it must be loaded with the <b>loadShape()</b> function. The <b>shape()</b> function is used to draw the shape to the display window.
 * The <b>PShape</b> object contain a group of methods, linked below, that can operate on the shape data. 
 * <br><br>The <b>loadShape()</b> method supports SVG files created with Inkscape and Adobe Illustrator.
 * It is not a full SVG implementation, but offers some straightforward support for handling vector data.
 * =advanced
 * 
 * In-progress class to handle shape data, currently to be considered of
 * alpha or beta quality. Major structural work may be performed on this class
 * after the release of Processing 1.0. Such changes may include:
 *
 * <ul>
 * <li> addition of proper accessors to read shape vertex and coloring data
 * (this is the second most important part of having a PShape class after all).
 * <li> a means of creating PShape objects ala beginShape() and endShape().
 * <li> load(), update(), and cache methods ala PImage, so that shapes can
 * have renderer-specific optimizations, such as vertex arrays in OpenGL.
 * <li> splitting this class into multiple classes to handle different
 * varieties of shape data (primitives vs collections of vertices vs paths)
 * <li> change of package declaration, for instance moving the code into
 * package processing.shape (if the code grows too much).
 * </ul>
 *
 * <p>For the time being, this class and its shape() and loadShape() friends in
 * PApplet exist as placeholders for more exciting things to come. If you'd
 * like to work with this class, make a subclass (see how PShapeSVG works)
 * and you can play with its internal methods all you like.</p>
 *
 * <p>Library developers are encouraged to create PShape objects when loading
 * shape data, so that they can eventually hook into the bounty that will be
 * the PShape interface, and the ease of loadShape() and shape().</p>
 * 
 * @webref Shape
 * @usage Web &amp; Application
 * @see PApplet#shape(PShape)
 * @see PApplet#loadShape(String)
 * @see PApplet#shapeMode(int)
 * @instanceName sh any variable of type PShape
 */
public class PShape implements PConstants {

  protected String name;
  protected HashMap<String,PShape> nameTable;

  /** Generic, only draws its child objects. */
  static public final int GROUP = 0;
  /** A line, ellipse, arc, image, etc. */
  static public final int PRIMITIVE = 1;
  /** A series of vertex, curveVertex, and bezierVertex calls. */
  static public final int PATH = 2;
  /** Collections of vertices created with beginShape(). */
  static public final int GEOMETRY = 3;
  /** The shape type, one of GROUP, PRIMITIVE, PATH, or GEOMETRY. */
  protected int family;

  /** ELLIPSE, LINE, QUAD; TRIANGLE_FAN, QUAD_STRIP; etc. */
  protected int kind;

  protected PMatrix matrix;

  /** Texture or image data associated with this shape. */
  protected PImage image;

  // boundary box of this shape
  //protected float x;
  //protected float y;
  //protected float width;
  //protected float height;
  /**
   * The width of the PShape document.
   * @webref
   * @brief  	Shape document width
   */
  public float width;
  /**
   * The width of the PShape document.
   * @webref
   * @brief  	Shape document height
   */
  public float height;

  // set to false if the object is hidden in the layers palette
  protected boolean visible = true;

  protected boolean stroke;
  protected int strokeColor;
  protected float strokeWeight; // default is 1
  protected int strokeCap;
  protected int strokeJoin;

  protected boolean fill;
  protected int fillColor;

  /** Temporary toggle for whether styles should be honored. */
  protected boolean style = true;

  /** For primitive shapes in particular, parms like x/y/w/h or x1/y1/x2/y2. */
  protected float[] params;

  protected int vertexCount;
  /**
   * When drawing POLYGON shapes, the second param is an array of length
   * VERTEX_FIELD_COUNT. When drawing PATH shapes, the second param has only
   * two variables.
   */
  protected float[][] vertices;

  static public final int VERTEX = 0;
  static public final int BEZIER_VERTEX = 1;
  static public final int CURVE_VERTEX = 2;
  static public final int BREAK = 3;
  /** Array of VERTEX, BEZIER_VERTEX, and CURVE_VERTEXT calls. */
  protected int vertexCodeCount;
  protected int[] vertexCodes;
  /** True if this is a closed path. */
  protected boolean close;

  // should this be called vertices (consistent with PGraphics internals)
  // or does that hurt flexibility?

  protected PShape parent;
  protected int childCount;
  protected PShape[] children;

  // POINTS, LINES, xLINE_STRIP, xLINE_LOOP
  // TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN
  // QUADS, QUAD_STRIP
  // xPOLYGON
//  static final int PATH = 1;  // POLYGON, LINE_LOOP, LINE_STRIP
//  static final int GROUP = 2;

  // how to handle rectmode/ellipsemode?
  // are they bitshifted into the constant?
  // CORNER, CORNERS, CENTER, (CENTER_RADIUS?)
//  static final int RECT = 3; // could just be QUAD, but would be x1/y1/x2/y2
//  static final int ELLIPSE = 4;
//
//  static final int VERTEX = 7;
//  static final int CURVE = 5;
//  static final int BEZIER = 6;


  // fill and stroke functions will need a pointer to the parent
  // PGraphics object.. may need some kind of createShape() fxn
  // or maybe the values are stored until draw() is called?

  // attaching images is very tricky.. it's a different type of data

  // material parameters will be thrown out,
  // except those currently supported (kinds of lights)

  // pivot point for transformations
//  public float px;
//  public float py;


  public PShape() {
    this.family = GROUP;
  }


  public PShape(int family) {
    this.family = family;
  }


  public void setName(String name) {
    this.name = name;
  }


  public String getName() {
    return name;
  }

  /**
   * Returns a boolean value "true" if the image is set to be visible, "false" if not. This is modified with the <b>setVisible()</b> parameter.
   * <br><br>The visibility of a shape is usually controlled by whatever program created the SVG file.
   * For instance, this parameter is controlled by showing or hiding the shape in the layers palette in Adobe Illustrator.
   * 
   * @webref
   * @brief Returns a boolean value "true" if the image is set to be visible, "false" if not
   */
  public boolean isVisible() {
    return visible;
  }

  /**
   * Sets the shape to be visible or invisible. This is determined by the value of the <b>visible</b> parameter.
   * <br><br>The visibility of a shape is usually controlled by whatever program created the SVG file.
   * For instance, this parameter is controlled by showing or hiding the shape in the layers palette in Adobe Illustrator.
   * @param visible "false" makes the shape invisible and "true" makes it visible
   * @webref
   * @brief Sets the shape to be visible or invisible
   */
  public void setVisible(boolean visible) {
    this.visible = visible;
  }


  /**
   * Disables the shape's style data and uses Processing's current styles. Styles include attributes such as colors, stroke weight, and stroke joints.
   *  =advanced
   * Overrides this shape's style information and uses PGraphics styles and
   * colors. Identical to ignoreStyles(true). Also disables styles for all
   * child shapes.
   * @webref
   * @brief  	Disables the shape's style data and uses Processing styles
   */
  public void disableStyle() {
    style = false;

    for (int i = 0; i < childCount; i++) {
      children[i].disableStyle();
    }
  }


  /**
   * Enables the shape's style data and ignores Processing's current styles. Styles include attributes such as colors, stroke weight, and stroke joints.
   * @webref
   * @brief Enables the shape's style data and ignores the Processing styles
   */
  public void enableStyle() {
    style = true;

    for (int i = 0; i < childCount; i++) {
      children[i].enableStyle();
    }
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


//  protected void checkBounds() {
//    if (width == 0 || height == 0) {
//      // calculate bounds here (also take kids into account)
//      width = 1;
//      height = 1;
//    }
//  }


  /**
   * Get the width of the drawing area (not necessarily the shape boundary). 
   */
  public float getWidth() {
    //checkBounds();
    return width;
  }


  /**
   * Get the height of the drawing area (not necessarily the shape boundary). 
   */
  public float getHeight() {
    //checkBounds();
    return height;
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /*
  boolean strokeSaved;
  int strokeColorSaved;
  float strokeWeightSaved;
  int strokeCapSaved;
  int strokeJoinSaved;

  boolean fillSaved;
  int fillColorSaved;

  int rectModeSaved;
  int ellipseModeSaved;
  int shapeModeSaved;
  */


  protected void pre(PGraphics g) {
    if (matrix != null) {
      g.pushMatrix();
      g.applyMatrix(matrix);
    }

    /*
    strokeSaved = g.stroke;
    strokeColorSaved = g.strokeColor;
    strokeWeightSaved = g.strokeWeight;
    strokeCapSaved = g.strokeCap;
    strokeJoinSaved = g.strokeJoin;

    fillSaved = g.fill;
    fillColorSaved = g.fillColor;

    rectModeSaved = g.rectMode;
    ellipseModeSaved = g.ellipseMode;
    shapeModeSaved = g.shapeMode;
    */
    if (style) {
      g.pushStyle();
      styles(g);
    }
  }


  protected void styles(PGraphics g) {
    // should not be necessary because using only the int version of color
    //parent.colorMode(PConstants.RGB, 255);

    if (stroke) {
      g.stroke(strokeColor);
      g.strokeWeight(strokeWeight);
      g.strokeCap(strokeCap);
      g.strokeJoin(strokeJoin);
    } else {
      g.noStroke();
    }

    if (fill) {
      //System.out.println("filling " + PApplet.hex(fillColor));
      g.fill(fillColor);
    } else {
      g.noFill();
    }
  }


  public void post(PGraphics g) {
//    for (int i = 0; i < childCount; i++) {
//      children[i].draw(g);
//    }

    /*
    // TODO this is not sufficient, since not saving fillR et al.
    g.stroke = strokeSaved;
    g.strokeColor = strokeColorSaved;
    g.strokeWeight = strokeWeightSaved;
    g.strokeCap = strokeCapSaved;
    g.strokeJoin = strokeJoinSaved;

    g.fill = fillSaved;
    g.fillColor = fillColorSaved;

    g.ellipseMode = ellipseModeSaved;
    */

    if (matrix != null) {
      g.popMatrix();
    }

    if (style) {
      g.popStyle();
    }
  }


  /**
   * Called by the following (the shape() command adds the g)
   * PShape s = loadShapes("blah.svg");
   * shape(s);
   */
  public void draw(PGraphics g) {
    if (visible) {
      pre(g);
      drawImpl(g);
      post(g);
    }
  }


  /**
   * Draws the SVG document.
   */
  public void drawImpl(PGraphics g) {
    //System.out.println("drawing " + family);
    if (family == GROUP) {
      drawGroup(g);
    } else if (family == PRIMITIVE) {
      drawPrimitive(g);
    } else if (family == GEOMETRY) {
      drawGeometry(g);
    } else if (family == PATH) {
      drawPath(g);
    }
  }


  protected void drawGroup(PGraphics g) {
    for (int i = 0; i < childCount; i++) {
      children[i].draw(g);
    }
  }


  protected void drawPrimitive(PGraphics g) {
    if (kind == POINT) {
      g.point(params[0], params[1]);

    } else if (kind == LINE) {
      if (params.length == 4) {  // 2D
        g.line(params[0], params[1],
               params[2], params[3]);
      } else {  // 3D
        g.line(params[0], params[1], params[2],
               params[3], params[4], params[5]);
      }

    } else if (kind == TRIANGLE) {
      g.triangle(params[0], params[1],
                 params[2], params[3],
                 params[4], params[5]);

    } else if (kind == QUAD) {
      g.quad(params[0], params[1],
             params[2], params[3],
             params[4], params[5],
             params[6], params[7]);

    } else if (kind == RECT) {
      if (image != null) {
        g.imageMode(CORNER);
        g.image(image, params[0], params[1], params[2], params[3]);
      } else {
        g.rectMode(CORNER);
        g.rect(params[0], params[1], params[2], params[3]);
      }

    } else if (kind == ELLIPSE) {
      g.ellipseMode(CORNER);
      g.ellipse(params[0], params[1], params[2], params[3]);

    } else if (kind == ARC) {
      g.ellipseMode(CORNER);
      g.arc(params[0], params[1], params[2], params[3], params[4], params[5]);

    } else if (kind == BOX) {
      if (params.length == 1) {
        g.box(params[0]);
      } else {
        g.box(params[0], params[1], params[2]);
      }

    } else if (kind == SPHERE) {
      g.sphere(params[0]);
    }
  }


  protected void drawGeometry(PGraphics g) {
    g.beginShape(kind);
    if (style) {
      for (int i = 0; i < vertexCount; i++) {
        g.vertex(vertices[i]);
      }
    } else {
      for (int i = 0; i < vertexCount; i++) {
        float[] vert = vertices[i];
        if (vert[PGraphics.Z] == 0) {
          g.vertex(vert[X], vert[Y]);
        } else {
          g.vertex(vert[X], vert[Y], vert[Z]);
        }
      }
    }
    g.endShape();
  }


  /*
  protected void drawPath(PGraphics g) {
    g.beginShape();
    for (int j = 0; j < childCount; j++) {
      if (j > 0) g.breakShape();
      int count = children[j].vertexCount;
      float[][] vert = children[j].vertices;
      int[] code = children[j].vertexCodes;

      for (int i = 0; i < count; i++) {
        if (style) {
          if (children[j].fill) {
            g.fill(vert[i][R], vert[i][G], vert[i][B]);
          } else {
            g.noFill();
          }
          if (children[j].stroke) {
            g.stroke(vert[i][R], vert[i][G], vert[i][B]);
          } else {
            g.noStroke();
          }
        }
        g.edge(vert[i][EDGE] == 1);

        if (code[i] == VERTEX) {
          g.vertex(vert[i]);

        } else if (code[i] == BEZIER_VERTEX) {
          float z0 = vert[i+0][Z];
          float z1 = vert[i+1][Z];
          float z2 = vert[i+2][Z];
          if (z0 == 0 && z1 == 0 && z2 == 0) {
            g.bezierVertex(vert[i+0][X], vert[i+0][Y], z0,
                           vert[i+1][X], vert[i+1][Y], z1,
                           vert[i+2][X], vert[i+2][Y], z2);
          } else {
            g.bezierVertex(vert[i+0][X], vert[i+0][Y],
                           vert[i+1][X], vert[i+1][Y],
                           vert[i+2][X], vert[i+2][Y]);
          }
        } else if (code[i] == CURVE_VERTEX) {
          float z = vert[i][Z];
          if (z == 0) {
            g.curveVertex(vert[i][X], vert[i][Y]);
          } else {
            g.curveVertex(vert[i][X], vert[i][Y], z);
          }
        }
      }
    }
    g.endShape();
  }
  */


  protected void drawPath(PGraphics g) {
    // Paths might be empty (go figure)
    // http://dev.processing.org/bugs/show_bug.cgi?id=982
    if (vertices == null) return;

    g.beginShape();

    if (vertexCodeCount == 0) {  // each point is a simple vertex
      if (vertices[0].length == 2) {  // drawing 2D vertices
        for (int i = 0; i < vertexCount; i++) {
          g.vertex(vertices[i][X], vertices[i][Y]);
        }
      } else {  // drawing 3D vertices
        for (int i = 0; i < vertexCount; i++) {
          g.vertex(vertices[i][X], vertices[i][Y], vertices[i][Z]);
        }
      }

    } else {  // coded set of vertices
      int index = 0;

      if (vertices[0].length == 2) {  // drawing a 2D path
        for (int j = 0; j < vertexCodeCount; j++) {
          switch (vertexCodes[j]) {

          case VERTEX:
            g.vertex(vertices[index][X], vertices[index][Y]);
            index++;
            break;

          case BEZIER_VERTEX:
            g.bezierVertex(vertices[index+0][X], vertices[index+0][Y],
                           vertices[index+1][X], vertices[index+1][Y],
                           vertices[index+2][X], vertices[index+2][Y]);
            index += 3;
            break;

          case CURVE_VERTEX:
            g.curveVertex(vertices[index][X], vertices[index][Y]);
            index++;

          case BREAK:
            g.breakShape();
          }
        }
      } else {  // drawing a 3D path
        for (int j = 0; j < vertexCodeCount; j++) {
          switch (vertexCodes[j]) {

          case VERTEX:
            g.vertex(vertices[index][X], vertices[index][Y], vertices[index][Z]);
            index++;
            break;

          case BEZIER_VERTEX:
            g.bezierVertex(vertices[index+0][X], vertices[index+0][Y], vertices[index+0][Z],
                           vertices[index+1][X], vertices[index+1][Y], vertices[index+1][Z],
                           vertices[index+2][X], vertices[index+2][Y], vertices[index+2][Z]);
            index += 3;
            break;

          case CURVE_VERTEX:
            g.curveVertex(vertices[index][X], vertices[index][Y], vertices[index][Z]);
            index++;

          case BREAK:
            g.breakShape();
          }
        }
      }
    }
    g.endShape(close ? CLOSE : OPEN);
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  public int getChildCount() {
    return childCount;
  }

  /**
   * 
   * @param index the layer position of the shape to get
   */
  public PShape getChild(int index) {
    return children[index];
  }

  /**
   * Extracts a child shape from a parent shape. Specify the name of the shape with the <b>target</b> parameter.
   * The shape is returned as a <b>PShape</b> object, or <b>null</b> is returned if there is an error.
   * @param target the name of the shape to get
   * @webref
   * @brief Returns a child element of a shape as a PShape object
   */
  public PShape getChild(String target) {
    if (name != null && name.equals(target)) {
      return this;
    }
    if (nameTable != null) {
      PShape found = nameTable.get(target);
      if (found != null) return found;
    }
    for (int i = 0; i < childCount; i++) {
      PShape found = children[i].getChild(target);
      if (found != null) return found;
    }
    return null;
  }


  /**
   * Same as getChild(name), except that it first walks all the way up the
   * hierarchy to the farthest parent, so that children can be found anywhere.
   */
  public PShape findChild(String target) {
    if (parent == null) {
      return getChild(target);

    } else {
      return parent.findChild(target);
    }
  }


  // can't be just 'add' because that suggests additive geometry
  public void addChild(PShape who) {
    if (children == null) {
      children = new PShape[1];
    }
    if (childCount == children.length) {
      children = (PShape[]) PApplet.expand(children);
    }
    children[childCount++] = who;
    who.parent = this;

    if (who.getName() != null) {
      addName(who.getName(), who);
    }
  }


  /**
   * Add a shape to the name lookup table.
   */
  protected void addName(String nom, PShape shape) {
    if (parent != null) {
      parent.addName(nom, shape);
    } else {
      if (nameTable == null) {
        nameTable = new HashMap<String,PShape>();
      }
      nameTable.put(nom, shape);
    }
  }


//  public PShape createGroup() {
//    PShape group = new PShape();
//    group.kind = GROUP;
//    addChild(group);
//    return group;
//  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  // translate, rotate, scale, apply (no push/pop)
  //   these each call matrix.translate, etc
  // if matrix is null when one is called,
  //   it is created and set to identity

  public void translate(float tx, float ty) {
    checkMatrix(2);
    matrix.translate(tx, ty);
  }

  /**
   * Specifies an amount to displace the shape. The <b>x</b> parameter specifies left/right translation, the <b>y</b> parameter specifies up/down translation, and the <b>z</b> parameter specifies translations toward/away from the screen. Subsequent calls to the method accumulates the effect. For example, calling <b>translate(50, 0)</b> and then <b>translate(20, 0)</b> is the same as <b>translate(70, 0)</b>. This transformation is applied directly to the shape, it's not refreshed each time <b>draw()</b> is run. 
   * <br><br>Using this method with the <b>z</b> parameter requires using the P3D or OPENGL parameter in combination with size.
   * @webref
   * @param tx left/right translation
   * @param ty up/down translation
   * @param tz forward/back translation
   * @brief Displaces the shape
   */
  public void translate(float tx, float ty, float tz) {
    checkMatrix(3);
    matrix.translate(tx, ty, 0);
  }
  
  /**
   * Rotates a shape around the x-axis the amount specified by the <b>angle</b> parameter. Angles should be specified in radians (values from 0 to TWO_PI) or converted to radians with the <b>radians()</b> method.
   * <br><br>Shapes are always rotated around the upper-left corner of their bounding box. Positive numbers rotate objects in a clockwise direction.
   * Subsequent calls to the method accumulates the effect. For example, calling <b>rotateX(HALF_PI)</b> and then <b>rotateX(HALF_PI)</b> is the same as <b>rotateX(PI)</b>.
   * This transformation is applied directly to the shape, it's not refreshed each time <b>draw()</b> is run.  
   * <br><br>This method requires a 3D renderer. You need to pass P3D or OPENGL as a third parameter into the <b>size()</b> method as shown in the example above.
   * @param angle angle of rotation specified in radians
   * @webref
   * @brief Rotates the shape around the x-axis
   */
  public void rotateX(float angle) {
    rotate(angle, 1, 0, 0);
  }

  /**
   * Rotates a shape around the y-axis the amount specified by the <b>angle</b> parameter. Angles should be specified in radians (values from 0 to TWO_PI) or converted to radians with the <b>radians()</b> method.
   * <br><br>Shapes are always rotated around the upper-left corner of their bounding box. Positive numbers rotate objects in a clockwise direction.
   * Subsequent calls to the method accumulates the effect. For example, calling <b>rotateY(HALF_PI)</b> and then <b>rotateY(HALF_PI)</b> is the same as <b>rotateY(PI)</b>.
   * This transformation is applied directly to the shape, it's not refreshed each time <b>draw()</b> is run. 
   * <br><br>This method requires a 3D renderer. You need to pass P3D or OPENGL as a third parameter into the <b>size()</b> method as shown in the example above.
   * @param angle angle of rotation specified in radians
   * @webref
   * @brief Rotates the shape around the y-axis
   */
  public void rotateY(float angle) {
    rotate(angle, 0, 1, 0);
  }


  /**
   * Rotates a shape around the z-axis the amount specified by the <b>angle</b> parameter. Angles should be specified in radians (values from 0 to TWO_PI) or converted to radians with the <b>radians()</b> method.
   * <br><br>Shapes are always rotated around the upper-left corner of their bounding box. Positive numbers rotate objects in a clockwise direction.
   * Subsequent calls to the method accumulates the effect. For example, calling <b>rotateZ(HALF_PI)</b> and then <b>rotateZ(HALF_PI)</b> is the same as <b>rotateZ(PI)</b>.
   * This transformation is applied directly to the shape, it's not refreshed each time <b>draw()</b> is run. 
   * <br><br>This method requires a 3D renderer. You need to pass P3D or OPENGL as a third parameter into the <b>size()</b> method as shown in the example above.
   * @param angle angle of rotation specified in radians
   * @webref
   * @brief Rotates the shape around the z-axis
   */
  public void rotateZ(float angle) {
    rotate(angle, 0, 0, 1);
  }
  
  /**
   * Rotates a shape the amount specified by the <b>angle</b> parameter. Angles should be specified in radians (values from 0 to TWO_PI) or converted to radians with the <b>radians()</b> method.
   * <br><br>Shapes are always rotated around the upper-left corner of their bounding box. Positive numbers rotate objects in a clockwise direction.
   * Transformations apply to everything that happens after and subsequent calls to the method accumulates the effect.
   * For example, calling <b>rotate(HALF_PI)</b> and then <b>rotate(HALF_PI)</b> is the same as <b>rotate(PI)</b>.
   * This transformation is applied directly to the shape, it's not refreshed each time <b>draw()</b> is run.
   * @param angle angle of rotation specified in radians 
   * @webref
   * @brief Rotates the shape
   */
  public void rotate(float angle) {
    checkMatrix(2);  // at least 2...
    matrix.rotate(angle);
  }


  public void rotate(float angle, float v0, float v1, float v2) {
    checkMatrix(3);
    matrix.rotate(angle, v0, v1, v2);
  }


  //
  
  /**
   * @param s percentage to scale the object
   */
  public void scale(float s) {
    checkMatrix(2);  // at least 2...
    matrix.scale(s);
  }


  public void scale(float x, float y) {
    checkMatrix(2);
    matrix.scale(x, y);
  }


  /**
   * Increases or decreases the size of a shape by expanding and contracting vertices. Shapes always scale from the relative origin of their bounding box.
   * Scale values are specified as decimal percentages. For example, the method call <b>scale(2.0)</b> increases the dimension of a shape by 200%.
   * Subsequent calls to the method multiply the effect. For example, calling <b>scale(2.0)</b> and then <b>scale(1.5)</b> is the same as <b>scale(3.0)</b>.
   * This transformation is applied directly to the shape, it's not refreshed each time <b>draw()</b> is run. 
   * <br><br>Using this fuction with the <b>z</b> parameter requires passing P3D or OPENGL into the size() parameter.
   * @param x percentage to scale the object in the x-axis
   * @param y percentage to scale the object in the y-axis
   * @param z percentage to scale the object in the z-axis
   * @webref
   * @brief Increases and decreases the size of a shape
   */
  public void scale(float x, float y, float z) {
    checkMatrix(3);
    matrix.scale(x, y, z);
  }


  //


  public void resetMatrix() {
    checkMatrix(2);
    matrix.reset();
  }


  public void applyMatrix(PMatrix source) {
    if (source instanceof PMatrix2D) {
      applyMatrix((PMatrix2D) source);
    } else if (source instanceof PMatrix3D) {
      applyMatrix((PMatrix3D) source);
    }
  }


  public void applyMatrix(PMatrix2D source) {
    applyMatrix(source.m00, source.m01, 0, source.m02,
                source.m10, source.m11, 0, source.m12,
                0, 0, 1, 0,
                0, 0, 0, 1);
  }


  public void applyMatrix(float n00, float n01, float n02,
                          float n10, float n11, float n12) {
    checkMatrix(2);
    matrix.apply(n00, n01, n02, 0,
                 n10, n11, n12, 0,
                 0,   0,   1,   0,
                 0,   0,   0,   1);
  }


  public void apply(PMatrix3D source) {
    applyMatrix(source.m00, source.m01, source.m02, source.m03,
                source.m10, source.m11, source.m12, source.m13,
                source.m20, source.m21, source.m22, source.m23,
                source.m30, source.m31, source.m32, source.m33);
  }


  public void applyMatrix(float n00, float n01, float n02, float n03,
                          float n10, float n11, float n12, float n13,
                          float n20, float n21, float n22, float n23,
                          float n30, float n31, float n32, float n33) {
    checkMatrix(3);
    matrix.apply(n00, n01, n02, n03,
                 n10, n11, n12, n13,
                 n20, n21, n22, n23,
                 n30, n31, n32, n33);
  }


  //


  /**
   * Make sure that the shape's matrix is 1) not null, and 2) has a matrix
   * that can handle <em>at least</em> the specified number of dimensions.
   */
  protected void checkMatrix(int dimensions) {
    if (matrix == null) {
      if (dimensions == 2) {
        matrix = new PMatrix2D();
      } else {
        matrix = new PMatrix3D();
      }
    } else if (dimensions == 3 && (matrix instanceof PMatrix2D)) {
      // time for an upgrayedd for a double dose of my pimpin'
      matrix = new PMatrix3D(matrix);
    }
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Center the shape based on its bounding box. Can't assume
   * that the bounding box is 0, 0, width, height. Common case will be
   * opening a letter size document in Illustrator, and drawing something
   * in the middle, then reading it in as an svg file.
   * This will also need to flip the y axis (scale(1, -1)) in cases
   * like Adobe Illustrator where the coordinates start at the bottom.
   */
//  public void center() {
//  }


  /**
   * Set the pivot point for all transformations.
   */
//  public void pivot(float x, float y) {
//    px = x;
//    py = y;
//  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

}