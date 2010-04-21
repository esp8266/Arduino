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

import java.awt.image.*;
import java.io.*;
import java.util.HashMap;

import javax.imageio.ImageIO;




/**
 * Datatype for storing images. Processing can display <b>.gif</b>, <b>.jpg</b>, <b>.tga</b>, and <b>.png</b> images. Images may be displayed in 2D and 3D space.
 * Before an image is used, it must be loaded with the <b>loadImage()</b> function.
 * The <b>PImage</b> object contains fields for the <b>width</b> and <b>height</b> of the image,
 * as well as an array called <b>pixels[]</b> which contains the values for every pixel in the image.
 * A group of methods, described below, allow easy access to the image's pixels and alpha channel and simplify the process of compositing. 
 * <br><br>Before using the <b>pixels[]</b> array, be sure to use the <b>loadPixels()</b> method on the image to make sure that the pixel data is properly loaded.
 * <br><br>To create a new image, use the <b>createImage()</b> function (do not use <b>new PImage()</b>).
 * =advanced
 * 
 * Storage class for pixel data. This is the base class for most image and
 * pixel information, such as PGraphics and the video library classes.
 * <P>
 * Code for copying, resizing, scaling, and blending contributed
 * by <A HREF="http://www.toxi.co.uk">toxi</A>.
 * <P>
 * 
 * @webref image
 * @usage Web &amp; Application
 * @instanceName img any variable of type PImage
 * @see processing.core.PApplet#loadImage(String)
 * @see processing.core.PApplet#imageMode(int)
 * @see processing.core.PApplet#createImage(int, int)
 */
public class PImage implements PConstants, Cloneable {

  /**
   * Format for this image, one of RGB, ARGB or ALPHA.
   * note that RGB images still require 0xff in the high byte
   * because of how they'll be manipulated by other functions
   */
  public int format;

  /**
   * Array containing the values for all the pixels in the image. These values are of the color datatype.
   * This array is the size of the image, meaning if the image is 100x100 pixels, there will be 10000 values
   * and if the window is 200x300 pixels, there will be 60000 values.
   * The <b>index</b> value defines the position of a value within the array.
   * For example, the statement <b>color b = img.pixels[230]</b> will set the variable <b>b</b> equal to the value at that location in the array.
   * Before accessing this array, the data must loaded with the <b>loadPixels()</b> method.
   * After the array data has been modified, the <b>updatePixels()</b> method must be run to update the changes.
   * Without <b>loadPixels()</b>, running the code may (or will in future releases) result in a NullPointerException.
   * @webref
   * @brief  	Array containing the color of every pixel in the image
   */
  public int[] pixels;
  
  /**
   * The width of the image in units of pixels.
   * @webref
   * @brief  	Image width
   */
  public int width;
  /**
   * The height of the image in units of pixels.
   * @webref
   * @brief  	Image height
   */
  public int height;

  /**
   * Path to parent object that will be used with save().
   * This prevents users from needing savePath() to use PImage.save().
   */
  public PApplet parent;


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /** for subclasses that need to store info about the image */
  protected HashMap<Object,Object> cacheMap;


  /** modified portion of the image */
  protected boolean modified;
  protected int mx1, my1, mx2, my2;


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  // private fields
  private int fracU, ifU, fracV, ifV, u1, u2, v1, v2, sX, sY, iw, iw1, ih1;
  private int ul, ll, ur, lr, cUL, cLL, cUR, cLR;
  private int srcXOffset, srcYOffset;
  private int r, g, b, a;
  private int[] srcBuffer;

  // fixed point precision is limited to 15 bits!!
  static final int PRECISIONB = 15;
  static final int PRECISIONF = 1 << PRECISIONB;
  static final int PREC_MAXVAL = PRECISIONF-1;
  static final int PREC_ALPHA_SHIFT = 24-PRECISIONB;
  static final int PREC_RED_SHIFT = 16-PRECISIONB;

  // internal kernel stuff for the gaussian blur filter
  private int blurRadius;
  private int blurKernelSize;
  private int[] blurKernel;
  private int[][] blurMult;


  //////////////////////////////////////////////////////////////


  /**
   * Create an empty image object, set its format to RGB.
   * The pixel array is not allocated.
   */
  public PImage() {
    format = ARGB;  // default to ARGB images for release 0116
//    cache = null;
  }


  /**
   * Create a new RGB (alpha ignored) image of a specific size.
   * All pixels are set to zero, meaning black, but since the
   * alpha is zero, it will be transparent.
   */
  public PImage(int width, int height) {
    init(width, height, RGB);

    // toxi: is it maybe better to init the image with max alpha enabled?
    //for(int i=0; i<pixels.length; i++) pixels[i]=0xffffffff;
    // fry: i'm opting for the full transparent image, which is how
    // photoshop works, and our audience oughta be familiar with.
    // also, i want to avoid having to set all those pixels since
    // in java it's super slow, and most using this fxn will be
    // setting all the pixels anyway.
    // toxi: agreed and same reasons why i left it out ;)
  }

  /**
   * 
   * @param width image width
   * @param height image height
   * @param format Either RGB, ARGB, ALPHA (grayscale alpha channel)
   */
  public PImage(int width, int height, int format) {
    init(width, height, format);
  }


  /**
   * Function to be used by subclasses of PImage to init later than
   * at the constructor, or re-init later when things changes.
   * Used by Capture and Movie classes (and perhaps others),
   * because the width/height will not be known when super() is called.
   * (Leave this public so that other libraries can do the same.)
   */
  public void init(int width, int height, int format) {  // ignore
    this.width = width;
    this.height = height;
    this.pixels = new int[width*height];
    this.format = format;
//    this.cache = null;
  }


  /**
   * Check the alpha on an image, using a really primitive loop.
   */
  protected void checkAlpha() {
    if (pixels == null) return;

    for (int i = 0; i < pixels.length; i++) {
      // since transparency is often at corners, hopefully this
      // will find a non-transparent pixel quickly and exit
      if ((pixels[i] & 0xff000000) != 0xff000000) {
        format = ARGB;
        break;
      }
    }
  }


  //////////////////////////////////////////////////////////////


  /**
   * Construct a new PImage from a java.awt.Image. This constructor assumes
   * that you've done the work of making sure a MediaTracker has been used
   * to fully download the data and that the img is valid.
   * 
   * @param img assumes a MediaTracker has been used to fully download the data and the img is valid
   */
  public PImage(java.awt.Image img) {
    if (img instanceof BufferedImage) {
      BufferedImage bi = (BufferedImage) img;
      width = bi.getWidth();
      height = bi.getHeight();
      pixels = new int[width * height];
      WritableRaster raster = bi.getRaster();
      raster.getDataElements(0, 0, width, height, pixels);

    } else {  // go the old school java 1.0 route
//        System.out.println(img.getClass().getName());
      width = img.getWidth(null);
      height = img.getHeight(null);
      pixels = new int[width * height];
      PixelGrabber pg =
        new PixelGrabber(img, 0, 0, width, height, pixels, 0, width);
      try {
        pg.grabPixels();
      } catch (InterruptedException e) { }
    }

    format = RGB;
//    cache = null;
  }


  /**
   * Returns a BufferedImage from this PImage.
   */
  public java.awt.Image getImage() {
    loadPixels();
    int type = (format == RGB) ?
      BufferedImage.TYPE_INT_RGB : BufferedImage.TYPE_INT_ARGB;
    BufferedImage image = new BufferedImage(width, height, type);
    WritableRaster wr = image.getRaster();
    wr.setDataElements(0, 0, width, height, pixels);
    return image;
  }


  //////////////////////////////////////////////////////////////


  /**
   * Store data of some kind for a renderer that requires extra metadata of
   * some kind. Usually this is a renderer-specific representation of the
   * image data, for instance a BufferedImage with tint() settings applied for
   * PGraphicsJava2D, or resized image data and OpenGL texture indices for
   * PGraphicsOpenGL.
   */
  public void setCache(Object parent, Object storage) {
    if (cacheMap == null) cacheMap = new HashMap<Object, Object>();
    cacheMap.put(parent, storage);
  }


  /**
   * Get cache storage data for the specified renderer. Because each renderer
   * will cache data in different formats, it's necessary to store cache data
   * keyed by the renderer object. Otherwise, attempting to draw the same
   * image to both a PGraphicsJava2D and a PGraphicsOpenGL will cause errors.
   * @param parent The PGraphics object (or any object, really) associated
   * @return data stored for the specified parent
   */
  public Object getCache(Object parent) {
    if (cacheMap == null) return null;
    return cacheMap.get(parent);
  }


  /**
   * Remove information associated with this renderer from the cache, if any.
   * @param parent The PGraphics object whose cache data should be removed
   */
  public void removeCache(Object parent) {
    if (cacheMap != null) {
      cacheMap.remove(parent);
    }
  }



  //////////////////////////////////////////////////////////////

  // MARKING IMAGE AS MODIFIED / FOR USE w/ GET/SET


  public boolean isModified() {  // ignore
    return modified;
  }


  public void setModified() {  // ignore
    modified = true;
  }


  public void setModified(boolean m) {  // ignore
    modified = m;
  }


  /**
   * Loads the pixel data for the image into its <b>pixels[]</b> array. This function must always be called before reading from or writing to <b>pixels[]</b>.
   * <br><br>Certain renderers may or may not seem to require <b>loadPixels()</b> or <b>updatePixels()</b>. However, the rule is that any time you want to manipulate the <b>pixels[]</b> array, you must first call <b>loadPixels()</b>, and after changes have been made, call <b>updatePixels()</b>. Even if the renderer may not seem to use this function in the current Processing release, this will always be subject to change.
   * =advanced
   * Call this when you want to mess with the pixels[] array.
   * <p/>
   * For subclasses where the pixels[] buffer isn't set by default,
   * this should copy all data into the pixels[] array
   * 
   * @webref
   * @brief Loads the pixel data for the image into its pixels[] array
   */
  public void loadPixels() {  // ignore
  }

  public void updatePixels() {  // ignore
    updatePixelsImpl(0, 0, width, height);
  }

  /**
   * Updates the image with the data in its <b>pixels[]</b> array. Use in conjunction with <b>loadPixels()</b>. If you're only reading pixels from the array, there's no need to call <b>updatePixels()</b>.
   * <br><br>Certain renderers may or may not seem to require <b>loadPixels()</b> or <b>updatePixels()</b>. However, the rule is that any time you want to manipulate the <b>pixels[]</b> array, you must first call <b>loadPixels()</b>, and after changes have been made, call <b>updatePixels()</b>. Even if the renderer may not seem to use this function in the current Processing release, this will always be subject to change.
   * <br><br>Currently, none of the renderers use the additional parameters to <b>updatePixels()</b>, however this may be implemented in the future.
   * =advanced
   * Mark the pixels in this region as needing an update.
   * This is not currently used by any of the renderers, however the api
   * is structured this way in the hope of being able to use this to
   * speed things up in the future.
   * @webref
   * @brief Updates the image with the data in its pixels[] array
   * @param x
   * @param y
   * @param w
   * @param h
   */
  public void updatePixels(int x, int y, int w, int h) {  // ignore
//    if (imageMode == CORNER) {  // x2, y2 are w/h
//      x2 += x1;
//      y2 += y1;
//
//    } else if (imageMode == CENTER) {
//      x1 -= x2 / 2;
//      y1 -= y2 / 2;
//      x2 += x1;
//      y2 += y1;
//    }
    updatePixelsImpl(x, y, w, h);
  }


