/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2006 Daniel Shiffman
  With minor modifications by Ben Fry for Processing 0125+

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License.

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

import java.io.File;

import quicktime.Errors;
import quicktime.QTException;
import quicktime.QTSession;
import quicktime.io.OpenMovieFile;
import quicktime.io.QTFile;
import quicktime.qd.QDConstants;
import quicktime.qd.QDGraphics;
import quicktime.qd.QDRect;
import quicktime.std.StdQTConstants;
import quicktime.std.StdQTException;
import quicktime.std.image.CSequence;
import quicktime.std.image.CodecComponent;
import quicktime.std.image.CompressedFrameInfo;
import quicktime.std.image.ImageDescription;
import quicktime.std.image.QTImage;
import quicktime.std.movies.Movie;
import quicktime.std.movies.Track;
import quicktime.std.movies.media.VideoMedia;
import quicktime.util.QTHandle;
import quicktime.util.QTUtils;
import quicktime.util.RawEncodedImage;

import processing.core.*;


/**
 * Library to create a QuickTime movie from a Processing pixel array.
 * Written by <A HREF="http://www.shiffman.net">Daniel Shiffman</A>.
 * Thanks to Dan O'Sullivan and Shawn Van Every.
 * <BR> <BR>
 * Please note that some constructors and variable names were altered
 * slightly when the library was added to the Processing distribution.
 * <PRE>
 * // Declare MovieMaker object
 * MovieMaker mm;
 *
 * void setup() {
 *   size(320, 240);
 *
 *   // Create MovieMaker object with size, filename,
 *   // compression codec and quality, framerate
 *   mm = new MovieMaker(this, width, height, "drawing.mov", 30,
 *                       MovieMaker.H263, MovieMaker.HIGH);
 *   background(160, 32, 32);
 * }
 *
 * void draw() {
 *   stroke(7, 146, 168);
 *   strokeWeight(4);
 *
 *   // Draw if mouse is pressed
 *   if (mousePressed) {
 *     line(pmouseX, pmouseY, mouseX, mouseY);
 *   }
 *
 *   // Add window's pixels to movie
 *   mm.addFrame();
 * }
 *
 * void keyPressed() {
 *   // Finish the movie if space bar is pressed!
 *   if (key == ' ') {
 *     mm.finish();
 *   }
 * }
 * </PRE>
 */
public class MovieMaker {

  public static final int RAW = StdQTConstants.kRawCodecType;
  public static final int ANIMATION = StdQTConstants.kAnimationCodecType;
  public static final int BASE = StdQTConstants.kBaseCodecType;
  public static final int BMP = StdQTConstants.kBMPCodecType;
  public static final int CINEPAK = StdQTConstants.kCinepakCodecType;
  public static final int COMPONENT = StdQTConstants.kComponentVideoCodecType;
  public static final int CMYK = StdQTConstants.kCMYKCodecType;
  public static final int GIF = StdQTConstants.kGIFCodecType;
  public static final int GRAPHICS = StdQTConstants.kGraphicsCodecType;
  public static final int H261 = StdQTConstants.kH261CodecType;
  public static final int H263 = StdQTConstants.kH263CodecType;
  // H.264 encoding, added because no constant is available in QTJava
  public static final int H264 = QTUtils.toOSType("avc1");
  public static final int JPEG = StdQTConstants.kJPEGCodecType;
  public static final int MS_VIDEO = StdQTConstants.kMicrosoftVideo1CodecType;
  public static final int MOTION_JPEG_A = StdQTConstants.kMotionJPEGACodecType;
  public static final int MOTION_JPEG_B = StdQTConstants.kMotionJPEGBCodecType;
  public static final int SORENSON = StdQTConstants.kSorensonCodecType;
  public static final int VIDEO = StdQTConstants.kVideoCodecType;

