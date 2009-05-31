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

package processing.pdf;

import java.io.*;
import java.util.*;

import com.lowagie.text.*;
import com.lowagie.text.pdf.*;

import processing.core.*;


public class PGraphicsPDF extends PGraphicsJava2D {

  File temp;
  File file;
  Document document;
  PdfWriter writer;
  PdfContentByte content;
//  PdfTemplate template;
  DefaultFontMapper mapper;

  // BaseFont baseFont = mapper.awtToPdf(java.awt.Font awtFont)


  public PGraphicsPDF() { }
  
  
  public void setPath(String path) {
	this.path = path;
    if (path != null) {
      file = new File(path);
      if (!file.isAbsolute()) file = null;
    }
    if (file == null) {
      throw new RuntimeException("PGraphicsPDF requires an absolute path " +
                                 "for the location of the output file.");
    }
  }

    //if (applet != null) {
    //  applet.registerDispose(this);
    //}

    //System.out.println("making " + path);

    //if (path == null) path = "output.pdf";
    //this.file = new File(path);

    // don't want to require PApplet as the way to do this.. but how?
    //if (applet != null) {
    //applet.registerDispose(this);
    //}

    /*
    mapper = new DefaultFontMapper();
    FontFactory.registerDirectories();

    // ummm.. problematic?
    //mapper.insertDirectory("c:\\winxp\\fonts");
    mapper.insertDirectory("/System/Library/Fonts");
    mapper.insertDirectory("/Library/Fonts");
    mapper.insertDirectory("/Users/fry/Library/Fonts");
    */

    // seems to only pick up ttf and otf fonts
    //FontFactory.registerDirectory("/System/Library/Fonts");
    //FontFactory.registerDirectory("/Library/Fonts");
    //FontFactory.registerDirectory("/Users/fry/Library/Fonts");

    /*
    Set registered = FontFactory.getRegisteredFonts();
    for (Iterator i = registered.iterator(); i.hasNext(); ) {
      System.out.println((String) i.next());
    }
    */


//  public void setPrimarySurface() {
//    // set as main drawing surface
//    primarySurface = true;
//    // this shouldn't actually affect anything
//    format = RGB;
//    // don't bother adding listeners for this guy
//    //parent.addListeners();
//  }


  // create a temporary file and put the graphics crap there
  // don't start a fresh page if frameCount is zero (setup isn't its own page)

  /**
   * all the init stuff happens in here, in case someone calls size()
   * along the way and wants to hork things up.
   */
  protected void allocate() {
    // can't do anything here, because this will be called by the
    // superclass PGraphics, and the file/path object won't be set yet
    // (since super() called right at the beginning of the constructor)
  }


  /*
  public void defaults() {
    System.out.println("PGraphicsPDF.defaults()");
    super.defaults();
  }
  */


  // if the File object changes, then need to start a new file
  //
  /*
  public void record(int frameCount, File ifile) {
    this.frameCount = frameCount;
    if (ifile == file) {
      // same shit, different pile
      // start a new page on the file that's currently open
      return;

    } else {

    if (!file.getName().endsWith(".pdf")) {
      // yeaeaargh
    }
  }
  */