  protected void updatePixelsImpl(int x, int y, int w, int h) {
    int x2 = x + w;
    int y2 = y + h;

    if (!modified) {
      mx1 = x;
      mx2 = x2;
      my1 = y;
      my2 = y2;
      modified = true;

    } else {
      if (x < mx1) mx1 = x;
      if (x > mx2) mx2 = x;
      if (y < my1) my1 = y;
      if (y > my2) my2 = y;

      if (x2 < mx1) mx1 = x2;
      if (x2 > mx2) mx2 = x2;
      if (y2 < my1) my1 = y2;
      if (y2 > my2) my2 = y2;
    }
  }



  //////////////////////////////////////////////////////////////

  // COPYING IMAGE DATA


  /**
   * Duplicate an image, returns new PImage object.
   * The pixels[] array for the new object will be unique
   * and recopied from the source image. This is implemented as an
   * override of Object.clone(). We recommend using get() instead,
   * because it prevents you from needing to catch the
   * CloneNotSupportedException, and from doing a cast from the result.
   */
  public Object clone() throws CloneNotSupportedException {  // ignore
    PImage c = (PImage) super.clone();

    // super.clone() will only copy the reference to the pixels
    // array, so this will do a proper duplication of it instead.
    c.pixels = new int[width * height];
    System.arraycopy(pixels, 0, c.pixels, 0, pixels.length);

    // return the goods
    return c;
  }


  /**
   * Resize the image to a new width and height. To make the image scale proportionally, use 0 as the value for the <b>wide</b> or <b>high</b> parameter.
   * 
   * @webref
   * @brief Changes the size of an image to a new width and height
   * @param wide the resized image width
   * @param high the resized image height
   * 
   * @see processing.core.PImage#get(int, int, int, int)
   */
  public void resize(int wide, int high) {  // ignore
    // Make sure that the pixels[] array is valid
    loadPixels();

    if (wide <= 0 && high <= 0) {
      width = 0;  // Gimme a break, don't waste my time
      height = 0;
      pixels = new int[0];

    } else {
      if (wide == 0) {  // Use height to determine relative size
        float diff = (float) high / (float) height;
        wide = (int) (width * diff);
      } else if (high == 0) {  // Use the width to determine relative size
        float diff = (float) wide / (float) width;
        high = (int) (height * diff);
      }
      PImage temp = new PImage(wide, high, this.format);
      temp.copy(this, 0, 0, width, height, 0, 0, wide, high);
      this.width = wide;
      this.height = high;
      this.pixels = temp.pixels;
    }
    // Mark the pixels array as altered
    updatePixels();
  }



  //////////////////////////////////////////////////////////////

  // GET/SET PIXELS


  /**
   * Returns an ARGB "color" type (a packed 32 bit int with the color.
   * If the coordinate is outside the image, zero is returned
   * (black, but completely transparent).
   * <P>
   * If the image is in RGB format (i.e. on a PVideo object),
   * the value will get its high bits set, just to avoid cases where
   * they haven't been set already.
   * <P>
   * If the image is in ALPHA format, this returns a white with its
   * alpha value set.
   * <P>
   * This function is included primarily for beginners. It is quite
   * slow because it has to check to see if the x, y that was provided
   * is inside the bounds, and then has to check to see what image
   * type it is. If you want things to be more efficient, access the
   * pixels[] array directly.
   */
  public int get(int x, int y) {
    if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) return 0;

