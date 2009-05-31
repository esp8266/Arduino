/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-08 Ben Fry and Casey Reas
  The previous version of this code was developed by Hernando Barragan

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

package processing.video;
import processing.core.*;

import java.lang.reflect.*;

import quicktime.*;
import quicktime.qd.*;
import quicktime.std.*;
import quicktime.std.sg.*;
import quicktime.util.RawEncodedImage;


/**
 * Watchin' shit on the telly.
 */
public class Capture extends PImage implements Runnable {

  // there are more, but these are all we'll provide for now
  // The useful ref page for <a href="http://developer.apple.com/documentation/Java/Reference/1.4.1/Java141API_QTJ/constant-values.html">quicktime constants</a>
  static public final int COMPOSITE = StdQTConstants.compositeIn;  // 0
  static public final int SVIDEO = StdQTConstants.sVideoIn;  // 1
  static public final int COMPONENT = StdQTConstants.rgbComponentIn;  // 2
  static public final int TUNER = StdQTConstants.tvTunerIn;  // 6

  static public final int NTSC = StdQTConstants.ntscIn;
  static public final int PAL = StdQTConstants.palIn;
  static public final int SECAM = StdQTConstants.secamIn;

  // no longer needed because parent field added to PImage
  //PApplet parent;

  Method captureEventMethod;
  String name; // keep track for error messages (unused)
  Thread runner;

  boolean available = false;

  /** Temporary storage for the raw image
      data read directly from the capture device */
  public int data[];

  public int dataWidth;
  public int dataHeight;
  public int dataRowBytes;

  /** True if this image is currently being cropped */
  public boolean crop;

  public int cropX;
  public int cropY;
  public int cropW;
  public int cropH;

  public int frameRate;

  public RawEncodedImage raw;
  public SequenceGrabber capture;

  /** the guy who's doing all the work */
  public SGVideoChannel channel;

  /** boundary of image at the requested size */
  protected QDRect qdrect;

    /*
  static {
    try {
      QTSession.open();
    } catch (QTException e) {
      e.printStackTrace();
    }
      // this doesn't appear to do jack
    QTRuntimeException.registerHandler(new QTRuntimeHandler() {
        public void exceptionOccurred(QTRuntimeException e,
                                      Object obj, String s, boolean flag) {
          System.err.println("Problem inside Capture");
          e.printStackTrace();
        }
      });
  }
    */


  public Capture(PApplet parent, int requestWidth, int requestHeight) {
    this(parent, requestWidth, requestHeight, null, 30);
  }

  public Capture(PApplet parent, int reqWidth, int reqHeight, int frameRate) {
    this(parent, reqWidth, reqHeight, null, frameRate);
  }

  public Capture(PApplet parent, int reqWidth, int reqHeight, String name) {
    this(parent, reqWidth, reqHeight, name, 30);
  }


  /**
   * If 'name' is null or the empty string, it won't set a specific
   * device, which means that QuickTime will use that last device
   * used by a QuickTime application.
   * <P/>
   * Unfortunately, Apple's QuickTime API uses the name to select devices,
   * and in some cases there might be cameras with the same name on a machine.
   * If you ask for a camera of the same name in sequence, you might see if it
   * just does the right thing and grabs each separate camera in succession.
   * If that doesn't work, you might try calling settings() which will
   * bring up the prompt where you can select a capture device.
   * <P/>
   * If the following function:
   * <PRE>public void captureEvent(Capture c)</PRE>
   * is defined in the host PApplet, then it will be called every
   * time a new frame is available from the capture device.
   */
  public Capture(final PApplet parent,
                 final int requestWidth, final int requestHeight,
                 final String name, final int frameRate) {
      // Running on EDT because of weird hang on OS X
      // http://dev.processing.org/bugs/show_bug.cgi?id=882
      // QTSession.open() is hanging, not sure why, but it seems to prefer
      // being run from the EDT. Not sure if that's a mistaken expectation in
      // QTJava (we hadn't had trouble in the past because we did everything
      // on the EDT) or if something broken in more recent QTJ. Or (maybe most
      // likely) we're simply hitting some other threading strangeness, and
      // using invokeLater() isolates us from that. Which is a nice way of
      // saying that it's a hack.
    //SwingUtilities.invokeLater(new Runnable() {
    //  public void run() {
    init(parent, requestWidth, requestHeight, name, frameRate);
    //}
    //});
  }