  public void beginDraw() {
    // temporary
    //file = new File(filename); //"test.pdf");
    //System.out.println("pdf beginDraw()");
    //document = new Document();

    if (document == null) {
      document = new Document(new Rectangle(width, height));
      try {
        FileOutputStream fos = new FileOutputStream(file);
        BufferedOutputStream bos = new BufferedOutputStream(fos, 16384);
        writer = PdfWriter.getInstance(document, bos);
        document.open();
        content = writer.getDirectContent();
//        template = content.createTemplate(width, height);

      } catch (Exception e) {
        e.printStackTrace();
        throw new RuntimeException("Problem saving the PDF file.");
      }

      // how to call newPage() in here?
      /*
        System.out.println("beginDraw() " + width + ", " + height);
        tp = content.createTemplate(width, height);
        //g2 = tp.createGraphics(width, height, mapper);
        g2 = tp.createGraphicsShapes(width, height);
        //System.out.println("g2 is " + g2);
        tp.setWidth(width);
        tp.setHeight(height);
      */

      // what's a good way to switch between these?
      // the regular createGraphics doesn't seem to recognize fonts
      // how should the insertDirectory stuff be used properly?
      //g2 = content.createGraphics(width, height);
//      g2 = content.createGraphicsShapes(width, height);

      mapper = new DefaultFontMapper();
      //System.out.println("registering directories");
      //FontFactory.registerDirectories();
      //mapper.insertDirectory("c:\\windows\\fonts");
      //System.out.println("done registering directories");

      if (PApplet.platform == PApplet.MACOSX) {
        try {
          String homeLibraryFonts =
            System.getProperty("user.home") + "/Library/Fonts";
          mapper.insertDirectory(homeLibraryFonts);
        } catch (Exception e) {
          // might be a security issue with getProperty() and user.home
          // if this sketch is running from the web
        }
        // add the system font paths
        mapper.insertDirectory("/System/Library/Fonts");
        mapper.insertDirectory("/Library/Fonts");

      } else if (PApplet.platform == PApplet.WINDOWS) {
        // how to get the windows fonts directory?
        // could be c:\winnt\fonts or c:\windows\fonts or not even c:
        // maybe do a Runtime.exec() on echo %WINDIR% ?
        // Runtime.exec solution might be a mess on systems where the
        // the backslash/colon characters not really used (i.e. JP)

        // find the windows fonts folder
        File roots[] = File.listRoots();
        /*
        PApplet.println(roots);
        roots = new File[] { new File("A:\\"),
                             new File("C:\\"),
                             new File("D:\\") };
        PApplet.println(roots);
        */
        for (int i = 0; i < roots.length; i++) {
          if (roots[i].toString().startsWith("A:")) {
            // Seems to be a problem with some machines that the A:
            // drive is returned as an actual root, even if not available.
            // This won't fix the issue if the same thing happens with
            // other removable drive devices, but should fix the
            // initial/problem as cited by the bug report:
            // http://dev.processing.org/bugs/show_bug.cgi?id=478
            // If not, will need to use the other fileExists() code below.
            continue;
          }

          File folder = new File(roots[i], "WINDOWS/Fonts");
          if (folder.exists()) {
            mapper.insertDirectory(folder.getAbsolutePath());
            break;
          }
          folder = new File(roots[i], "WINNT/Fonts");
          if (folder.exists()) {
            mapper.insertDirectory(folder.getAbsolutePath());
            break;
          }
        }
      }
      g2 = content.createGraphics(width, height, mapper);
//      g2 = template.createGraphics(width, height, mapper);
    }
    super.beginDraw();
  }

  
   public void endDraw() {
     // This needs to be overridden so that the endDraw() from PGraphicsJava2D
     // is not inherited (it calls loadPixels).
     // http://dev.processing.org/bugs/show_bug.cgi?id=1169
   }


  /**
   * Gives the same basic functionality of File.exists but can be
   * used to look for removable media without showing a system
   * dialog if the media is not present. Workaround pulled from the
   * <A HREF="http://bugs.sun.com/bugdatabase/view_bug.do?bug_id=4089199">
   * bug report</A> on bugs.sun.com. This bug was fixed in Java 6, and we
   * can remove the workaround when we start requiring Java 6.  
   */
  protected static boolean fileExists(File file) {
    try {
      Process process =
        Runtime.getRuntime().exec(new String[] {
            "cmd.exe", "/c", "dir", file.getAbsolutePath()
          });

      // We need to consume all available output or the process will block.
      boolean haveExitCode = false;
      int exitCode = -1;
      InputStream out = process.getInputStream();
      InputStream err = process.getErrorStream();

      while (!haveExitCode) {
        while (out.read() >= 0) {
        }
        while (err.read() >= 0) {
        }

        try {
          exitCode = process.exitValue();
          haveExitCode = true;
        } catch ( IllegalThreadStateException e ) {
          // Not yet complete.
          Thread.sleep(100);
        }
      }
      //int exitCode = process.waitFor();
      return exitCode == 0;

    } catch (IOException e) {
      System.out.println("Unable to check for file: " + file + " : " + e);
      return false;

    } catch ( InterruptedException e ) {
      System.out.println("Unable to check for file.  Interrupted: " +
                         file + " : " + e);
      return false;
    }
  }


