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

import java.awt.Cursor;
import java.awt.event.KeyEvent;


/**
 * Numbers shared throughout processing.core.
 * <P>
 * An attempt is made to keep the constants as short/non-verbose
 * as possible. For instance, the constant is TIFF instead of
 * FILE_TYPE_TIFF. We'll do this as long as we can get away with it.
 * 
 * @usage Web &amp; Application
 */
public interface PConstants {

  static public final int X = 0;  // model coords xyz (formerly MX/MY/MZ)
  static public final int Y = 1;
  static public final int Z = 2;

  static public final int R = 3;  // actual rgb, after lighting
  static public final int G = 4;  // fill stored here, transform in place
  static public final int B = 5;  // TODO don't do that anymore (?)
  static public final int A = 6;

  static public final int U = 7; // texture
  static public final int V = 8;

  static public final int NX = 9; // normal
  static public final int NY = 10;
  static public final int NZ = 11;

  static public final int EDGE = 12;


  // stroke

  /** stroke argb values */
  static public final int SR = 13;
  static public final int SG = 14;
  static public final int SB = 15;
  static public final int SA = 16;

  /** stroke weight */
  static public final int SW = 17;


  // transformations (2D and 3D)

  static public final int TX = 18; // transformed xyzw
  static public final int TY = 19;
  static public final int TZ = 20;

  static public final int VX = 21; // view space coords
  static public final int VY = 22;
  static public final int VZ = 23;
  static public final int VW = 24;


  // material properties

  // Ambient color (usually to be kept the same as diffuse)
  // fill(_) sets both ambient and diffuse.
  static public final int AR = 25;
  static public final int AG = 26;
  static public final int AB = 27;

  // Diffuse is shared with fill.
  static public final int DR = 3;  // TODO needs to not be shared, this is a material property
  static public final int DG = 4;
  static public final int DB = 5;
  static public final int DA = 6;

  // specular (by default kept white)
  static public final int SPR = 28;
  static public final int SPG = 29;
  static public final int SPB = 30;

  static public final int SHINE = 31;

  // emissive (by default kept black)
  static public final int ER = 32;
  static public final int EG = 33;
  static public final int EB = 34;

  // has this vertex been lit yet
  static public final int BEEN_LIT = 35;

  static public final int VERTEX_FIELD_COUNT = 36;


  // renderers known to processing.core

  static final String P2D    = "processing.core.PGraphics2D";
  static final String P3D    = "processing.core.PGraphics3D";
  static final String JAVA2D = "processing.core.PGraphicsJava2D";
  static final String OPENGL = "processing.opengl.PGraphicsOpenGL";
  static final String PDF    = "processing.pdf.PGraphicsPDF";
  static final String DXF    = "processing.dxf.RawDXF";


  // platform IDs for PApplet.platform

  static final int OTHER   = 0;
  static final int WINDOWS = 1;
  static final int MACOSX  = 2;
  static final int LINUX   = 3;

  static final String[] platformNames = {
    "other", "windows", "macosx", "linux"
  };


  static final float EPSILON = 0.0001f;


  // max/min values for numbers

  /**
   * Same as Float.MAX_VALUE, but included for parity with MIN_VALUE,
   * and to avoid teaching static methods on the first day.
   */
  static final float MAX_FLOAT = Float.MAX_VALUE;
  /**
   * Note that Float.MIN_VALUE is the smallest <EM>positive</EM> value
   * for a floating point number, not actually the minimum (negative) value
   * for a float. This constant equals 0xFF7FFFFF, the smallest (farthest
   * negative) value a float can have before it hits NaN.
   */
  static final float MIN_FLOAT = -Float.MAX_VALUE;
  /** Largest possible (positive) integer value */
  static final int MAX_INT = Integer.MAX_VALUE;
  /** Smallest possible (negative) integer value */
  static final int MIN_INT = Integer.MIN_VALUE;