  public void init(PApplet parent, int requestWidth, int requestHeight,
                   String name, int frameRate) {
    this.parent = parent;
    this.name = name;
    this.frameRate = frameRate;

    try {
      QTSession.open();
    } catch (QTException e) {
      e.printStackTrace(System.out);
      return;
    }

    try {
      qdrect = new QDRect(requestWidth, requestHeight);
      // workaround for bug with the intel macs
      QDGraphics qdgraphics = null; //new QDGraphics(qdrect);
      if (quicktime.util.EndianOrder.isNativeLittleEndian()) {
        qdgraphics = new QDGraphics(QDConstants.k32BGRAPixelFormat, qdrect);
      } else {
        qdgraphics = new QDGraphics(QDGraphics.kDefaultPixelFormat, qdrect);
      }

      capture = new SequenceGrabber();
      capture.setGWorld(qdgraphics, null);

      channel = new SGVideoChannel(capture);
      channel.setBounds(qdrect);
      channel.setUsage(2);  // what is this usage number?
      capture.startPreview();  // maybe this comes later?

      PixMap pixmap = qdgraphics.getPixMap();
      raw = pixmap.getPixelData();

      /*
      if (name == null) {
        channel.settingsDialog();

      } else if (name.length() > 0) {
        channel.setDevice(name);
      }
      */
      if ((name != null) && (name.length() > 0)) {
        channel.setDevice(name);
      }

      dataRowBytes = raw.getRowBytes();
      dataWidth = dataRowBytes / 4;
      dataHeight = raw.getSize() / dataRowBytes;

      if (dataWidth != requestWidth) {
        crop = true;
        cropX = 0;
        cropY = 0;
        cropW = requestWidth;
        cropH = requestHeight;
      }
      // initialize my PImage self
      super.init(requestWidth, requestHeight, RGB);

      parent.registerDispose(this);

      try {
        captureEventMethod =
          parent.getClass().getMethod("captureEvent",
                                      new Class[] { Capture.class });
      } catch (Exception e) {
        // no such method, or an error.. which is fine, just ignore
      }

      runner = new Thread(this);
      runner.start();

    } catch (QTException qte) {
      //} catch (StdQTException qte) {
      //qte.printStackTrace();

      int errorCode = qte.errorCode();
      if (errorCode == Errors.couldntGetRequiredComponent) {
        // this can happen when the capture device isn't available
        // or wasn't shut down properly
        parent.die("No capture could be found, " +
                   "or the VDIG is not installed correctly.", qte);
      } else {
        parent.die("Error while setting up Capture", qte);
      }
    } catch (Exception e) {
      parent.die("Error while setting up Capture", e);
    }
  }


  /**
   * True if a frame is ready to be read.
   * <PRE>
   * // put this somewhere inside draw
   * if (capture.available()) capture.read();
   * </PRE>
   * Alternatively, you can use captureEvent(Capture c) to notify you
   * whenever available() is set to true. In which case, things might
   * look like this:
   * <PRE>
   * public void captureEvent(Capture c) {
   *   c.read();
   *   // do something exciting now that c has been updated
   * }
   * </PRE>
   */
  public boolean available() {
    return available;
  }


  /**
   * Set the video to crop from its original.
   * <P>
   * It seems common that captures add lines to the top or bottom
   * of an image, so this can be useful for removing them.
   * Internally, the pixel buffer size returned from QuickTime is
   * often a different size than requested, so crop will be set
   * more often than not.
   */
  public void crop(int x, int y, int w, int h) {
    /*
    if (imageMode == CORNERS) {
      w -= x;  // w was actually x2
      h -= y;  // h was actually y2
    }
    */

    crop = true;
    cropX = Math.max(0, x);
    cropY = Math.max(0, y);
    cropW = Math.min(w, dataWidth);
    cropH = Math.min(dataHeight, y + h) - cropY;

    // if size has changed, re-init this image
    if ((cropW != width) || (cropH != height)) {
      init(w, h, RGB);
    }
  }


  /**
   * Remove the cropping (if any) of the image.
   * <P>
   * By default, cropping is often enabled to trim out black pixels.
   * But if you'd rather deal with them yourself (so as to avoid
   * an extra lag while the data is moved around) you can shut it off.
   */
  public void noCrop() {
    crop = false;
  }


  public void read() {
    //try {
    //synchronized (capture) {
    loadPixels();
    synchronized (pixels) {
      //System.out.println("read1");
      if (crop) {
        //System.out.println("read2a");
        // f#$)(#$ing quicktime / jni is so g-d slow, calling copyToArray
        // for the invidual rows is literally 100x slower. instead, first
        // copy the entire buffer to a separate array (i didn't need that
        // memory anyway), and do an arraycopy for each row.
        if (data == null) {
          data = new int[dataWidth * dataHeight];
        }
        raw.copyToArray(0, data, 0, dataWidth * dataHeight);
        int sourceOffset = cropX + cropY*dataWidth;
        int destOffset = 0;
        for (int y = 0; y < cropH; y++) {
          System.arraycopy(data, sourceOffset, pixels, destOffset, cropW);
          sourceOffset += dataWidth;
          destOffset += width;
        }
      } else {  // no crop, just copy directly
        //System.out.println("read2b");
        raw.copyToArray(0, pixels, 0, width * height);
      }
      //System.out.println("read3");

      available = false;
      // mark this image as modified so that PGraphicsJava2D and
      // PGraphicsOpenGL will properly re-blit and draw this guy
      updatePixels();
      //System.out.println("read4");
    }
  }


