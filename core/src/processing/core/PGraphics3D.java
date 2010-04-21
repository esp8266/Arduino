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

import java.awt.Toolkit;
import java.awt.image.*;
import java.util.*;


/**
 * Subclass of PGraphics that handles 3D rendering.
 * It can render 3D inside a browser window and requires no plug-ins.
 * <p/>
 * The renderer is mostly set up based on the structure of the OpenGL API,
 * if you have questions about specifics that aren't covered here,
 * look for reference on the OpenGL implementation of a similar feature.
 * <p/>
 * Lighting and camera implementation by Simon Greenwold.
 */
public class PGraphics3D extends PGraphics {

  /** The depth buffer. */
  public float[] zbuffer;

  // ........................................................

  /** The modelview matrix. */
  public PMatrix3D modelview;

  /** Inverse modelview matrix, used for lighting. */
  public PMatrix3D modelviewInv;

  /** 
   * Marks when changes to the size have occurred, so that the camera 
   * will be reset in beginDraw().
   */
  protected boolean sizeChanged;
  
  /** The camera matrix, the modelview will be set to this on beginDraw. */
  public PMatrix3D camera;

  /** Inverse camera matrix */
  protected PMatrix3D cameraInv;

  /** Camera field of view. */
  public float cameraFOV;

  /** Position of the camera. */
  public float cameraX, cameraY, cameraZ;
  public float cameraNear, cameraFar;
  /** Aspect ratio of camera's view. */
  public float cameraAspect;

  /** Current projection matrix. */
  public PMatrix3D projection;


  //////////////////////////////////////////////////////////////


  /**
   * Maximum lights by default is 8, which is arbitrary for this renderer,
   * but is the minimum defined by OpenGL
   */
  public static final int MAX_LIGHTS = 8;

  public int lightCount = 0;

  /** Light types */
  public int[] lightType;

  /** Light positions */
  //public float[][] lightPosition;
  public PVector[] lightPosition;

  /** Light direction (normalized vector) */
  //public float[][] lightNormal;
  public PVector[] lightNormal;

  /** Light falloff */
  public float[] lightFalloffConstant;
  public float[] lightFalloffLinear;
  public float[] lightFalloffQuadratic;

  /** Light spot angle */
  public float[] lightSpotAngle;

  /** Cosine of light spot angle */
  public float[] lightSpotAngleCos;

  /** Light spot concentration */
  public float[] lightSpotConcentration;

  /** Diffuse colors for lights.
   *  For an ambient light, this will hold the ambient color.
   *  Internally these are stored as numbers between 0 and 1. */
  public float[][] lightDiffuse;

  /** Specular colors for lights.
      Internally these are stored as numbers between 0 and 1. */
  public float[][] lightSpecular;

  /** Current specular color for lighting */
  public float[] currentLightSpecular;

  /** Current light falloff */
  public float currentLightFalloffConstant;
  public float currentLightFalloffLinear;
  public float currentLightFalloffQuadratic;


  //////////////////////////////////////////////////////////////


  static public final int TRI_DIFFUSE_R = 0;
  static public final int TRI_DIFFUSE_G = 1;
  static public final int TRI_DIFFUSE_B = 2;
  static public final int TRI_DIFFUSE_A = 3;
  static public final int TRI_SPECULAR_R = 4;
  static public final int TRI_SPECULAR_G = 5;
  static public final int TRI_SPECULAR_B = 6;
  static public final int TRI_COLOR_COUNT = 7;

  // ........................................................

  // Whether or not we have to worry about vertex position for lighting calcs
  private boolean lightingDependsOnVertexPosition;

  static final int LIGHT_AMBIENT_R = 0;
  static final int LIGHT_AMBIENT_G = 1;
  static final int LIGHT_AMBIENT_B = 2;
  static final int LIGHT_DIFFUSE_R = 3;
  static final int LIGHT_DIFFUSE_G = 4;
  static final int LIGHT_DIFFUSE_B = 5;
  static final int LIGHT_SPECULAR_R = 6;
  static final int LIGHT_SPECULAR_G = 7;
  static final int LIGHT_SPECULAR_B = 8;
  static final int LIGHT_COLOR_COUNT = 9;

  // Used to shuttle lighting calcs around
  // (no need to re-allocate all the time)
  protected float[] tempLightingContribution = new float[LIGHT_COLOR_COUNT];
//  protected float[] worldNormal = new float[4];

  /// Used in lightTriangle(). Allocated here once to avoid re-allocating
  protected PVector lightTriangleNorm = new PVector();

  // ........................................................

  /**
   * This is turned on at beginCamera, and off at endCamera
   * Currently we don't support nested begin/end cameras.
   * If we wanted to, this variable would have to become a stack.
   */
  protected boolean manipulatingCamera;

  float[][] matrixStack = new float[MATRIX_STACK_DEPTH][16];
  float[][] matrixInvStack = new float[MATRIX_STACK_DEPTH][16];
  int matrixStackDepth;

  // These two matrices always point to either the modelview
  // or the modelviewInv, but they are swapped during
  // when in camera manipulation mode. That way camera transforms
  // are automatically accumulated in inverse on the modelview matrix.
  protected PMatrix3D forwardTransform;
  protected PMatrix3D reverseTransform;

  // Added by ewjordan for accurate texturing purposes. Screen plane is
  // not scaled to pixel-size, so these manually keep track of its size
  // from frustum() calls. Sorry to add public vars, is there a way
  // to compute these from something publicly available without matrix ops?
  // (used once per triangle in PTriangle with ENABLE_ACCURATE_TEXTURES)
  protected float leftScreen;
  protected float rightScreen;
  protected float topScreen;
  protected float bottomScreen;
  protected float nearPlane; //depth of near clipping plane

  /** true if frustum has been called to set perspective, false if ortho */
  private boolean frustumMode = false;

  /**
   * Use PSmoothTriangle for rendering instead of PTriangle?
   * Usually set by calling smooth() or noSmooth()
   */
  static protected boolean s_enableAccurateTextures = false; //maybe just use smooth instead?

  /** Used for anti-aliased and perspective corrected rendering. */
  public PSmoothTriangle smoothTriangle;


  // ........................................................

  // pos of first vertex of current shape in vertices array
  protected int shapeFirst;

  // i think vertex_end is actually the last vertex in the current shape
  // and is separate from vertexCount for occasions where drawing happens
  // on endDraw() with all the triangles being depth sorted
  protected int shapeLast;

  // vertices may be added during clipping against the near plane.
  protected int shapeLastPlusClipped;

  // used for sorting points when triangulating a polygon
  // warning - maximum number of vertices for a polygon is DEFAULT_VERTICES
  protected int vertexOrder[] = new int[DEFAULT_VERTICES];

  // ........................................................

  // This is done to keep track of start/stop information for lines in the
  // line array, so that lines can be shown as a single path, rather than just
  // individual segments. Currently only in use inside PGraphicsOpenGL.
  protected int pathCount;
  protected int[] pathOffset = new int[64];
  protected int[] pathLength = new int[64];

  // ........................................................

  // line & triangle fields (note that these overlap)
//  static protected final int INDEX = 0;          // shape index
  static protected final int VERTEX1 = 0;
  static protected final int VERTEX2 = 1;
  static protected final int VERTEX3 = 2;        // (triangles only)
  /** used to store the strokeColor int for efficient drawing. */
  static protected final int STROKE_COLOR = 1;   // (points only)
  static protected final int TEXTURE_INDEX = 3;  // (triangles only)
  //static protected final int STROKE_MODE = 2;    // (lines only)
  //static protected final int STROKE_WEIGHT = 3;  // (lines only)

  static protected final int POINT_FIELD_COUNT = 2;  //4
  static protected final int LINE_FIELD_COUNT = 2;  //4
  static protected final int TRIANGLE_FIELD_COUNT = 4;

  // points
  static final int DEFAULT_POINTS = 512;
  protected int[][] points = new int[DEFAULT_POINTS][POINT_FIELD_COUNT];
  protected int pointCount;

  // lines
  static final int DEFAULT_LINES = 512;
  public PLine line;  // used for drawing
  protected int[][] lines = new int[DEFAULT_LINES][LINE_FIELD_COUNT];
  protected int lineCount;

  // triangles
  static final int DEFAULT_TRIANGLES = 256;
  public PTriangle triangle;
  protected int[][] triangles =
    new int[DEFAULT_TRIANGLES][TRIANGLE_FIELD_COUNT];
  protected float triangleColors[][][] =
    new float[DEFAULT_TRIANGLES][3][TRI_COLOR_COUNT];
  protected int triangleCount;   // total number of triangles

  // cheap picking someday
  //public int shape_index;

  // ........................................................

  static final int DEFAULT_TEXTURES = 3;
  protected PImage[] textures = new PImage[DEFAULT_TEXTURES];
  int textureIndex;

  // ........................................................

  DirectColorModel cm;
  MemoryImageSource mis;


  //////////////////////////////////////////////////////////////


  public PGraphics3D() { }


  //public void setParent(PApplet parent)


  //public void setPrimary(boolean primary)


  //public void setPath(String path)


  /**
   * Called in response to a resize event, handles setting the
   * new width and height internally, as well as re-allocating
   * the pixel buffer for the new size.
   *
   * Note that this will nuke any cameraMode() settings.
   * 
   * No drawing can happen in this function, and no talking to the graphics
   * context. That is, no glXxxx() calls, or other things that change state.
   */
  public void setSize(int iwidth, int iheight) {  // ignore
    width = iwidth;
    height = iheight;
    width1 = width - 1;
    height1 = height - 1;

    allocate();
    reapplySettings();

    // init lights (in resize() instead of allocate() b/c needed by opengl)
    lightType = new int[MAX_LIGHTS];
    lightPosition = new PVector[MAX_LIGHTS];
    lightNormal = new PVector[MAX_LIGHTS];
    for (int i = 0; i < MAX_LIGHTS; i++) {
      lightPosition[i] = new PVector();
      lightNormal[i] = new PVector();
    }
    lightDiffuse = new float[MAX_LIGHTS][3];
    lightSpecular = new float[MAX_LIGHTS][3];
    lightFalloffConstant = new float[MAX_LIGHTS];
    lightFalloffLinear = new float[MAX_LIGHTS];
    lightFalloffQuadratic = new float[MAX_LIGHTS];
    lightSpotAngle = new float[MAX_LIGHTS];
    lightSpotAngleCos = new float[MAX_LIGHTS];
    lightSpotConcentration = new float[MAX_LIGHTS];
    currentLightSpecular = new float[3];

    projection = new PMatrix3D();
    modelview = new PMatrix3D();
    modelviewInv = new PMatrix3D();

//    modelviewStack = new float[MATRIX_STACK_DEPTH][16];
//    modelviewInvStack = new float[MATRIX_STACK_DEPTH][16];
//    modelviewStackPointer = 0;

    forwardTransform = modelview;
    reverseTransform = modelviewInv;

    // init perspective projection based on new dimensions
    cameraFOV = 60 * DEG_TO_RAD; // at least for now
    cameraX = width / 2.0f;
    cameraY = height / 2.0f;
    cameraZ = cameraY / ((float) Math.tan(cameraFOV / 2.0f));
    cameraNear = cameraZ / 10.0f;
    cameraFar = cameraZ * 10.0f;
    cameraAspect = (float)width / (float)height;

    camera = new PMatrix3D();
    cameraInv = new PMatrix3D();

    // set this flag so that beginDraw() will do an update to the camera.
    sizeChanged = true;
  }


  protected void allocate() {
    //System.out.println(this + " allocating for " + width + " " + height);
    //new Exception().printStackTrace();

    pixelCount = width * height;
    pixels = new int[pixelCount];
    zbuffer = new float[pixelCount];

    if (primarySurface) {
      cm = new DirectColorModel(32, 0x00ff0000, 0x0000ff00, 0x000000ff);;
      mis = new MemoryImageSource(width, height, pixels, 0, width);
      mis.setFullBufferUpdates(true);
      mis.setAnimated(true);
      image = Toolkit.getDefaultToolkit().createImage(mis);

    } else {
      // when not the main drawing surface, need to set the zbuffer,
      // because there's a possibility that background() will not be called
      Arrays.fill(zbuffer, Float.MAX_VALUE);
    }

    line = new PLine(this);
    triangle = new PTriangle(this);
    smoothTriangle = new PSmoothTriangle(this);
  }


  //public void dispose()


  ////////////////////////////////////////////////////////////


  //public boolean canDraw()


  public void beginDraw() {
    // need to call defaults(), but can only be done when it's ok
    // to draw (i.e. for opengl, no drawing can be done outside
    // beginDraw/endDraw).
    if (!settingsInited) defaultSettings();

    if (sizeChanged) {
      // set up the default camera
      camera();

      // defaults to perspective, if the user has setup up their
      // own projection, they'll need to fix it after resize anyway.
      // this helps the people who haven't set up their own projection.
      perspective();
      
      // clear the flag
      sizeChanged = false;
    }
    
    resetMatrix(); // reset model matrix

    // reset vertices
    vertexCount = 0;

    modelview.set(camera);
    modelviewInv.set(cameraInv);

    // clear out the lights, they'll have to be turned on again
    lightCount = 0;
    lightingDependsOnVertexPosition = false;
    lightFalloff(1, 0, 0);
    lightSpecular(0, 0, 0);

    /*
    // reset lines
    lineCount = 0;
    if (line != null) line.reset();  // is this necessary?
    pathCount = 0;

    // reset triangles
    triangleCount = 0;
    if (triangle != null) triangle.reset();  // necessary?
    */

    shapeFirst = 0;

    // reset textures
    Arrays.fill(textures, null);
    textureIndex = 0;

    normal(0, 0, 1);
  }


  /**
   * See notes in PGraphics.
   * If z-sorting has been turned on, then the triangles will
   * all be quicksorted here (to make alpha work more properly)
   * and then blit to the screen.
   */
  public void endDraw() {
    // no need to z order and render
    // shapes were already rendered in endShape();
    // (but can't return, since needs to update memimgsrc)
    if (hints[ENABLE_DEPTH_SORT]) {
      flush();
    }
    if (mis != null) {
      mis.newPixels(pixels, cm, 0, width);
    }
    // mark pixels as having been updated, so that they'll work properly
    // when this PGraphics is drawn using image().
    updatePixels();
  }


  ////////////////////////////////////////////////////////////


  //protected void checkSettings()


  protected void defaultSettings() {
    super.defaultSettings();

    manipulatingCamera = false;
    forwardTransform = modelview;
    reverseTransform = modelviewInv;

    // set up the default camera
    camera();

    // defaults to perspective, if the user has setup up their
    // own projection, they'll need to fix it after resize anyway.
    // this helps the people who haven't set up their own projection.
    perspective();

    // easiest for beginners
    textureMode(IMAGE);

    emissive(0.0f);
    specular(0.5f);
    shininess(1.0f);
  }


  //protected void reapplySettings()


  ////////////////////////////////////////////////////////////


  public void hint(int which) {
    if (which == DISABLE_DEPTH_SORT) {
      flush();
    } else if (which == DISABLE_DEPTH_TEST) {
      if (zbuffer != null) {  // will be null in OpenGL and others
        Arrays.fill(zbuffer, Float.MAX_VALUE);
      }
    }
    super.hint(which);
  }


  //////////////////////////////////////////////////////////////


  //public void beginShape()


  public void beginShape(int kind) {
    shape = kind;

//    shape_index = shape_index + 1;
//    if (shape_index == -1) {
//      shape_index = 0;
//    }

    if (hints[ENABLE_DEPTH_SORT]) {
      // continue with previous vertex, line and triangle count
      // all shapes are rendered at endDraw();
      shapeFirst = vertexCount;
      shapeLast = 0;

    } else {
      // reset vertex, line and triangle information
      // every shape is rendered at endShape();
      vertexCount = 0;
      if (line != null) line.reset();  // necessary?
      lineCount = 0;
//      pathCount = 0;
      if (triangle != null) triangle.reset();  // necessary?
      triangleCount = 0;
    }

    textureImage = null;
    curveVertexCount = 0;
    normalMode = NORMAL_MODE_AUTO;
//    normalCount = 0;
  }


  //public void normal(float nx, float ny, float nz)


  //public void textureMode(int mode)


  public void texture(PImage image) {
    textureImage = image;

    if (textureIndex == textures.length - 1) {
      textures = (PImage[]) PApplet.expand(textures);
    }
    if (textures[textureIndex] != null) {  // ???
      textureIndex++;
    }
    textures[textureIndex] = image;
  }


  public void vertex(float x, float y) {
    // override so that the default 3D implementation will be used,
    // which will pick up all 3D settings (e.g. emissive, ambient)
    vertex(x, y, 0);
  }


  //public void vertex(float x, float y, float z)


  public void vertex(float x, float y, float u, float v) {
    // see vertex(x, y) for note
    vertex(x, y, 0, u, v);
  }


  //public void vertex(float x, float y, float z, float u, float v)


  //public void breakShape()


