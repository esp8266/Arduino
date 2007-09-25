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


// take a look at the obj loader to see how this fits with things

// PShape.line() PShape.ellipse()?
// PShape s = beginShape()
// line()
// endShape(s)

public class PShape {

  int kind;
  PMatrix matrix;

  int[] opcode;
  int opcodeCount;
  // need to reorder vertex fields to make a VERTEX_SHORT_COUNT
  // that puts all the non-rendering fields into later indices
  float[][] data;  // second param is the VERTEX_FIELD_COUNT
  // should this be called vertices (consistent with PGraphics internals)
  // or does that hurt flexibility?

  int childCount;
  PShape[] children;

  // POINTS, LINES, xLINE_STRIP, xLINE_LOOP
  // TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN
  // QUADS, QUAD_STRIP
  // xPOLYGON
  static final int PATH = 1;  // POLYGON, LINE_LOOP, LINE_STRIP
  static final int GROUP = 2;

  // how to handle rectmode/ellipsemode?
  // are they bitshifted into the constant?
  // CORNER, CORNERS, CENTER, (CENTER_RADIUS?)
  static final int RECT = 3; // could just be QUAD, but would be x1/y1/x2/y2
  static final int ELLIPSE = 4;

  static final int VERTEX = 7;
  static final int CURVE = 5;
  static final int BEZIER = 6;


  // fill and stroke functions will need a pointer to the parent
  // PGraphics object.. may need some kind of createShape() fxn
  // or maybe the values are stored until draw() is called?

  // attaching images is very tricky.. it's a different type of data

  // material parameters will be thrown out,
  // except those currently supported (kinds of lights)

  // setAxis -> .x and .y to move x and y coords of origin
  public float x;
  public float y;

  // pivot point for transformations
  public float px;
  public float py;


  public PShape() {
  }


  public PShape(float x, float y) {
    this.x = x;
    this.y = y;
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Called by the following (the shape() command adds the g)
   * PShape s = loadShapes("blah.svg");
   * shape(s);
   */
  public void draw(PGraphics g) {
    boolean flat = g instanceof PGraphics3D;

    if (matrix != null) {
      g.pushMatrix();
      if (flat) {
        g.applyMatrix(matrix.m00, matrix.m01, matrix.m02,
                      matrix.m10, matrix.m11, matrix.m12);
      } else {
        g.applyMatrix(matrix.m00, matrix.m01, matrix.m02, matrix.m03,
                      matrix.m10, matrix.m11, matrix.m12, matrix.m13,
                      matrix.m20, matrix.m21, matrix.m22, matrix.m23,
                      matrix.m30, matrix.m31, matrix.m32, matrix.m33);
      }
    }

    // if g subclasses PGraphics2, ignore all lighting stuff and z coords
    // otherwise if PGraphics3, need to call diffuse() etc

    // unfortunately, also a problem with no way to encode stroke/fill
    // being enabled/disabled.. this quickly gets into just having opcodes
    // for the entire api, to deal with things like textures and images

    switch (kind) {
    case PATH:
      for (int i = 0; i < opcodeCount; i++) {
        switch (opcode[i]) {
        case VERTEX:
          break;
        }
      }
      break;

    case GROUP:
      break;

    case RECT:
      break;
    }

    if (matrix != null) {
      g.popMatrix();
    }
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  // can't be 'add' because that suggests additive geometry
  public void addChild(PShape who) {
  }


  public PShape createGroup() {
    PShape group = new PShape();
    group.kind = GROUP;
    addChild(group);
    return group;
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  // translate, rotate, scale, apply (no push/pop)
  //   these each call matrix.translate, etc
  // if matrix is null when one is called,
  //   it is created and set to identity


  public void translate(float tx, float ty) {
    translate(tx, ty, 0);
  }

  public void translate(float tx, float ty, float tz) {
    checkMatrix();
    matrix.translate(tx, ty, 0);
  }


  //


  public void rotateX(float angle) {
    rotate(angle, 1, 0, 0);
  }

  public void rotateY(float angle) {
    rotate(angle, 0, 1, 0);
  }

  public void rotateZ(float angle) {
    rotate(angle, 0, 0, 1);
  }

  public void rotate(float angle) {
    rotateZ(angle);
  }

  public void rotate(float angle, float v0, float v1, float v2) {
    checkMatrix();
    matrix.rotate(angle, v0, v1, v2);
  }


  //


  public void scale(float s) {
    scale(s, s, s);
  }

  public void scale(float sx, float sy) {
    scale(sx, sy, 1);
  }

  public void scale(float x, float y, float z) {
    checkMatrix();
    matrix.scale(x, y, z);
  }


  //


  public void applyMatrix(float n00, float n01, float n02,
                          float n10, float n11, float n12) {
    checkMatrix();
    matrix.apply(n00, n01, n02, 0,
                 n10, n11, n12, 0,
                 0,   0,   1,   0,
                 0,   0,   0,   1);
  }

  public void applyMatrix(float n00, float n01, float n02, float n03,
                          float n10, float n11, float n12, float n13,
                          float n20, float n21, float n22, float n23,
                          float n30, float n31, float n32, float n33) {
    checkMatrix();
    matrix.apply(n00, n01, n02, n03,
                 n10, n11, n12, n13,
                 n20, n21, n22, n23,
                 n30, n31, n32, n33);
  }


  //


  protected void checkMatrix() {
    if (matrix == null) {
      matrix = new PMatrix();
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
  public void center() {
  }


  /**
   * Set the pivot point for all transformations.
   */
  public void pivot(float x, float y) {
    px = x;
    py = y;
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .



}