  /**
   * Change the textMode() to either SHAPE or MODEL.
   * <br/>
   * This resets all renderer settings, and should therefore
   * be called <EM>before</EM> any other commands that set the fill()
   * or the textFont() or anything. Unlike other renderers,
   * use textMode() directly after the size() command.
   */
  public void textMode(int mode) {
    if (textMode != mode) {
      if (mode == SHAPE) {
        g2.dispose();
        g2 = content.createGraphicsShapes(width, height);
      } else if (mode == MODEL) {
        g2.dispose();
        g2 = content.createGraphics(width, height, mapper);
//        g2 = template.createGraphics(width, height, mapper);
      } else if (mode == SCREEN) {
        throw new RuntimeException("textMode(SCREEN) not supported with PDF");
      } else {
        throw new RuntimeException("That textMode() does not exist");
      }
    }
  }


  /**
   * Call to explicitly go to the next page from within a single draw().
   */
  public void nextPage() {
	  PStyle savedStyle = getStyle();
    g2.dispose();
    
    try {
//    writer.setPageEmpty(false);  // maybe useful later
      document.newPage();  // is this bad if no addl pages are made?
    } catch (Exception e) {
      e.printStackTrace();
    }
    if (textMode == SHAPE) {
      g2 = content.createGraphicsShapes(width, height);
    } else if (textMode == MODEL) {
      g2 = content.createGraphics(width, height, mapper);
    }
    style(savedStyle);

    // should there be a beginDraw/endDraw in here?
  }


  public void dispose() {
    if (document != null) {
      g2.dispose();
      document.close();  // can't be done in finalize, not always called
      document = null;
    }
    //new Exception().printStackTrace(System.out);
  }


  /**
   * Don't open a window for this renderer, it won't be used.
   */
  public boolean displayable() {
    return false;
  }

  /*
  protected void finalize() throws Throwable {
    System.out.println("calling finalize");
  //document.close();  // do this in dispose instead?
  }
  */


  //////////////////////////////////////////////////////////////


  /*
  public void endRecord() {
    super.endRecord();
    dispose();
  }


  public void endRaw() {
    System.out.println("ending raw");
    super.endRaw();
    System.out.println("disposing");
    dispose();
    System.out.println("done");
  }
  */


  //////////////////////////////////////////////////////////////


  /*
  protected void rectImpl(float x1, float y1, float x2, float y2) {
    //rect.setFrame(x1, y1, x2-x1, y2-y1);
    //draw_shape(rect);
    System.out.println("rect implements");
    g2.fillRect((int)x1, (int)y1, (int) (x2-x1), (int) (y2-y1));
  }
  *

  /*
  public void clear() {
    g2.setColor(Color.red);
    g2.fillRect(0, 0, width, height);
  }
  */


  //////////////////////////////////////////////////////////////


  /*
  protected void imageImplAWT(java.awt.Image awtImage,
                              float x1, float y1, float x2, float y2,
                              int u1, int v1, int u2, int v2) {
    pushMatrix();
    translate(x1, y1);
    int awtImageWidth = awtImage.getWidth(null);
    int awtImageHeight = awtImage.getHeight(null);
    scale((x2 - x1) / (float)awtImageWidth,
          (y2 - y1) / (float)awtImageHeight);
    g2.drawImage(awtImage,
                 0, 0, awtImageWidth, awtImageHeight,
                 u1, v1, u2, v2, null);
    popMatrix();
  }
  */


  //////////////////////////////////////////////////////////////


  public void loadPixels() {
    nope("loadPixels");
  }