  //public void endShape()


  public void endShape(int mode) {
    shapeLast = vertexCount;
    shapeLastPlusClipped = shapeLast;

    // don't try to draw if there are no vertices
    // (fixes a bug in LINE_LOOP that re-adds a nonexistent vertex)
    if (vertexCount == 0) {
      shape = 0;
      return;
    }

    // convert points from model (X/Y/Z) to camera space (VX/VY/VZ).
    // Do this now because we will be clipping them on add_triangle.
    endShapeModelToCamera(shapeFirst, shapeLast);

    if (stroke) {
      endShapeStroke(mode);
    }

    if (fill || textureImage != null) {
      endShapeFill();
    }

    // transform, light, and clip
    endShapeLighting(lightCount > 0 && fill);

    // convert points from camera space (VX, VY, VZ) to screen space (X, Y, Z)
    // (this appears to be wasted time with the OpenGL renderer)
    endShapeCameraToScreen(shapeFirst, shapeLastPlusClipped);

    // render shape and fill here if not saving the shapes for later
    // if true, the shapes will be rendered on endDraw
    if (!hints[ENABLE_DEPTH_SORT]) {
      if (fill || textureImage != null) {
        if (triangleCount > 0) {
          renderTriangles(0, triangleCount);
          if (raw != null) {
            rawTriangles(0, triangleCount);
          }
          triangleCount = 0;
        }
      }
      if (stroke) {
        if (pointCount > 0) {
          renderPoints(0, pointCount);
          if (raw != null) {
            rawPoints(0, pointCount);
          }
          pointCount = 0;
        }

        if (lineCount > 0) {
          renderLines(0, lineCount);
          if (raw != null) {
            rawLines(0, lineCount);
          }
          lineCount = 0;
        }
      }
      pathCount = 0;
    }

    shape = 0;
  }


  protected void endShapeModelToCamera(int start, int stop) {
    for (int i = start; i < stop; i++) {
      float vertex[] = vertices[i];

      vertex[VX] =
        modelview.m00*vertex[X] + modelview.m01*vertex[Y] +
        modelview.m02*vertex[Z] + modelview.m03;
      vertex[VY] =
        modelview.m10*vertex[X] + modelview.m11*vertex[Y] +
        modelview.m12*vertex[Z] + modelview.m13;
      vertex[VZ] =
        modelview.m20*vertex[X] + modelview.m21*vertex[Y] +
        modelview.m22*vertex[Z] + modelview.m23;
      vertex[VW] =
        modelview.m30*vertex[X] + modelview.m31*vertex[Y] +
        modelview.m32*vertex[Z] + modelview.m33;

      // normalize
      if (vertex[VW] != 0 && vertex[VW] != 1) {
        vertex[VX] /= vertex[VW];
        vertex[VY] /= vertex[VW];
        vertex[VZ] /= vertex[VW];
      }
      vertex[VW] = 1;
    }
  }


  protected void endShapeStroke(int mode) {
    switch (shape) {
    case POINTS:
    {
      int stop = shapeLast;
      for (int i = shapeFirst; i < stop; i++) {
//        if (strokeWeight == 1) {
        addPoint(i);
//        } else {
//          addLineBreak();  // total overkill for points
//          addLine(i, i);
//        }
      }
    }
    break;

    case LINES:
    {
      // store index of first vertex
      int first = lineCount;
      int stop = shapeLast - 1;
      //increment = (shape == LINES) ? 2 : 1;

      // for LINE_STRIP and LINE_LOOP, make this all one path
      if (shape != LINES) addLineBreak();

      for (int i = shapeFirst; i < stop; i += 2) {
        // for LINES, make a new path for each segment
        if (shape == LINES) addLineBreak();
        addLine(i, i+1);
      }

      // for LINE_LOOP, close the loop with a final segment
      //if (shape == LINE_LOOP) {
      if (mode == CLOSE) {
        addLine(stop, lines[first][VERTEX1]);
      }
    }
    break;

    case TRIANGLES:
    {
      for (int i = shapeFirst; i < shapeLast-2; i += 3) {
        addLineBreak();
        //counter = i - vertex_start;
        addLine(i+0, i+1);
        addLine(i+1, i+2);
        addLine(i+2, i+0);
      }
    }
    break;

    case TRIANGLE_STRIP:
    {
      // first draw all vertices as a line strip
      int stop = shapeLast-1;

      addLineBreak();
      for (int i = shapeFirst; i < stop; i++) {
        //counter = i - vertex_start;
        addLine(i, i+1);
      }

      // then draw from vertex (n) to (n+2)
      stop = shapeLast-2;
      for (int i = shapeFirst; i < stop; i++) {
        addLineBreak();
        addLine(i, i+2);
      }
    }
    break;

    case TRIANGLE_FAN:
    {
      // this just draws a series of line segments
      // from the center to each exterior point
      for (int i = shapeFirst + 1; i < shapeLast; i++) {
        addLineBreak();
        addLine(shapeFirst, i);
      }

      // then a single line loop around the outside.
      addLineBreak();
      for (int i = shapeFirst + 1; i < shapeLast-1; i++) {
        addLine(i, i+1);
      }
      // closing the loop
      addLine(shapeLast-1, shapeFirst + 1);
    }
    break;

    case QUADS:
    {
      for (int i = shapeFirst; i < shapeLast; i += 4) {
        addLineBreak();
        //counter = i - vertex_start;
        addLine(i+0, i+1);
        addLine(i+1, i+2);
        addLine(i+2, i+3);
        addLine(i+3, i+0);
      }
    }
    break;

    case QUAD_STRIP:
    {
      for (int i = shapeFirst; i < shapeLast - 3; i += 2) {
        addLineBreak();
        addLine(i+0, i+2);
        addLine(i+2, i+3);
        addLine(i+3, i+1);
        addLine(i+1, i+0);
      }
    }
    break;

    case POLYGON:
    {
      // store index of first vertex
      int stop = shapeLast - 1;

      addLineBreak();
      for (int i = shapeFirst; i < stop; i++) {
        addLine(i, i+1);
      }
      if (mode == CLOSE) {
        // draw the last line connecting back to the first point in poly
        addLine(stop, shapeFirst); //lines[first][VERTEX1]);
      }
    }
    break;
    }
  }


  protected void endShapeFill() {
    switch (shape) {
    case TRIANGLE_FAN:
    {
      int stop = shapeLast - 1;
      for (int i = shapeFirst + 1; i < stop; i++) {
        addTriangle(shapeFirst, i, i+1);
      }
    }
    break;

    case TRIANGLES:
    {
      int stop = shapeLast - 2;
      for (int i = shapeFirst; i < stop; i += 3) {
        // have to switch between clockwise/counter-clockwise
        // otherwise the feller is backwards and renderer won't draw
        if ((i % 2) == 0) {
          addTriangle(i, i+2, i+1);
        } else {
          addTriangle(i, i+1, i+2);
        }
      }
    }
    break;

    case TRIANGLE_STRIP:
    {
      int stop = shapeLast - 2;
      for (int i = shapeFirst; i < stop; i++) {
        // have to switch between clockwise/counter-clockwise
        // otherwise the feller is backwards and renderer won't draw
        if ((i % 2) == 0) {
          addTriangle(i, i+2, i+1);
        } else {
          addTriangle(i, i+1, i+2);
        }
      }
    }
    break;

    case QUADS:
    {
      int stop = vertexCount-3;
      for (int i = shapeFirst; i < stop; i += 4) {
        // first triangle
        addTriangle(i, i+1, i+2);
        // second triangle
        addTriangle(i, i+2, i+3);
      }
    }
    break;

    case QUAD_STRIP:
    {
      int stop = vertexCount-3;
      for (int i = shapeFirst; i < stop; i += 2) {
        // first triangle
        addTriangle(i+0, i+2, i+1);
        // second triangle
        addTriangle(i+2, i+3, i+1);
      }
    }
    break;

    case POLYGON:
    {
      addPolygonTriangles();
    }
    break;
    }
  }


  protected void endShapeLighting(boolean lights) {
    if (lights) {
      // If the lighting does not depend on vertex position and there is a single
      // normal specified for this shape, go ahead and apply the same lighting
      // contribution to every vertex in this shape (one lighting calc!)
      if (!lightingDependsOnVertexPosition && normalMode == NORMAL_MODE_SHAPE) {
        calcLightingContribution(shapeFirst, tempLightingContribution);
        for (int tri = 0; tri < triangleCount; tri++) {
          lightTriangle(tri, tempLightingContribution);
        }
      } else {  // Otherwise light each triangle individually...
        for (int tri = 0; tri < triangleCount; tri++) {
          lightTriangle(tri);
        }
      }
    } else {
      for (int tri = 0; tri < triangleCount; tri++) {
        int index = triangles[tri][VERTEX1];
        copyPrelitVertexColor(tri, index, 0);
        index = triangles[tri][VERTEX2];
        copyPrelitVertexColor(tri, index, 1);
        index = triangles[tri][VERTEX3];
        copyPrelitVertexColor(tri, index, 2);
      }
    }
  }


  protected void endShapeCameraToScreen(int start, int stop) {
    for (int i = start; i < stop; i++) {
      float vx[] = vertices[i];

      float ox =
        projection.m00*vx[VX] + projection.m01*vx[VY] +
        projection.m02*vx[VZ] + projection.m03*vx[VW];
      float oy =
        projection.m10*vx[VX] + projection.m11*vx[VY] +
        projection.m12*vx[VZ] + projection.m13*vx[VW];
      float oz =
        projection.m20*vx[VX] + projection.m21*vx[VY] +
        projection.m22*vx[VZ] + projection.m23*vx[VW];
      float ow =
        projection.m30*vx[VX] + projection.m31*vx[VY] +
        projection.m32*vx[VZ] + projection.m33*vx[VW];

      if (ow != 0 && ow != 1) {
        ox /= ow; oy /= ow; oz /= ow;
      }

      vx[TX] = width * (1 + ox) / 2.0f;
      vx[TY] = height * (1 + oy) / 2.0f;
      vx[TZ] = (oz + 1) / 2.0f;
    }
  }



  /////////////////////////////////////////////////////////////////////////////

  // POINTS


  protected void addPoint(int a) {
    if (pointCount == points.length) {
      int[][] temp = new int[pointCount << 1][LINE_FIELD_COUNT];
      System.arraycopy(points, 0, temp, 0, lineCount);
      points = temp;
    }
    points[pointCount][VERTEX1] = a;
    //points[pointCount][STROKE_MODE] = strokeCap | strokeJoin;
    points[pointCount][STROKE_COLOR] = strokeColor;
    //points[pointCount][STROKE_WEIGHT] = (int) (strokeWeight + 0.5f); // hmm
    pointCount++;
  }


  protected void renderPoints(int start, int stop) {
    if (strokeWeight != 1) {
      for (int i = start; i < stop; i++) {
        float[] a = vertices[points[i][VERTEX1]];
        renderLineVertices(a, a);
      }
    } else {
      for (int i = start; i < stop; i++) {
        float[] a = vertices[points[i][VERTEX1]];
        int sx = (int) (a[TX] + 0.4999f);
        int sy = (int) (a[TY] + 0.4999f);
        if (sx >= 0 && sx < width && sy >= 0 && sy < height) {
          int index = sy*width + sx;
          pixels[index] = points[i][STROKE_COLOR];
          zbuffer[index] = a[TZ];
        }
      }
    }
  }


  // alternative implementations of point rendering code...

  /*
      int sx = (int) (screenX(x, y, z) + 0.5f);
      int sy = (int) (screenY(x, y, z) + 0.5f);

      int index = sy*width + sx;
      pixels[index] = strokeColor;
      zbuffer[index] = screenZ(x, y, z);

   */

  /*
  protected void renderPoints(int start, int stop) {
    for (int i = start; i < stop; i++) {
      float a[] = vertices[points[i][VERTEX1]];

      line.reset();

      line.setIntensities(a[SR], a[SG], a[SB], a[SA],
                          a[SR], a[SG], a[SB], a[SA]);

      line.setVertices(a[TX], a[TY], a[TZ],
                       a[TX] + 0.5f, a[TY] + 0.5f, a[TZ] + 0.5f);

      line.draw();
    }
  }
  */

  /*
  // handle points with an actual stroke weight (or scaled by renderer)
  private void point3(float x, float y, float z, int color) {
    // need to get scaled version of the stroke
    float x1 = screenX(x - 0.5f, y - 0.5f, z);
    float y1 = screenY(x - 0.5f, y - 0.5f, z);
    float x2 = screenX(x + 0.5f, y + 0.5f, z);
    float y2 = screenY(x + 0.5f, y + 0.5f, z);

    float weight = (abs(x2 - x1) + abs(y2 - y1)) / 2f;
    if (weight < 1.5f) {
      int xx = (int) ((x1 + x2) / 2f);
      int yy = (int) ((y1 + y2) / 2f);
      //point0(xx, yy, z, color);
      zbuffer[yy*width + xx] = screenZ(x, y, z);
      //stencil?

    } else {
      // actually has some weight, need to draw shapes instead
      // these will be
    }
  }
  */


  protected void rawPoints(int start, int stop) {
    raw.colorMode(RGB, 1);
    raw.noFill();
    raw.strokeWeight(vertices[lines[start][VERTEX1]][SW]);
    raw.beginShape(POINTS);

    for (int i = start; i < stop; i++) {
      float a[] = vertices[lines[i][VERTEX1]];

      if (raw.is3D()) {
        if (a[VW] != 0) {
          raw.stroke(a[SR], a[SG], a[SB], a[SA]);
          raw.vertex(a[VX] / a[VW], a[VY] / a[VW], a[VZ] / a[VW]);
        }
      } else {  // if is2D()
        raw.stroke(a[SR], a[SG], a[SB], a[SA]);
        raw.vertex(a[TX], a[TY]);
      }
    }
    raw.endShape();
  }



  /////////////////////////////////////////////////////////////////////////////

  // LINES


  /**
   * Begin a new section of stroked geometry.
   */
  protected final void addLineBreak() {
    if (pathCount == pathOffset.length) {
      pathOffset = PApplet.expand(pathOffset);
      pathLength = PApplet.expand(pathLength);
    }
    pathOffset[pathCount] = lineCount;
    pathLength[pathCount] = 0;
    pathCount++;
  }


  protected void addLine(int a, int b) {
    addLineWithClip(a, b);
  }


  protected final void addLineWithClip(int a, int b) {
    float az = vertices[a][VZ];
    float bz = vertices[b][VZ];
    if (az > cameraNear) {
      if (bz > cameraNear) {
        return;
      }
      int cb = interpolateClipVertex(a, b);
      addLineWithoutClip(cb, b);
      return;
    }
    else {
      if (bz <= cameraNear) {
        addLineWithoutClip(a, b);
        return;
      }
      int cb = interpolateClipVertex(a, b);
      addLineWithoutClip(a, cb);
      return;
    }
  }


  protected final void addLineWithoutClip(int a, int b) {
    if (lineCount == lines.length) {
      int temp[][] = new int[lineCount<<1][LINE_FIELD_COUNT];
      System.arraycopy(lines, 0, temp, 0, lineCount);
      lines = temp;
    }
    lines[lineCount][VERTEX1] = a;
    lines[lineCount][VERTEX2] = b;

    //lines[lineCount][STROKE_MODE] = strokeCap | strokeJoin;
    //lines[lineCount][STROKE_WEIGHT] = (int) (strokeWeight + 0.5f); // hmm
    lineCount++;

    // mark this piece as being part of the current path
    pathLength[pathCount-1]++;
  }


  protected void renderLines(int start, int stop) {
    for (int i = start; i < stop; i++) {
      renderLineVertices(vertices[lines[i][VERTEX1]],
                         vertices[lines[i][VERTEX2]]);
    }
  }


