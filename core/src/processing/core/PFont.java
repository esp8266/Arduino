/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-07 Ben Fry & Casey Reas
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

import java.awt.*;
import java.awt.image.BufferedImage;
import java.awt.image.Raster;
import java.io.*;
//import java.lang.reflect.*;
import java.util.Arrays;


/**
 * Grayscale bitmap font class used by Processing.
 * <P>
 * Awful (and by that, I mean awesome) ascii (non)art for how this works:
 * <PRE>
 *   |
 *   |                   height is the full used height of the image
 *   |
 *   |   ..XX..       }
 *   |   ..XX..       }
 *   |   ......       }
 *   |   XXXX..       }  topExtent (top y is baseline - topExtent)
 *   |   ..XX..       }
 *   |   ..XX..       }  dotted areas are where the image data
 *   |   ..XX..       }  is actually located for the character
 *   +---XXXXXX----   }  (it extends to the right and down
 *   |                   for power of two texture sizes)
 *   ^^^^ leftExtent (amount to move over before drawing the image
 *
 *   ^^^^^^^^^^^^^^ setWidth (width displaced by char)
 * </PRE>
 */
public class PFont implements PConstants {

  public int charCount;
  public PImage images[];

  /**
   * Native Java version of the font. If possible, this allows the
   * PGraphics subclass to just use Java's font rendering stuff
   * in situations where that's faster.
   */
  protected Font font;
  protected boolean fontSearched;

  /**
   * Name of the font as seen by Java when it was created.
   * If the font is available, the native version will be used.
   */
  public String name;

  /**
   * Postscript name of the font that this bitmap was created from.
   */
  public String psname;

  /** "natural" size of the font (most often 48) */
  public int size;

  /** true if smoothing was enabled for this font, used for native impl */
  public boolean smooth;

  /** next power of 2 over the max image size (usually 64) */
  public int mbox2;

  /** floating point width (convenience) */
  protected float fwidth;

  /** floating point width (convenience) */
  protected float fheight;

  /** texture width, same as mbox2, but reserved for future use */
  public int twidth;

  /** texture height, same as mbox2, but reserved for future use */
  public int theight;

  public int value[];  // char code
  public int height[]; // height of the bitmap data
  public int width[];  // width of bitmap data
  public int setWidth[];  // width displaced by the char
  public int topExtent[];  // offset for the top
  public int leftExtent[];  // offset for the left

  public int ascent;
  public int descent;

  protected int ascii[];  // quick lookup for the ascii chars

  // shared by the text() functions to avoid incessant allocation of memory
  //protected char textBuffer[] = new char[8 * 1024];
  //protected char widthBuffer[] = new char[8 * 1024];
  
  static protected Font[] fonts;


  public PFont() { }  // for subclasses