  public static final int WORST = StdQTConstants.codecMinQuality;
  public static final int LOW = StdQTConstants.codecLowQuality;
  public static final int MEDIUM = StdQTConstants.codecNormalQuality;
  public static final int HIGH = StdQTConstants.codecHighQuality;
  public static final int BEST = StdQTConstants.codecMaxQuality;
  public static final int LOSSLESS = StdQTConstants.codecLosslessQuality;

  private int width;
  private int height;

  private boolean readyForFrames;

  // Changed from 1000 to 600 in release 0154 to enable exact 30 fps output.
  // http://dev.processing.org/bugs/show_bug.cgi?id=988
  private int TIME_SCALE = 600;

  // QT Stuff
  private VideoMedia videoMedia;
  private Track videoTrack;
  private Movie movie;
  private QTFile movFile;
  private CSequence seq;
  private QTHandle imageHandle;
  private QDGraphics gw;
  private QDRect bounds;
  private ImageDescription imgDesc;
  private RawEncodedImage compressedImage;

  private int rate;
  private int keyFrameRate = 15;
  private int codecType, codecQuality;

  // my hack to make sure we don't get error -8691
  private boolean temporalSupported = true;

  private PApplet parent;


  /**
   * Create a movie with the specified width, height, and filename.
   * The movie will be created at 15 frames per second.
   * The codec will be set to RAW and quality set to HIGH.
   */
  public MovieMaker(PApplet p, int _w, int _h, String _filename) {
    this(p, _w, _h, _filename, 30, RAW, HIGH, 15);
  }


  /**
   * Create a movie with the specified width, height, filename, and frame rate.
   * The codec will be set to RAW and quality set to HIGH.
   */
  public MovieMaker(PApplet p, int _w, int _h, String _filename, int _rate) {
    this(p, _w, _h, _filename, _rate, RAW, HIGH, 15);
  }


  /**
   * Create a movie with the specified width, height, filename, frame rate,
   * and codec type and quality. Key frames will be set at 15 frames.
   */
  public MovieMaker(PApplet p, int _w, int _h, String _filename, int _rate,
                    int _codecType, int _codecQuality) {
    this(p, _w, _h, _filename, _rate, _codecType, _codecQuality, 15);
  }


  /**
   * Create a movie with the specified width, height, filename, frame rate,
   * codec type and quality, and key frame rate.
   */
  public MovieMaker(PApplet p, int _w, int _h, String _filename, int _rate,
                    int _codecType, int _codecQuality,
                    int _keyFrameRate) {
    parent = p;

    width = _w;
    height = _h;
    rate = _rate;

    try {
      QTSession.open();
    } catch (QTException e1) {
      e1.printStackTrace();
    }

    try {
      ImageDescription imgD = null;
      if (quicktime.util.EndianOrder.isNativeLittleEndian()) {
        imgD = new ImageDescription(QDConstants.k32BGRAPixelFormat);
      } else {
        imgD = new ImageDescription(QDGraphics.kDefaultPixelFormat);
      }
      imgD.setWidth(width);
      imgD.setHeight(height);
      gw = new QDGraphics(imgD, 0);

    } catch (QTException e) {
      e.printStackTrace();
    }
    codecType = _codecType;
    codecQuality = _codecQuality;
    keyFrameRate = _keyFrameRate;
    initMovie(_filename);

    parent.registerDispose(this);
  }