  protected void renderLineVertices(float[] a, float[] b) {
    // 2D hack added by ewjordan 6/13/07
    // Offset coordinates by a little bit if drawing 2D graphics.
    // http://dev.processing.org/bugs/show_bug.cgi?id=95

    // This hack fixes a bug caused by numerical precision issues when
    // applying the 3D transformations to coordinates in the screen plane
    // that should actually not be altered under said transformations.
    // It will not be applied if any transformations other than translations
    // are active, nor should it apply in OpenGL mode (PGraphicsOpenGL
    // overrides render_lines(), so this should be fine).
    // This fix exposes a last-pixel bug in the lineClipCode() function
    // of PLine.java, so that fix must remain in place if this one is used.

    // Note: the "true" fix for this bug is to change the pixel coverage
    // model so that the threshold for display does not lie on an integer
    // boundary. Search "diamond exit rule" for info the OpenGL approach.

    /*
      // removing for 0149 with the return of P2D
      if (drawing2D() && a[Z] == 0) {
        a[TX] += 0.01;
        a[TY] += 0.01;
        a[VX] += 0.01*a[VW];
        a[VY] += 0.01*a[VW];
        b[TX] += 0.01;
        b[TY] += 0.01;
        b[VX] += 0.01*b[VW];
        b[VY] += 0.01*b[VW];
      }
     */
    // end 2d-hack

    if (a[SW] > 1.25f || a[SW] < 0.75f) {
      float ox1 = a[TX];
      float oy1 = a[TY];
      float ox2 = b[TX];
      float oy2 = b[TY];

      // TODO strokeWeight should be transformed!
      float weight = a[SW] / 2;

      // when drawing points with stroke weight, need to extend a bit
      if (ox1 == ox2 && oy1 == oy2) {
        oy1 -= weight;
        oy2 += weight;
      }

      float dX = ox2 - ox1 + EPSILON;
      float dY = oy2 - oy1 + EPSILON;
      float len = (float) Math.sqrt(dX*dX + dY*dY);

      float rh = weight / len;

      float dx0 = rh * dY;
      float dy0 = rh * dX;
      float dx1 = rh * dY;
      float dy1 = rh * dX;

      float ax1 = ox1+dx0;
      float ay1 = oy1-dy0;

      float ax2 = ox1-dx0;
      float ay2 = oy1+dy0;

      float bx1 = ox2+dx1;
      float by1 = oy2-dy1;

      float bx2 = ox2-dx1;
      float by2 = oy2+dy1;

      if (smooth) {
        smoothTriangle.reset(3);
        smoothTriangle.smooth = true;
        smoothTriangle.interpARGB = true;  // ?

        // render first triangle for thick line
        smoothTriangle.setVertices(ax1, ay1, a[TZ],
                                   bx2, by2, b[TZ],
                                   ax2, ay2, a[TZ]);
        smoothTriangle.setIntensities(a[SR], a[SG], a[SB], a[SA],
                                      b[SR], b[SG], b[SB], b[SA],
                                      a[SR], a[SG], a[SB], a[SA]);
        smoothTriangle.render();

        // render second triangle for thick line
        smoothTriangle.setVertices(ax1, ay1, a[TZ],
                                   bx2, by2, b[TZ],
                                   bx1, by1, b[TZ]);
        smoothTriangle.setIntensities(a[SR], a[SG], a[SB], a[SA],
                                      b[SR], b[SG], b[SB], b[SA],
                                      b[SR], b[SG], b[SB], b[SA]);
        smoothTriangle.render();

      } else {
        triangle.reset();

        // render first triangle for thick line
        triangle.setVertices(ax1, ay1, a[TZ],
                             bx2, by2, b[TZ],
                             ax2, ay2, a[TZ]);
        triangle.setIntensities(a[SR], a[SG], a[SB], a[SA],
                                b[SR], b[SG], b[SB], b[SA],
                                a[SR], a[SG], a[SB], a[SA]);
        triangle.render();

        // render second triangle for thick line
        triangle.setVertices(ax1, ay1, a[TZ],
                             bx2, by2, b[TZ],
                             bx1, by1, b[TZ]);
        triangle.setIntensities(a[SR], a[SG], a[SB], a[SA],
                                b[SR], b[SG], b[SB], b[SA],
                                b[SR], b[SG], b[SB], b[SA]);
        triangle.render();
      }

    } else {
      line.reset();

      line.setIntensities(a[SR], a[SG], a[SB], a[SA],
                          b[SR], b[SG], b[SB], b[SA]);

      line.setVertices(a[TX], a[TY], a[TZ],
                       b[TX], b[TY], b[TZ]);

        /*
        // Seems okay to remove this because these vertices are not used again,
        // but if problems arise, this needs to be uncommented because the above
        // change is destructive and may need to be undone before proceeding.
        if (drawing2D() && a[MZ] == 0) {
          a[X] -= 0.01;
          a[Y] -= 0.01;
          a[VX] -= 0.01*a[VW];
          a[VY] -= 0.01*a[VW];
          b[X] -= 0.01;
          b[Y] -= 0.01;
          b[VX] -= 0.01*b[VW];
          b[VY] -= 0.01*b[VW];
        }
        */

      line.draw();
    }
  }


  /**
   * Handle echoing line data to a raw shape recording renderer. This has been
   * broken out of the renderLines() procedure so that renderLines() can be
   * optimized per-renderer without having to deal with this code. This code,
   * for instance, will stay the same when OpenGL is in use, but renderLines()
   * can be optimized significantly.
   * <br/> <br/>
   * Values for start and stop are specified, so that in the future, sorted
   * rendering can be implemented, which will require sequences of lines,
   * triangles, or points to be rendered in the neighborhood of one another.
   * That is, if we're gonna depth sort, we can't just draw all the triangles
   * and then draw all the lines, cuz that defeats the purpose.
   */
  protected void rawLines(int start, int stop) {
    raw.colorMode(RGB, 1);
    raw.noFill();
    raw.beginShape(LINES);

    for (int i = start; i < stop; i++) {
      float a[] = vertices[lines[i][VERTEX1]];
      float b[] = vertices[lines[i][VERTEX2]];
      raw.strokeWeight(vertices[lines[i][VERTEX2]][SW]);

      if (raw.is3D()) {
        if ((a[VW] != 0) && (b[VW] != 0)) {
          raw.stroke(a[SR], a[SG], a[SB], a[SA]);
          raw.vertex(a[VX] / a[VW], a[VY] / a[VW], a[VZ] / a[VW]);
          raw.stroke(b[SR], b[SG], b[SB], b[SA]);
          raw.vertex(b[VX] / b[VW], b[VY] / b[VW], b[VZ] / b[VW]);
        }
      } else if (raw.is2D()) {
        raw.stroke(a[SR], a[SG], a[SB], a[SA]);
        raw.vertex(a[TX], a[TY]);
        raw.stroke(b[SR], b[SG], b[SB], b[SA]);
        raw.vertex(b[TX], b[TY]);
      }
    }
    raw.endShape();
  }



  /////////////////////////////////////////////////////////////////////////////

  // TRIANGLES


  protected void addTriangle(int a, int b, int c) {
    addTriangleWithClip(a, b, c);
  }


  protected final void addTriangleWithClip(int a, int b, int c) {
    boolean aClipped = false;
    boolean bClipped = false;
    int clippedCount = 0;

    // This is a hack for temporary clipping. Clipping still needs to
    // be implemented properly, however. Please help!
    // http://dev.processing.org/bugs/show_bug.cgi?id=1393
    cameraNear = -8;
    if (vertices[a][VZ] > cameraNear) {
      aClipped = true;
      clippedCount++;
    }
    if (vertices[b][VZ] > cameraNear) {
      bClipped = true;
      clippedCount++;
    }
    if (vertices[c][VZ] > cameraNear) {
      //cClipped = true;
      clippedCount++;
    }
    if (clippedCount == 0) {
//        if (vertices[a][VZ] < cameraFar &&
//                vertices[b][VZ] < cameraFar &&
//                vertices[c][VZ] < cameraFar) {
      addTriangleWithoutClip(a, b, c);
//        }

//    } else if (true) {
//        return;

    } else if (clippedCount == 3) {
      // In this case there is only one visible point.            |/|
      // So we'll have to make two new points on the clip line   <| |
      // and add that triangle instead.                           |\|

    } else if (clippedCount == 2) {
      //System.out.println("Clipped two");

      int ca, cb, cc, cd, ce;
      if (!aClipped) {
        ca = a;
        cb = b;
        cc = c;
      }
      else if (!bClipped) {
        ca = b;
        cb = a;
        cc = c;
      }
      else { //if (!cClipped) {
        ca = c;
        cb = b;
        cc = a;
      }

      cd = interpolateClipVertex(ca, cb);
      ce = interpolateClipVertex(ca, cc);
      addTriangleWithoutClip(ca, cd, ce);

    } else { // (clippedCount == 1) {
      //                                                          . |
      // In this case there are two visible points.               |\|
      // So we'll have to make two new points on the clip line    | |>
      // and then add two new triangles.                          |/|
      //                                                          . |
      //System.out.println("Clipped one");
      int ca, cb, cc, cd, ce;
      if (aClipped) {
        //System.out.println("aClipped");
        ca = c;
        cb = b;
        cc = a;
      }
      else if (bClipped) {
        //System.out.println("bClipped");
        ca = a;
        cb = c;
        cc = b;
      }
      else { //if (cClipped) {
        //System.out.println("cClipped");
        ca = a;
        cb = b;
        cc = c;
      }

      cd = interpolateClipVertex(ca, cc);
      ce = interpolateClipVertex(cb, cc);
      addTriangleWithoutClip(ca, cd, cb);
      //System.out.println("ca: " + ca + ", " + vertices[ca][VX] + ", " + vertices[ca][VY] + ", " + vertices[ca][VZ]);
      //System.out.println("cd: " + cd + ", " + vertices[cd][VX] + ", " + vertices[cd][VY] + ", " + vertices[cd][VZ]);
      //System.out.println("cb: " + cb + ", " + vertices[cb][VX] + ", " + vertices[cb][VY] + ", " + vertices[cb][VZ]);
      addTriangleWithoutClip(cb, cd, ce);
    }
  }


  protected final int interpolateClipVertex(int a, int b) {
    float[] va;
    float[] vb;
    // Set up va, vb such that va[VZ] >= vb[VZ]
    if (vertices[a][VZ] < vertices[b][VZ]) {
      va = vertices[b];
      vb = vertices[a];
    }
    else {
      va = vertices[a];
      vb = vertices[b];
    }
    float az = va[VZ];
    float bz = vb[VZ];

    float dz = az - bz;
    // If they have the same z, just use pt. a.
    if (dz == 0) {
      return a;
    }
    //float pa = (az - cameraNear) / dz;
    //float pb = (cameraNear - bz) / dz;
    float pa = (cameraNear - bz) / dz;
    float pb = 1 - pa;

    vertex(pa * va[X] + pb * vb[X],
           pa * va[Y] + pb * vb[Y],
           pa * va[Z] + pb * vb[Z]);
    int irv = vertexCount - 1;
    shapeLastPlusClipped++;

    float[] rv = vertices[irv];

    rv[TX] = pa * va[TX] + pb * vb[TX];
    rv[TY] = pa * va[TY] + pb * vb[TY];
    rv[TZ] = pa * va[TZ] + pb * vb[TZ];

    rv[VX] = pa * va[VX] + pb * vb[VX];
    rv[VY] = pa * va[VY] + pb * vb[VY];
    rv[VZ] = pa * va[VZ] + pb * vb[VZ];
    rv[VW] = pa * va[VW] + pb * vb[VW];

    rv[R] = pa * va[R] + pb * vb[R];
    rv[G] = pa * va[G] + pb * vb[G];
    rv[B] = pa * va[B] + pb * vb[B];
    rv[A] = pa * va[A] + pb * vb[A];

    rv[U] = pa * va[U] + pb * vb[U];
    rv[V] = pa * va[V] + pb * vb[V];

    rv[SR] = pa * va[SR] + pb * vb[SR];
    rv[SG] = pa * va[SG] + pb * vb[SG];
    rv[SB] = pa * va[SB] + pb * vb[SB];
    rv[SA] = pa * va[SA] + pb * vb[SA];

    rv[NX] = pa * va[NX] + pb * vb[NX];
    rv[NY] = pa * va[NY] + pb * vb[NY];
    rv[NZ] = pa * va[NZ] + pb * vb[NZ];

//    rv[SW] = pa * va[SW] + pb * vb[SW];

    rv[AR] = pa * va[AR] + pb * vb[AR];
    rv[AG] = pa * va[AG] + pb * vb[AG];
    rv[AB] = pa * va[AB] + pb * vb[AB];

    rv[SPR] = pa * va[SPR] + pb * vb[SPR];
    rv[SPG] = pa * va[SPG] + pb * vb[SPG];
    rv[SPB] = pa * va[SPB] + pb * vb[SPB];
    //rv[SPA] = pa * va[SPA] + pb * vb[SPA];

    rv[ER] = pa * va[ER] + pb * vb[ER];
    rv[EG] = pa * va[EG] + pb * vb[EG];
    rv[EB] = pa * va[EB] + pb * vb[EB];

    rv[SHINE] = pa * va[SHINE] + pb * vb[SHINE];

    rv[BEEN_LIT] = 0;

    return irv;
  }


  protected final void addTriangleWithoutClip(int a, int b, int c) {
    if (triangleCount == triangles.length) {
      int temp[][] = new int[triangleCount<<1][TRIANGLE_FIELD_COUNT];
      System.arraycopy(triangles, 0, temp, 0, triangleCount);
      triangles = temp;
      //message(CHATTER, "allocating more triangles " + triangles.length);
      float ftemp[][][] = new float[triangleCount<<1][3][TRI_COLOR_COUNT];
      System.arraycopy(triangleColors, 0, ftemp, 0, triangleCount);
      triangleColors = ftemp;
    }
    triangles[triangleCount][VERTEX1] = a;
    triangles[triangleCount][VERTEX2] = b;
    triangles[triangleCount][VERTEX3] = c;

    if (textureImage == null) {
      triangles[triangleCount][TEXTURE_INDEX] = -1;
    } else {
      triangles[triangleCount][TEXTURE_INDEX] = textureIndex;
    }

//    triangles[triangleCount][INDEX] = shape_index;
    triangleCount++;
  }