  public PFont(InputStream input) throws IOException {
    DataInputStream is = new DataInputStream(input);

    // number of character images stored in this font
    charCount = is.readInt();

    // bit count is ignored since this is always 8
    //int numBits = is.readInt();
    // used to be the bitCount, but now used for version number.
    // version 8 is any font before 69, so 9 is anything from 83+
    // 9 was buggy so gonna increment to 10.
    int version = is.readInt();

    // this was formerly ignored, now it's the actual font size
    //mbox = is.readInt();
    size = is.readInt();
    // this was formerly mboxY, the one that was used
    // this will make new fonts downward compatible
    //mbox2 = is.readInt();
    mbox2 = is.readInt();

    fwidth = size; //mbox;
    fheight = size; //mbox;

    // size for image ("texture") is next power of 2
    // over the font size. for most vlw fonts, the size is 48
    // so the next power of 2 is 64.
    // double-check to make sure that mbox2 is a power of 2
    // there was a bug in the old font generator that broke this
    //mbox2 = (int) Math.pow(2, Math.ceil(Math.log(mbox2) / Math.log(2)));
    mbox2 = (int) Math.pow(2, Math.ceil(Math.log(mbox2) / Math.log(2)));
    // size for the texture is stored in the font
    twidth = theight = mbox2; //mbox2;

    ascent  = is.readInt();  // formerly baseHt (zero/ignored)
    descent = is.readInt();  // formerly ignored struct padding

    // allocate enough space for the character info
    value       = new int[charCount];
    height      = new int[charCount];
    width       = new int[charCount];
    setWidth    = new int[charCount];
    topExtent   = new int[charCount];
    leftExtent  = new int[charCount];

    ascii = new int[128];
    for (int i = 0; i < 128; i++) ascii[i] = -1;

    // read the information about the individual characters
    for (int i = 0; i < charCount; i++) {
      value[i]      = is.readInt();
      height[i]     = is.readInt();
      width[i]      = is.readInt();
      setWidth[i]   = is.readInt();
      topExtent[i]  = is.readInt();
      leftExtent[i] = is.readInt();

      // pointer in the c version, ignored
      is.readInt();

      // cache locations of the ascii charset
      if (value[i] < 128) ascii[value[i]] = i;

      // the values for getAscent() and getDescent() from FontMetrics
      // seem to be way too large.. perhaps they're the max?
      // as such, use a more traditional marker for ascent/descent
      if (value[i] == 'd') {
        if (ascent == 0) ascent = topExtent[i];
      }
      if (value[i] == 'p') {
        if (descent == 0) descent = -topExtent[i] + height[i];
      }
    }

    // not a roman font, so throw an error and ask to re-build.
    // that way can avoid a bunch of error checking hacks in here.
    if ((ascent == 0) && (descent == 0)) {
      throw new RuntimeException("Please use \"Create Font\" to " +
                                 "re-create this font.");
    }

    images = new PImage[charCount];
    for (int i = 0; i < charCount; i++) {
      images[i] = new PImage(twidth, theight, ALPHA);
      int bitmapSize = height[i] * width[i];

      byte temp[] = new byte[bitmapSize];
      is.readFully(temp);

      // convert the bitmap to an alpha channel
      int w = width[i];
      int h = height[i];
      for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
          int valu = temp[y*w + x] & 0xff;
          images[i].pixels[y * twidth + x] = valu;
          //(valu << 24) | 0xFFFFFF;  // windows
          //0xFFFFFF00 | valu;  // macosx

          //System.out.print((images[i].pixels[y*64+x] > 128) ? "*" : ".");
        }
        //System.out.println();
      }
      //System.out.println();
    }

    if (version >= 10) {  // includes the font name at the end of the file
      name = is.readUTF();
      psname = is.readUTF();
    }
    if (version == 11) {
      smooth = is.readBoolean();
    }
  }


  /**
   * Set the native complement of this font.
   */
  public void setFont(Font font) {
    this.font = font;
  }
  
  
  /**
   * Return the native java.awt.Font associated with this PFont (if any).
   */
  public Font getFont() {
//    if (font == null && !fontSearched) {
//      font = findFont();
//    }
    return font;
  }


  /**
   * Attempt to find the native version of this font.
   * (Public so that it can be used by OpenGL or other renderers.)
   */
  public Font findFont() {
    if (font == null) {
      if (!fontSearched) {
        // this font may or may not be installed
        font = new Font(name, Font.PLAIN, size);
        // if the ps name matches, then we're in fine shape
        if (!font.getPSName().equals(psname)) {
          // on osx java 1.4 (not 1.3.. ugh), you can specify the ps name
          // of the font, so try that in case this .vlw font was created on pc
          // and the name is different, but the ps name is found on the
          // java 1.4 mac that's currently running this sketch.
          font = new Font(psname, Font.PLAIN, size);
        }
        // check again, and if still bad, screw em
        if (!font.getPSName().equals(psname)) {
          font = null;
        }
        fontSearched = true;
      }
    }
    return font;
  }


  /**
   * Write this PFont to an OutputStream.
   * <p>
   * This is used by the Create Font tool, or whatever anyone else dreams
   * up for messing with fonts themselves.
   * <p>
   * It is assumed that the calling class will handle closing
   * the stream when finished.
   */
  public void save(OutputStream output) throws IOException {
    DataOutputStream os = new DataOutputStream(output);

    os.writeInt(charCount);

    if ((name == null) || (psname == null)) {
      name = "";
      psname = "";
    }
    // formerly numBits, now used for version number
    //os.writeInt((name != null) ? 11 : 8);
    os.writeInt(11);

    os.writeInt(size);    // formerly mboxX (was 64, now 48)
    os.writeInt(mbox2);   // formerly mboxY (was 64, still 64)
    os.writeInt(ascent);  // formerly baseHt (was ignored)
    os.writeInt(descent); // formerly struct padding for c version

    for (int i = 0; i < charCount; i++) {
      os.writeInt(value[i]);
      os.writeInt(height[i]);
      os.writeInt(width[i]);
      os.writeInt(setWidth[i]);
      os.writeInt(topExtent[i]);
      os.writeInt(leftExtent[i]);
      os.writeInt(0); // padding
    }

    for (int i = 0; i < charCount; i++) {
      for (int y = 0; y < height[i]; y++) {
        for (int x = 0; x < width[i]; x++) {
          os.write(images[i].pixels[y * mbox2 + x] & 0xff);
        }
      }
    }

    //if (name != null) {  // version 11
    os.writeUTF(name);
    os.writeUTF(psname);
    os.writeBoolean(smooth);
    //}

    os.flush();
  }


  /**
   * Get index for the char (convert from unicode to bagel charset).
   * @return index into arrays or -1 if not found
   */
  public int index(char c) {
    // degenerate case, but the find function will have trouble
    // if there are somehow zero chars in the lookup
    //if (value.length == 0) return -1;
    if (charCount == 0) return -1;

    // quicker lookup for the ascii fellers
    if (c < 128) return ascii[c];

    // some other unicode char, hunt it out
    //return index_hunt(c, 0, value.length-1);
    return indexHunt(c, 0, charCount-1);
  }


  protected int indexHunt(int c, int start, int stop) {
    int pivot = (start + stop) / 2;

    // if this is the char, then return it
    if (c == value[pivot]) return pivot;

    // char doesn't exist, otherwise would have been the pivot
    //if (start == stop) return -1;
    if (start >= stop) return -1;

    // if it's in the lower half, continue searching that
    if (c < value[pivot]) return indexHunt(c, start, pivot-1);

    // if it's in the upper half, continue there
    return indexHunt(c, pivot+1, stop);
  }


  /**
   * Currently un-implemented for .vlw fonts,
   * but honored for layout in case subclasses use it.
   */
  public float kern(char a, char b) {
    return 0;
  }


  /**
   * Returns the ascent of this font from the baseline.
   * The value is based on a font of size 1.
   */
  public float ascent() {
    return ((float)ascent / fheight);
  }


  /**
   * Returns how far this font descends from the baseline.
   * The value is based on a font size of 1.
   */
  public float descent() {
    return ((float)descent / fheight);
  }


  /**
   * Width of this character for a font of size 1.
   */
  public float width(char c) {
    if (c == 32) return width('i');

    int cc = index(c);
    if (cc == -1) return 0;

    return ((float)setWidth[cc] / fwidth);
  }


  //////////////////////////////////////////////////////////////


  static final char[] EXTRA_CHARS = {
    0x0080, 0x0081, 0x0082, 0x0083, 0x0084, 0x0085, 0x0086, 0x0087,
    0x0088, 0x0089, 0x008A, 0x008B, 0x008C, 0x008D, 0x008E, 0x008F,
    0x0090, 0x0091, 0x0092, 0x0093, 0x0094, 0x0095, 0x0096, 0x0097,
    0x0098, 0x0099, 0x009A, 0x009B, 0x009C, 0x009D, 0x009E, 0x009F,
    0x00A0, 0x00A1, 0x00A2, 0x00A3, 0x00A4, 0x00A5, 0x00A6, 0x00A7,
    0x00A8, 0x00A9, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00AF,
    0x00B0, 0x00B1, 0x00B4, 0x00B5, 0x00B6, 0x00B7, 0x00B8, 0x00BA,
    0x00BB, 0x00BF, 0x00C0, 0x00C1, 0x00C2, 0x00C3, 0x00C4, 0x00C5,
    0x00C6, 0x00C7, 0x00C8, 0x00C9, 0x00CA, 0x00CB, 0x00CC, 0x00CD,
    0x00CE, 0x00CF, 0x00D1, 0x00D2, 0x00D3, 0x00D4, 0x00D5, 0x00D6,
    0x00D7, 0x00D8, 0x00D9, 0x00DA, 0x00DB, 0x00DC, 0x00DD, 0x00DF,
    0x00E0, 0x00E1, 0x00E2, 0x00E3, 0x00E4, 0x00E5, 0x00E6, 0x00E7,
    0x00E8, 0x00E9, 0x00EA, 0x00EB, 0x00EC, 0x00ED, 0x00EE, 0x00EF,
    0x00F1, 0x00F2, 0x00F3, 0x00F4, 0x00F5, 0x00F6, 0x00F7, 0x00F8,
    0x00F9, 0x00FA, 0x00FB, 0x00FC, 0x00FD, 0x00FF, 0x0102, 0x0103,
    0x0104, 0x0105, 0x0106, 0x0107, 0x010C, 0x010D, 0x010E, 0x010F,
    0x0110, 0x0111, 0x0118, 0x0119, 0x011A, 0x011B, 0x0131, 0x0139,
    0x013A, 0x013D, 0x013E, 0x0141, 0x0142, 0x0143, 0x0144, 0x0147,
    0x0148, 0x0150, 0x0151, 0x0152, 0x0153, 0x0154, 0x0155, 0x0158,
    0x0159, 0x015A, 0x015B, 0x015E, 0x015F, 0x0160, 0x0161, 0x0162,
    0x0163, 0x0164, 0x0165, 0x016E, 0x016F, 0x0170, 0x0171, 0x0178,
    0x0179, 0x017A, 0x017B, 0x017C, 0x017D, 0x017E, 0x0192, 0x02C6,
    0x02C7, 0x02D8, 0x02D9, 0x02DA, 0x02DB, 0x02DC, 0x02DD, 0x03A9,
    0x03C0, 0x2013, 0x2014, 0x2018, 0x2019, 0x201A, 0x201C, 0x201D,
    0x201E, 0x2020, 0x2021, 0x2022, 0x2026, 0x2030, 0x2039, 0x203A,
    0x2044, 0x20AC, 0x2122, 0x2202, 0x2206, 0x220F, 0x2211, 0x221A,
    0x221E, 0x222B, 0x2248, 0x2260, 0x2264, 0x2265, 0x25CA, 0xF8FF,
    0xFB01, 0xFB02
  };


  /**
   * The default Processing character set.
   * <P>
   * This is the union of the Mac Roman and Windows ANSI (CP1250)
   * character sets. ISO 8859-1 Latin 1 is Unicode characters 0x80 -> 0xFF,
   * and would seem a good standard, but in practice, most P5 users would
   * rather have characters that they expect from their platform's fonts.
   * <P>
   * This is more of an interim solution until a much better
   * font solution can be determined. (i.e. create fonts on
   * the fly from some sort of vector format).
   * <P>
   * Not that I expect that to happen.
   */
  static public char[] DEFAULT_CHARSET;
  static {
    DEFAULT_CHARSET = new char[126-33+1 + EXTRA_CHARS.length];
    int index = 0;
    for (int i = 33; i <= 126; i++) {
      DEFAULT_CHARSET[index++] = (char)i;
    }
    for (int i = 0; i < EXTRA_CHARS.length; i++) {
      DEFAULT_CHARSET[index++] = EXTRA_CHARS[i];
    }
  };


  /**
   * Create a new image-based font on the fly.
   *
   * @param font the font object to create from
   * @param charset array of all unicode chars that should be included
   * @param smooth true to enable smoothing/anti-aliasing
   */
  public PFont(Font font, boolean smooth, char charset[]) {
    // save this so that we can use the native version
    this.font = font;
    this.smooth = smooth;

    name = font.getName();
    psname = font.getPSName();

    // fix regression from sorting (bug #564)
    if (charset != null) {
      // charset needs to be sorted to make index lookup run more quickly
      // http://dev.processing.org/bugs/show_bug.cgi?id=494
      Arrays.sort(charset);
    }

    // the count gets reset later based on how many of
    // the chars are actually found inside the font.
    this.charCount = (charset == null) ? 65536 : charset.length;
    this.size = font.getSize();

    fwidth = fheight = size;

    PImage bitmaps[] = new PImage[charCount];

    // allocate enough space for the character info
    value       = new int[charCount];
    height      = new int[charCount];
    width       = new int[charCount];
    setWidth    = new int[charCount];
    topExtent   = new int[charCount];
    leftExtent  = new int[charCount];

    ascii = new int[128];
    for (int i = 0; i < 128; i++) ascii[i] = -1;

    int mbox3 = size * 3;

    BufferedImage playground =
      new BufferedImage(mbox3, mbox3, BufferedImage.TYPE_INT_RGB);

    Graphics2D g = (Graphics2D) playground.getGraphics();
    g.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                       smooth ?
                       RenderingHints.VALUE_ANTIALIAS_ON :
                       RenderingHints.VALUE_ANTIALIAS_OFF);

    g.setFont(font);
    FontMetrics metrics = g.getFontMetrics();

    int samples[] = new int[mbox3 * mbox3];

    int maxWidthHeight = 0;
    int index = 0;
    for (int i = 0; i < charCount; i++) {
      char c = (charset == null) ? (char)i : charset[i];

      if (!font.canDisplay(c)) {  // skip chars not in the font
        continue;
      }

      g.setColor(Color.white);
      g.fillRect(0, 0, mbox3, mbox3);
      g.setColor(Color.black);
      g.drawString(String.valueOf(c), size, size * 2);

      // grabs copy of the current data.. so no updates (do each time)
      Raster raster = playground.getData();
      raster.getSamples(0, 0, mbox3, mbox3, 0, samples);

      int minX = 1000, maxX = 0;
      int minY = 1000, maxY = 0;
      boolean pixelFound = false;

      for (int y = 0; y < mbox3; y++) {
        for (int x = 0; x < mbox3; x++) {
          //int sample = raster.getSample(x, y, 0);  // maybe?
          int sample = samples[y * mbox3 + x] & 0xff;
          // or int samples[] = raster.getPixel(x, y, null);

          //if (sample == 0) {  // or just not white? hmm
          if (sample != 255) {
            if (x < minX) minX = x;
            if (y < minY) minY = y;
            if (x > maxX) maxX = x;
            if (y > maxY) maxY = y;
            pixelFound = true;
          }
        }
      }

      if (!pixelFound) {
        minX = minY = 0;
        maxX = maxY = 0;
        // this will create a 1 pixel white (clear) character..
        // maybe better to set one to -1 so nothing is added?
      }

      value[index] = c;
      height[index] = (maxY - minY) + 1;
      width[index] = (maxX - minX) + 1;
      setWidth[index] = metrics.charWidth(c);
      //System.out.println((char)c + " " + setWidth[index]);

      // cache locations of the ascii charset
      //if (value[i] < 128) ascii[value[i]] = i;
      if (c < 128) ascii[c] = index;

      // offset from vertical location of baseline
      // of where the char was drawn (size*2)
      topExtent[index] = size*2 - minY;

      // offset from left of where coord was drawn
      leftExtent[index] = minX - size;

      if (c == 'd') {
        ascent = topExtent[index];
      }
      if (c == 'p') {
        descent = -topExtent[index] + height[index];
      }

      if (width[index] > maxWidthHeight) maxWidthHeight = width[index];
      if (height[index] > maxWidthHeight) maxWidthHeight = height[index];

      bitmaps[index] = new PImage(width[index], height[index], ALPHA);

      for (int y = minY; y <= maxY; y++) {
        for (int x = minX; x <= maxX; x++) {
          int val = 255 - (samples[y * mbox3 + x] & 0xff);
          int pindex = (y - minY) * width[index] + (x - minX);
          bitmaps[index].pixels[pindex] = val;
        }
      }
      index++;
    }
    charCount = index;

    // foreign font, so just make ascent the max topExtent
    if ((ascent == 0) && (descent == 0)) {
      for (int i = 0; i < charCount; i++) {
        char cc = (char) value[i];
        if (Character.isWhitespace(cc) ||
            (cc == '\u00A0') || (cc == '\u2007') || (cc == '\u202F')) {
          continue;
        }
        if (topExtent[i] > ascent) {
          ascent = topExtent[i];
        }
        int d = -topExtent[i] + height[i];
        if (d > descent) {
          descent = d;
        }
      }
    }
    // size for image/texture is next power of 2 over largest char
    mbox2 = (int)
      Math.pow(2, Math.ceil(Math.log(maxWidthHeight) / Math.log(2)));
    twidth = theight = mbox2;

    // shove the smaller PImage data into textures of next-power-of-2 size,
    // so that this font can be used immediately by p5.
    images = new PImage[charCount];
    for (int i = 0; i < charCount; i++) {
      images[i] = new PImage(mbox2, mbox2, ALPHA);
      for (int y = 0; y < height[i]; y++) {
        System.arraycopy(bitmaps[i].pixels, y*width[i],
                         images[i].pixels, y*mbox2,
                         width[i]);
      }
      bitmaps[i] = null;
    }
  }


  /**
   * Get a list of the fonts installed on the system that can be used
   * by Java. Not all fonts can be used in Java, in fact it's mostly
   * only TrueType fonts. OpenType fonts with CFF data such as Adobe's
   * OpenType fonts seem to have trouble (even though they're sort of
   * TrueType fonts as well, or may have a .ttf extension). Regular
   * PostScript fonts seem to work OK, however.
   * <P>
   * Not recommended for use in applets, but this is implemented
   * in PFont because the Java methods to access this information
   * have changed between 1.1 and 1.4, and the 1.4 method is
   * typical of the sort of undergraduate-level over-abstraction
   * that the seems to have made its way into the Java API after 1.1.
   */
  static public String[] list() {
    loadFonts();
    String list[] = new String[fonts.length];
    for (int i = 0; i < list.length; i++) {
      list[i] = fonts[i].getName();
    }
    return list;
  }
  

  static public void loadFonts() {
    if (fonts == null) {
      GraphicsEnvironment ge =
        GraphicsEnvironment.getLocalGraphicsEnvironment();
      fonts = ge.getAllFonts();
    }
  }


  /** 
   * Starting with Java 1.5, Apple broke the ability to specify most fonts.
   * This has been filed as bug #4769141 at bugreporter.apple.com. More info at
   * <a href="http://dev.processing.org/bugs/show_bug.cgi?id=407">Bug 407</a>.
  */
  static public Font findFont(String name) {
    loadFonts();
    if (PApplet.platform == PConstants.MACOSX) {
      for (int i = 0; i < fonts.length; i++) {
        if (name.equals(fonts[i].getName())) {
          return fonts[i];
        }
      }
    }
    return new Font(name, Font.PLAIN, 1);
  }
}