  public void run() {
    while ((Thread.currentThread() == runner) && (capture != null)) {
      try {
        synchronized (capture) {
          capture.idle();
          //read();
          available = true;

          if (captureEventMethod != null) {
            try {
              captureEventMethod.invoke(parent, new Object[] { this });
            } catch (Exception e) {
              System.err.println("Disabling captureEvent() for " + name +
                                 " because of an error.");
              e.printStackTrace();
              captureEventMethod = null;
            }
          }
        }

      } catch (QTException e) {
        errorMessage("run", e);
      }

      try {
        Thread.sleep(1000 / frameRate);
      } catch (InterruptedException e) { }
    }
  }


  /**
   * Set the frameRate for how quickly new frames are read
   * from the capture device.
   */
  public void frameRate(int iframeRate) {
    if (iframeRate <= 0) {
      System.err.println("Capture: ignoring bad frameRate of " +
                         iframeRate + " fps.");
      return;
    }
    frameRate = iframeRate;
  }


  /**
   * Called by applets to stop capturing video.
   */
  public void stop() {
    if (capture != null) {
      try {
        capture.stop(); // stop the "preview"
      } catch (StdQTException e) {
        e.printStackTrace();
      }
      capture = null;
    }
    runner = null; // unwind the thread
  }


  /**
   * Called by PApplet to shut down video so that QuickTime
   * can be used later by another applet.
   */
  public void dispose() {
    stop();
    //System.out.println("calling dispose");
    // this is important so that the next app can do video
    QTSession.close();
  }


  /**
   * General error reporting, all corraled here just in case
   * I think of something slightly more intelligent to do.
   */
  protected void errorMessage(String where, Exception e) {
    parent.die("Error inside Capture." + where + "()", e);
  }


  /**
   * Set the format to ask for from the video digitizer:
   * TUNER, COMPOSITE, SVIDEO, or COMPONENT.
   * <P>
   * The constants are just aliases to the constants returned from
   * QuickTime's getInputFormat() function, so any valid constant from
   * that will work just fine.
   */
  public void source(int which) {
    try {
      VideoDigitizer digitizer = channel.getDigitizerComponent();
      int count = digitizer.getNumberOfInputs();
      for (int i = 0; i < count; i++) {
        //System.out.println("format " + digitizer.getInputFormat(i));
        if (digitizer.getInputFormat(i) == which) {
          digitizer.setInput(i);
          return;
        }
      }
      throw new RuntimeException("The specified source() is not available.");

    } catch (StdQTException e) {
      e.printStackTrace();
      throw new RuntimeException("Could not set the video input source.");
    }
  }


  /**
   * Set the video format standard to use on the
   * video digitizer: NTSC, PAL, or SECAM.
   * <P>
   * The constants are just aliases to the constants used for
   * QuickTime's setInputStandard() function, so any valid
   * constant from that will work just fine.
   */
  public void format(int which) {
    try {
      VideoDigitizer digitizer = channel.getDigitizerComponent();
      digitizer.setInputStandard(which);
    } catch (StdQTException e) {
      e.printStackTrace();
      //throw new RuntimeException("Could not set the video input format");
    }
  }


  /**
   * Show the settings dialog for this input device.
   */
  public void settings() {
    try {
      // fix for crash here submitted by hansi (stop/startPreview lines)
      capture.stop();

      // Whenever settingsDialog() is called, the boundries change,
      // causing the image to be cropped. Fix for Bug #366
      // http://dev.processing.org/bugs/show_bug.cgi?id=366
      channel.setBounds(qdrect);

      // Open the settings dialog (throws an Exception if canceled)
      channel.settingsDialog();

    } catch (StdQTException qte) {
      int errorCode = qte.errorCode();
      if (errorCode == Errors.userCanceledErr) {
        // User only canceled the settings dialog, continue as we were
      } else {
        qte.printStackTrace();
        throw new RuntimeException("Error inside Capture.settings()");
      }
    }
    try {
      // Start the preview again (unreachable if newly thrown exception)
      capture.startPreview();
    } catch (StdQTException qte) {
      qte.printStackTrace();
    }
  }


  /**
   * Get a list of all available captures as a String array.
   * i.e. println(Capture.list()) will show you the goodies.
   */
  static public String[] list() {
    try {
      QTSession.open();
      SequenceGrabber grabber = new SequenceGrabber();
      SGVideoChannel channel = new SGVideoChannel(grabber);

      SGDeviceList deviceList = channel.getDeviceList(0);  // flags is 0
      String listing[] = new String[deviceList.getCount()];
      for (int i = 0; i < deviceList.getCount(); i++) {
        listing[i] = deviceList.getDeviceName(i).getName();
      }
      // properly shut down the channel so the app can use it again
      grabber.disposeChannel(channel);
      QTSession.close();
      return listing;

    } catch (QTException qte) {
      int errorCode = qte.errorCode();
      if (errorCode == Errors.couldntGetRequiredComponent) {
        throw new RuntimeException("Couldn't find any capture devices, " +
                                   "read the video reference for more info.");
      } else {
        qte.printStackTrace();
        throw new RuntimeException("Problem listing capture devices, " +
                                   "read the video reference for more info.");
      }
    }
    //return null;
  }
}