  /**
   * Triangulate the current polygon.
   * <BR> <BR>
   * Simple ear clipping polygon triangulation adapted from code by
   * John W. Ratcliff (jratcliff at verant.com). Presumably
   * <A HREF="http://www.flipcode.org/cgi-bin/fcarticles.cgi?show=63943">this</A>
   * bit of code from the web.
   */
  protected void addPolygonTriangles() {
    if (vertexOrder.length != vertices.length) {
      int[] temp = new int[vertices.length];
      // vertex_start may not be zero, might need to keep old stuff around
      // also, copy vertexOrder.length, not vertexCount because vertexCount
      // may be larger than vertexOrder.length (since this is a post-processing
      // step that happens after the vertex arrays are built).
      PApplet.arrayCopy(vertexOrder, temp, vertexOrder.length);
      vertexOrder = temp;
    }

    // this clipping algorithm only works in 2D, so in cases where a
    // polygon is drawn perpendicular to the z-axis, the area will be zero,
    // and triangulation will fail. as such, when the area calculates to
    // zero, figure out whether x or y is empty, and calculate based on the
    // two dimensions that actually contain information.
    // http://dev.processing.org/bugs/show_bug.cgi?id=111
    int d1 = X;
    int d2 = Y;
    // this brings up the nastier point that there may be cases where
    // a polygon is irregular in space and will throw off the
    // clockwise/counterclockwise calculation. for instance, if clockwise
    // relative to x and z, but counter relative to y and z or something
    // like that.. will wait to see if this is in fact a problem before
    // hurting my head on the math.

    /*
    // trying to track down bug #774
    for (int i = vertex_start; i < vertex_end; i++) {
      if (i > vertex_start) {
        if (vertices[i-1][MX] == vertices[i][MX] &&
            vertices[i-1][MY] == vertices[i][MY]) {
          System.out.print("**** " );
        }
      }
      System.out.println(i + " " + vertices[i][MX] + " " + vertices[i][MY]);
    }
    System.out.println();
    */

    // first we check if the polygon goes clockwise or counterclockwise
    float area = 0;
    for (int p = shapeLast - 1, q = shapeFirst; q < shapeLast; p = q++) {
      area += (vertices[q][d1] * vertices[p][d2] -
               vertices[p][d1] * vertices[q][d2]);
    }
    // rather than checking for the perpendicular case first, only do it
    // when the area calculates to zero. checking for perpendicular would be
    // a needless waste of time for the 99% case.
    if (area == 0) {
      // figure out which dimension is the perpendicular axis
      boolean foundValidX = false;
      boolean foundValidY = false;

      for (int i = shapeFirst; i < shapeLast; i++) {
        for (int j = i; j < shapeLast; j++){
          if ( vertices[i][X] != vertices[j][X] ) foundValidX = true;
          if ( vertices[i][Y] != vertices[j][Y] ) foundValidY = true;
        }
      }

      if (foundValidX) {
        //d1 = MX;  // already the case
        d2 = Z;
      } else if (foundValidY) {
        // ermm.. which is the proper order for cw/ccw here?
        d1 = Y;
        d2 = Z;
      } else {
        // screw it, this polygon is just f-ed up
        return;
      }

      // re-calculate the area, with what should be good values
      for (int p = shapeLast - 1, q = shapeFirst; q < shapeLast; p = q++) {
        area += (vertices[q][d1] * vertices[p][d2] -
                 vertices[p][d1] * vertices[q][d2]);
      }
    }

    // don't allow polygons to come back and meet themselves,
    // otherwise it will anger the triangulator
    // http://dev.processing.org/bugs/show_bug.cgi?id=97
    float vfirst[] = vertices[shapeFirst];
    float vlast[] = vertices[shapeLast-1];
    if ((abs(vfirst[X] - vlast[X]) < EPSILON) &&
        (abs(vfirst[Y] - vlast[Y]) < EPSILON) &&
        (abs(vfirst[Z] - vlast[Z]) < EPSILON)) {
      shapeLast--;
    }

    // then sort the vertices so they are always in a counterclockwise order
    int j = 0;
    if (area > 0) {
      for (int i = shapeFirst; i < shapeLast; i++) {
        j = i - shapeFirst;
        vertexOrder[j] = i;
      }
    } else {
      for (int i = shapeFirst; i < shapeLast; i++) {
        j = i - shapeFirst;
        vertexOrder[j] = (shapeLast - 1) - j;
      }
    }

    // remove vc-2 Vertices, creating 1 triangle every time
    int vc = shapeLast - shapeFirst;
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
      double Ax = -10 * vertices[vertexOrder[u]][d1];
      double Ay =  10 * vertices[vertexOrder[u]][d2];
      double Bx = -10 * vertices[vertexOrder[v]][d1];
      double By =  10 * vertices[vertexOrder[v]][d2];
      double Cx = -10 * vertices[vertexOrder[w]][d1];
      double Cy =  10 * vertices[vertexOrder[w]][d2];

      // first we check if <u,v,w> continues going ccw
      if (EPSILON > (((Bx-Ax) * (Cy-Ay)) - ((By-Ay) * (Cx-Ax)))) {
        continue;
      }

      for (int p = 0; p < vc; p++) {
        if ((p == u) || (p == v) || (p == w)) {
          continue;
        }

        double Px = -10 * vertices[vertexOrder[p]][d1];
        double Py =  10 * vertices[vertexOrder[p]][d2];

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
        addTriangle(vertexOrder[u], vertexOrder[v], vertexOrder[w]);

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


  private void toWorldNormal(float nx, float ny, float nz, float[] out) {
    out[0] =
      modelviewInv.m00*nx + modelviewInv.m10*ny +
      modelviewInv.m20*nz + modelviewInv.m30;
    out[1] =
      modelviewInv.m01*nx + modelviewInv.m11*ny +
      modelviewInv.m21*nz + modelviewInv.m31;
    out[2] =
      modelviewInv.m02*nx + modelviewInv.m12*ny +
      modelviewInv.m22*nz + modelviewInv.m32;
    out[3] =
      modelviewInv.m03*nx + modelviewInv.m13*ny +
      modelviewInv.m23*nz + modelviewInv.m33;

    if (out[3] != 0 && out[3] != 1) {
      // divide by perspective coordinate
      out[0] /= out[3]; out[1] /= out[3]; out[2] /= out[3];
    }
    out[3] = 1;

    float nlen = mag(out[0], out[1], out[2]);  // normalize
    if (nlen != 0 && nlen != 1) {
      out[0] /= nlen; out[1] /= nlen; out[2] /= nlen;
    }
  }


  //private PVector calcLightingNorm = new PVector();
  //private PVector calcLightingWorldNorm = new PVector();
  float[] worldNormal = new float[4];


  private void calcLightingContribution(int vIndex,
                                        float[] contribution) {
    calcLightingContribution(vIndex, contribution, false);
  }


  private void calcLightingContribution(int vIndex,
                                        float[] contribution,
                                        boolean normalIsWorld) {
    float[] v = vertices[vIndex];

    float sr = v[SPR];
    float sg = v[SPG];
    float sb = v[SPB];

    float wx = v[VX];
    float wy = v[VY];
    float wz = v[VZ];
    float shine = v[SHINE];

    float nx = v[NX];
    float ny = v[NY];
    float nz = v[NZ];

    if (!normalIsWorld) {
//      System.out.println("um, hello?");
//      calcLightingNorm.set(nx, ny, nz);
//      //modelviewInv.mult(calcLightingNorm, calcLightingWorldNorm);
//
////      PMatrix3D mvi = modelViewInv;
////      float ox = mvi.m00*nx + mvi.m10*ny + mvi*m20+nz +
//      modelviewInv.cmult(calcLightingNorm, calcLightingWorldNorm);
//
//      calcLightingWorldNorm.normalize();
//      nx = calcLightingWorldNorm.x;
//      ny = calcLightingWorldNorm.y;
//      nz = calcLightingWorldNorm.z;

      toWorldNormal(v[NX], v[NY], v[NZ], worldNormal);
      nx = worldNormal[X];
      ny = worldNormal[Y];
      nz = worldNormal[Z];

//      float wnx = modelviewInv.multX(nx, ny, nz);
//      float wny = modelviewInv.multY(nx, ny, nz);
//      float wnz = modelviewInv.multZ(nx, ny, nz);
//      float wnw = modelviewInv.multW(nx, ny, nz);

//      if (wnw != 0 && wnw != 1) {
//        wnx /= wnw;
//        wny /= wnw;
//        wnz /= wnw;
//      }
//      float nlen = mag(wnx, wny, wnw);
//      if (nlen != 0 && nlen != 1) {
//        nx = wnx / nlen;
//        ny = wny / nlen;
//        nz = wnz / nlen;
//      } else {
//        nx = wnx;
//        ny = wny;
//        nz = wnz;
//      }
//      */
    } else {
      nx = v[NX];
      ny = v[NY];
      nz = v[NZ];
    }

    // Since the camera space == world space,
    // we can test for visibility by the dot product of
    // the normal with the direction from pt. to eye.
    float dir = dot(nx, ny, nz, -wx, -wy, -wz);
    // If normal is away from camera, choose its opposite.
    // If we add backface culling, this will be backfacing
    // (but since this is per vertex, it's more complicated)
    if (dir < 0) {
      nx = -nx;
      ny = -ny;
      nz = -nz;
    }

    // These two terms will sum the contributions from the various lights
    contribution[LIGHT_AMBIENT_R] = 0;
    contribution[LIGHT_AMBIENT_G] = 0;
    contribution[LIGHT_AMBIENT_B] = 0;

    contribution[LIGHT_DIFFUSE_R] = 0;
    contribution[LIGHT_DIFFUSE_G] = 0;
    contribution[LIGHT_DIFFUSE_B] = 0;

    contribution[LIGHT_SPECULAR_R] = 0;
    contribution[LIGHT_SPECULAR_G] = 0;
    contribution[LIGHT_SPECULAR_B] = 0;

    // for (int i = 0; i < MAX_LIGHTS; i++) {
    // if (!light[i]) continue;
    for (int i = 0; i < lightCount; i++) {

      float denom = lightFalloffConstant[i];
      float spotTerm = 1;

      if (lightType[i] == AMBIENT) {
        if (lightFalloffQuadratic[i] != 0 || lightFalloffLinear[i] != 0) {
          // Falloff depends on distance
          float distSq = mag(lightPosition[i].x - wx,
                             lightPosition[i].y - wy,
                             lightPosition[i].z - wz);
          denom +=
            lightFalloffQuadratic[i] * distSq +
            lightFalloffLinear[i] * sqrt(distSq);
        }
        if (denom == 0) denom = 1;

        contribution[LIGHT_AMBIENT_R] += lightDiffuse[i][0] / denom;
        contribution[LIGHT_AMBIENT_G] += lightDiffuse[i][1] / denom;
        contribution[LIGHT_AMBIENT_B] += lightDiffuse[i][2] / denom;

      } else {
        // If not ambient, we must deal with direction

        // li is the vector from the vertex to the light
        float lix, liy, liz;
        float lightDir_dot_li = 0;
        float n_dot_li = 0;

        if (lightType[i] == DIRECTIONAL) {
          lix = -lightNormal[i].x;
          liy = -lightNormal[i].y;
          liz = -lightNormal[i].z;
          denom = 1;
          n_dot_li = (nx * lix + ny * liy + nz * liz);
          // If light is lighting the face away from the camera, ditch
          if (n_dot_li <= 0) {
            continue;
          }
        } else { // Point or spot light (must deal also with light location)
          lix = lightPosition[i].x - wx;
          liy = lightPosition[i].y - wy;
          liz = lightPosition[i].z - wz;
          // normalize
          float distSq = mag(lix, liy, liz);
          if (distSq != 0) {
            lix /= distSq;
            liy /= distSq;
            liz /= distSq;
          }
          n_dot_li = (nx * lix + ny * liy + nz * liz);
          // If light is lighting the face away from the camera, ditch
          if (n_dot_li <= 0) {
            continue;
          }

          if (lightType[i] == SPOT) { // Must deal with spot cone
            lightDir_dot_li =
              -(lightNormal[i].x * lix +
                lightNormal[i].y * liy +
                lightNormal[i].z * liz);
            // Outside of spot cone
            if (lightDir_dot_li <= lightSpotAngleCos[i]) {
              continue;
            }
            spotTerm = (float) Math.pow(lightDir_dot_li, lightSpotConcentration[i]);
          }

          if (lightFalloffQuadratic[i] != 0 || lightFalloffLinear[i] != 0) {
            // Falloff depends on distance
            denom +=
              lightFalloffQuadratic[i] * distSq +
              lightFalloffLinear[i] * (float) sqrt(distSq);
          }
        }
        // Directional, point, or spot light:

        // We know n_dot_li > 0 from above "continues"

        if (denom == 0)
          denom = 1;
        float mul = n_dot_li * spotTerm / denom;
        contribution[LIGHT_DIFFUSE_R] += lightDiffuse[i][0] * mul;
        contribution[LIGHT_DIFFUSE_G] += lightDiffuse[i][1] * mul;
        contribution[LIGHT_DIFFUSE_B] += lightDiffuse[i][2] * mul;

        // SPECULAR

        // If the material and light have a specular component.
        if ((sr > 0 || sg > 0 || sb > 0) &&
            (lightSpecular[i][0] > 0 ||
             lightSpecular[i][1] > 0 ||
             lightSpecular[i][2] > 0)) {

          float vmag = mag(wx, wy, wz);
          if (vmag != 0) {
            wx /= vmag;
            wy /= vmag;
            wz /= vmag;
          }
          float sx = lix - wx;
          float sy = liy - wy;
          float sz = liz - wz;
          vmag = mag(sx, sy, sz);
          if (vmag != 0) {
            sx /= vmag;
            sy /= vmag;
            sz /= vmag;
          }
          float s_dot_n = (sx * nx + sy * ny + sz * nz);

          if (s_dot_n > 0) {
            s_dot_n = (float) Math.pow(s_dot_n, shine);
            mul = s_dot_n * spotTerm / denom;
            contribution[LIGHT_SPECULAR_R] += lightSpecular[i][0] * mul;
            contribution[LIGHT_SPECULAR_G] += lightSpecular[i][1] * mul;
            contribution[LIGHT_SPECULAR_B] += lightSpecular[i][2] * mul;
          }

        }
      }
    }
    return;
  }


  // Multiply the lighting contribution into the vertex's colors.
  // Only do this when there is ONE lighting per vertex
  // (MANUAL_VERTEX_NORMAL or SHAPE_NORMAL mode).
  private void applyLightingContribution(int vIndex, float[] contribution) {
    float[] v = vertices[vIndex];

    v[R] = clamp(v[ER] + v[AR] * contribution[LIGHT_AMBIENT_R] + v[DR] * contribution[LIGHT_DIFFUSE_R]);
    v[G] = clamp(v[EG] + v[AG] * contribution[LIGHT_AMBIENT_G] + v[DG] * contribution[LIGHT_DIFFUSE_G]);
    v[B] = clamp(v[EB] + v[AB] * contribution[LIGHT_AMBIENT_B] + v[DB] * contribution[LIGHT_DIFFUSE_B]);
    v[A] = clamp(v[DA]);

    v[SPR] = clamp(v[SPR] * contribution[LIGHT_SPECULAR_R]);
    v[SPG] = clamp(v[SPG] * contribution[LIGHT_SPECULAR_G]);
    v[SPB] = clamp(v[SPB] * contribution[LIGHT_SPECULAR_B]);
    //v[SPA] = min(1, v[SPA]);

    v[BEEN_LIT] = 1;
  }


  private void lightVertex(int vIndex, float[] contribution) {
    calcLightingContribution(vIndex, contribution);
    applyLightingContribution(vIndex, contribution);
  }


  private void lightUnlitVertex(int vIndex, float[] contribution) {
    if (vertices[vIndex][BEEN_LIT] == 0) {
      lightVertex(vIndex, contribution);
    }
  }


  private void copyPrelitVertexColor(int triIndex, int index, int colorIndex) {
    float[] triColor = triangleColors[triIndex][colorIndex];
    float[] v = vertices[index];

    triColor[TRI_DIFFUSE_R] = v[R];
    triColor[TRI_DIFFUSE_G] = v[G];
    triColor[TRI_DIFFUSE_B] = v[B];
    triColor[TRI_DIFFUSE_A] = v[A];
    triColor[TRI_SPECULAR_R] = v[SPR];
    triColor[TRI_SPECULAR_G] = v[SPG];
    triColor[TRI_SPECULAR_B] = v[SPB];
    //triColor[TRI_SPECULAR_A] = v[SPA];
  }


  private void copyVertexColor(int triIndex, int index, int colorIndex,
                               float[] contrib) {
    float[] triColor = triangleColors[triIndex][colorIndex];
    float[] v = vertices[index];

    triColor[TRI_DIFFUSE_R] =
      clamp(v[ER] + v[AR] * contrib[LIGHT_AMBIENT_R] + v[DR] * contrib[LIGHT_DIFFUSE_R]);
    triColor[TRI_DIFFUSE_G] =
      clamp(v[EG] + v[AG] * contrib[LIGHT_AMBIENT_G] + v[DG] * contrib[LIGHT_DIFFUSE_G]);
    triColor[TRI_DIFFUSE_B] =
      clamp(v[EB] + v[AB] * contrib[LIGHT_AMBIENT_B] + v[DB] * contrib[LIGHT_DIFFUSE_B]);
    triColor[TRI_DIFFUSE_A] = clamp(v[DA]);

    triColor[TRI_SPECULAR_R] = clamp(v[SPR] * contrib[LIGHT_SPECULAR_R]);
    triColor[TRI_SPECULAR_G] = clamp(v[SPG] * contrib[LIGHT_SPECULAR_G]);
    triColor[TRI_SPECULAR_B] = clamp(v[SPB] * contrib[LIGHT_SPECULAR_B]);
  }


  private void lightTriangle(int triIndex, float[] lightContribution) {
    int vIndex = triangles[triIndex][VERTEX1];
    copyVertexColor(triIndex, vIndex, 0, lightContribution);
    vIndex = triangles[triIndex][VERTEX2];
    copyVertexColor(triIndex, vIndex, 1, lightContribution);
    vIndex = triangles[triIndex][VERTEX3];
    copyVertexColor(triIndex, vIndex, 2, lightContribution);
  }


  private void lightTriangle(int triIndex) {
    int vIndex;

    // Handle lighting on, but no lights (in this case, just use emissive)
    // This wont be used currently because lightCount == 0 is don't use
    // lighting at all... So. OK. If that ever changes, use the below:
    /*
    if (lightCount == 0) {
      vIndex = triangles[triIndex][VERTEX1];
      copy_emissive_vertex_color_to_triangle(triIndex, vIndex, 0);
      vIndex = triangles[triIndex][VERTEX2];
      copy_emissive_vertex_color_to_triangle(triIndex, vIndex, 1);
      vIndex = triangles[triIndex][VERTEX3];
      copy_emissive_vertex_color_to_triangle(triIndex, vIndex, 2);
      return;
    }
    */

    // In MANUAL_VERTEX_NORMAL mode, we have a specific normal
    // for each vertex. In that case, we light any verts that
    // haven't already been lit and copy their colors straight
    // into the triangle.
    if (normalMode == NORMAL_MODE_VERTEX) {
      vIndex = triangles[triIndex][VERTEX1];
      lightUnlitVertex(vIndex, tempLightingContribution);
      copyPrelitVertexColor(triIndex, vIndex, 0);

      vIndex = triangles[triIndex][VERTEX2];
      lightUnlitVertex(vIndex, tempLightingContribution);
      copyPrelitVertexColor(triIndex, vIndex, 1);

      vIndex = triangles[triIndex][VERTEX3];
      lightUnlitVertex(vIndex, tempLightingContribution);
      copyPrelitVertexColor(triIndex, vIndex, 2);

    }

    // If the lighting doesn't depend on the vertex position, do the
    // following: We've already dealt with NORMAL_MODE_SHAPE mode before
    // we got into this function, so here we only have to deal with
    // NORMAL_MODE_AUTO. So we calculate the normal for this triangle,
    // and use that for the lighting.
    else if (!lightingDependsOnVertexPosition) {
      vIndex = triangles[triIndex][VERTEX1];
      int vIndex2 = triangles[triIndex][VERTEX2];
      int vIndex3 = triangles[triIndex][VERTEX3];

      /*
      dv1[0] = vertices[vIndex2][VX] - vertices[vIndex][VX];
      dv1[1] = vertices[vIndex2][VY] - vertices[vIndex][VY];
      dv1[2] = vertices[vIndex2][VZ] - vertices[vIndex][VZ];

      dv2[0] = vertices[vIndex3][VX] - vertices[vIndex][VX];
      dv2[1] = vertices[vIndex3][VY] - vertices[vIndex][VY];
      dv2[2] = vertices[vIndex3][VZ] - vertices[vIndex][VZ];

      cross(dv1, dv2, norm);
      */

      cross(vertices[vIndex2][VX] - vertices[vIndex][VX],
            vertices[vIndex2][VY] - vertices[vIndex][VY],
            vertices[vIndex2][VZ] - vertices[vIndex][VZ],
            vertices[vIndex3][VX] - vertices[vIndex][VX],
            vertices[vIndex3][VY] - vertices[vIndex][VY],
            vertices[vIndex3][VZ] - vertices[vIndex][VZ], lightTriangleNorm);

      lightTriangleNorm.normalize();
      vertices[vIndex][NX] = lightTriangleNorm.x;
      vertices[vIndex][NY] = lightTriangleNorm.y;
      vertices[vIndex][NZ] = lightTriangleNorm.z;

      // The true at the end says the normal is already in world coordinates
      calcLightingContribution(vIndex, tempLightingContribution, true);
      copyVertexColor(triIndex, vIndex, 0, tempLightingContribution);
      copyVertexColor(triIndex, vIndex2, 1, tempLightingContribution);
      copyVertexColor(triIndex, vIndex3, 2, tempLightingContribution);
    }

    // If lighting is position-dependent
    else {
      if (normalMode == NORMAL_MODE_SHAPE) {
        vIndex = triangles[triIndex][VERTEX1];
        vertices[vIndex][NX] = vertices[shapeFirst][NX];
        vertices[vIndex][NY] = vertices[shapeFirst][NY];
        vertices[vIndex][NZ] = vertices[shapeFirst][NZ];
        calcLightingContribution(vIndex, tempLightingContribution);
        copyVertexColor(triIndex, vIndex, 0, tempLightingContribution);

        vIndex = triangles[triIndex][VERTEX2];
        vertices[vIndex][NX] = vertices[shapeFirst][NX];
        vertices[vIndex][NY] = vertices[shapeFirst][NY];
        vertices[vIndex][NZ] = vertices[shapeFirst][NZ];
        calcLightingContribution(vIndex, tempLightingContribution);
        copyVertexColor(triIndex, vIndex, 1, tempLightingContribution);

        vIndex = triangles[triIndex][VERTEX3];
        vertices[vIndex][NX] = vertices[shapeFirst][NX];
        vertices[vIndex][NY] = vertices[shapeFirst][NY];
        vertices[vIndex][NZ] = vertices[shapeFirst][NZ];
        calcLightingContribution(vIndex, tempLightingContribution);
        copyVertexColor(triIndex, vIndex, 2, tempLightingContribution);
      }

      // lighting mode is AUTO_NORMAL
      else {
        vIndex = triangles[triIndex][VERTEX1];
        int vIndex2 = triangles[triIndex][VERTEX2];
        int vIndex3 = triangles[triIndex][VERTEX3];

        /*
        dv1[0] = vertices[vIndex2][VX] - vertices[vIndex][VX];
        dv1[1] = vertices[vIndex2][VY] - vertices[vIndex][VY];
        dv1[2] = vertices[vIndex2][VZ] - vertices[vIndex][VZ];

        dv2[0] = vertices[vIndex3][VX] - vertices[vIndex][VX];
        dv2[1] = vertices[vIndex3][VY] - vertices[vIndex][VY];
        dv2[2] = vertices[vIndex3][VZ] - vertices[vIndex][VZ];

        cross(dv1, dv2, norm);
        */

        cross(vertices[vIndex2][VX] - vertices[vIndex][VX],
              vertices[vIndex2][VY] - vertices[vIndex][VY],
              vertices[vIndex2][VZ] - vertices[vIndex][VZ],
              vertices[vIndex3][VX] - vertices[vIndex][VX],
              vertices[vIndex3][VY] - vertices[vIndex][VY],
              vertices[vIndex3][VZ] - vertices[vIndex][VZ], lightTriangleNorm);
//        float nmag = mag(norm[X], norm[Y], norm[Z]);
//        if (nmag != 0 && nmag != 1) {
//          norm[X] /= nmag; norm[Y] /= nmag; norm[Z] /= nmag;
//        }
        lightTriangleNorm.normalize();
        vertices[vIndex][NX] = lightTriangleNorm.x;
        vertices[vIndex][NY] = lightTriangleNorm.y;
        vertices[vIndex][NZ] = lightTriangleNorm.z;
        // The true at the end says the normal is already in world coordinates
        calcLightingContribution(vIndex, tempLightingContribution, true);
        copyVertexColor(triIndex, vIndex, 0, tempLightingContribution);

        vertices[vIndex2][NX] = lightTriangleNorm.x;
        vertices[vIndex2][NY] = lightTriangleNorm.y;
        vertices[vIndex2][NZ] = lightTriangleNorm.z;
        // The true at the end says the normal is already in world coordinates
        calcLightingContribution(vIndex2, tempLightingContribution, true);
        copyVertexColor(triIndex, vIndex2, 1, tempLightingContribution);

        vertices[vIndex3][NX] = lightTriangleNorm.x;
        vertices[vIndex3][NY] = lightTriangleNorm.y;
        vertices[vIndex3][NZ] = lightTriangleNorm.z;
        // The true at the end says the normal is already in world coordinates
        calcLightingContribution(vIndex3, tempLightingContribution, true);
        copyVertexColor(triIndex, vIndex3, 2, tempLightingContribution);
      }
    }
  }


  protected void renderTriangles(int start, int stop) {
    for (int i = start; i < stop; i++) {
      float a[] = vertices[triangles[i][VERTEX1]];
      float b[] = vertices[triangles[i][VERTEX2]];
      float c[] = vertices[triangles[i][VERTEX3]];
      int tex = triangles[i][TEXTURE_INDEX];

      /*
      // removing for 0149 with the return of P2D
      // ewjordan: hack to 'fix' accuracy issues when drawing in 2d
      // see also render_lines() where similar hack is employed
      float shift = 0.15f;//was 0.49f
      boolean shifted = false;
      if (drawing2D() && (a[Z] == 0)) {
        shifted = true;
        a[TX] += shift;
        a[TY] += shift;
        a[VX] += shift*a[VW];
        a[VY] += shift*a[VW];
        b[TX] += shift;
        b[TY] += shift;
        b[VX] += shift*b[VW];
        b[VY] += shift*b[VW];
        c[TX] += shift;
        c[TY] += shift;
        c[VX] += shift*c[VW];
        c[VY] += shift*c[VW];
      }
      */

      triangle.reset();

      // This is only true when not textured.
      // We really should pass specular straight through to triangle rendering.
      float ar = clamp(triangleColors[i][0][TRI_DIFFUSE_R] + triangleColors[i][0][TRI_SPECULAR_R]);
      float ag = clamp(triangleColors[i][0][TRI_DIFFUSE_G] + triangleColors[i][0][TRI_SPECULAR_G]);
      float ab = clamp(triangleColors[i][0][TRI_DIFFUSE_B] + triangleColors[i][0][TRI_SPECULAR_B]);
      float br = clamp(triangleColors[i][1][TRI_DIFFUSE_R] + triangleColors[i][1][TRI_SPECULAR_R]);
      float bg = clamp(triangleColors[i][1][TRI_DIFFUSE_G] + triangleColors[i][1][TRI_SPECULAR_G]);
      float bb = clamp(triangleColors[i][1][TRI_DIFFUSE_B] + triangleColors[i][1][TRI_SPECULAR_B]);
      float cr = clamp(triangleColors[i][2][TRI_DIFFUSE_R] + triangleColors[i][2][TRI_SPECULAR_R]);
      float cg = clamp(triangleColors[i][2][TRI_DIFFUSE_G] + triangleColors[i][2][TRI_SPECULAR_G]);
      float cb = clamp(triangleColors[i][2][TRI_DIFFUSE_B] + triangleColors[i][2][TRI_SPECULAR_B]);

      // ACCURATE TEXTURE CODE
      boolean failedToPrecalc = false;
      if (s_enableAccurateTextures && frustumMode){
        boolean textured = true;
        smoothTriangle.reset(3);
        smoothTriangle.smooth = true;
        smoothTriangle.interpARGB = true;
        smoothTriangle.setIntensities(ar, ag, ab, a[A],
                                      br, bg, bb, b[A],
                                      cr, cg, cb, c[A]);
        if (tex > -1 && textures[tex] != null) {
          smoothTriangle.setCamVertices(a[VX], a[VY], a[VZ],
                                        b[VX], b[VY], b[VZ],
                                        c[VX], c[VY], c[VZ]);
          smoothTriangle.interpUV = true;
          smoothTriangle.texture(textures[tex]);
          float umult = textures[tex].width;  // apparently no check for textureMode is needed here
          float vmult = textures[tex].height;
          smoothTriangle.vertices[0][U] = a[U]*umult;
          smoothTriangle.vertices[0][V] = a[V]*vmult;
          smoothTriangle.vertices[1][U] = b[U]*umult;
          smoothTriangle.vertices[1][V] = b[V]*vmult;
          smoothTriangle.vertices[2][U] = c[U]*umult;
          smoothTriangle.vertices[2][V] = c[V]*vmult;
        } else {
          smoothTriangle.interpUV = false;
          textured = false;
        }

        smoothTriangle.setVertices(a[TX], a[TY], a[TZ],
                                   b[TX], b[TY], b[TZ],
                                   c[TX], c[TY], c[TZ]);


        if (!textured || smoothTriangle.precomputeAccurateTexturing()){
                smoothTriangle.render();
        } else {
                // Something went wrong with the precomputation,
                // so we need to fall back on normal PTriangle
                // rendering.
                failedToPrecalc = true;
        }
      }

      // Normal triangle rendering
      // Note: this is not an end-if from the smoothed texturing mode
      // because it's possible that the precalculation will fail and we
      // need to fall back on normal rendering.
      if (!s_enableAccurateTextures || failedToPrecalc || (frustumMode == false)){
      if (tex > -1 && textures[tex] != null) {
        triangle.setTexture(textures[tex]);
        triangle.setUV(a[U], a[V], b[U], b[V], c[U], c[V]);
      }

      triangle.setIntensities(ar, ag, ab, a[A],
                              br, bg, bb, b[A],
                              cr, cg, cb, c[A]);

      triangle.setVertices(a[TX], a[TY], a[TZ],
                           b[TX], b[TY], b[TZ],
                           c[TX], c[TY], c[TZ]);

          triangle.render();
      }

      /*
      // removing for 0149 with the return of P2D
      if (drawing2D() && shifted){
        a[TX] -= shift;
        a[TY] -= shift;
        a[VX] -= shift*a[VW];
        a[VY] -= shift*a[VW];
        b[TX] -= shift;
        b[TY] -= shift;
        b[VX] -= shift*b[VW];
        b[VY] -= shift*b[VW];
        c[TX] -= shift;
        c[TY] -= shift;
        c[VX] -= shift*c[VW];
        c[VY] -= shift*c[VW];
      }
      */
    }
  }


  protected void rawTriangles(int start, int stop) {
    raw.colorMode(RGB, 1);
    raw.noStroke();
    raw.beginShape(TRIANGLES);

    for (int i = start; i < stop; i++) {
      float a[] = vertices[triangles[i][VERTEX1]];
      float b[] = vertices[triangles[i][VERTEX2]];
      float c[] = vertices[triangles[i][VERTEX3]];

      float ar = clamp(triangleColors[i][0][TRI_DIFFUSE_R] + triangleColors[i][0][TRI_SPECULAR_R]);
      float ag = clamp(triangleColors[i][0][TRI_DIFFUSE_G] + triangleColors[i][0][TRI_SPECULAR_G]);
      float ab = clamp(triangleColors[i][0][TRI_DIFFUSE_B] + triangleColors[i][0][TRI_SPECULAR_B]);
      float br = clamp(triangleColors[i][1][TRI_DIFFUSE_R] + triangleColors[i][1][TRI_SPECULAR_R]);
      float bg = clamp(triangleColors[i][1][TRI_DIFFUSE_G] + triangleColors[i][1][TRI_SPECULAR_G]);
      float bb = clamp(triangleColors[i][1][TRI_DIFFUSE_B] + triangleColors[i][1][TRI_SPECULAR_B]);
      float cr = clamp(triangleColors[i][2][TRI_DIFFUSE_R] + triangleColors[i][2][TRI_SPECULAR_R]);
      float cg = clamp(triangleColors[i][2][TRI_DIFFUSE_G] + triangleColors[i][2][TRI_SPECULAR_G]);
      float cb = clamp(triangleColors[i][2][TRI_DIFFUSE_B] + triangleColors[i][2][TRI_SPECULAR_B]);

      int tex = triangles[i][TEXTURE_INDEX];
      PImage texImage = (tex > -1) ? textures[tex] : null;
      if (texImage != null) {
        if (raw.is3D()) {
          if ((a[VW] != 0) && (b[VW] != 0) && (c[VW] != 0)) {
            raw.fill(ar, ag, ab, a[A]);
            raw.vertex(a[VX] / a[VW], a[VY] / a[VW], a[VZ] / a[VW], a[U], a[V]);
            raw.fill(br, bg, bb, b[A]);
            raw.vertex(b[VX] / b[VW], b[VY] / b[VW], b[VZ] / b[VW], b[U], b[V]);
            raw.fill(cr, cg, cb, c[A]);
            raw.vertex(c[VX] / c[VW], c[VY] / c[VW], c[VZ] / c[VW], c[U], c[V]);
          }
        } else if (raw.is2D()) {
          raw.fill(ar, ag, ab, a[A]);
          raw.vertex(a[TX], a[TY], a[U], a[V]);
          raw.fill(br, bg, bb, b[A]);
          raw.vertex(b[TX], b[TY], b[U], b[V]);
          raw.fill(cr, cg, cb, c[A]);
          raw.vertex(c[TX], c[TY], c[U], c[V]);
        }
      } else {  // no texture
        if (raw.is3D()) {
          if ((a[VW] != 0) && (b[VW] != 0) && (c[VW] != 0)) {
            raw.fill(ar, ag, ab, a[A]);
            raw.vertex(a[VX] / a[VW], a[VY] / a[VW], a[VZ] / a[VW]);
            raw.fill(br, bg, bb, b[A]);
            raw.vertex(b[VX] / b[VW], b[VY] / b[VW], b[VZ] / b[VW]);
            raw.fill(cr, cg, cb, c[A]);
            raw.vertex(c[VX] / c[VW], c[VY] / c[VW], c[VZ] / c[VW]);
          }
        } else if (raw.is2D()) {
          raw.fill(ar, ag, ab, a[A]);
          raw.vertex(a[TX], a[TY]);
          raw.fill(br, bg, bb, b[A]);
          raw.vertex(b[TX], b[TY]);
          raw.fill(cr, cg, cb, c[A]);
          raw.vertex(c[TX], c[TY]);
        }
      }
    }

    raw.endShape();
  }


  //////////////////////////////////////////////////////////////


  //public void bezierVertex(float x2, float y2,
  //                         float x3, float y3,
  //                         float x4, float y4)


  //public void bezierVertex(float x2, float y2, float z2,
  //                         float x3, float y3, float z3,
  //                         float x4, float y4, float z4)



  //////////////////////////////////////////////////////////////


  //public void curveVertex(float x, float y)


  //public void curveVertex(float x, float y, float z)



  ////////////////////////////////////////////////////////////


  /**
   * Emit any sorted geometry that's been collected on this frame.
   */
  public void flush() {
    if (hints[ENABLE_DEPTH_SORT]) {
      sort();
    }
    render();

    /*
    if (triangleCount > 0) {
      if (hints[ENABLE_DEPTH_SORT]) {
        sortTriangles();
      }
      renderTriangles();
    }
    if (lineCount > 0) {
      if (hints[ENABLE_DEPTH_SORT]) {
        sortLines();
      }
      renderLines();
    }
    // Clear this out in case flush() is called again.
    // For instance, with hint(ENABLE_DEPTH_SORT), it will be called
    // once on endRaw(), and once again at endDraw().
    triangleCount = 0;
    lineCount = 0;
    */
  }


  protected void render() {
    if (pointCount > 0) {
      renderPoints(0, pointCount);
      if (raw != null) {
        rawPoints(0, pointCount);
      }
      pointCount = 0;
    }
    if (lineCount > 0) {
      renderLines(0, lineCount);
      if (raw != null) {
        rawLines(0, lineCount);
      }
      lineCount = 0;
      pathCount = 0;
    }
    if (triangleCount > 0) {
      renderTriangles(0, triangleCount);
      if (raw != null) {
        rawTriangles(0, triangleCount);
      }
      triangleCount = 0;
    }
  }


  /**
   * Handle depth sorting of geometry. Currently this only handles triangles,
   * however in the future it will be expanded for points and lines, which
   * will also need to be interspersed with one another while rendering.
   */
  protected void sort() {
    if (triangleCount > 0) {
      sortTrianglesInternal(0, triangleCount-1);
    }
  }


  private void sortTrianglesInternal(int i, int j) {
    int pivotIndex = (i+j)/2;
    sortTrianglesSwap(pivotIndex, j);
    int k = sortTrianglesPartition(i-1, j);
    sortTrianglesSwap(k, j);
    if ((k-i) > 1) sortTrianglesInternal(i, k-1);
    if ((j-k) > 1) sortTrianglesInternal(k+1, j);
  }


  private int sortTrianglesPartition(int left, int right) {
    int pivot = right;
    do {
      while (sortTrianglesCompare(++left, pivot) < 0) { }
      while ((right != 0) &&
             (sortTrianglesCompare(--right, pivot) > 0)) { }
      sortTrianglesSwap(left, right);
    } while (left < right);
    sortTrianglesSwap(left, right);
    return left;
  }


  private void sortTrianglesSwap(int a, int b) {
    int tempi[] = triangles[a];
    triangles[a] = triangles[b];
    triangles[b] = tempi;
    float tempf[][] = triangleColors[a];
    triangleColors[a] = triangleColors[b];
    triangleColors[b] = tempf;
  }


  private float sortTrianglesCompare(int a, int b) {
    /*
    if (Float.isNaN(vertices[triangles[a][VERTEX1]][TZ]) ||
        Float.isNaN(vertices[triangles[a][VERTEX2]][TZ]) ||
        Float.isNaN(vertices[triangles[a][VERTEX3]][TZ]) ||
        Float.isNaN(vertices[triangles[b][VERTEX1]][TZ]) ||
        Float.isNaN(vertices[triangles[b][VERTEX2]][TZ]) ||
        Float.isNaN(vertices[triangles[b][VERTEX3]][TZ])) {
      System.err.println("NaN values in triangle");
    }
    */
    return ((vertices[triangles[b][VERTEX1]][TZ] +
             vertices[triangles[b][VERTEX2]][TZ] +
             vertices[triangles[b][VERTEX3]][TZ]) -
            (vertices[triangles[a][VERTEX1]][TZ] +
             vertices[triangles[a][VERTEX2]][TZ] +
             vertices[triangles[a][VERTEX3]][TZ]));
  }



  //////////////////////////////////////////////////////////////

  // POINT, LINE, TRIANGLE, QUAD

  // Because vertex(x, y) is mapped to vertex(x, y, 0), none of these commands
  // need to be overridden from their default implementation in PGraphics.


  //public void point(float x, float y)


  //public void point(float x, float y, float z)


  //public void line(float x1, float y1, float x2, float y2)


  //public void line(float x1, float y1, float z1,
  //                 float x2, float y2, float z2)


  //public void triangle(float x1, float y1, float x2, float y2,
  //                     float x3, float y3)


  //public void quad(float x1, float y1, float x2, float y2,
  //                 float x3, float y3, float x4, float y4)



  //////////////////////////////////////////////////////////////

  // RECT


  //public void rectMode(int mode)


  //public void rect(float a, float b, float c, float d)


  //protected void rectImpl(float x1, float y1, float x2, float y2)



  //////////////////////////////////////////////////////////////

  // ELLIPSE


  //public void ellipseMode(int mode)


  //public void ellipse(float a, float b, float c, float d)


  protected void ellipseImpl(float x, float y, float w, float h) {
    float radiusH = w / 2;
    float radiusV = h / 2;

    float centerX = x + radiusH;
    float centerY = y + radiusV;

//    float sx1 = screenX(x, y);
//    float sy1 = screenY(x, y);
//    float sx2 = screenX(x+w, y+h);
//    float sy2 = screenY(x+w, y+h);

    // returning to pre-1.0 version of algorithm because of problems
    int rough = (int)(4+Math.sqrt(w+h)*3);
    int accuracy = PApplet.constrain(rough, 6, 100);

    if (fill) {
      // returning to pre-1.0 version of algorithm because of problems
//      int rough = (int)(4+Math.sqrt(w+h)*3);
//      int rough = (int) (TWO_PI * PApplet.dist(sx1, sy1, sx2, sy2) / 20);
//      int accuracy = PApplet.constrain(rough, 6, 100);

      float inc = (float)SINCOS_LENGTH / accuracy;
      float val = 0;

      boolean strokeSaved = stroke;
      stroke = false;
      boolean smoothSaved = smooth;
      if (smooth && stroke) {
        smooth = false;
      }

      beginShape(TRIANGLE_FAN);
      normal(0, 0, 1);
      vertex(centerX, centerY);
      for (int i = 0; i < accuracy; i++) {
        vertex(centerX + cosLUT[(int) val] * radiusH,
               centerY + sinLUT[(int) val] * radiusV);
        val = (val + inc) % SINCOS_LENGTH;
      }
      // back to the beginning
      vertex(centerX + cosLUT[0] * radiusH,
             centerY + sinLUT[0] * radiusV);
      endShape();

      stroke = strokeSaved;
      smooth = smoothSaved;
    }

    if (stroke) {
//      int rough = (int) (TWO_PI * PApplet.dist(sx1, sy1, sx2, sy2) / 8);
//      int accuracy = PApplet.constrain(rough, 6, 100);

      float inc = (float)SINCOS_LENGTH / accuracy;
      float val = 0;

      boolean savedFill = fill;
      fill = false;

      val = 0;
      beginShape();
      for (int i = 0; i < accuracy; i++) {
        vertex(centerX + cosLUT[(int) val] * radiusH,
               centerY + sinLUT[(int) val] * radiusV);
        val = (val + inc) % SINCOS_LENGTH;
      }
      endShape(CLOSE);

      fill = savedFill;
    }
  }


  //public void arc(float a, float b, float c, float d,
  //                float start, float stop)


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

      int startLUT = (int) (0.5f + (start / TWO_PI) * SINCOS_LENGTH);
      int stopLUT = (int) (0.5f + (stop / TWO_PI) * SINCOS_LENGTH);

      beginShape(TRIANGLE_FAN);
      vertex(centerX, centerY);
      int increment = 1; // what's a good algorithm? stopLUT - startLUT;
      for (int i = startLUT; i < stopLUT; i += increment) {
        int ii = i % SINCOS_LENGTH;
        // modulo won't make the value positive
        if (ii < 0) ii += SINCOS_LENGTH;
        vertex(centerX + cosLUT[ii] * hr,
               centerY + sinLUT[ii] * vr);
      }
      // draw last point explicitly for accuracy
      vertex(centerX + cosLUT[stopLUT % SINCOS_LENGTH] * hr,
             centerY + sinLUT[stopLUT % SINCOS_LENGTH] * vr);
      endShape();

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


  //public void box(float size)


  public void box(float w, float h, float d) {
    if (triangle != null) {  // triangle is null in gl
      triangle.setCulling(true);
    }

    super.box(w, h, d);

    if (triangle != null) {  // triangle is null in gl
      triangle.setCulling(false);
    }
  }



  //////////////////////////////////////////////////////////////

  // SPHERE


  //public void sphereDetail(int res)


  //public void sphereDetail(int ures, int vres)


  public void sphere(float r) {
    if (triangle != null) {  // triangle is null in gl
      triangle.setCulling(true);
    }

    super.sphere(r);

    if (triangle != null) {  // triangle is null in gl
      triangle.setCulling(false);
    }
  }



  //////////////////////////////////////////////////////////////

  // BEZIER


  //public float bezierPoint(float a, float b, float c, float d, float t)


  //public float bezierTangent(float a, float b, float c, float d, float t)


  //public void bezierDetail(int detail)


  //public void bezier(float x1, float y1,
  //                   float x2, float y2,
  //                   float x3, float y3,
  //                   float x4, float y4)


  //public void bezier(float x1, float y1, float z1,
  //                   float x2, float y2, float z2,
  //                   float x3, float y3, float z3,
  //                   float x4, float y4, float z4)



  //////////////////////////////////////////////////////////////

  // CATMULL-ROM CURVES


  //public float curvePoint(float a, float b, float c, float d, float t)


  //public float curveTangent(float a, float b, float c, float d, float t)


  //public void curveDetail(int detail)


  //public void curveTightness(float tightness)


  //public void curve(float x1, float y1,
  //                  float x2, float y2,
  //                  float x3, float y3,
  //                  float x4, float y4)


  //public void curve(float x1, float y1, float z1,
  //                  float x2, float y2, float z2,
  //                  float x3, float y3, float z3,
  //                  float x4, float y4, float z4)



  //////////////////////////////////////////////////////////////

  // SMOOTH


  public void smooth() {
    //showMethodWarning("smooth");
          s_enableAccurateTextures = true;
          smooth = true;
  }


  public void noSmooth() {
          s_enableAccurateTextures = false;
          smooth = false;
  }



  //////////////////////////////////////////////////////////////

  // IMAGES


  //public void imageMode(int mode)


  //public void image(PImage image, float x, float y)


  //public void image(PImage image, float x, float y, float c, float d)


  //public void image(PImage image,
  //                  float a, float b, float c, float d,
  //                  int u1, int v1, int u2, int v2)


  //protected void imageImpl(PImage image,
  //                         float x1, float y1, float x2, float y2,
  //                         int u1, int v1, int u2, int v2)



  //////////////////////////////////////////////////////////////

  // SHAPE


  //public void shapeMode(int mode)


  //public void shape(PShape shape)


  //public void shape(PShape shape, float x, float y)


  //public void shape(PShape shape, float x, float y, float c, float d)



  //////////////////////////////////////////////////////////////

  // TEXT SETTINGS

  // Only textModeCheck overridden from PGraphics, no textAlign, textAscent,
  // textDescent, textFont, textLeading, textMode, textSize, textWidth


  protected boolean textModeCheck(int mode) {
    return (textMode == MODEL) || (textMode == SCREEN);
  }



  //////////////////////////////////////////////////////////////

  // TEXT

  // None of the variations of text() are overridden from PGraphics.



  //////////////////////////////////////////////////////////////

  // TEXT IMPL

  // Not even the text drawing implementation stuff is overridden.



  //////////////////////////////////////////////////////////////

  // MATRIX STACK


  public void pushMatrix() {
    if (matrixStackDepth == MATRIX_STACK_DEPTH) {
      throw new RuntimeException(ERROR_PUSHMATRIX_OVERFLOW);
    }
    modelview.get(matrixStack[matrixStackDepth]);
    modelviewInv.get(matrixInvStack[matrixStackDepth]);
    matrixStackDepth++;
  }


  public void popMatrix() {
    if (matrixStackDepth == 0) {
      throw new RuntimeException(ERROR_PUSHMATRIX_UNDERFLOW);
    }
    matrixStackDepth--;
    modelview.set(matrixStack[matrixStackDepth]);
    modelviewInv.set(matrixInvStack[matrixStackDepth]);
  }



  //////////////////////////////////////////////////////////////

  // MATRIX TRANSFORMATIONS


  public void translate(float tx, float ty) {
    translate(tx, ty, 0);
  }


  public void translate(float tx, float ty, float tz) {
    forwardTransform.translate(tx, ty, tz);
    reverseTransform.invTranslate(tx, ty, tz);
  }


  /**
   * Two dimensional rotation. Same as rotateZ (this is identical
   * to a 3D rotation along the z-axis) but included for clarity --
   * it'd be weird for people drawing 2D graphics to be using rotateZ.
   * And they might kick our a-- for the confusion.
   */
  public void rotate(float angle) {
    rotateZ(angle);
  }


  public void rotateX(float angle) {
    forwardTransform.rotateX(angle);
    reverseTransform.invRotateX(angle);
  }


  public void rotateY(float angle) {
    forwardTransform.rotateY(angle);
    reverseTransform.invRotateY(angle);
  }


  public void rotateZ(float angle) {
    forwardTransform.rotateZ(angle);
    reverseTransform.invRotateZ(angle);
  }


  /**
   * Rotate around an arbitrary vector, similar to glRotate(),
   * except that it takes radians (instead of degrees).
   */
  public void rotate(float angle, float v0, float v1, float v2) {
    forwardTransform.rotate(angle, v0, v1, v2);
    reverseTransform.invRotate(angle, v0, v1, v2);
  }


  /**
   * Same as scale(s, s, s).
   */
  public void scale(float s) {
    scale(s, s, s);
  }


  /**
   * Same as scale(sx, sy, 1).
   */
  public void scale(float sx, float sy) {
    scale(sx, sy, 1);
  }


  /**
   * Scale in three dimensions.
   */
  public void scale(float x, float y, float z) {
    forwardTransform.scale(x, y, z);
    reverseTransform.invScale(x, y, z);
  }



  //////////////////////////////////////////////////////////////

  // MATRIX MORE!


  public void resetMatrix() {
    forwardTransform.reset();
    reverseTransform.reset();
  }


  public void applyMatrix(PMatrix2D source) {
    applyMatrix(source.m00, source.m01, source.m02,
                source.m10, source.m11, source.m12);
  }


  public void applyMatrix(float n00, float n01, float n02,
                          float n10, float n11, float n12) {
    applyMatrix(n00, n01, n02, 0,
                n10, n11, n12, 0,
                0,   0,   1,   0,
                0,   0,   0,   1);
  }


  public void applyMatrix(PMatrix3D source) {
    applyMatrix(source.m00, source.m01, source.m02, source.m03,
                source.m10, source.m11, source.m12, source.m13,
                source.m20, source.m21, source.m22, source.m23,
                source.m30, source.m31, source.m32, source.m33);
  }


  /**
   * Apply a 4x4 transformation matrix. Same as glMultMatrix().
   * This call will be slow because it will try to calculate the
   * inverse of the transform. So avoid it whenever possible.
   */
  public void applyMatrix(float n00, float n01, float n02, float n03,
                          float n10, float n11, float n12, float n13,
                          float n20, float n21, float n22, float n23,
                          float n30, float n31, float n32, float n33) {

    forwardTransform.apply(n00, n01, n02, n03,
                           n10, n11, n12, n13,
                           n20, n21, n22, n23,
                           n30, n31, n32, n33);

    reverseTransform.invApply(n00, n01, n02, n03,
                              n10, n11, n12, n13,
                              n20, n21, n22, n23,
                              n30, n31, n32, n33);
  }



  //////////////////////////////////////////////////////////////

  // MATRIX GET/SET/PRINT


  public PMatrix getMatrix() {
    return modelview.get();
  }


  //public PMatrix2D getMatrix(PMatrix2D target)


  public PMatrix3D getMatrix(PMatrix3D target) {
    if (target == null) {
      target = new PMatrix3D();
    }
    target.set(modelview);
    return target;
  }


  //public void setMatrix(PMatrix source)


  public void setMatrix(PMatrix2D source) {
    // not efficient, but at least handles the inverse stuff.
    resetMatrix();
    applyMatrix(source);
  }


  /**
   * Set the current transformation to the contents of the specified source.
   */
  public void setMatrix(PMatrix3D source) {
    // not efficient, but at least handles the inverse stuff.
    resetMatrix();
    applyMatrix(source);
  }


  /**
   * Print the current model (or "transformation") matrix.
   */
  public void printMatrix() {
    modelview.print();
  }


  /*
   * This function checks if the modelview matrix is set up to likely be
   * drawing in 2D. It merely checks if the non-translational piece of the
   * matrix is unity. If this is to be used, it should be coupled with a
   * check that the raw vertex coordinates lie in the z=0 plane.
   * Mainly useful for applying sub-pixel shifts to avoid 2d artifacts
   * in the screen plane.
   * Added by ewjordan 6/13/07
   *
   * TODO need to invert the logic here so that we can simply return
   * the value, rather than calculating true/false and returning it.
   */
  /*
  private boolean drawing2D() {
    if (modelview.m00 != 1.0f ||
        modelview.m11 != 1.0f ||
        modelview.m22 != 1.0f || // check scale
        modelview.m01 != 0.0f ||
        modelview.m02 != 0.0f || // check rotational pieces
        modelview.m10 != 0.0f ||
        modelview.m12 != 0.0f ||
        modelview.m20 != 0.0f ||
        modelview.m21 != 0.0f ||
        !((camera.m23-modelview.m23) <= EPSILON &&
          (camera.m23-modelview.m23) >= -EPSILON)) { // check for z-translation
      // Something about the modelview matrix indicates 3d drawing
      // (or rotated 2d, in which case 2d subpixel fixes probably aren't needed)
      return false;
    } else {
      //The matrix is mapping z=0 vertices to the screen plane,
      // which means it's likely that 2D drawing is happening.
      return true;
    }
  }
  */



  //////////////////////////////////////////////////////////////

  // CAMERA


  /**
   * Set matrix mode to the camera matrix (instead of the current
   * transformation matrix). This means applyMatrix, resetMatrix, etc.
   * will affect the camera.
   * <P>
   * Note that the camera matrix is *not* the perspective matrix,
   * it is in front of the modelview matrix (hence the name "model"
   * and "view" for that matrix).
   * <P>
   * beginCamera() specifies that all coordinate transforms until endCamera()
   * should be pre-applied in inverse to the camera transform matrix.
   * Note that this is only challenging when a user specifies an arbitrary
   * matrix with applyMatrix(). Then that matrix will need to be inverted,
   * which may not be possible. But take heart, if a user is applying a
   * non-invertible matrix to the camera transform, then he is clearly
   * up to no good, and we can wash our hands of those bad intentions.
   * <P>
   * begin/endCamera clauses do not automatically reset the camera transform
   * matrix. That's because we set up a nice default camera transform int
   * setup(), and we expect it to hold through draw(). So we don't reset
   * the camera transform matrix at the top of draw(). That means that an
   * innocuous-looking clause like
   * <PRE>
   * beginCamera();
   * translate(0, 0, 10);
   * endCamera();
   * </PRE>
   * at the top of draw(), will result in a runaway camera that shoots
   * infinitely out of the screen over time. In order to prevent this,
   * it is necessary to call some function that does a hard reset of the
   * camera transform matrix inside of begin/endCamera. Two options are
   * <PRE>
   * camera(); // sets up the nice default camera transform
   * resetMatrix(); // sets up the identity camera transform
   * </PRE>
   * So to rotate a camera a constant amount, you might try
   * <PRE>
   * beginCamera();
   * camera();
   * rotateY(PI/8);
   * endCamera();
   * </PRE>
   */
  public void beginCamera() {
    if (manipulatingCamera) {
      throw new RuntimeException("beginCamera() cannot be called again " +
                                 "before endCamera()");
    } else {
      manipulatingCamera = true;
      forwardTransform = cameraInv;
      reverseTransform = camera;
    }
  }


  /**
   * Record the current settings into the camera matrix, and set
   * the matrix mode back to the current transformation matrix.
   * <P>
   * Note that this will destroy any settings to scale(), translate(),
   * or whatever, because the final camera matrix will be copied
   * (not multiplied) into the modelview.
   */
  public void endCamera() {
    if (!manipulatingCamera) {
      throw new RuntimeException("Cannot call endCamera() " +
                                 "without first calling beginCamera()");
    }
    // reset the modelview to use this new camera matrix
    modelview.set(camera);
    modelviewInv.set(cameraInv);

    // set matrix mode back to modelview
    forwardTransform = modelview;
    reverseTransform = modelviewInv;

    // all done
    manipulatingCamera = false;
  }


  /**
   * Set camera to the default settings.
   * <P>
   * Processing camera behavior:
   * <P>
   * Camera behavior can be split into two separate components, camera
   * transformation, and projection. The transformation corresponds to the
   * physical location, orientation, and scale of the camera. In a physical
   * camera metaphor, this is what can manipulated by handling the camera
   * body (with the exception of scale, which doesn't really have a physcial
   * analog). The projection corresponds to what can be changed by
   * manipulating the lens.
   * <P>
   * We maintain separate matrices to represent the camera transform and
   * projection. An important distinction between the two is that the camera
   * transform should be invertible, where the projection matrix should not,
   * since it serves to map three dimensions to two. It is possible to bake
   * the two matrices into a single one just by multiplying them together,
   * but it isn't a good idea, since lighting, z-ordering, and z-buffering
   * all demand a true camera z coordinate after modelview and camera
   * transforms have been applied but before projection. If the camera
   * transform and projection are combined there is no way to recover a
   * good camera-space z-coordinate from a model coordinate.
   * <P>
   * Fortunately, there are no functions that manipulate both camera
   * transformation and projection.
   * <P>
   * camera() sets the camera position, orientation, and center of the scene.
   * It replaces the camera transform with a new one. This is different from
   * gluLookAt(), but I think the only reason that GLU's lookat doesn't fully
   * replace the camera matrix with the new one, but instead multiplies it,
   * is that GL doesn't enforce the separation of camera transform and
   * projection, so it wouldn't be safe (you'd probably stomp your projection).
   * <P>
   * The transformation functions are the same ones used to manipulate the
   * modelview matrix (scale, translate, rotate, etc.). But they are bracketed
   * with beginCamera(), endCamera() to indicate that they should apply
   * (in inverse), to the camera transformation matrix.
   * <P>
   * This differs considerably from camera transformation in OpenGL.
   * OpenGL only lets you say, apply everything from here out to the
   * projection or modelview matrix. This makes it very hard to treat camera
   * manipulation as if it were a physical camera. Imagine that you want to
   * move your camera 100 units forward. In OpenGL, you need to apply the
   * inverse of that transformation or else you'll move your scene 100 units
   * forward--whether or not you've specified modelview or projection matrix.
   * Remember they're just multiplied by model coods one after another.
   * So in order to treat a camera like a physical camera, it is necessary
   * to pre-apply inverse transforms to a matrix that will be applied to model
   * coordinates. OpenGL provides nothing of this sort, but Processing does!
   * This is the camera transform matrix.
   */
  public void camera() {
    camera(cameraX, cameraY, cameraZ,
           cameraX, cameraY, 0,
           0, 1, 0);
  }


  /**
   * More flexible method for dealing with camera().
   * <P>
   * The actual call is like gluLookat. Here's the real skinny on
   * what does what:
   * <PRE>
   * camera(); or
   * camera(ex, ey, ez, cx, cy, cz, ux, uy, uz);
   * </PRE>
   * do not need to be called from with beginCamera();/endCamera();
   * That's because they always apply to the camera transformation,
   * and they always totally replace it. That means that any coordinate
   * transforms done before camera(); in draw() will be wiped out.
   * It also means that camera() always operates in untransformed world
   * coordinates. Therefore it is always redundant to call resetMatrix();
   * before camera(); This isn't technically true of gluLookat, but it's
   * pretty much how it's used.
   * <P>
   * Now, beginCamera(); and endCamera(); are useful if you want to move
   * the camera around using transforms like translate(), etc. They will
   * wipe out any coordinate system transforms that occur before them in
   * draw(), but they will not automatically wipe out the camera transform.
   * This means that they should be at the top of draw(). It also means
   * that the following:
   * <PRE>
   * beginCamera();
   * rotateY(PI/8);
   * endCamera();
   * </PRE>
   * will result in a camera that spins without stopping. If you want to
   * just rotate a small constant amount, try this:
   * <PRE>
   * beginCamera();
   * camera(); // sets up the default view
   * rotateY(PI/8);
   * endCamera();
   * </PRE>
   * That will rotate a little off of the default view. Note that this
   * is entirely equivalent to
   * <PRE>
   * camera(); // sets up the default view
   * beginCamera();
   * rotateY(PI/8);
   * endCamera();
   * </PRE>
   * because camera() doesn't care whether or not it's inside a
   * begin/end clause. Basically it's safe to use camera() or
   * camera(ex, ey, ez, cx, cy, cz, ux, uy, uz) as naked calls because
   * they do all the matrix resetting automatically.
   */
  public void camera(float eyeX, float eyeY, float eyeZ,
                     float centerX, float centerY, float centerZ,
                     float upX, float upY, float upZ) {
    float z0 = eyeX - centerX;
    float z1 = eyeY - centerY;
    float z2 = eyeZ - centerZ;
    float mag = sqrt(z0*z0 + z1*z1 + z2*z2);

    if (mag != 0) {
      z0 /= mag;
      z1 /= mag;
      z2 /= mag;
    }

    float y0 = upX;
    float y1 = upY;
    float y2 = upZ;

    float x0 =  y1*z2 - y2*z1;
    float x1 = -y0*z2 + y2*z0;
    float x2 =  y0*z1 - y1*z0;

    y0 =  z1*x2 - z2*x1;
    y1 = -z0*x2 + z2*x0;
    y2 =  z0*x1 - z1*x0;

    mag = sqrt(x0*x0 + x1*x1 + x2*x2);
    if (mag != 0) {
      x0 /= mag;
      x1 /= mag;
      x2 /= mag;
    }

    mag = sqrt(y0*y0 + y1*y1 + y2*y2);
    if (mag != 0) {
      y0 /= mag;
      y1 /= mag;
      y2 /= mag;
    }

    // just does an apply to the main matrix,
    // since that'll be copied out on endCamera
    camera.set(x0, x1, x2, 0,
               y0, y1, y2, 0,
               z0, z1, z2, 0,
               0,  0,  0,  1);
    camera.translate(-eyeX, -eyeY, -eyeZ);

    cameraInv.reset();
    cameraInv.invApply(x0, x1, x2, 0,
                       y0, y1, y2, 0,
                       z0, z1, z2, 0,
                       0,  0,  0,  1);
    cameraInv.translate(eyeX, eyeY, eyeZ);

    modelview.set(camera);
    modelviewInv.set(cameraInv);
  }


  /**
   * Print the current camera matrix.
   */
  public void printCamera() {
    camera.print();
  }


  //////////////////////////////////////////////////////////////

  // PROJECTION


  /**
   * Calls ortho() with the proper parameters for Processing's
   * standard orthographic projection.
   */
  public void ortho() {
    ortho(0, width, 0, height, -10, 10);
  }


  /**
   * Similar to gluOrtho(), but wipes out the current projection matrix.
   * <P>
   * Implementation partially based on Mesa's matrix.c.
   */
  public void ortho(float left, float right,
                    float bottom, float top,
                    float near, float far) {
    float x =  2.0f / (right - left);
    float y =  2.0f / (top - bottom);
    float z = -2.0f / (far - near);

    float tx = -(right + left) / (right - left);
    float ty = -(top + bottom) / (top - bottom);
    float tz = -(far + near) / (far - near);

    projection.set(x, 0, 0, tx,
                   0, y, 0, ty,
                   0, 0, z, tz,
                   0, 0, 0, 1);
    updateProjection();

    frustumMode = false;
  }


  /**
   * Calls perspective() with Processing's standard coordinate projection.
   * <P>
   * Projection functions:
   * <UL>
   * <LI>frustrum()
   * <LI>ortho()
   * <LI>perspective()
   * </UL>
   * Each of these three functions completely replaces the projection
   * matrix with a new one. They can be called inside setup(), and their
   * effects will be felt inside draw(). At the top of draw(), the projection
   * matrix is not reset. Therefore the last projection function to be
   * called always dominates. On resize, the default projection is always
   * established, which has perspective.
   * <P>
   * This behavior is pretty much familiar from OpenGL, except where
   * functions replace matrices, rather than multiplying against the
   * previous.
   * <P>
   */
  public void perspective() {
    perspective(cameraFOV, cameraAspect, cameraNear, cameraFar);
  }


  /**
   * Similar to gluPerspective(). Implementation based on Mesa's glu.c
   */
  public void perspective(float fov, float aspect, float zNear, float zFar) {
    //float ymax = zNear * tan(fovy * PI / 360.0f);
    float ymax = zNear * (float) Math.tan(fov / 2);
    float ymin = -ymax;

    float xmin = ymin * aspect;
    float xmax = ymax * aspect;

    frustum(xmin, xmax, ymin, ymax, zNear, zFar);
  }


  /**
   * Same as glFrustum(), except that it wipes out (rather than
   * multiplies against) the current perspective matrix.
   * <P>
   * Implementation based on the explanation in the OpenGL blue book.
   */
  public void frustum(float left, float right, float bottom,
                      float top, float znear, float zfar) {

    leftScreen = left;
    rightScreen = right;
    bottomScreen = bottom;
    topScreen = top;
    nearPlane = znear;
    frustumMode = true;

    //System.out.println(projection);
    projection.set((2*znear)/(right-left), 0, (right+left)/(right-left), 0,
                   0, (2*znear)/(top-bottom), (top+bottom)/(top-bottom), 0,
                   0, 0, -(zfar+znear)/(zfar-znear),-(2*zfar*znear)/(zfar-znear),
                   0, 0, -1, 0);
    updateProjection();
  }


  /** Called after the 'projection' PMatrix3D has changed. */
  protected void updateProjection() {
  }


  /**
   * Print the current projection matrix.
   */
  public void printProjection() {
    projection.print();
  }



  //////////////////////////////////////////////////////////////

  // SCREEN AND MODEL COORDS


  public float screenX(float x, float y) {
    return screenX(x, y, 0);
  }


  public float screenY(float x, float y) {
    return screenY(x, y, 0);
  }


  public float screenX(float x, float y, float z) {
    float ax =
      modelview.m00*x + modelview.m01*y + modelview.m02*z + modelview.m03;
    float ay =
      modelview.m10*x + modelview.m11*y + modelview.m12*z + modelview.m13;
    float az =
      modelview.m20*x + modelview.m21*y + modelview.m22*z + modelview.m23;
    float aw =
      modelview.m30*x + modelview.m31*y + modelview.m32*z + modelview.m33;

    float ox =
      projection.m00*ax + projection.m01*ay +
      projection.m02*az + projection.m03*aw;
    float ow =
      projection.m30*ax + projection.m31*ay +
      projection.m32*az + projection.m33*aw;

    if (ow != 0) ox /= ow;
    return width * (1 + ox) / 2.0f;
  }


  public float screenY(float x, float y, float z) {
    float ax =
      modelview.m00*x + modelview.m01*y + modelview.m02*z + modelview.m03;
    float ay =
      modelview.m10*x + modelview.m11*y + modelview.m12*z + modelview.m13;
    float az =
      modelview.m20*x + modelview.m21*y + modelview.m22*z + modelview.m23;
    float aw =
      modelview.m30*x + modelview.m31*y + modelview.m32*z + modelview.m33;

    float oy =
      projection.m10*ax + projection.m11*ay +
      projection.m12*az + projection.m13*aw;
    float ow =
      projection.m30*ax + projection.m31*ay +
      projection.m32*az + projection.m33*aw;

    if (ow != 0) oy /= ow;
    return height * (1 + oy) / 2.0f;
  }


  public float screenZ(float x, float y, float z) {
    float ax =
      modelview.m00*x + modelview.m01*y + modelview.m02*z + modelview.m03;
    float ay =
      modelview.m10*x + modelview.m11*y + modelview.m12*z + modelview.m13;
    float az =
      modelview.m20*x + modelview.m21*y + modelview.m22*z + modelview.m23;
    float aw =
      modelview.m30*x + modelview.m31*y + modelview.m32*z + modelview.m33;

    float oz =
      projection.m20*ax + projection.m21*ay +
      projection.m22*az + projection.m23*aw;
    float ow =
      projection.m30*ax + projection.m31*ay +
      projection.m32*az + projection.m33*aw;

    if (ow != 0) oz /= ow;
    return (oz + 1) / 2.0f;
  }


  public float modelX(float x, float y, float z) {
    float ax =
      modelview.m00*x + modelview.m01*y + modelview.m02*z + modelview.m03;
    float ay =
      modelview.m10*x + modelview.m11*y + modelview.m12*z + modelview.m13;
    float az =
      modelview.m20*x + modelview.m21*y + modelview.m22*z + modelview.m23;
    float aw =
      modelview.m30*x + modelview.m31*y + modelview.m32*z + modelview.m33;

    float ox =
      cameraInv.m00*ax + cameraInv.m01*ay +
      cameraInv.m02*az + cameraInv.m03*aw;
    float ow =
      cameraInv.m30*ax + cameraInv.m31*ay +
      cameraInv.m32*az + cameraInv.m33*aw;

    return (ow != 0) ? ox / ow : ox;
  }


  public float modelY(float x, float y, float z) {
    float ax =
      modelview.m00*x + modelview.m01*y + modelview.m02*z + modelview.m03;
    float ay =
      modelview.m10*x + modelview.m11*y + modelview.m12*z + modelview.m13;
    float az =
      modelview.m20*x + modelview.m21*y + modelview.m22*z + modelview.m23;
    float aw =
      modelview.m30*x + modelview.m31*y + modelview.m32*z + modelview.m33;

    float oy =
      cameraInv.m10*ax + cameraInv.m11*ay +
      cameraInv.m12*az + cameraInv.m13*aw;
    float ow =
      cameraInv.m30*ax + cameraInv.m31*ay +
      cameraInv.m32*az + cameraInv.m33*aw;

    return (ow != 0) ? oy / ow : oy;
  }


  public float modelZ(float x, float y, float z) {
    float ax =
      modelview.m00*x + modelview.m01*y + modelview.m02*z + modelview.m03;
    float ay =
      modelview.m10*x + modelview.m11*y + modelview.m12*z + modelview.m13;
    float az =
      modelview.m20*x + modelview.m21*y + modelview.m22*z + modelview.m23;
    float aw =
      modelview.m30*x + modelview.m31*y + modelview.m32*z + modelview.m33;

    float oz =
      cameraInv.m20*ax + cameraInv.m21*ay +
      cameraInv.m22*az + cameraInv.m23*aw;
    float ow =
      cameraInv.m30*ax + cameraInv.m31*ay +
      cameraInv.m32*az + cameraInv.m33*aw;

    return (ow != 0) ? oz / ow : oz;
  }



  //////////////////////////////////////////////////////////////

  // STYLE

  // pushStyle(), popStyle(), style() and getStyle() inherited.



  //////////////////////////////////////////////////////////////

  // STROKE CAP/JOIN/WEIGHT


//  public void strokeWeight(float weight) {
//    if (weight != DEFAULT_STROKE_WEIGHT) {
//      showMethodWarning("strokeWeight");
//    }
//  }


  public void strokeJoin(int join) {
    if (join != DEFAULT_STROKE_JOIN) {
      showMethodWarning("strokeJoin");
    }
  }


  public void strokeCap(int cap) {
    if (cap != DEFAULT_STROKE_CAP) {
      showMethodWarning("strokeCap");
    }
  }



  //////////////////////////////////////////////////////////////

  // STROKE COLOR

  // All methods inherited from PGraphics.



  //////////////////////////////////////////////////////////////

  // TINT COLOR

  // All methods inherited from PGraphics.



  //////////////////////////////////////////////////////////////

  // FILL COLOR


  protected void fillFromCalc() {
    super.fillFromCalc();
    ambientFromCalc();
  }



  //////////////////////////////////////////////////////////////

  // MATERIAL PROPERTIES

  // ambient, specular, shininess, and emissive all inherited.



  //////////////////////////////////////////////////////////////

  // LIGHTS


  PVector lightPositionVec = new PVector();
  PVector lightDirectionVec = new PVector();

  /**
   * Sets up an ambient and directional light.
   * <PRE>
   * The Lighting Skinny:
   *
   * The way lighting works is complicated enough that it's worth
   * producing a document to describe it. Lighting calculations proceed
   * pretty much exactly as described in the OpenGL red book.
   *
   * Light-affecting material properties:
   *
   *   AMBIENT COLOR
   *   - multiplies by light's ambient component
   *   - for believability this should match diffuse color
   *
   *   DIFFUSE COLOR
   *   - multiplies by light's diffuse component
   *
   *   SPECULAR COLOR
   *   - multiplies by light's specular component
   *   - usually less colored than diffuse/ambient
   *
   *   SHININESS
   *   - the concentration of specular effect
   *   - this should be set pretty high (20-50) to see really
   *     noticeable specularity
   *
   *   EMISSIVE COLOR
   *   - constant additive color effect
   *
   * Light types:
   *
   *   AMBIENT
   *   - one color
   *   - no specular color
   *   - no direction
   *   - may have falloff (constant, linear, and quadratic)
   *   - may have position (which matters in non-constant falloff case)
   *   - multiplies by a material's ambient reflection
   *
   *   DIRECTIONAL
   *   - has diffuse color
   *   - has specular color
   *   - has direction
   *   - no position
   *   - no falloff
   *   - multiplies by a material's diffuse and specular reflections
   *
   *   POINT
   *   - has diffuse color
   *   - has specular color
   *   - has position
   *   - no direction
   *   - may have falloff (constant, linear, and quadratic)
   *   - multiplies by a material's diffuse and specular reflections
   *
   *   SPOT
   *   - has diffuse color
   *   - has specular color
   *   - has position
   *   - has direction
   *   - has cone angle (set to half the total cone angle)
   *   - has concentration value
   *   - may have falloff (constant, linear, and quadratic)
   *   - multiplies by a material's diffuse and specular reflections
   *
   * Normal modes:
   *
   * All of the primitives (rect, box, sphere, etc.) have their normals
   * set nicely. During beginShape/endShape normals can be set by the user.
   *
   *   AUTO-NORMAL
   *   - if no normal is set during the shape, we are in auto-normal mode
   *   - auto-normal calculates one normal per triangle (face-normal mode)
   *
   *   SHAPE-NORMAL
   *   - if one normal is set during the shape, it will be used for
   *     all vertices
   *
   *   VERTEX-NORMAL
   *   - if multiple normals are set, each normal applies to
   *     subsequent vertices
   *   - (except for the first one, which applies to previous
   *     and subsequent vertices)
   *
   * Efficiency consequences:
   *
   *   There is a major efficiency consequence of position-dependent
   *   lighting calculations per vertex. (See below for determining
   *   whether lighting is vertex position-dependent.) If there is no
   *   position dependency then the only factors that affect the lighting
   *   contribution per vertex are its colors and its normal.
   *   There is a major efficiency win if
   *
   *   1) lighting is not position dependent
   *   2) we are in AUTO-NORMAL or SHAPE-NORMAL mode
   *
   *   because then we can calculate one lighting contribution per shape
   *   (SHAPE-NORMAL) or per triangle (AUTO-NORMAL) and simply multiply it
   *   into the vertex colors. The converse is our worst-case performance when
   *
   *   1) lighting is position dependent
   *   2) we are in AUTO-NORMAL mode
   *
   *   because then we must calculate lighting per-face * per-vertex.
   *   Each vertex has a different lighting contribution per face in
   *   which it appears. Yuck.
   *
   * Determining vertex position dependency:
   *
   *   If any of the following factors are TRUE then lighting is
   *   vertex position dependent:
   *
   *   1) Any lights uses non-constant falloff
   *   2) There are any point or spot lights
   *   3) There is a light with specular color AND there is a
   *      material with specular color
   *
   * So worth noting is that default lighting (a no-falloff ambient
   * and a directional without specularity) is not position-dependent.
   * We should capitalize.
   *
   * Simon Greenwold, April 2005
   * </PRE>
   */
  public void lights() {
    // need to make sure colorMode is RGB 255 here
    int colorModeSaved = colorMode;
    colorMode = RGB;

    lightFalloff(1, 0, 0);
    lightSpecular(0, 0, 0);

    ambientLight(colorModeX * 0.5f,
                 colorModeY * 0.5f,
                 colorModeZ * 0.5f);
    directionalLight(colorModeX * 0.5f,
                     colorModeY * 0.5f,
                     colorModeZ * 0.5f,
                     0, 0, -1);

    colorMode = colorModeSaved;

    lightingDependsOnVertexPosition = false;
  }


  /**
   * Turn off all lights.
   */
  public void noLights() {
    // write any queued geometry, because lighting will be goofed after
    flush();
    // set the light count back to zero
    lightCount = 0;
  }


  /**
   * Add an ambient light based on the current color mode.
   */
  public void ambientLight(float r, float g, float b) {
    ambientLight(r, g, b, 0, 0, 0);
  }


  /**
   * Add an ambient light based on the current color mode.
   * This version includes an (x, y, z) position for situations
   * where the falloff distance is used.
   */
  public void ambientLight(float r, float g, float b,
                           float x, float y, float z) {
    if (lightCount == MAX_LIGHTS) {
      throw new RuntimeException("can only create " + MAX_LIGHTS + " lights");
    }
    colorCalc(r, g, b);
    lightDiffuse[lightCount][0] = calcR;
    lightDiffuse[lightCount][1] = calcG;
    lightDiffuse[lightCount][2] = calcB;

    lightType[lightCount] = AMBIENT;
    lightFalloffConstant[lightCount] = currentLightFalloffConstant;
    lightFalloffLinear[lightCount] = currentLightFalloffLinear;
    lightFalloffQuadratic[lightCount] = currentLightFalloffQuadratic;
    lightPosition(lightCount, x, y, z);
    lightCount++;
    //return lightCount-1;
  }


  public void directionalLight(float r, float g, float b,
                               float nx, float ny, float nz) {
    if (lightCount == MAX_LIGHTS) {
      throw new RuntimeException("can only create " + MAX_LIGHTS + " lights");
    }
    colorCalc(r, g, b);
    lightDiffuse[lightCount][0] = calcR;
    lightDiffuse[lightCount][1] = calcG;
    lightDiffuse[lightCount][2] = calcB;

    lightType[lightCount] = DIRECTIONAL;
    lightFalloffConstant[lightCount] = currentLightFalloffConstant;
    lightFalloffLinear[lightCount] = currentLightFalloffLinear;
    lightFalloffQuadratic[lightCount] = currentLightFalloffQuadratic;
    lightSpecular[lightCount][0] = currentLightSpecular[0];
    lightSpecular[lightCount][1] = currentLightSpecular[1];
    lightSpecular[lightCount][2] = currentLightSpecular[2];
    lightDirection(lightCount, nx, ny, nz);
    lightCount++;
  }


  public void pointLight(float r, float g, float b,
                         float x, float y, float z) {
    if (lightCount == MAX_LIGHTS) {
      throw new RuntimeException("can only create " + MAX_LIGHTS + " lights");
    }
    colorCalc(r, g, b);
    lightDiffuse[lightCount][0] = calcR;
    lightDiffuse[lightCount][1] = calcG;
    lightDiffuse[lightCount][2] = calcB;

    lightType[lightCount] = POINT;
    lightFalloffConstant[lightCount] = currentLightFalloffConstant;
    lightFalloffLinear[lightCount] = currentLightFalloffLinear;
    lightFalloffQuadratic[lightCount] = currentLightFalloffQuadratic;
    lightSpecular[lightCount][0] = currentLightSpecular[0];
    lightSpecular[lightCount][1] = currentLightSpecular[1];
    lightSpecular[lightCount][2] = currentLightSpecular[2];
    lightPosition(lightCount, x, y, z);
    lightCount++;

    lightingDependsOnVertexPosition = true;
  }


  public void spotLight(float r, float g, float b,
                        float x, float y, float z,
                        float nx, float ny, float nz,
                        float angle, float concentration) {
    if (lightCount == MAX_LIGHTS) {
      throw new RuntimeException("can only create " + MAX_LIGHTS + " lights");
    }
    colorCalc(r, g, b);
    lightDiffuse[lightCount][0] = calcR;
    lightDiffuse[lightCount][1] = calcG;
    lightDiffuse[lightCount][2] = calcB;

    lightType[lightCount] = SPOT;
    lightFalloffConstant[lightCount] = currentLightFalloffConstant;
    lightFalloffLinear[lightCount] = currentLightFalloffLinear;
    lightFalloffQuadratic[lightCount] = currentLightFalloffQuadratic;
    lightSpecular[lightCount][0] = currentLightSpecular[0];
    lightSpecular[lightCount][1] = currentLightSpecular[1];
    lightSpecular[lightCount][2] = currentLightSpecular[2];
    lightPosition(lightCount, x, y, z);
    lightDirection(lightCount, nx, ny, nz);
    lightSpotAngle[lightCount] = angle;
    lightSpotAngleCos[lightCount] = Math.max(0, (float) Math.cos(angle));
    lightSpotConcentration[lightCount] = concentration;
    lightCount++;

    lightingDependsOnVertexPosition = true;
  }


  /**
   * Set the light falloff rates for the last light that was created.
   * Default is lightFalloff(1, 0, 0).
   */
  public void lightFalloff(float constant, float linear, float quadratic) {
    currentLightFalloffConstant = constant;
    currentLightFalloffLinear = linear;
    currentLightFalloffQuadratic = quadratic;

    lightingDependsOnVertexPosition = true;
  }


  /**
   * Set the specular color of the last light created.
   */
  public void lightSpecular(float x, float y, float z) {
    colorCalc(x, y, z);
    currentLightSpecular[0] = calcR;
    currentLightSpecular[1] = calcG;
    currentLightSpecular[2] = calcB;

    lightingDependsOnVertexPosition = true;
  }


  /**
   * internal function to set the light position
   * based on the current modelview matrix.
   */
  protected void lightPosition(int num, float x, float y, float z) {
    lightPositionVec.set(x, y, z);
    modelview.mult(lightPositionVec, lightPosition[num]);
    /*
    lightPosition[num][0] =
      modelview.m00*x + modelview.m01*y + modelview.m02*z + modelview.m03;
    lightPosition[num][1] =
      modelview.m10*x + modelview.m11*y + modelview.m12*z + modelview.m13;
    lightPosition[num][2] =
      modelview.m20*x + modelview.m21*y + modelview.m22*z + modelview.m23;
    */
  }


  /**
   * internal function to set the light direction
   * based on the current modelview matrix.
   */
  protected void lightDirection(int num, float x, float y, float z) {
    lightNormal[num].set(modelviewInv.m00*x + modelviewInv.m10*y + modelviewInv.m20*z + modelviewInv.m30,
                         modelviewInv.m01*x + modelviewInv.m11*y + modelviewInv.m21*z + modelviewInv.m31,
                         modelviewInv.m02*x + modelviewInv.m12*y + modelviewInv.m22*z + modelviewInv.m32);
    lightNormal[num].normalize();

    /*
    lightDirectionVec.set(x, y, z);
    System.out.println("dir vec " + lightDirectionVec);
    //modelviewInv.mult(lightDirectionVec, lightNormal[num]);
    modelviewInv.cmult(lightDirectionVec, lightNormal[num]);
    System.out.println("cmult vec " + lightNormal[num]);
    lightNormal[num].normalize();
    System.out.println("setting light direction " + lightNormal[num]);
    */

    /*
    // Multiply by inverse transpose.
    lightNormal[num][0] =
      modelviewInv.m00*x + modelviewInv.m10*y +
      modelviewInv.m20*z + modelviewInv.m30;
    lightNormal[num][1] =
      modelviewInv.m01*x + modelviewInv.m11*y +
      modelviewInv.m21*z + modelviewInv.m31;
    lightNormal[num][2] =
      modelviewInv.m02*x + modelviewInv.m12*y +
      modelviewInv.m22*z + modelviewInv.m32;

    float n = mag(lightNormal[num][0], lightNormal[num][1], lightNormal[num][2]);
    if (n == 0 || n == 1) return;

    lightNormal[num][0] /= n;
    lightNormal[num][1] /= n;
    lightNormal[num][2] /= n;
    */
  }



  //////////////////////////////////////////////////////////////

  // BACKGROUND

  // Base background() variations inherited from PGraphics.


  protected void backgroundImpl(PImage image) {
    System.arraycopy(image.pixels, 0, pixels, 0, pixels.length);
    Arrays.fill(zbuffer, Float.MAX_VALUE);
  }


  /**
   * Clear pixel buffer. With P3D and OPENGL, this also clears the zbuffer.
   */
  protected void backgroundImpl() {
    Arrays.fill(pixels, backgroundColor);
    Arrays.fill(zbuffer, Float.MAX_VALUE);
  }



  //////////////////////////////////////////////////////////////

  // COLOR MODE

  // all colorMode() variations inherited from PGraphics.



  //////////////////////////////////////////////////////////////

  // COLOR CALCULATIONS

  // protected colorCalc and colorCalcARGB inherited.



  //////////////////////////////////////////////////////////////

  // COLOR DATATYPE STUFFING

  // final color() variations inherited.



  //////////////////////////////////////////////////////////////

  // COLOR DATATYPE EXTRACTION

  // final methods alpha, red, green, blue,
  // hue, saturation, and brightness all inherited.



  //////////////////////////////////////////////////////////////

  // COLOR DATATYPE INTERPOLATION

  // both lerpColor variants inherited.



  //////////////////////////////////////////////////////////////

  // BEGIN/END RAW

  // beginRaw, endRaw() both inherited.



  //////////////////////////////////////////////////////////////

  // WARNINGS and EXCEPTIONS

  // showWarning and showException inherited.



  //////////////////////////////////////////////////////////////

  // RENDERER SUPPORT QUERIES


  //public boolean displayable()


  public boolean is2D() {
    return false;
  }


  public boolean is3D() {
    return true;
  }



  //////////////////////////////////////////////////////////////

  // PIMAGE METHODS

  // All these methods are inherited, because this render has a
  // pixels[] array that can be accessed directly.

  // getImage
  // setCache, getCache, removeCache
  // isModified, setModified
  // loadPixels, updatePixels
  // resize
  // get, getImpl, set, setImpl
  // mask
  // filter
  // copy
  // blendColor, blend



  //////////////////////////////////////////////////////////////

  // MATH (internal use only)


  private final float sqrt(float a) {
    return (float) Math.sqrt(a);
  }


  private final float mag(float a, float b, float c) {
    return (float) Math.sqrt(a*a + b*b + c*c);
  }


  private final float clamp(float a) {
    return (a < 1) ? a : 1;
  }


  private final float abs(float a) {
    return (a < 0) ? -a : a;
  }


  private float dot(float ax, float ay, float az,
                    float bx, float by, float bz) {
    return ax*bx + ay*by + az*bz;
  }


  /*
  private final void cross(float a0, float a1, float a2,
                           float b0, float b1, float b2,
                           float[] out) {
    out[0] = a1*b2 - a2*b1;
    out[1] = a2*b0 - a0*b2;
    out[2] = a0*b1 - a1*b0;
  }
  */


  private final void cross(float a0, float a1, float a2,
                           float b0, float b1, float b2,
                           PVector out) {
    out.x = a1*b2 - a2*b1;
    out.y = a2*b0 - a0*b2;
    out.z = a0*b1 - a1*b0;
  }


  /*
  private final void cross(float[] a, float[] b, float[] out) {
    out[0] = a[1]*b[2] - a[2]*b[1];
    out[1] = a[2]*b[0] - a[0]*b[2];
    out[2] = a[0]*b[1] - a[1]*b[0];
  }
  */
}