    switch (format) {
      case RGB:
        return pixels[y*width + x] | 0xff000000;

      case ARGB:
        return pixels[y*width + x];

      case ALPHA:
        return (pixels[y*width + x] << 24) | 0xffffff;
    }
    return 0;
  }


  /**
   * Reads the color of any pixel or grabs a group of pixels. If no parameters are specified, the entire image is returned. Get the value of one pixel by specifying an x,y coordinate. Get a section of the display window by specifing an additional <b>width</b> and <b>height</b> parameter. If the pixel requested is outside of the image window, black is returned. The numbers returned are scaled according to the current color ranges, but only RGB values are returned by this function. Even though you may have drawn a shape with <b>colorMode(HSB)</b>, the numbers returned will be in RGB.
   * <br><br>Getting the color of a single pixel with <b>get(x, y)</b> is easy, but not as fast as grabbing the data directly from <b>pixels[]</b>. The equivalent statement to "get(x, y)" using <b>pixels[]</b> is "pixels[y*width+x]". Processing requires calling <b>loadPixels()</b> to load the display window data into the <b>pixels[]</b> array before getting the values.
   * <br><br>As of release 0149, this function ignores <b>imageMode()</b>.
   * 
   * @webref
   * @brief  	Reads the color of any pixel or grabs a rectangle of pixels
   * @param x x-coordinate of the pixel
   * @param y y-coordinate of the pixel
   * @param w width of pixel rectangle to get
   * @param h height of pixel rectangle to get
   * 
   * @see processing.core.PImage#set(int, int, int)
   * @see processing.core.PImage#pixels
   * @see processing.core.PImage#copy(PImage, int, int, int, int, int, int, int, int)
   */
  public PImage get(int x, int y, int w, int h) {
    /*
    if (imageMode == CORNERS) {  // if CORNER, do nothing
      //x2 += x1; y2 += y1;
      // w/h are x2/y2 in this case, bring em down to size
      w = (w - x);
      h = (h - y);
    } else if (imageMode == CENTER) {
      x -= w/2;
      y -= h/2;
    }
    */

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

    return getImpl(x, y, w, h);
  }


  /**
   * Internal function to actually handle getting a block of pixels that
   * has already been properly cropped to a valid region. That is, x/y/w/h
   * are guaranteed to be inside the image space, so the implementation can
   * use the fastest possible pixel copying method.
   */
  protected PImage getImpl(int x, int y, int w, int h) {
    PImage newbie = new PImage(w, h, format);
    newbie.parent = parent;

    int index = y*width + x;
    int index2 = 0;
    for (int row = y; row < y+h; row++) {
      System.arraycopy(pixels, index, newbie.pixels, index2, w);
      index += width;
      index2 += w;
    }
    return newbie;
  }


  /**
   * Returns a copy of this PImage. Equivalent to get(0, 0, width, height).
   */
  public PImage get() {
    try {
      PImage clone = (PImage) clone();
      // don't want to pass this down to the others
      // http://dev.processing.org/bugs/show_bug.cgi?id=1245
      clone.cacheMap = null;
      return clone;
    } catch (CloneNotSupportedException e) {
      return null;
    }
  }

  /**
   * Changes the color of any pixel or writes an image directly into the image. The <b>x</b> and <b>y</b> parameter specify the pixel or the upper-left corner of the image. The <b>color</b> parameter specifies the color value.<br><br>Setting the color of a single pixel with <b>set(x, y)</b> is easy, but not as fast as putting the data directly into <b>pixels[]</b>. The equivalent statement to "set(x, y, #000000)" using <b>pixels[]</b> is "pixels[y*width+x] = #000000". Processing requires calling <b>loadPixels()</b> to load the display window data into the <b>pixels[]</b> array before getting the values and calling <b>updatePixels()</b> to update the window.
   * <br><br>As of release 0149, this function ignores <b>imageMode()</b>.
   * 
   * @webref
   * @brief  	Writes a color to any pixel or writes an image into another
   * @param x x-coordinate of the pixel or upper-left corner of the image
   * @param y y-coordinate of the pixel or upper-left corner of the image
   * @param c any value of the color datatype
   * 
   * @see processing.core.PImage#get(int, int, int, int)
   * @see processing.core.PImage#pixels
   * @see processing.core.PImage#copy(PImage, int, int, int, int, int, int, int, int)
   */
  public void set(int x, int y, int c) {
    if ((x < 0) || (y < 0) || (x >= width) || (y >= height)) return;
    pixels[y*width + x] = c;
    updatePixelsImpl(x, y, x+1, y+1);  // slow?
  }


  /**
   * Efficient method of drawing an image's pixels directly to this surface.
   * No variations are employed, meaning that any scale, tint, or imageMode
   * settings will be ignored.
   */
  public void set(int x, int y, PImage src) {
    int sx = 0;
    int sy = 0;
    int sw = src.width;
    int sh = src.height;

//    if (imageMode == CENTER) {
//      x -= src.width/2;
//      y -= src.height/2;
//    }
    if (x < 0) {  // off left edge
      sx -= x;
      sw += x;
      x = 0;
    }
    if (y < 0) {  // off top edge
      sy -= y;
      sh += y;
      y = 0;
    }
    if (x + sw > width) {  // off right edge
      sw = width - x;
    }
    if (y + sh > height) {  // off bottom edge
      sh = height - y;
    }

    // this could be nonexistant
    if ((sw <= 0) || (sh <= 0)) return;

    setImpl(x, y, sx, sy, sw, sh, src);
  }


  /**
   * Internal function to actually handle setting a block of pixels that
   * has already been properly cropped from the image to a valid region.
   */
  protected void setImpl(int dx, int dy, int sx, int sy, int sw, int sh,
                         PImage src) {
    int srcOffset = sy * src.width + sx;
    int dstOffset = dy * width + dx;

    for (int y = sy; y < sy + sh; y++) {
      System.arraycopy(src.pixels, srcOffset, pixels, dstOffset, sw);
      srcOffset += src.width;
      dstOffset += width;
    }
    updatePixelsImpl(sx, sy, sx+sw, sy+sh);
  }



  //////////////////////////////////////////////////////////////

  // ALPHA CHANNEL


  /**
   * Set alpha channel for an image. Black colors in the source
   * image will make the destination image completely transparent,
   * and white will make things fully opaque. Gray values will
   * be in-between steps.
   * <P>
   * Strictly speaking the "blue" value from the source image is
   * used as the alpha color. For a fully grayscale image, this
   * is correct, but for a color image it's not 100% accurate.
   * For a more accurate conversion, first use filter(GRAY)
   * which will make the image into a "correct" grayscale by
   * performing a proper luminance-based conversion.
   * 
   * @param maskArray any array of Integer numbers used as the alpha channel, needs to be same length as the image's pixel array
   */
  public void mask(int maskArray[]) {
    loadPixels();
    // don't execute if mask image is different size
    if (maskArray.length != pixels.length) {
      throw new RuntimeException("The PImage used with mask() must be " +
                                 "the same size as the applet.");
    }
    for (int i = 0; i < pixels.length; i++) {
      pixels[i] = ((maskArray[i] & 0xff) << 24) | (pixels[i] & 0xffffff);
    }
    format = ARGB;
    updatePixels();
  }


  /**
   * Masks part of an image from displaying by loading another image and using it as an alpha channel.
   *  This mask image should only contain grayscale data, but only the blue color channel is used.
   *  The mask image needs to be the same size as the image to which it is applied.
   *  In addition to using a mask image, an integer array containing the alpha channel data can be specified directly.
   *  This method is useful for creating dynamically generated alpha masks.
   *  This array must be of the same length as the target image's pixels array and should contain only grayscale data of values between 0-255.
   * @webref
   * @brief  	Masks part of the image from displaying
   * @param maskImg any PImage object used as the alpha channel for "img", needs to be same size as "img"
   */
  public void mask(PImage maskImg) {
    mask(maskImg.pixels);
  }



  //////////////////////////////////////////////////////////////

  // IMAGE FILTERS
  public void filter(int kind) {
    loadPixels();

    switch (kind) {
      case BLUR:
        // TODO write basic low-pass filter blur here
        // what does photoshop do on the edges with this guy?
        // better yet.. why bother? just use gaussian with radius 1
        filter(BLUR, 1);
        break;

      case GRAY:
        if (format == ALPHA) {
          // for an alpha image, convert it to an opaque grayscale
          for (int i = 0; i < pixels.length; i++) {
            int col = 255 - pixels[i];
            pixels[i] = 0xff000000 | (col << 16) | (col << 8) | col;
          }
          format = RGB;

        } else {
          // Converts RGB image data into grayscale using
          // weighted RGB components, and keeps alpha channel intact.
          // [toxi 040115]
          for (int i = 0; i < pixels.length; i++) {
            int col = pixels[i];
            // luminance = 0.3*red + 0.59*green + 0.11*blue
            // 0.30 * 256 =  77
            // 0.59 * 256 = 151
            // 0.11 * 256 =  28
            int lum = (77*(col>>16&0xff) + 151*(col>>8&0xff) + 28*(col&0xff))>>8;
            pixels[i] = (col & ALPHA_MASK) | lum<<16 | lum<<8 | lum;
          }
        }
        break;

      case INVERT:
        for (int i = 0; i < pixels.length; i++) {
          //pixels[i] = 0xff000000 |
          pixels[i] ^= 0xffffff;
        }
        break;

      case POSTERIZE:
        throw new RuntimeException("Use filter(POSTERIZE, int levels) " +
        "instead of filter(POSTERIZE)");

      case OPAQUE:
        for (int i = 0; i < pixels.length; i++) {
          pixels[i] |= 0xff000000;
        }
        format = RGB;
        break;

      case THRESHOLD:
        filter(THRESHOLD, 0.5f);
        break;

        // [toxi20050728] added new filters
      case ERODE:
        dilate(true);
        break;

      case DILATE:
        dilate(false);
        break;
    }
    updatePixels();  // mark as modified
  }


  /**
   * Filters an image as defined by one of the following modes:<br><br>THRESHOLD - converts the image to black and white pixels depending if they are above or below the threshold defined by the level parameter. The level must be between 0.0 (black) and 1.0(white). If no level is specified, 0.5 is used.<br><br>GRAY - converts any colors in the image to grayscale equivalents<br><br>INVERT - sets each pixel to its inverse value<br><br>POSTERIZE - limits each channel of the image to the number of colors specified as the level parameter<br><br>BLUR - executes a Guassian blur with the level parameter specifying the extent of the blurring. If no level parameter is used, the blur is equivalent to Guassian blur of radius 1.<br><br>OPAQUE - sets the alpha channel to entirely opaque.<br><br>ERODE - reduces the light areas with the amount defined by the level parameter.<br><br>DILATE - increases the light areas with the amount defined by the level parameter
   * =advanced
   * Method to apply a variety of basic filters to this image.
   * <P>
   * <UL>
   * <LI>filter(BLUR) provides a basic blur.
   * <LI>filter(GRAY) converts the image to grayscale based on luminance.
   * <LI>filter(INVERT) will invert the color components in the image.
   * <LI>filter(OPAQUE) set all the high bits in the image to opaque
   * <LI>filter(THRESHOLD) converts the image to black and white.
   * <LI>filter(DILATE) grow white/light areas
   * <LI>filter(ERODE) shrink white/light areas
   * </UL>
   * Luminance conversion code contributed by
   * <A HREF="http://www.toxi.co.uk">toxi</A>
   * <P/>
   * Gaussian blur code contributed by
   * <A HREF="http://incubator.quasimondo.com">Mario Klingemann</A>
   * 
   * @webref
   * @brief Converts the image to grayscale or black and white
   * @param kind Either THRESHOLD, GRAY, INVERT, POSTERIZE, BLUR, OPAQUE, ERODE, or DILATE
   * @param param in the range from 0 to 1
   */
  public void filter(int kind, float param) {
    loadPixels();

    switch (kind) {
      case BLUR:
        if (format == ALPHA)
          blurAlpha(param);
        else if (format == ARGB)
          blurARGB(param);
        else
          blurRGB(param);
        break;

      case GRAY:
        throw new RuntimeException("Use filter(GRAY) instead of " +
                                   "filter(GRAY, param)");

      case INVERT:
        throw new RuntimeException("Use filter(INVERT) instead of " +
                                   "filter(INVERT, param)");

      case OPAQUE:
        throw new RuntimeException("Use filter(OPAQUE) instead of " +
                                   "filter(OPAQUE, param)");

      case POSTERIZE:
        int levels = (int)param;
        if ((levels < 2) || (levels > 255)) {
          throw new RuntimeException("Levels must be between 2 and 255 for " +
                                     "filter(POSTERIZE, levels)");
        }
        int levels1 = levels - 1;
        for (int i = 0; i < pixels.length; i++) {
          int rlevel = (pixels[i] >> 16) & 0xff;
          int glevel = (pixels[i] >> 8) & 0xff;
          int blevel = pixels[i] & 0xff;
          rlevel = (((rlevel * levels) >> 8) * 255) / levels1;
          glevel = (((glevel * levels) >> 8) * 255) / levels1;
          blevel = (((blevel * levels) >> 8) * 255) / levels1;
          pixels[i] = ((0xff000000 & pixels[i]) |
                       (rlevel << 16) |
                       (glevel << 8) |
                       blevel);
        }
        break;

      case THRESHOLD:  // greater than or equal to the threshold
        int thresh = (int) (param * 255);
        for (int i = 0; i < pixels.length; i++) {
          int max = Math.max((pixels[i] & RED_MASK) >> 16,
                             Math.max((pixels[i] & GREEN_MASK) >> 8,
                                      (pixels[i] & BLUE_MASK)));
          pixels[i] = (pixels[i] & ALPHA_MASK) |
            ((max < thresh) ? 0x000000 : 0xffffff);
        }
        break;

        // [toxi20050728] added new filters
        case ERODE:
          throw new RuntimeException("Use filter(ERODE) instead of " +
                                     "filter(ERODE, param)");
        case DILATE:
          throw new RuntimeException("Use filter(DILATE) instead of " +
                                     "filter(DILATE, param)");
    }
    updatePixels();  // mark as modified
  }


  /**
   * Optimized code for building the blur kernel.
   * further optimized blur code (approx. 15% for radius=20)
   * bigger speed gains for larger radii (~30%)
   * added support for various image types (ALPHA, RGB, ARGB)
   * [toxi 050728]
   */
  protected void buildBlurKernel(float r) {
    int radius = (int) (r * 3.5f);
    radius = (radius < 1) ? 1 : ((radius < 248) ? radius : 248);
    if (blurRadius != radius) {
      blurRadius = radius;
      blurKernelSize = 1 + blurRadius<<1;
      blurKernel = new int[blurKernelSize];
      blurMult = new int[blurKernelSize][256];

      int bk,bki;
      int[] bm,bmi;

      for (int i = 1, radiusi = radius - 1; i < radius; i++) {
        blurKernel[radius+i] = blurKernel[radiusi] = bki = radiusi * radiusi;
        bm=blurMult[radius+i];
        bmi=blurMult[radiusi--];
        for (int j = 0; j < 256; j++)
          bm[j] = bmi[j] = bki*j;
      }
      bk = blurKernel[radius] = radius * radius;
      bm = blurMult[radius];
      for (int j = 0; j < 256; j++)
        bm[j] = bk*j;
    }
  }


  protected void blurAlpha(float r) {
    int sum, cb;
    int read, ri, ym, ymi, bk0;
    int b2[] = new int[pixels.length];
    int yi = 0;

    buildBlurKernel(r);

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        //cb = cg = cr = sum = 0;
        cb = sum = 0;
        read = x - blurRadius;
        if (read<0) {
          bk0=-read;
          read=0;
        } else {
          if (read >= width)
            break;
          bk0=0;
        }
        for (int i = bk0; i < blurKernelSize; i++) {
          if (read >= width)
            break;
          int c = pixels[read + yi];
          int[] bm=blurMult[i];
          cb += bm[c & BLUE_MASK];
          sum += blurKernel[i];
          read++;
        }
        ri = yi + x;
        b2[ri] = cb / sum;
      }
      yi += width;
    }

    yi = 0;
    ym=-blurRadius;
    ymi=ym*width;

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        //cb = cg = cr = sum = 0;
        cb = sum = 0;
        if (ym<0) {
          bk0 = ri = -ym;
          read = x;
        } else {
          if (ym >= height)
            break;
          bk0 = 0;
          ri = ym;
          read = x + ymi;
        }
        for (int i = bk0; i < blurKernelSize; i++) {
          if (ri >= height)
            break;
          int[] bm=blurMult[i];
          cb += bm[b2[read]];
          sum += blurKernel[i];
          ri++;
          read += width;
        }
        pixels[x+yi] = (cb/sum);
      }
      yi += width;
      ymi += width;
      ym++;
    }
  }


  protected void blurRGB(float r) {
    int sum, cr, cg, cb; //, k;
    int /*pixel,*/ read, ri, /*roff,*/ ym, ymi, /*riw,*/ bk0;
    int r2[] = new int[pixels.length];
    int g2[] = new int[pixels.length];
    int b2[] = new int[pixels.length];
    int yi = 0;

    buildBlurKernel(r);

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        cb = cg = cr = sum = 0;
        read = x - blurRadius;
        if (read<0) {
          bk0=-read;
          read=0;
        } else {
          if (read >= width)
            break;
          bk0=0;
        }
        for (int i = bk0; i < blurKernelSize; i++) {
          if (read >= width)
            break;
          int c = pixels[read + yi];
          int[] bm=blurMult[i];
          cr += bm[(c & RED_MASK) >> 16];
          cg += bm[(c & GREEN_MASK) >> 8];
          cb += bm[c & BLUE_MASK];
          sum += blurKernel[i];
          read++;
        }
        ri = yi + x;
        r2[ri] = cr / sum;
        g2[ri] = cg / sum;
        b2[ri] = cb / sum;
      }
      yi += width;
    }

    yi = 0;
    ym=-blurRadius;
    ymi=ym*width;

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        cb = cg = cr = sum = 0;
        if (ym<0) {
          bk0 = ri = -ym;
          read = x;
        } else {
          if (ym >= height)
            break;
          bk0 = 0;
          ri = ym;
          read = x + ymi;
        }
        for (int i = bk0; i < blurKernelSize; i++) {
          if (ri >= height)
            break;
          int[] bm=blurMult[i];
          cr += bm[r2[read]];
          cg += bm[g2[read]];
          cb += bm[b2[read]];
          sum += blurKernel[i];
          ri++;
          read += width;
        }
        pixels[x+yi] = 0xff000000 | (cr/sum)<<16 | (cg/sum)<<8 | (cb/sum);
      }
      yi += width;
      ymi += width;
      ym++;
    }
  }


  protected void blurARGB(float r) {
    int sum, cr, cg, cb, ca;
    int /*pixel,*/ read, ri, /*roff,*/ ym, ymi, /*riw,*/ bk0;
    int wh = pixels.length;
    int r2[] = new int[wh];
    int g2[] = new int[wh];
    int b2[] = new int[wh];
    int a2[] = new int[wh];
    int yi = 0;

    buildBlurKernel(r);

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        cb = cg = cr = ca = sum = 0;
        read = x - blurRadius;
        if (read<0) {
          bk0=-read;
          read=0;
        } else {
          if (read >= width)
            break;
          bk0=0;
        }
        for (int i = bk0; i < blurKernelSize; i++) {
          if (read >= width)
            break;
          int c = pixels[read + yi];
          int[] bm=blurMult[i];
          ca += bm[(c & ALPHA_MASK) >>> 24];
          cr += bm[(c & RED_MASK) >> 16];
          cg += bm[(c & GREEN_MASK) >> 8];
          cb += bm[c & BLUE_MASK];
          sum += blurKernel[i];
          read++;
        }
        ri = yi + x;
        a2[ri] = ca / sum;
        r2[ri] = cr / sum;
        g2[ri] = cg / sum;
        b2[ri] = cb / sum;
      }
      yi += width;
    }

    yi = 0;
    ym=-blurRadius;
    ymi=ym*width;

    for (int y = 0; y < height; y++) {
      for (int x = 0; x < width; x++) {
        cb = cg = cr = ca = sum = 0;
        if (ym<0) {
          bk0 = ri = -ym;
          read = x;
        } else {
          if (ym >= height)
            break;
          bk0 = 0;
          ri = ym;
          read = x + ymi;
        }
        for (int i = bk0; i < blurKernelSize; i++) {
          if (ri >= height)
            break;
          int[] bm=blurMult[i];
          ca += bm[a2[read]];
          cr += bm[r2[read]];
          cg += bm[g2[read]];
          cb += bm[b2[read]];
          sum += blurKernel[i];
          ri++;
          read += width;
        }
        pixels[x+yi] = (ca/sum)<<24 | (cr/sum)<<16 | (cg/sum)<<8 | (cb/sum);
      }
      yi += width;
      ymi += width;
      ym++;
    }
  }


  /**
   * Generic dilate/erode filter using luminance values
   * as decision factor. [toxi 050728]
   */
  protected void dilate(boolean isInverted) {
    int currIdx=0;
    int maxIdx=pixels.length;
    int[] out=new int[maxIdx];

    if (!isInverted) {
      // erosion (grow light areas)
      while (currIdx<maxIdx) {
        int currRowIdx=currIdx;
        int maxRowIdx=currIdx+width;
        while (currIdx<maxRowIdx) {
          int colOrig,colOut;
          colOrig=colOut=pixels[currIdx];
          int idxLeft=currIdx-1;
          int idxRight=currIdx+1;
          int idxUp=currIdx-width;
          int idxDown=currIdx+width;
          if (idxLeft<currRowIdx)
            idxLeft=currIdx;
          if (idxRight>=maxRowIdx)
            idxRight=currIdx;
          if (idxUp<0)
            idxUp=0;
          if (idxDown>=maxIdx)
            idxDown=currIdx;

          int colUp=pixels[idxUp];
          int colLeft=pixels[idxLeft];
          int colDown=pixels[idxDown];
          int colRight=pixels[idxRight];

          // compute luminance
          int currLum =
            77*(colOrig>>16&0xff) + 151*(colOrig>>8&0xff) + 28*(colOrig&0xff);
          int lumLeft =
            77*(colLeft>>16&0xff) + 151*(colLeft>>8&0xff) + 28*(colLeft&0xff);
          int lumRight =
            77*(colRight>>16&0xff) + 151*(colRight>>8&0xff) + 28*(colRight&0xff);
          int lumUp =
            77*(colUp>>16&0xff) + 151*(colUp>>8&0xff) + 28*(colUp&0xff);
          int lumDown =
            77*(colDown>>16&0xff) + 151*(colDown>>8&0xff) + 28*(colDown&0xff);

          if (lumLeft>currLum) {
            colOut=colLeft;
            currLum=lumLeft;
          }
          if (lumRight>currLum) {
            colOut=colRight;
            currLum=lumRight;
          }
          if (lumUp>currLum) {
            colOut=colUp;
            currLum=lumUp;
          }
          if (lumDown>currLum) {
            colOut=colDown;
            currLum=lumDown;
          }
          out[currIdx++]=colOut;
        }
      }
    } else {
      // dilate (grow dark areas)
      while (currIdx<maxIdx) {
        int currRowIdx=currIdx;
        int maxRowIdx=currIdx+width;
        while (currIdx<maxRowIdx) {
          int colOrig,colOut;
          colOrig=colOut=pixels[currIdx];
          int idxLeft=currIdx-1;
          int idxRight=currIdx+1;
          int idxUp=currIdx-width;
          int idxDown=currIdx+width;
          if (idxLeft<currRowIdx)
            idxLeft=currIdx;
          if (idxRight>=maxRowIdx)
            idxRight=currIdx;
          if (idxUp<0)
            idxUp=0;
          if (idxDown>=maxIdx)
            idxDown=currIdx;

          int colUp=pixels[idxUp];
          int colLeft=pixels[idxLeft];
          int colDown=pixels[idxDown];
          int colRight=pixels[idxRight];

          // compute luminance
          int currLum =
            77*(colOrig>>16&0xff) + 151*(colOrig>>8&0xff) + 28*(colOrig&0xff);
          int lumLeft =
            77*(colLeft>>16&0xff) + 151*(colLeft>>8&0xff) + 28*(colLeft&0xff);
          int lumRight =
            77*(colRight>>16&0xff) + 151*(colRight>>8&0xff) + 28*(colRight&0xff);
          int lumUp =
            77*(colUp>>16&0xff) + 151*(colUp>>8&0xff) + 28*(colUp&0xff);
          int lumDown =
            77*(colDown>>16&0xff) + 151*(colDown>>8&0xff) + 28*(colDown&0xff);

          if (lumLeft<currLum) {
            colOut=colLeft;
            currLum=lumLeft;
          }
          if (lumRight<currLum) {
            colOut=colRight;
            currLum=lumRight;
          }
          if (lumUp<currLum) {
            colOut=colUp;
            currLum=lumUp;
          }
          if (lumDown<currLum) {
            colOut=colDown;
            currLum=lumDown;
          }
          out[currIdx++]=colOut;
        }
      }
    }
    System.arraycopy(out,0,pixels,0,maxIdx);
  }



  //////////////////////////////////////////////////////////////

  // COPY


  /**
   * Copy things from one area of this image
   * to another area in the same image.
   */
  public void copy(int sx, int sy, int sw, int sh,
                   int dx, int dy, int dw, int dh) {
    blend(this, sx, sy, sw, sh, dx, dy, dw, dh, REPLACE);
  }


  /**
   * Copies a region of pixels from one image into another. If the source and destination regions aren't the same size, it will automatically resize source pixels to fit the specified target region. No alpha information is used in the process, however if the source image has an alpha channel set, it will be copied as well.
   * <br><br>As of release 0149, this function ignores <b>imageMode()</b>.
   * 
   * @webref
   * @brief  	Copies the entire image
   * @param sx X coordinate of the source's upper left corner
   * @param sy Y coordinate of the source's upper left corner
   * @param sw source image width
   * @param sh source image height
   * @param dx X coordinate of the destination's upper left corner
   * @param dy Y coordinate of the destination's upper left corner
   * @param dw destination image width
   * @param dh destination image height
   * @param src an image variable referring to the source image.
   * 
   * @see processing.core.PApplet#alpha(int)
   * @see processing.core.PApplet#blend(PImage, int, int, int, int, int, int, int, int, int)
   */
  public void copy(PImage src,
                   int sx, int sy, int sw, int sh,
                   int dx, int dy, int dw, int dh) {
    blend(src, sx, sy, sw, sh, dx, dy, dw, dh, REPLACE);
  }



  //////////////////////////////////////////////////////////////

  // BLEND


  /**
   * Blend two colors based on a particular mode.
   * <UL>
   * <LI>REPLACE - destination colour equals colour of source pixel: C = A.
   *     Sometimes called "Normal" or "Copy" in other software.
   *
   * <LI>BLEND - linear interpolation of colours:
   *     <TT>C = A*factor + B</TT>
   *
   * <LI>ADD - additive blending with white clip:
   *     <TT>C = min(A*factor + B, 255)</TT>.
   *     Clipped to 0..255, Photoshop calls this "Linear Burn",
   *     and Director calls it "Add Pin".
   *
   * <LI>SUBTRACT - substractive blend with black clip:
   *     <TT>C = max(B - A*factor, 0)</TT>.
   *     Clipped to 0..255, Photoshop calls this "Linear Dodge",
   *     and Director calls it "Subtract Pin".
   *
   * <LI>DARKEST - only the darkest colour succeeds:
   *     <TT>C = min(A*factor, B)</TT>.
   *     Illustrator calls this "Darken".
   *
   * <LI>LIGHTEST - only the lightest colour succeeds:
   *     <TT>C = max(A*factor, B)</TT>.
   *     Illustrator calls this "Lighten".
   *
   * <LI>DIFFERENCE - subtract colors from underlying image.
   *
   * <LI>EXCLUSION - similar to DIFFERENCE, but less extreme.
   *
   * <LI>MULTIPLY - Multiply the colors, result will always be darker.
   *
   * <LI>SCREEN - Opposite multiply, uses inverse values of the colors.
   *
   * <LI>OVERLAY - A mix of MULTIPLY and SCREEN. Multiplies dark values,
   *     and screens light values.
   *
   * <LI>HARD_LIGHT - SCREEN when greater than 50% gray, MULTIPLY when lower.
   *
   * <LI>SOFT_LIGHT - Mix of DARKEST and LIGHTEST.
   *     Works like OVERLAY, but not as harsh.
   *
   * <LI>DODGE - Lightens light tones and increases contrast, ignores darks.
   *     Called "Color Dodge" in Illustrator and Photoshop.
   *
   * <LI>BURN - Darker areas are applied, increasing contrast, ignores lights.
   *     Called "Color Burn" in Illustrator and Photoshop.
   * </UL>
   * <P>A useful reference for blending modes and their algorithms can be
   * found in the <A HREF="http://www.w3.org/TR/SVG12/rendering.html">SVG</A>
   * specification.</P>
   * <P>It is important to note that Processing uses "fast" code, not
   * necessarily "correct" code. No biggie, most software does. A nitpicker
   * can find numerous "off by 1 division" problems in the blend code where
   * <TT>&gt;&gt;8</TT> or <TT>&gt;&gt;7</TT> is used when strictly speaking
   * <TT>/255.0</T> or <TT>/127.0</TT> should have been used.</P>
   * <P>For instance, exclusion (not intended for real-time use) reads
   * <TT>r1 + r2 - ((2 * r1 * r2) / 255)</TT> because <TT>255 == 1.0</TT>
   * not <TT>256 == 1.0</TT>. In other words, <TT>(255*255)>>8</TT> is not
   * the same as <TT>(255*255)/255</TT>. But for real-time use the shifts
   * are preferrable, and the difference is insignificant for applications
   * built with Processing.</P>
   */
  static public int blendColor(int c1, int c2, int mode) {
    switch (mode) {
    case REPLACE:    return c2;
    case BLEND:      return blend_blend(c1, c2);

    case ADD:        return blend_add_pin(c1, c2);
    case SUBTRACT:   return blend_sub_pin(c1, c2);

    case LIGHTEST:   return blend_lightest(c1, c2);
    case DARKEST:    return blend_darkest(c1, c2);

    case DIFFERENCE: return blend_difference(c1, c2);
    case EXCLUSION:  return blend_exclusion(c1, c2);

    case MULTIPLY:   return blend_multiply(c1, c2);
    case SCREEN:     return blend_screen(c1, c2);

    case HARD_LIGHT: return blend_hard_light(c1, c2);
    case SOFT_LIGHT: return blend_soft_light(c1, c2);
    case OVERLAY:    return blend_overlay(c1, c2);

    case DODGE:      return blend_dodge(c1, c2);
    case BURN:       return blend_burn(c1, c2);
    }
    return 0;
  }


  /**
   * Blends one area of this image to another area.
   * 
   * 
   * @see processing.core.PImage#blendColor(int,int,int)
   */
  public void blend(int sx, int sy, int sw, int sh,
                    int dx, int dy, int dw, int dh, int mode) {
    blend(this, sx, sy, sw, sh, dx, dy, dw, dh, mode);
  }


  /**
   * Blends a region of pixels into the image specified by the <b>img</b> parameter. These copies utilize full alpha channel support and a choice of the following modes to blend the colors of source pixels (A) with the ones of pixels in the destination image (B):<br><br>
   * BLEND - linear interpolation of colours: C = A*factor + B<br><br>
   * ADD - additive blending with white clip: C = min(A*factor + B, 255)<br><br>
   * SUBTRACT - subtractive blending with black clip: C = max(B - A*factor, 0)<br><br>
   * DARKEST - only the darkest colour succeeds: C = min(A*factor, B)<br><br>
   * LIGHTEST - only the lightest colour succeeds: C = max(A*factor, B)<br><br>
   * DIFFERENCE - subtract colors from underlying image.<br><br>
   * EXCLUSION - similar to DIFFERENCE, but less extreme.<br><br>
   * MULTIPLY - Multiply the colors, result will always be darker.<br><br>
   * SCREEN - Opposite multiply, uses inverse values of the colors.<br><br>
   * OVERLAY - A mix of MULTIPLY and SCREEN. Multiplies dark values, and screens light values.<br><br>
   * HARD_LIGHT - SCREEN when greater than 50% gray, MULTIPLY when lower.<br><br>
   * SOFT_LIGHT - Mix of DARKEST and LIGHTEST. Works like OVERLAY, but not as harsh.<br><br>
   * DODGE - Lightens light tones and increases contrast, ignores darks. Called "Color Dodge" in Illustrator and Photoshop.<br><br>
   * BURN - Darker areas are applied, increasing contrast, ignores lights. Called "Color Burn" in Illustrator and Photoshop.<br><br>
   * All modes use the alpha information (highest byte) of source image pixels as the blending factor. If the source and destination regions are different sizes, the image will be automatically resized to match the destination size. If the <b>srcImg</b> parameter is not used, the display window is used as the source image.<br><br>
   * As of release 0149, this function ignores <b>imageMode()</b>.
   * 
   * @webref
   * @brief  Copies a pixel or rectangle of pixels using different blending modes
   * @param src an image variable referring to the source image
   * @param sx X coordinate of the source's upper left corner
   * @param sy Y coordinate of the source's upper left corner
   * @param sw source image width
   * @param sh source image height
   * @param dx X coordinate of the destinations's upper left corner
   * @param dy Y coordinate of the destinations's upper left corner
   * @param dw destination image width
   * @param dh destination image height
   * @param mode Either BLEND, ADD, SUBTRACT, LIGHTEST, DARKEST, DIFFERENCE, EXCLUSION, MULTIPLY, SCREEN, OVERLAY, HARD_LIGHT, SOFT_LIGHT, DODGE, BURN
   * 
   * @see processing.core.PApplet#alpha(int)
   * @see processing.core.PApplet#copy(PImage, int, int, int, int, int, int, int, int)
   * @see processing.core.PImage#blendColor(int,int,int)
   */
  public void blend(PImage src,
                    int sx, int sy, int sw, int sh,
                    int dx, int dy, int dw, int dh, int mode) {
    /*
    if (imageMode == CORNER) {  // if CORNERS, do nothing
      sx2 += sx1;
      sy2 += sy1;
      dx2 += dx1;
      dy2 += dy1;

    } else if (imageMode == CENTER) {
      sx1 -= sx2 / 2f;
      sy1 -= sy2 / 2f;
      sx2 += sx1;
      sy2 += sy1;
      dx1 -= dx2 / 2f;
      dy1 -= dy2 / 2f;
      dx2 += dx1;
      dy2 += dy1;
    }
    */
    int sx2 = sx + sw;
    int sy2 = sy + sh;
    int dx2 = dx + dw;
    int dy2 = dy + dh;

    loadPixels();
    if (src == this) {
      if (intersect(sx, sy, sx2, sy2, dx, dy, dx2, dy2)) {
        blit_resize(get(sx, sy, sx2 - sx, sy2 - sy),
                    0, 0, sx2 - sx - 1, sy2 - sy - 1,
                    pixels, width, height, dx, dy, dx2, dy2, mode);
      } else {
        // same as below, except skip the loadPixels() because it'd be redundant
        blit_resize(src, sx, sy, sx2, sy2,
                    pixels, width, height, dx, dy, dx2, dy2, mode);
      }
    } else {
      src.loadPixels();
      blit_resize(src, sx, sy, sx2, sy2,
                  pixels, width, height, dx, dy, dx2, dy2, mode);
      //src.updatePixels();
    }
    updatePixels();
  }


  /**
   * Check to see if two rectangles intersect one another
   */
  private boolean intersect(int sx1, int sy1, int sx2, int sy2,
                            int dx1, int dy1, int dx2, int dy2) {
    int sw = sx2 - sx1 + 1;
    int sh = sy2 - sy1 + 1;
    int dw = dx2 - dx1 + 1;
    int dh = dy2 - dy1 + 1;

    if (dx1 < sx1) {
      dw += dx1 - sx1;
      if (dw > sw) {
        dw = sw;
      }
    } else {
      int w = sw + sx1 - dx1;
      if (dw > w) {
        dw = w;
      }
    }
    if (dy1 < sy1) {
      dh += dy1 - sy1;
      if (dh > sh) {
        dh = sh;
      }
    } else {
      int h = sh + sy1 - dy1;
      if (dh > h) {
        dh = h;
      }
    }
    return !(dw <= 0 || dh <= 0);
  }


  //////////////////////////////////////////////////////////////


  /**
   * Internal blitter/resizer/copier from toxi.
   * Uses bilinear filtering if smooth() has been enabled
   * 'mode' determines the blending mode used in the process.
   */
  private void blit_resize(PImage img,
                           int srcX1, int srcY1, int srcX2, int srcY2,
                           int[] destPixels, int screenW, int screenH,
                           int destX1, int destY1, int destX2, int destY2,
                           int mode) {
    if (srcX1 < 0) srcX1 = 0;
    if (srcY1 < 0) srcY1 = 0;
    if (srcX2 >= img.width) srcX2 = img.width - 1;
    if (srcY2 >= img.height) srcY2 = img.height - 1;

    int srcW = srcX2 - srcX1;
    int srcH = srcY2 - srcY1;
    int destW = destX2 - destX1;
    int destH = destY2 - destY1;

    boolean smooth = true;  // may as well go with the smoothing these days

    if (!smooth) {
      srcW++; srcH++;
    }

    if (destW <= 0 || destH <= 0 ||
        srcW <= 0 || srcH <= 0 ||
        destX1 >= screenW || destY1 >= screenH ||
        srcX1 >= img.width || srcY1 >= img.height) {
      return;
    }

    int dx = (int) (srcW / (float) destW * PRECISIONF);
    int dy = (int) (srcH / (float) destH * PRECISIONF);

    srcXOffset = (int) (destX1 < 0 ? -destX1 * dx : srcX1 * PRECISIONF);
    srcYOffset = (int) (destY1 < 0 ? -destY1 * dy : srcY1 * PRECISIONF);

    if (destX1 < 0) {
      destW += destX1;
      destX1 = 0;
    }
    if (destY1 < 0) {
      destH += destY1;
      destY1 = 0;
    }

    destW = low(destW, screenW - destX1);
    destH = low(destH, screenH - destY1);

    int destOffset = destY1 * screenW + destX1;
    srcBuffer = img.pixels;

    if (smooth) {
      // use bilinear filtering
      iw = img.width;
      iw1 = img.width - 1;
      ih1 = img.height - 1;

      switch (mode) {

      case BLEND:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            // davbol  - renamed old blend_multiply to blend_blend
            destPixels[destOffset + x] =
              blend_blend(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case ADD:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_add_pin(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case SUBTRACT:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_sub_pin(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case LIGHTEST:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_lightest(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case DARKEST:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_darkest(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case REPLACE:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] = filter_bilinear();
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case DIFFERENCE:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_difference(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case EXCLUSION:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_exclusion(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case MULTIPLY:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_multiply(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case SCREEN:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_screen(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case OVERLAY:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_overlay(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case HARD_LIGHT:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_hard_light(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case SOFT_LIGHT:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_soft_light(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      // davbol - proposed 2007-01-09
      case DODGE:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_dodge(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case BURN:
        for (int y = 0; y < destH; y++) {
          filter_new_scanline();
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_burn(destPixels[destOffset + x], filter_bilinear());
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      }

    } else {
      // nearest neighbour scaling (++fast!)
      switch (mode) {

      case BLEND:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            // davbol - renamed old blend_multiply to blend_blend
            destPixels[destOffset + x] =
              blend_blend(destPixels[destOffset + x],
                             srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case ADD:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_add_pin(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case SUBTRACT:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_sub_pin(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case LIGHTEST:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_lightest(destPixels[destOffset + x],
                             srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case DARKEST:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_darkest(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case REPLACE:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] = srcBuffer[sY + (sX >> PRECISIONB)];
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case DIFFERENCE:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_difference(destPixels[destOffset + x],
                               srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case EXCLUSION:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_exclusion(destPixels[destOffset + x],
                              srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case MULTIPLY:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_multiply(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case SCREEN:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_screen(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case OVERLAY:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_overlay(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case HARD_LIGHT:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_hard_light(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case SOFT_LIGHT:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_soft_light(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      // davbol - proposed 2007-01-09
      case DODGE:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_dodge(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      case BURN:
        for (int y = 0; y < destH; y++) {
          sX = srcXOffset;
          sY = (srcYOffset >> PRECISIONB) * img.width;
          for (int x = 0; x < destW; x++) {
            destPixels[destOffset + x] =
              blend_burn(destPixels[destOffset + x],
                            srcBuffer[sY + (sX >> PRECISIONB)]);
            sX += dx;
          }
          destOffset += screenW;
          srcYOffset += dy;
        }
        break;

      }
    }
  }


  private void filter_new_scanline() {
    sX = srcXOffset;
    fracV = srcYOffset & PREC_MAXVAL;
    ifV = PREC_MAXVAL - fracV;
    v1 = (srcYOffset >> PRECISIONB) * iw;
    v2 = low((srcYOffset >> PRECISIONB) + 1, ih1) * iw;
  }


  private int filter_bilinear() {
    fracU = sX & PREC_MAXVAL;
    ifU = PREC_MAXVAL - fracU;
    ul = (ifU * ifV) >> PRECISIONB;
    ll = (ifU * fracV) >> PRECISIONB;
    ur = (fracU * ifV) >> PRECISIONB;
    lr = (fracU * fracV) >> PRECISIONB;
    u1 = (sX >> PRECISIONB);
    u2 = low(u1 + 1, iw1);

    // get color values of the 4 neighbouring texels
    cUL = srcBuffer[v1 + u1];
    cUR = srcBuffer[v1 + u2];
    cLL = srcBuffer[v2 + u1];
    cLR = srcBuffer[v2 + u2];

    r = ((ul*((cUL&RED_MASK)>>16) + ll*((cLL&RED_MASK)>>16) +
          ur*((cUR&RED_MASK)>>16) + lr*((cLR&RED_MASK)>>16))
         << PREC_RED_SHIFT) & RED_MASK;

    g = ((ul*(cUL&GREEN_MASK) + ll*(cLL&GREEN_MASK) +
          ur*(cUR&GREEN_MASK) + lr*(cLR&GREEN_MASK))
         >>> PRECISIONB) & GREEN_MASK;

    b = (ul*(cUL&BLUE_MASK) + ll*(cLL&BLUE_MASK) +
         ur*(cUR&BLUE_MASK) + lr*(cLR&BLUE_MASK))
           >>> PRECISIONB;

    a = ((ul*((cUL&ALPHA_MASK)>>>24) + ll*((cLL&ALPHA_MASK)>>>24) +
          ur*((cUR&ALPHA_MASK)>>>24) + lr*((cLR&ALPHA_MASK)>>>24))
         << PREC_ALPHA_SHIFT) & ALPHA_MASK;

    return a | r | g | b;
  }



  //////////////////////////////////////////////////////////////

  // internal blending methods


  private static int low(int a, int b) {
    return (a < b) ? a : b;
  }


  private static int high(int a, int b) {
    return (a > b) ? a : b;
  }

  // davbol - added peg helper, equiv to constrain(n,0,255)
  private static int peg(int n) {
    return (n < 0) ? 0 : ((n > 255) ? 255 : n);
  }

  private static int mix(int a, int b, int f) {
    return a + (((b - a) * f) >> 8);
  }



  /////////////////////////////////////////////////////////////

  // BLEND MODE IMPLEMENTIONS


  private static int blend_blend(int a, int b) {
    int f = (b & ALPHA_MASK) >>> 24;

    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            mix(a & RED_MASK, b & RED_MASK, f) & RED_MASK |
            mix(a & GREEN_MASK, b & GREEN_MASK, f) & GREEN_MASK |
            mix(a & BLUE_MASK, b & BLUE_MASK, f));
  }


  /**
   * additive blend with clipping
   */
  private static int blend_add_pin(int a, int b) {
    int f = (b & ALPHA_MASK) >>> 24;

    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            low(((a & RED_MASK) +
                 ((b & RED_MASK) >> 8) * f), RED_MASK) & RED_MASK |
            low(((a & GREEN_MASK) +
                 ((b & GREEN_MASK) >> 8) * f), GREEN_MASK) & GREEN_MASK |
            low((a & BLUE_MASK) +
                (((b & BLUE_MASK) * f) >> 8), BLUE_MASK));
  }


  /**
   * subtractive blend with clipping
   */
  private static int blend_sub_pin(int a, int b) {
    int f = (b & ALPHA_MASK) >>> 24;

    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            high(((a & RED_MASK) - ((b & RED_MASK) >> 8) * f),
                 GREEN_MASK) & RED_MASK |
            high(((a & GREEN_MASK) - ((b & GREEN_MASK) >> 8) * f),
                 BLUE_MASK) & GREEN_MASK |
            high((a & BLUE_MASK) - (((b & BLUE_MASK) * f) >> 8), 0));
  }


  /**
   * only returns the blended lightest colour
   */
  private static int blend_lightest(int a, int b) {
    int f = (b & ALPHA_MASK) >>> 24;

    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            high(a & RED_MASK, ((b & RED_MASK) >> 8) * f) & RED_MASK |
            high(a & GREEN_MASK, ((b & GREEN_MASK) >> 8) * f) & GREEN_MASK |
            high(a & BLUE_MASK, ((b & BLUE_MASK) * f) >> 8));
  }


  /**
   * only returns the blended darkest colour
   */
  private static int blend_darkest(int a, int b) {
    int f = (b & ALPHA_MASK) >>> 24;

    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            mix(a & RED_MASK,
                low(a & RED_MASK,
                    ((b & RED_MASK) >> 8) * f), f) & RED_MASK |
            mix(a & GREEN_MASK,
                low(a & GREEN_MASK,
                    ((b & GREEN_MASK) >> 8) * f), f) & GREEN_MASK |
            mix(a & BLUE_MASK,
                low(a & BLUE_MASK,
                    ((b & BLUE_MASK) * f) >> 8), f));
  }


  /**
   * returns the absolute value of the difference of the input colors
   * C = |A - B|
   */
  private static int blend_difference(int a, int b) {
    // setup (this portion will always be the same)
    int f = (b & ALPHA_MASK) >>> 24;
    int ar = (a & RED_MASK) >> 16;
    int ag = (a & GREEN_MASK) >> 8;
    int ab = (a & BLUE_MASK);
    int br = (b & RED_MASK) >> 16;
    int bg = (b & GREEN_MASK) >> 8;
    int bb = (b & BLUE_MASK);
    // formula:
    int cr = (ar > br) ? (ar-br) : (br-ar);
    int cg = (ag > bg) ? (ag-bg) : (bg-ag);
    int cb = (ab > bb) ? (ab-bb) : (bb-ab);
    // alpha blend (this portion will always be the same)
    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            (peg(ar + (((cr - ar) * f) >> 8)) << 16) |
            (peg(ag + (((cg - ag) * f) >> 8)) << 8) |
            (peg(ab + (((cb - ab) * f) >> 8)) ) );
  }


  /**
   * Cousin of difference, algorithm used here is based on a Lingo version
   * found here: http://www.mediamacros.com/item/item-1006687616/
   * (Not yet verified to be correct).
   */
  private static int blend_exclusion(int a, int b) {
    // setup (this portion will always be the same)
    int f = (b & ALPHA_MASK) >>> 24;
    int ar = (a & RED_MASK) >> 16;
    int ag = (a & GREEN_MASK) >> 8;
    int ab = (a & BLUE_MASK);
    int br = (b & RED_MASK) >> 16;
    int bg = (b & GREEN_MASK) >> 8;
    int bb = (b & BLUE_MASK);
    // formula:
    int cr = ar + br - ((ar * br) >> 7);
    int cg = ag + bg - ((ag * bg) >> 7);
    int cb = ab + bb - ((ab * bb) >> 7);
    // alpha blend (this portion will always be the same)
    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            (peg(ar + (((cr - ar) * f) >> 8)) << 16) |
            (peg(ag + (((cg - ag) * f) >> 8)) << 8) |
            (peg(ab + (((cb - ab) * f) >> 8)) ) );
  }


  /**
   * returns the product of the input colors
   * C = A * B
   */
  private static int blend_multiply(int a, int b) {
    // setup (this portion will always be the same)
    int f = (b & ALPHA_MASK) >>> 24;
    int ar = (a & RED_MASK) >> 16;
    int ag = (a & GREEN_MASK) >> 8;
    int ab = (a & BLUE_MASK);
    int br = (b & RED_MASK) >> 16;
    int bg = (b & GREEN_MASK) >> 8;
    int bb = (b & BLUE_MASK);
    // formula:
    int cr = (ar * br) >> 8;
    int cg = (ag * bg) >> 8;
    int cb = (ab * bb) >> 8;
    // alpha blend (this portion will always be the same)
    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            (peg(ar + (((cr - ar) * f) >> 8)) << 16) |
            (peg(ag + (((cg - ag) * f) >> 8)) << 8) |
            (peg(ab + (((cb - ab) * f) >> 8)) ) );
  }


  /**
   * returns the inverse of the product of the inverses of the input colors
   * (the inverse of multiply).  C = 1 - (1-A) * (1-B)
   */
  private static int blend_screen(int a, int b) {
    // setup (this portion will always be the same)
    int f = (b & ALPHA_MASK) >>> 24;
    int ar = (a & RED_MASK) >> 16;
    int ag = (a & GREEN_MASK) >> 8;
    int ab = (a & BLUE_MASK);
    int br = (b & RED_MASK) >> 16;
    int bg = (b & GREEN_MASK) >> 8;
    int bb = (b & BLUE_MASK);
    // formula:
    int cr = 255 - (((255 - ar) * (255 - br)) >> 8);
    int cg = 255 - (((255 - ag) * (255 - bg)) >> 8);
    int cb = 255 - (((255 - ab) * (255 - bb)) >> 8);
    // alpha blend (this portion will always be the same)
    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            (peg(ar + (((cr - ar) * f) >> 8)) << 16) |
            (peg(ag + (((cg - ag) * f) >> 8)) << 8) |
            (peg(ab + (((cb - ab) * f) >> 8)) ) );
  }


  /**
   * returns either multiply or screen for darker or lighter values of A
   * (the inverse of hard light)
   * C =
   *   A < 0.5 : 2 * A * B
   *   A >=0.5 : 1 - (2 * (255-A) * (255-B))
   */
  private static int blend_overlay(int a, int b) {
    // setup (this portion will always be the same)
    int f = (b & ALPHA_MASK) >>> 24;
    int ar = (a & RED_MASK) >> 16;
    int ag = (a & GREEN_MASK) >> 8;
    int ab = (a & BLUE_MASK);
    int br = (b & RED_MASK) >> 16;
    int bg = (b & GREEN_MASK) >> 8;
    int bb = (b & BLUE_MASK);
    // formula:
    int cr = (ar < 128) ? ((ar*br)>>7) : (255-(((255-ar)*(255-br))>>7));
    int cg = (ag < 128) ? ((ag*bg)>>7) : (255-(((255-ag)*(255-bg))>>7));
    int cb = (ab < 128) ? ((ab*bb)>>7) : (255-(((255-ab)*(255-bb))>>7));
    // alpha blend (this portion will always be the same)
    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            (peg(ar + (((cr - ar) * f) >> 8)) << 16) |
            (peg(ag + (((cg - ag) * f) >> 8)) << 8) |
            (peg(ab + (((cb - ab) * f) >> 8)) ) );
  }


  /**
   * returns either multiply or screen for darker or lighter values of B
   * (the inverse of overlay)
   * C =
   *   B < 0.5 : 2 * A * B
   *   B >=0.5 : 1 - (2 * (255-A) * (255-B))
   */
   private static int blend_hard_light(int a, int b) {
    // setup (this portion will always be the same)
    int f = (b & ALPHA_MASK) >>> 24;
    int ar = (a & RED_MASK) >> 16;
    int ag = (a & GREEN_MASK) >> 8;
    int ab = (a & BLUE_MASK);
    int br = (b & RED_MASK) >> 16;
    int bg = (b & GREEN_MASK) >> 8;
    int bb = (b & BLUE_MASK);
    // formula:
    int cr = (br < 128) ? ((ar*br)>>7) : (255-(((255-ar)*(255-br))>>7));
    int cg = (bg < 128) ? ((ag*bg)>>7) : (255-(((255-ag)*(255-bg))>>7));
    int cb = (bb < 128) ? ((ab*bb)>>7) : (255-(((255-ab)*(255-bb))>>7));
    // alpha blend (this portion will always be the same)
    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            (peg(ar + (((cr - ar) * f) >> 8)) << 16) |
            (peg(ag + (((cg - ag) * f) >> 8)) << 8) |
            (peg(ab + (((cb - ab) * f) >> 8)) ) );
  }


  /**
   * returns the inverse multiply plus screen, which simplifies to
   * C = 2AB + A^2 - 2A^2B
   */
  private static int blend_soft_light(int a, int b) {
    // setup (this portion will always be the same)
    int f = (b & ALPHA_MASK) >>> 24;
    int ar = (a & RED_MASK) >> 16;
    int ag = (a & GREEN_MASK) >> 8;
    int ab = (a & BLUE_MASK);
    int br = (b & RED_MASK) >> 16;
    int bg = (b & GREEN_MASK) >> 8;
    int bb = (b & BLUE_MASK);
    // formula:
    int cr = ((ar*br)>>7) + ((ar*ar)>>8) - ((ar*ar*br)>>15);
    int cg = ((ag*bg)>>7) + ((ag*ag)>>8) - ((ag*ag*bg)>>15);
    int cb = ((ab*bb)>>7) + ((ab*ab)>>8) - ((ab*ab*bb)>>15);
    // alpha blend (this portion will always be the same)
    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            (peg(ar + (((cr - ar) * f) >> 8)) << 16) |
            (peg(ag + (((cg - ag) * f) >> 8)) << 8) |
            (peg(ab + (((cb - ab) * f) >> 8)) ) );
  }


  /**
   * Returns the first (underlay) color divided by the inverse of
   * the second (overlay) color. C = A / (255-B)
   */
  private static int blend_dodge(int a, int b) {
    // setup (this portion will always be the same)
    int f = (b & ALPHA_MASK) >>> 24;
    int ar = (a & RED_MASK) >> 16;
    int ag = (a & GREEN_MASK) >> 8;
    int ab = (a & BLUE_MASK);
    int br = (b & RED_MASK) >> 16;
    int bg = (b & GREEN_MASK) >> 8;
    int bb = (b & BLUE_MASK);
    // formula:
    int cr = (br==255) ? 255 : peg((ar << 8) / (255 - br)); // division requires pre-peg()-ing
    int cg = (bg==255) ? 255 : peg((ag << 8) / (255 - bg)); // "
    int cb = (bb==255) ? 255 : peg((ab << 8) / (255 - bb)); // "
    // alpha blend (this portion will always be the same)
    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            (peg(ar + (((cr - ar) * f) >> 8)) << 16) |
            (peg(ag + (((cg - ag) * f) >> 8)) << 8) |
            (peg(ab + (((cb - ab) * f) >> 8)) ) );
  }


  /**
   * returns the inverse of the inverse of the first (underlay) color
   * divided by the second (overlay) color. C = 255 - (255-A) / B
   */
  private static int blend_burn(int a, int b) {
    // setup (this portion will always be the same)
    int f = (b & ALPHA_MASK) >>> 24;
    int ar = (a & RED_MASK) >> 16;
    int ag = (a & GREEN_MASK) >> 8;
    int ab = (a & BLUE_MASK);
    int br = (b & RED_MASK) >> 16;
    int bg = (b & GREEN_MASK) >> 8;
    int bb = (b & BLUE_MASK);
    // formula:
    int cr = (br==0) ? 0 : 255 - peg(((255 - ar) << 8) / br); // division requires pre-peg()-ing
    int cg = (bg==0) ? 0 : 255 - peg(((255 - ag) << 8) / bg); // "
    int cb = (bb==0) ? 0 : 255 - peg(((255 - ab) << 8) / bb); // "
    // alpha blend (this portion will always be the same)
    return (low(((a & ALPHA_MASK) >>> 24) + f, 0xff) << 24 |
            (peg(ar + (((cr - ar) * f) >> 8)) << 16) |
            (peg(ag + (((cg - ag) * f) >> 8)) << 8) |
            (peg(ab + (((cb - ab) * f) >> 8)) ) );
  }


  //////////////////////////////////////////////////////////////

  // FILE I/O


  static byte TIFF_HEADER[] = {
    77, 77, 0, 42, 0, 0, 0, 8, 0, 9, 0, -2, 0, 4, 0, 0, 0, 1, 0, 0,
    0, 0, 1, 0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 0, 3, 0, 0, 0, 1,
    0, 0, 0, 0, 1, 2, 0, 3, 0, 0, 0, 3, 0, 0, 0, 122, 1, 6, 0, 3, 0,
    0, 0, 1, 0, 2, 0, 0, 1, 17, 0, 4, 0, 0, 0, 1, 0, 0, 3, 0, 1, 21,
    0, 3, 0, 0, 0, 1, 0, 3, 0, 0, 1, 22, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0,
    1, 23, 0, 4, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8, 0, 8, 0, 8
  };


  static final String TIFF_ERROR =
    "Error: Processing can only read its own TIFF files.";

  static protected PImage loadTIFF(byte tiff[]) {
    if ((tiff[42] != tiff[102]) ||  // width/height in both places
        (tiff[43] != tiff[103])) {
      System.err.println(TIFF_ERROR);
      return null;
    }

    int width =
      ((tiff[30] & 0xff) << 8) | (tiff[31] & 0xff);
    int height =
      ((tiff[42] & 0xff) << 8) | (tiff[43] & 0xff);

    int count =
      ((tiff[114] & 0xff) << 24) |
      ((tiff[115] & 0xff) << 16) |
      ((tiff[116] & 0xff) << 8) |
      (tiff[117] & 0xff);
    if (count != width * height * 3) {
      System.err.println(TIFF_ERROR + " (" + width + ", " + height +")");
      return null;
    }

    // check the rest of the header
    for (int i = 0; i < TIFF_HEADER.length; i++) {
      if ((i == 30) || (i == 31) || (i == 42) || (i == 43) ||
          (i == 102) || (i == 103) ||
          (i == 114) || (i == 115) || (i == 116) || (i == 117)) continue;

      if (tiff[i] != TIFF_HEADER[i]) {
        System.err.println(TIFF_ERROR + " (" + i + ")");
        return null;
      }
    }

    PImage outgoing = new PImage(width, height, RGB);
    int index = 768;
    count /= 3;
    for (int i = 0; i < count; i++) {
      outgoing.pixels[i] =
        0xFF000000 |
        (tiff[index++] & 0xff) << 16 |
        (tiff[index++] & 0xff) << 8 |
        (tiff[index++] & 0xff);
    }
    return outgoing;
  }


  protected boolean saveTIFF(OutputStream output) {
    // shutting off the warning, people can figure this out themselves
    /*
    if (format != RGB) {
      System.err.println("Warning: only RGB information is saved with " +
                         ".tif files. Use .tga or .png for ARGB images and others.");
    }
    */
    try {
      byte tiff[] = new byte[768];
      System.arraycopy(TIFF_HEADER, 0, tiff, 0, TIFF_HEADER.length);

      tiff[30] = (byte) ((width >> 8) & 0xff);
      tiff[31] = (byte) ((width) & 0xff);
      tiff[42] = tiff[102] = (byte) ((height >> 8) & 0xff);
      tiff[43] = tiff[103] = (byte) ((height) & 0xff);

      int count = width*height*3;
      tiff[114] = (byte) ((count >> 24) & 0xff);
      tiff[115] = (byte) ((count >> 16) & 0xff);
      tiff[116] = (byte) ((count >> 8) & 0xff);
      tiff[117] = (byte) ((count) & 0xff);

      // spew the header to the disk
      output.write(tiff);

      for (int i = 0; i < pixels.length; i++) {
        output.write((pixels[i] >> 16) & 0xff);
        output.write((pixels[i] >> 8) & 0xff);
        output.write(pixels[i] & 0xff);
      }
      output.flush();
      return true;

    } catch (IOException e) {
      e.printStackTrace();
    }
    return false;
  }


  /**
   * Creates a Targa32 formatted byte sequence of specified
   * pixel buffer using RLE compression.
   * </p>
   * Also figured out how to avoid parsing the image upside-down
   * (there's a header flag to set the image origin to top-left)
   * </p>
   * Starting with revision 0092, the format setting is taken into account:
   * <UL>
   * <LI><TT>ALPHA</TT> images written as 8bit grayscale (uses lowest byte)
   * <LI><TT>RGB</TT> &rarr; 24 bits
   * <LI><TT>ARGB</TT> &rarr; 32 bits
   * </UL>
   * All versions are RLE compressed.
   * </p>
   * Contributed by toxi 8-10 May 2005, based on this RLE
   * <A HREF="http://www.wotsit.org/download.asp?f=tga">specification</A>
   */
  protected boolean saveTGA(OutputStream output) {
    byte header[] = new byte[18];

     if (format == ALPHA) {  // save ALPHA images as 8bit grayscale
       header[2] = 0x0B;
       header[16] = 0x08;
       header[17] = 0x28;

     } else if (format == RGB) {
       header[2] = 0x0A;
       header[16] = 24;
       header[17] = 0x20;

     } else if (format == ARGB) {
       header[2] = 0x0A;
       header[16] = 32;
       header[17] = 0x28;

     } else {
       throw new RuntimeException("Image format not recognized inside save()");
     }
     // set image dimensions lo-hi byte order
     header[12] = (byte) (width & 0xff);
     header[13] = (byte) (width >> 8);
     header[14] = (byte) (height & 0xff);
     header[15] = (byte) (height >> 8);

     try {
       output.write(header);

       int maxLen = height * width;
       int index = 0;
       int col; //, prevCol;
       int[] currChunk = new int[128];

       // 8bit image exporter is in separate loop
       // to avoid excessive conditionals...
       if (format == ALPHA) {
         while (index < maxLen) {
           boolean isRLE = false;
           int rle = 1;
           currChunk[0] = col = pixels[index] & 0xff;
           while (index + rle < maxLen) {
             if (col != (pixels[index + rle]&0xff) || rle == 128) {
               isRLE = (rle > 1);
               break;
             }
             rle++;
           }
           if (isRLE) {
             output.write(0x80 | (rle - 1));
             output.write(col);

           } else {
             rle = 1;
             while (index + rle < maxLen) {
               int cscan = pixels[index + rle] & 0xff;
               if ((col != cscan && rle < 128) || rle < 3) {
                 currChunk[rle] = col = cscan;
               } else {
                 if (col == cscan) rle -= 2;
                 break;
               }
               rle++;
             }
             output.write(rle - 1);
             for (int i = 0; i < rle; i++) output.write(currChunk[i]);
           }
           index += rle;
         }
       } else {  // export 24/32 bit TARGA
         while (index < maxLen) {
           boolean isRLE = false;
           currChunk[0] = col = pixels[index];
           int rle = 1;
           // try to find repeating bytes (min. len = 2 pixels)
           // maximum chunk size is 128 pixels
           while (index + rle < maxLen) {
             if (col != pixels[index + rle] || rle == 128) {
               isRLE = (rle > 1); // set flag for RLE chunk
               break;
             }
             rle++;
           }
           if (isRLE) {
             output.write(128 | (rle - 1));
             output.write(col & 0xff);
             output.write(col >> 8 & 0xff);
             output.write(col >> 16 & 0xff);
             if (format == ARGB) output.write(col >>> 24 & 0xff);

           } else {  // not RLE
             rle = 1;
             while (index + rle < maxLen) {
               if ((col != pixels[index + rle] && rle < 128) || rle < 3) {
                 currChunk[rle] = col = pixels[index + rle];
               } else {
                 // check if the exit condition was the start of
                 // a repeating colour
                 if (col == pixels[index + rle]) rle -= 2;
                 break;
               }
               rle++;
             }
             // write uncompressed chunk
             output.write(rle - 1);
             if (format == ARGB) {
               for (int i = 0; i < rle; i++) {
                 col = currChunk[i];
                 output.write(col & 0xff);
                 output.write(col >> 8 & 0xff);
                 output.write(col >> 16 & 0xff);
                 output.write(col >>> 24 & 0xff);
               }
             } else {
               for (int i = 0; i < rle; i++) {
                 col = currChunk[i];
                 output.write(col & 0xff);
                 output.write(col >> 8 & 0xff);
                 output.write(col >> 16 & 0xff);
               }
             }
           }
           index += rle;
         }
       }
       output.flush();
       return true;

     } catch (IOException e) {
       e.printStackTrace();
       return false;
     }
  }


  /**
   * Use ImageIO functions from Java 1.4 and later to handle image save.
   * Various formats are supported, typically jpeg, png, bmp, and wbmp.
   * To get a list of the supported formats for writing, use: <BR>
   * <TT>println(javax.imageio.ImageIO.getReaderFormatNames())</TT>
   */
  protected void saveImageIO(String path) throws IOException {
    try {
      BufferedImage bimage =
        new BufferedImage(width, height, (format == ARGB) ?
                          BufferedImage.TYPE_INT_ARGB :
                          BufferedImage.TYPE_INT_RGB);
      /*
      Class bufferedImageClass =
        Class.forName("java.awt.image.BufferedImage");
      Constructor bufferedImageConstructor =
        bufferedImageClass.getConstructor(new Class[] {
            Integer.TYPE,
            Integer.TYPE,
            Integer.TYPE });
      Field typeIntRgbField = bufferedImageClass.getField("TYPE_INT_RGB");
      int typeIntRgb = typeIntRgbField.getInt(typeIntRgbField);
      Field typeIntArgbField = bufferedImageClass.getField("TYPE_INT_ARGB");
      int typeIntArgb = typeIntArgbField.getInt(typeIntArgbField);
      Object bimage =
        bufferedImageConstructor.newInstance(new Object[] {
            new Integer(width),
            new Integer(height),
            new Integer((format == ARGB) ? typeIntArgb : typeIntRgb)
          });
      */

      bimage.setRGB(0, 0, width, height, pixels, 0, width);
      /*
      Method setRgbMethod =
        bufferedImageClass.getMethod("setRGB", new Class[] {
            Integer.TYPE, Integer.TYPE,
            Integer.TYPE, Integer.TYPE,
            pixels.getClass(),
            Integer.TYPE, Integer.TYPE
          });
      setRgbMethod.invoke(bimage, new Object[] {
          new Integer(0), new Integer(0),
          new Integer(width), new Integer(height),
          pixels, new Integer(0), new Integer(width)
        });
      */

      File file = new File(path);
      String extension = path.substring(path.lastIndexOf('.') + 1);

      ImageIO.write(bimage, extension, file);
      /*
      Class renderedImageClass =
        Class.forName("java.awt.image.RenderedImage");
      Class ioClass = Class.forName("javax.imageio.ImageIO");
      Method writeMethod =
        ioClass.getMethod("write", new Class[] {
            renderedImageClass, String.class, File.class
          });
      writeMethod.invoke(null, new Object[] { bimage, extension, file });
      */

    } catch (Exception e) {
      e.printStackTrace();
      throw new IOException("image save failed.");
    }
  }


  protected String[] saveImageFormats;

  /**
   * Saves the image into a file. Images are saved in TIFF, TARGA, JPEG, and PNG format depending on the extension within the <b>filename</b> parameter.
   * For example, "image.tif" will have a TIFF image and "image.png" will save a PNG image.
   * If no extension is included in the filename, the image will save in TIFF format and <b>.tif</b> will be added to the name.
   * These files are saved to the sketch's folder, which may be opened by selecting "Show sketch folder" from the "Sketch" menu.
   * It is not possible to use <b>save()</b> while running the program in a web browser.<br><br>
   * To save an image created within the code, rather than through loading, it's necessary to make the image with the <b>createImage()</b>
   * function so it is aware of the location of the program and can therefore save the file to the right place.
   * See the <b>createImage()</b> reference for more information.
   * 
   * =advanced
   * Save this image to disk.
   * <p>
   * As of revision 0100, this function requires an absolute path,
   * in order to avoid confusion. To save inside the sketch folder,
   * use the function savePath() from PApplet, or use saveFrame() instead.
   * As of revision 0116, savePath() is not needed if this object has been
   * created (as recommended) via createImage() or createGraphics() or
   * one of its neighbors.
   * <p>
   * As of revision 0115, when using Java 1.4 and later, you can write
   * to several formats besides tga and tiff. If Java 1.4 is installed
   * and the extension used is supported (usually png, jpg, jpeg, bmp,
   * and tiff), then those methods will be used to write the image.
   * To get a list of the supported formats for writing, use: <BR>
   * <TT>println(javax.imageio.ImageIO.getReaderFormatNames())</TT>
   * <p>
   * To use the original built-in image writers, use .tga or .tif as the
   * extension, or don't include an extension. When no extension is used,
   * the extension .tif will be added to the file name.
   * <p>
   * The ImageIO API claims to support wbmp files, however they probably
   * require a black and white image. Basic testing produced a zero-length
   * file with no error.
   * 
   * @webref
   * @brief Saves the image to a TIFF, TARGA, PNG, or JPEG file
   * @param filename a sequence of letters and numbers
   */
  public void save(String filename) {  // ignore
    boolean success = false;

    File file = new File(filename);
    if (!file.isAbsolute()) {
      if (parent != null) {
        //file = new File(parent.savePath(filename));
        filename = parent.savePath(filename);
      } else {
        String msg = "PImage.save() requires an absolute path. " +
          "Use createImage(), or pass savePath() to save().";
        PGraphics.showException(msg);
      }
    }

    // Make sure the pixel data is ready to go
    loadPixels();

    try {
      OutputStream os = null;

      if (saveImageFormats == null) {
        saveImageFormats = javax.imageio.ImageIO.getWriterFormatNames();
      }
      if (saveImageFormats != null) {
        for (int i = 0; i < saveImageFormats.length; i++) {
          if (filename.endsWith("." + saveImageFormats[i])) {
            saveImageIO(filename);
            return;
          }
        }
      }

      if (filename.toLowerCase().endsWith(".tga")) {
        os = new BufferedOutputStream(new FileOutputStream(filename), 32768);
        success = saveTGA(os); //, pixels, width, height, format);

      } else {
        if (!filename.toLowerCase().endsWith(".tif") &&
            !filename.toLowerCase().endsWith(".tiff")) {
          // if no .tif extension, add it..
          filename += ".tif";
        }
        os = new BufferedOutputStream(new FileOutputStream(filename), 32768);
        success = saveTIFF(os); //, pixels, width, height);
      }
      os.flush();
      os.close();

    } catch (IOException e) {
      //System.err.println("Error while saving image.");
      e.printStackTrace();
      success = false;
    }
    if (!success) {
      throw new RuntimeException("Error while saving image.");
    }
  }
}