  private void initMovie(String filename) {
    try {
      String path = parent.savePath(filename);
      movFile = new QTFile(new File(path));
      movie = Movie.createMovieFile(movFile, StdQTConstants.kMoviePlayer, StdQTConstants.createMovieFileDeleteCurFile);
      int timeScale = TIME_SCALE; // 100 units per second
      videoTrack = movie.addTrack(width, height, 0);
      videoMedia = new VideoMedia(videoTrack, timeScale);
      videoMedia.beginEdits();
      bounds = new QDRect(0, 0, width, height);
      int rawImageSize = QTImage.getMaxCompressionSize(gw, bounds, gw.getPixMap().getPixelSize(), codecQuality, codecType, CodecComponent.anyCodec);
      imageHandle = new QTHandle(rawImageSize, true);
      imageHandle.lock();
      compressedImage = RawEncodedImage.fromQTHandle(imageHandle);
      seq = new CSequence(gw, bounds, gw.getPixMap().getPixelSize(), codecType, CodecComponent.bestFidelityCodec, codecQuality, codecQuality, keyFrameRate, null, 0);
      imgDesc = seq.getDescription();
      readyForFrames = true;

    } catch (QTException e) {
      if (e.errorCode() == Errors.noCodecErr) {
        if (imageHandle == null) {
          // This means QTImage.getMaxCompressionSize() failed
          System.err.println("The specified codec is not supported, " +
                             "please ensure that the parameters are valid, " +
                             "and in the correct order.");
        } else {
          // If it's a -8961 error, quietly do it the other way
          // (this happens when RAW is specified)
          temporalSupported = false;
          readyForFrames = true;
        }

      } else if (e.errorCode() == Errors.fBsyErr) {
        System.err.println("The movie file already exists.  " +
                           "Please delete it first.");

      } else {
        e.printStackTrace();
      }
    }
  }


  // A simple add function to just add whatever is in the parent window
  public void addFrame() {
    // http://dev.processing.org/bugs/show_bug.cgi?id=692
    parent.flush();
    parent.loadPixels();
    addFrame(parent.pixels, parent.width, parent.height);
  }


  public void addFrame(int[] _pixels, int w, int h) {
    if (readyForFrames){
      RawEncodedImage pixelData = gw.getPixMap().getPixelData();
      int rowBytes = pixelData.getRowBytes() / 4;
      int[] newpixels = new int[rowBytes*h];
      for (int i = 0; i < rowBytes; i++) {
        for (int j = 0; j < h; j++) {
          if (i < w) {
            newpixels[i+j*rowBytes] = _pixels[i+j*w];
          } else {
            newpixels[i+j*rowBytes] = 0;
          }
        }
      }
      pixelData.setInts(0,newpixels);
      compressAndAdd();
    }
  }


  private void compressAndAdd() {
    try {
      if (temporalSupported) {
        CompressedFrameInfo cfInfo = seq.compressFrame(gw, bounds, StdQTConstants.codecFlagUpdatePrevious, compressedImage);
        boolean syncSample = cfInfo.getSimilarity() == 0; // see developer.apple.com/qa/qtmcc/qtmcc20.html
        videoMedia.addSample(imageHandle, 0, cfInfo.getDataSize(), TIME_SCALE/rate, imgDesc, 1, syncSample ? 0 : StdQTConstants.mediaSampleNotSync);
      } else {
        imgDesc = QTImage.fCompress(gw,gw.getBounds(),32,codecQuality,codecType, CodecComponent.anyCodec, null, 0, RawEncodedImage.fromQTHandle(imageHandle));
        boolean syncSample = true;   // UM, what the hell should this be???
        videoMedia.addSample(imageHandle, 0, imgDesc.getDataSize(), TIME_SCALE/rate, imgDesc, 1, syncSample ? 0 : StdQTConstants.mediaSampleNotSync);
      }
    } catch (QTException e) {
      e.printStackTrace();
    }
  }


  /**
   * Close out and finish the movie file.
   */
  public void finish() {
    try {
      if (readyForFrames) {
        //System.out.println("Finishing movie file.");
        readyForFrames = false;
        videoMedia.endEdits();
        videoTrack.insertMedia(0, 0, videoMedia.getDuration(), 1);
        OpenMovieFile omf = OpenMovieFile.asWrite(movFile);
        movie.addResource(omf, StdQTConstants.movieInDataForkResID,
                          movFile.getName());
      }
    } catch (StdQTException se) {
      se.printStackTrace();
    } catch (QTException qe) {
      qe.printStackTrace();
    }
  }


  public void dispose() {
    if (readyForFrames) finish();

    try {
      QTSession.close();
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
}