  public void updatePixels() {
    nope("updatePixels");
  }

  public void updatePixels(int x, int y, int c, int d) {
    nope("updatePixels");
  }

  //

  public int get(int x, int y) {
    nope("get");
    return 0;  // not reached
  }

  public PImage get(int x, int y, int c, int d) {
    nope("get");
    return null;  // not reached
  }

  public PImage get() {
    nope("get");
    return null;  // not reached
  }

  public void set(int x, int y, int argb) {
    nope("set");
  }

  public void set(int x, int y, PImage image) {
    nope("set");
  }

  //

  public void mask(int alpha[]) {
    nope("mask");
  }

  public void mask(PImage alpha) {
    nope("mask");
  }

  //

  public void filter(int kind) {
    nope("filter");
  }

  public void filter(int kind, float param) {
    nope("filter");
  }

  //

  public void copy(int sx1, int sy1, int sx2, int sy2,
                   int dx1, int dy1, int dx2, int dy2) {
    nope("copy");
  }

  public void copy(PImage src,
                   int sx1, int sy1, int sx2, int sy2,
                   int dx1, int dy1, int dx2, int dy2) {
    nope("copy");
  }

  //

  public void blend(int sx, int sy, int dx, int dy, int mode) {
    nope("blend");
  }

  public void blend(PImage src,
                    int sx, int sy, int dx, int dy, int mode) {
    nope("blend");
  }

  public void blend(int sx1, int sy1, int sx2, int sy2,
                    int dx1, int dy1, int dx2, int dy2, int mode) {
    nope("blend");
  }

  public void blend(PImage src,
                    int sx1, int sy1, int sx2, int sy2,
                    int dx1, int dy1, int dx2, int dy2, int mode) {
    nope("blend");
  }

  //

  public void save(String filename) {
    nope("save");
  }


  //////////////////////////////////////////////////////////////


  /**
   * Add a directory that should be searched for font data.
   * <br/>
   * On Mac OS X, the following directories are added by default:
   * <UL>
   * <LI>/System/Library/Fonts
   * <LI>/Library/Fonts
   * <LI>~/Library/Fonts
   * </UL>
   * On Windows, all drive letters are searched for WINDOWS\Fonts
   * or WINNT\Fonts, any that exists is added.
   * <br/><br/>
   * On Linux or any other platform, you'll need to add the
   * directories by hand. (If there are actual standards here that we
   * can use as a starting point, please file a bug to make a note of it)
   */
  public void addFonts(String directory) {
    mapper.insertDirectory(directory);
  }


  /**
   * List the fonts known to the PDF renderer. This is like PFont.list(),
   * however not all those fonts are available by default.
   */
  public String[] listFonts() {
     /*
      //System.out.println("list of fonts");
      HashMap map = mapper.getAliases();
      //KeySet keys = map.keySet();
      Set entries = map.entrySet();
      Iterator it = entries.iterator();
      while (it.hasNext()) {
        Map.Entry entry = (Map.Entry) it.next();
        System.out.println(entry.getKey() + "-->" + entry.getValue());
      }
     */

     /*
    HashMap map = mapper.getAliases();
    KeySet keys = map.keySet();
    Iterator it = entries.iterator();
    while (it.hasNext()) {
      Map.Entry entry = (Map.Entry) it.next();
      System.out.println(entry.getKey() + "-->" + entry.getValue());
    }
     */

    HashMap map = mapper.getAliases();
    Set entries = map.entrySet();
    String list[] = new String[entries.size()];
    Iterator it = entries.iterator();
    int count = 0;
    while (it.hasNext()) {
      Map.Entry entry = (Map.Entry) it.next();
      //System.out.println(entry.getKey() + "-->" + entry.getValue());
      list[count++] = (String) entry.getKey();
    }
    return PApplet.sort(list);
  }


  //////////////////////////////////////////////////////////////


  protected void nope(String function) {
    throw new RuntimeException("No " + function + "() for PGraphicsPDF");
  }
}