  // useful goodness
  
  /**
   * PI is a mathematical constant with the value 3.14159265358979323846.
   * It is the ratio of the circumference of a circle to its diameter.
   * It is useful in combination with the trigonometric functions <b>sin()</b> and <b>cos()</b>.
   * 
   * @webref constants
   * @see processing.core.PConstants#HALF_PI
   * @see processing.core.PConstants#TWO_PI
   * @see processing.core.PConstants#QUARTER_PI
   * 
   */
  static final float PI = (float) Math.PI;
  /**
   * HALF_PI is a mathematical constant with the value 1.57079632679489661923.
   * It is half the ratio of the circumference of a circle to its diameter.
   * It is useful in combination with the trigonometric functions <b>sin()</b> and <b>cos()</b>.
   * 
   * @webref constants
   * @see processing.core.PConstants#PI
   * @see processing.core.PConstants#TWO_PI
   * @see processing.core.PConstants#QUARTER_PI
   */
  static final float HALF_PI    = PI / 2.0f;
  static final float THIRD_PI   = PI / 3.0f;
  /**
   * QUARTER_PI is a mathematical constant with the value 0.7853982.
   * It is one quarter the ratio of the circumference of a circle to its diameter.
   * It is useful in combination with the trigonometric functions <b>sin()</b> and <b>cos()</b>.
   * 
   * @webref constants
   * @see processing.core.PConstants#PI
   * @see processing.core.PConstants#TWO_PI
   * @see processing.core.PConstants#HALF_PI
   */
  static final float QUARTER_PI = PI / 4.0f;
  /**
   * TWO_PI is a mathematical constant with the value 6.28318530717958647693.
   * It is twice the ratio of the circumference of a circle to its diameter.
   * It is useful in combination with the trigonometric functions <b>sin()</b> and <b>cos()</b>.
   * 
   * @webref constants
   * @see processing.core.PConstants#PI
   * @see processing.core.PConstants#HALF_PI
   * @see processing.core.PConstants#QUARTER_PI
   */
  static final float TWO_PI     = PI * 2.0f;

  static final float DEG_TO_RAD = PI/180.0f;
  static final float RAD_TO_DEG = 180.0f/PI;


  // angle modes

  //static final int RADIANS = 0;
  //static final int DEGREES = 1;


  // used by split, all the standard whitespace chars
  // (also includes unicode nbsp, that little bostage)

  static final String WHITESPACE = " \t\n\r\f\u00A0";


  // for colors and/or images

  static final int RGB   = 1;  // image & color
  static final int ARGB  = 2;  // image
  static final int HSB   = 3;  // color
  static final int ALPHA = 4;  // image
  static final int CMYK  = 5;  // image & color (someday)


  // image file types

  static final int TIFF  = 0;
  static final int TARGA = 1;
  static final int JPEG  = 2;
  static final int GIF   = 3;


  // filter/convert types

  static final int BLUR      = 11;
  static final int GRAY      = 12;
  static final int INVERT    = 13;
  static final int OPAQUE    = 14;
  static final int POSTERIZE = 15;
  static final int THRESHOLD = 16;
  static final int ERODE     = 17;
  static final int DILATE    = 18;


  // blend mode keyword definitions
  // @see processing.core.PImage#blendColor(int,int,int)

  public final static int REPLACE    = 0;
  public final static int BLEND      = 1 << 0;
  public final static int ADD        = 1 << 1;
  public final static int SUBTRACT   = 1 << 2;
  public final static int LIGHTEST   = 1 << 3;
  public final static int DARKEST    = 1 << 4;
  public final static int DIFFERENCE = 1 << 5;
  public final static int EXCLUSION  = 1 << 6;
  public final static int MULTIPLY   = 1 << 7;
  public final static int SCREEN     = 1 << 8;
  public final static int OVERLAY    = 1 << 9;
  public final static int HARD_LIGHT = 1 << 10;
  public final static int SOFT_LIGHT = 1 << 11;
  public final static int DODGE      = 1 << 12;
  public final static int BURN       = 1 << 13;

  // colour component bitmasks

  public static final int ALPHA_MASK = 0xff000000;
  public static final int RED_MASK   = 0x00ff0000;
  public static final int GREEN_MASK = 0x0000ff00;
  public static final int BLUE_MASK  = 0x000000ff;


  // for messages

  static final int CHATTER   = 0;
  static final int COMPLAINT = 1;
  static final int PROBLEM   = 2;


  // types of projection matrices

  static final int CUSTOM       = 0; // user-specified fanciness
  static final int ORTHOGRAPHIC = 2; // 2D isometric projection
  static final int PERSPECTIVE  = 3; // perspective matrix


  // shapes

  // the low four bits set the variety,
  // higher bits set the specific shape type

  //static final int GROUP           = (1 << 2);

  static final int POINT           = 2;  // shared with light (!)
  static final int POINTS          = 2;

  static final int LINE            = 4;
  static final int LINES           = 4;

  static final int TRIANGLE        = 8;
  static final int TRIANGLES       = 9;
  static final int TRIANGLE_STRIP  = 10;
  static final int TRIANGLE_FAN    = 11;

  static final int QUAD            = 16;
  static final int QUADS           = 16;
  static final int QUAD_STRIP      = 17;

  static final int POLYGON         = 20;
  static final int PATH            = 21;

  static final int RECT            = 30;
  static final int ELLIPSE         = 31;
  static final int ARC             = 32;

  static final int SPHERE          = 40;
  static final int BOX             = 41;


  // shape closing modes

  static final int OPEN = 1;
  static final int CLOSE = 2;


  // shape drawing modes

  /** Draw mode convention to use (x, y) to (width, height) */
  static final int CORNER   = 0;
  /** Draw mode convention to use (x1, y1) to (x2, y2) coordinates */
  static final int CORNERS  = 1;
  /** Draw mode from the center, and using the radius */
  static final int RADIUS   = 2;
  /** @deprecated Use RADIUS instead. */
  static final int CENTER_RADIUS = 2;
  /**
   * Draw from the center, using second pair of values as the diameter.
   * Formerly called CENTER_DIAMETER in alpha releases.
   */
  static final int CENTER   = 3;
  /**
   * Synonym for the CENTER constant. Draw from the center,
   * using second pair of values as the diameter.
   */
  static final int DIAMETER = 3;
  /** @deprecated Use DIAMETER instead. */
  static final int CENTER_DIAMETER = 3;


  // vertically alignment modes for text

  /** Default vertical alignment for text placement */
  static final int BASELINE = 0;
  /** Align text to the top */
  static final int TOP = 101;
  /** Align text from the bottom, using the baseline. */
  static final int BOTTOM = 102;


  // uv texture orientation modes

  /** texture coordinates in 0..1 range */
  static final int NORMAL     = 1;
  /** @deprecated use NORMAL instead */
  static final int NORMALIZED = 1;
  /** texture coordinates based on image width/height */
  static final int IMAGE      = 2;


  // text placement modes

  /**
   * textMode(MODEL) is the default, meaning that characters
   * will be affected by transformations like any other shapes.
   * <p/>
   * Changed value in 0093 to not interfere with LEFT, CENTER, and RIGHT.
   */
  static final int MODEL = 4;

  /**
   * textMode(SHAPE) draws text using the the glyph outlines of
   * individual characters rather than as textures. If the outlines are
   * not available, then textMode(SHAPE) will be ignored and textMode(MODEL)
   * will be used instead. For this reason, be sure to call textMode()
   * <EM>after</EM> calling textFont().
   * <p/>
   * Currently, textMode(SHAPE) is only supported by OPENGL mode.
   * It also requires Java 1.2 or higher (OPENGL requires 1.4 anyway)
   */
  static final int SHAPE = 5;


  // text alignment modes
  // are inherited from LEFT, CENTER, RIGHT


  // stroke modes

  static final int SQUARE   = 1 << 0;  // called 'butt' in the svg spec
  static final int ROUND    = 1 << 1;
  static final int PROJECT  = 1 << 2;  // called 'square' in the svg spec
  static final int MITER    = 1 << 3;
  static final int BEVEL    = 1 << 5;


  // lighting

  static final int AMBIENT = 0;
  static final int DIRECTIONAL  = 1;
  //static final int POINT  = 2;  // shared with shape feature
  static final int SPOT = 3;


  // key constants

  // only including the most-used of these guys
  // if people need more esoteric keys, they can learn about
  // the esoteric java KeyEvent api and of virtual keys

  // both key and keyCode will equal these values
  // for 0125, these were changed to 'char' values, because they
  // can be upgraded to ints automatically by Java, but having them
  // as ints prevented split(blah, TAB) from working
  static final char BACKSPACE = 8;
  static final char TAB       = 9;
  static final char ENTER     = 10;
  static final char RETURN    = 13;
  static final char ESC       = 27;
  static final char DELETE    = 127;

  // i.e. if ((key == CODED) && (keyCode == UP))
  static final int CODED     = 0xffff;

  // key will be CODED and keyCode will be this value
  static final int UP        = KeyEvent.VK_UP;
  static final int DOWN      = KeyEvent.VK_DOWN;
  static final int LEFT      = KeyEvent.VK_LEFT;
  static final int RIGHT     = KeyEvent.VK_RIGHT;

  // key will be CODED and keyCode will be this value
  static final int ALT       = KeyEvent.VK_ALT;
  static final int CONTROL   = KeyEvent.VK_CONTROL;
  static final int SHIFT     = KeyEvent.VK_SHIFT;


  // cursor types

  static final int ARROW = Cursor.DEFAULT_CURSOR;
  static final int CROSS = Cursor.CROSSHAIR_CURSOR;
  static final int HAND  = Cursor.HAND_CURSOR;
  static final int MOVE  = Cursor.MOVE_CURSOR;
  static final int TEXT  = Cursor.TEXT_CURSOR;
  static final int WAIT  = Cursor.WAIT_CURSOR;


  // hints - hint values are positive for the alternate version,
  // negative of the same value returns to the normal/default state

  static final int DISABLE_OPENGL_2X_SMOOTH    =  1;
  static final int ENABLE_OPENGL_2X_SMOOTH     = -1;
  static final int ENABLE_OPENGL_4X_SMOOTH     =  2;

  static final int ENABLE_NATIVE_FONTS         =  3;

  static final int DISABLE_DEPTH_TEST          =  4;
  static final int ENABLE_DEPTH_TEST           = -4;

  static final int ENABLE_DEPTH_SORT           =  5;
  static final int DISABLE_DEPTH_SORT          = -5;

  static final int DISABLE_OPENGL_ERROR_REPORT =  6;
  static final int ENABLE_OPENGL_ERROR_REPORT  = -6;

  static final int ENABLE_ACCURATE_TEXTURES    =  7;
  static final int DISABLE_ACCURATE_TEXTURES   = -7;

  static final int HINT_COUNT                  = 10;


  // error messages

  static final String ERROR_BACKGROUND_IMAGE_SIZE =
    "background image must be the same size as your application";
  static final String ERROR_BACKGROUND_IMAGE_FORMAT =
    "background images should be RGB or ARGB";

  static final String ERROR_TEXTFONT_NULL_PFONT =
    "A null PFont was passed to textFont()";

  static final String ERROR_PUSHMATRIX_OVERFLOW =
    "Too many calls to pushMatrix().";
  static final String ERROR_PUSHMATRIX_UNDERFLOW =
    "Too many calls to popMatrix(), and not enough to pushMatrix().";
}
