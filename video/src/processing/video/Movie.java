/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-07 Ben Fry and Casey Reas
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

import java.io.*;
import java.lang.reflect.*;

import quicktime.*;
import quicktime.io.QTFile;
import quicktime.qd.*;
import quicktime.std.*;
import quicktime.std.movies.media.DataRef;
import quicktime.util.QTHandle;
import quicktime.util.RawEncodedImage;


public class Movie extends PImage implements PConstants, Runnable {
  // no longer needing a reference to the parent because PImage has one
  //PApplet parent;

  Method movieEventMethod;
  String filename;
  Thread runner;

  PImage borderImage;
  boolean removeBorders = true;

  boolean play;
  boolean repeat;
  boolean available;
  int fps;

  /**
   * The QuickTime for Java "Movie" object, made public
   * in case anyone wants to play with it.
   */
  public quicktime.std.movies.Movie movie;

  QDRect movieRect;
  QDGraphics movieGraphics;
  boolean firstFrame = true;
  RawEncodedImage raw;


    /*
  static {
    try {
      //System.out.println("jlp = " + System.getProperty("java.library.path"));
      QTSession.open();
    } catch (QTException e) {
      e.printStackTrace();
    }
      // shutting off for 0116, hoping for better exception handling
    QTRuntimeException.registerHandler(new QTRuntimeHandler() {
        public void exceptionOccurred(QTRuntimeException e,
                                      Object obj, String s, boolean flag) {
          System.err.println("Problem inside Movie");
          e.printStackTrace();
        }
      });
  }
    */


  public Movie(PApplet parent, String filename) {
    this(parent, filename, 30);
  }


  public Movie(final PApplet parent, final String filename, final int ifps) {
    // this creates a fake image so that the first time this
    // attempts to draw, something happens that's not an exception
    super(1, 1, RGB);

    // http://dev.processing.org/bugs/show_bug.cgi?id=882
    //SwingUtilities.invokeLater(new Runnable() {
    //public void run() {
    init(parent, filename, ifps);
    //}
    //});
  }


  public void init(PApplet parent, String filename, int fps) {
    this.parent = parent;
    this.fps = fps;

    try {
      QTSession.open();
    } catch (QTException e) {
      e.printStackTrace();
      return;
    }

    // first check to see if this can be read locally from a file.
    // otherwise, will have to load the file into memory, which is
    // gonna make people unhappy who are trying to play back 50 MB
    // quicktime movies with a locally installed piece exported
    // as an application.
    try {
      try {
        // first try a local file using the dataPath. usually this will
        // work ok, but sometimes the dataPath is inside a jar file,
        // which is less fun, so this will crap out.
        File file = new File(parent.dataPath(filename));
        if (file.exists()) {
          movie = fromDataRef(new DataRef(new QTFile(file)));
          //init(parent, movie, ifps);
          //return;
        }
      } catch (Exception e) { }  // ignored

      // read from a folder local to the current working dir
      // called "data". presumably this might be the data folder,
      // though that should be caught above, if such a folder exists.
      /*
      if (movie == null) {
        try {
          File file = new File("data", filename);
          if (file.exists()) {
            movie = fromDataRef(new DataRef(new QTFile(file)));
            init(parent, movie, ifps);
            return;
          }
        } catch (QTException e2) { }
      }
      */

      // read from a file just hanging out in the local folder.
      // this might happen when the video library is used with some
      // other application, or the person enters a full path name
      if (movie == null) {
        try {
          File file = new File(filename);
          if (file.exists()) {
            movie = fromDataRef(new DataRef(new QTFile(file)));
            //init(parent, movie, ifps);
            //return;
          }
        } catch (QTException e1) { }
      }

    } catch (SecurityException se) {
      // online, whups. catch the security exception out here rather than
      // doing it three times (or whatever) for each of the cases above.
    }

    // if the movie can't be read from a local file, it has to be read
    // into a byte array and passed to qtjava. it's annoying that apple
    // doesn't have something in the api to read a movie from a friggin
    // InputStream, but oh well. it's their api.
    if (movie == null) {
      byte data[] = parent.loadBytes(filename);
      //int dot = filename.lastIndexOf(".");
      // grab the extension from the file, use mov if there is none
      //String extension = (dot == -1) ? "mov" :
      //  filename.substring(dot + 1).toLowerCase();
      try {
        movie = fromDataRef(new DataRef(new QTHandle(data)));
      } catch (QTException e) {
        e.printStackTrace();
      }
    }

    /*
    URL url = null;
    this.filename = filename; // for error messages

    if (filename.startsWith("http://")) {
      try {
        url = new URL(filename);
        DataRef urlRef = new DataRef(url.toExternalForm());
        movie = fromDataRef(urlRef);
        init(parent, movie, ifps);
        return;

      } catch (QTException qte) {
        qte.printStackTrace();
        return;

      } catch (MalformedURLException e) {
        e.printStackTrace();
        return;
      }
    }

    // updated for new loading style of 0096
    ClassLoader cl = parent.getClass().getClassLoader();
    url = cl.getResource("data/" + filename);
    if (url != null) {
      init(parent, url, ifps);
      return;
    }

    try {
      try {
        File file = new File(parent.dataPath(filename));
        if (file.exists()) {
          movie = fromDataRef(new DataRef(new QTFile(file)));
          init(parent, movie, ifps);
          return;
        }
      } catch (Exception e) { }  // ignored

      try {
        File file = new File("data", filename);
        if (file.exists()) {
          movie = fromDataRef(new DataRef(new QTFile(file)));
          init(parent, movie, ifps);
          return;
        }
      } catch (QTException e2) { }

      try {
        File file = new File(filename);
        if (file.exists()) {
          movie = fromDataRef(new DataRef(new QTFile(file)));
          init(parent, movie, ifps);
          return;
        }
      } catch (QTException e1) { }

    } catch (SecurityException se) { }  // online, whups
    */

    if (movie == null) {
      parent.die("Could not find movie file " + filename, null);
    }

    // we've got a valid movie! let's rock.
    try {
      // this is probably causing the 2 seconds of audio
      // disabled pre-preroll on 0126 because of security problems
      //movie.prePreroll(0, 1.0f);
      movie.preroll(0, 1.0f);

      // this has a possibility of running forever..
      // should probably happen on the thread as well.
      while (movie.maxLoadedTimeInMovie() == 0) {
        movie.task(100);

        // 0106: tried adding sleep time so this doesn't spin out of control
        // works fine but doesn't really help anything
        //try {
        //Thread.sleep(5);
        //} catch (InterruptedException e) { }
      }
      movie.setRate(1);
      //fps = ifps;

      // register methods
      parent.registerDispose(this);

      try {
        movieEventMethod =
          parent.getClass().getMethod("movieEvent",
                                      new Class[] { Movie.class });
      } catch (Exception e) {
        // no such method, or an error.. which is fine, just ignore
      }

      // and now, make the magic happen
      runner = new Thread(this);
      runner.start();

    } catch (QTException qte) {
      qte.printStackTrace();

    } catch (Exception e) {
      e.printStackTrace();
    }
  }


  /*
  public Movie(PApplet parent, URL url) {
    init(parent, url, 30);
  }


  public Movie(PApplet parent, URL url, int ifps) {
    init(parent, url, ifps);
  }


  public void init(PApplet parent, URL url, int ifps) {

    String externalized = url.toExternalForm();
    System.out.println("externalized is " + externalized);

    // qtjava likes file: urls to read file:/// not file:/
    // so this changes them when appropriate
    if (externalized.startsWith("file:/") &&
        !externalized.startsWith("file:///")) {
      externalized = "file:///" + url.getPath();
    }

    // the url version is the only available that can take
    // an InputStream (indirectly) since it uses url syntax
    //DataRef urlRef = new DataRef(requestFile);
    try {
      System.out.println(url);
      System.out.println(externalized);
      DataRef urlRef = new DataRef(externalized);
      System.out.println(urlRef);

      movie = fromDataRef(urlRef);
      init(parent, movie, ifps);

    } catch (QTException e) {
      e.printStackTrace();
    }
  }
  */


  /**
   * Why does this function have to be so bizarre? i love the huge
   * constants! i think they're neato. i feel like i'm coding for
   * think pascal on my mac plus! those were happier times.
   */
  private quicktime.std.movies.Movie fromDataRef(DataRef ref)
    throws QTException {

    return
      quicktime.std.movies.Movie.fromDataRef(ref,
                                             StdQTConstants4.newMovieAsyncOK |
                                             StdQTConstants.newMovieActive);
  }


  /*
  public void init(PApplet parent,
                   quicktime.std.movies.Movie movie, int ifps) {
    this.parent = parent;

    try {
      // this is probably causing the 2 seconds of audio
      movie.prePreroll(0, 1.0f);
      movie.preroll(0, 1.0f);

      // this has a possibility of running forever..
      // should probably happen on the thread as well.
      while (movie.maxLoadedTimeInMovie() == 0) {
        movie.task(100);

        // 0106: tried adding sleep time so this doesn't spin out of control
        // works fine but doesn't really help anything
        //try {
        //Thread.sleep(5);
        //} catch (InterruptedException e) { }
      }
      movie.setRate(1);
      fps = ifps;

      runner = new Thread(this);
      runner.start();


      // register methods

      parent.registerDispose(this);

      try {
        movieEventMethod =
          parent.getClass().getMethod("movieEvent",
                                      new Class[] { Movie.class });
      } catch (Exception e) {
        // no such method, or an error.. which is fine, just ignore
      }

    } catch (QTException qte) {
      qte.printStackTrace();

    } catch (Exception e) {
      e.printStackTrace();
    }
  }
  */


  public boolean available() {
    return available;
  }


  public void read() {
    try {
      if (firstFrame) {
        movieRect = movie.getBox();
        //movieGraphics = new QDGraphics(movieRect);
        if (quicktime.util.EndianOrder.isNativeLittleEndian()) {
          movieGraphics =
            new QDGraphics(QDConstants.k32BGRAPixelFormat, movieRect);
        } else {
          movieGraphics =
            new QDGraphics(QDGraphics.kDefaultPixelFormat, movieRect);
        }
      }

      Pict pict = movie.getPict(movie.getTime());  // returns an int
      pict.draw(movieGraphics, movieRect);
      PixMap pixmap = movieGraphics.getPixMap();
      raw = pixmap.getPixelData();

      // It needs to get at least a small part
      // of the video to get the parameters
      if (firstFrame) {
        //int intsPerRow = pixmap.getRowBytes() / 4;
        int movieWidth = movieRect.getWidth();
        int movieHeight = movieRect.getHeight();
        int j = raw.getRowBytes() - movieWidth*4;
        // this doesn't round up.. does it need to?
        int k = j / 4;
        int dataWidth = movieWidth + k;

        if (dataWidth != movieWidth) {
          if (removeBorders) {
            borderImage = new PImage(dataWidth, movieHeight, RGB);
          } else {
            movieWidth = dataWidth;
          }
        }
        //int vpixels[] = new int[movieWidth * movieHeight];
        //image = new PImage(vpixels, movieWidth, movieHeight, RGB);
        super.init(movieWidth, movieHeight, RGB);
        //parent.video = image;
        firstFrame = false;
      }
      // this happens later (found by hernando)
      //raw.copyToArray(0, image.pixels, 0, image.width * image.height);

      loadPixels();
      // this is identical to a chunk of code inside PCamera
      // this might be a candidate to move up to PVideo or something
      if (borderImage != null) {  // need to remove borders
        raw.copyToArray(0, borderImage.pixels,
                        0, borderImage.width * borderImage.height);
        int borderIndex = 0;
        int targetIndex = 0;
        for (int i = 0; i < height; i++) {
          System.arraycopy(borderImage.pixels, borderIndex,
                           pixels, targetIndex, width);
          borderIndex += borderImage.width;
          targetIndex += width;
        }
      } else {  // just copy directly
        raw.copyToArray(0, pixels, 0, width * height);
      }

      // ready to rock
      //System.out.println("updating pixels");
      //updatePixels();  // mark as modified
      updatePixels();

    } catch (QTException qte) {
      qte.printStackTrace();
      //QTSession.close();  // let dispose() handle it
    }
  }


  /**
   * Begin playing the movie, with no repeat.
   */
  public void play() {
//    if (runner != null) {
//      stop();
//    }
    play = true;
//    runner = new Thread(this);
//    runner.start();
  }


  /**
   * Begin playing the movie, with repeat.
   */
  public void loop() {
    play();
    repeat = true;
  }


  /**
   * Shut off the repeating loop.
   */
  public void noLoop() {
    repeat = false;
  }


  /**
   * Pause the movie at its current time.
   */
  public void pause() {
    play = false;
    //System.out.println("pause");
  }


  /**
   * Stop the movie, and rewind.
   */
  public void stop() {
    play = false;
//    runner = null;

    try {
      movie.setTimeValue(0);

    } catch (StdQTException e) {
      errorMessage("stop", e);
    }
  }


  /**
   * Set how often new frames are to be read from the movie.
   * Does not actually set the speed of the movie playback,
   * that's handled by the speed() method.
   */
  public void frameRate(int ifps) {
    if (ifps <= 0) {
      System.err.println("Movie: ignoring bad frame rate of " +
                         ifps + " fps.");
    } else {
      fps = ifps;
    }
  }


  /**
   * Set a multiplier for how fast/slow the movie should be run.
   * The default is 1.0.
   * <UL>
   * <LI>speed(2) will play the movie at double speed (2x).
   * <LI>speed(0.5) will play at half speed.
   * <LI>speed(-1) will play backwards at regular speed.
   * </UL>
   */
  public void speed(float rate) {
    //rate = irate;
    try {
      movie.setRate(rate);

    } catch (StdQTException e) {
      errorMessage("speed", e);
    }
  }


  /**
   * Return the current time in seconds.
   * The number is a float so fractions of seconds can be used.
   */
  public float time() {
    try {
      return (float)movie.getTime() / (float)movie.getTimeScale();

    } catch (StdQTException e) {
      errorMessage("time", e);
    }
    return -1;
  }


  /**
   * Jump to a specific location (in seconds).
   * The number is a float so fractions of seconds can be used.
   */
  public void jump(float where) {
    try {
      //movie.setTime(new TimeRecord(rate, where));  // scale, value
      //movie.setTime(new TimeRecord(1, where));  // scale, value
      int scaledTime = (int) (where * movie.getTimeScale());
      movie.setTimeValue(scaledTime);

    } catch (StdQTException e) {
      errorMessage("jump", e);
    }
  }


  /**
   * Get the full length of this movie (in seconds).
   */
  public float duration() {
    try {
      return (float)movie.getDuration() / (float)movie.getTimeScale();

    } catch (StdQTException e) {
      errorMessage("length", e);
    }
    return -1;
  }


  /*
  public void play() {
    if(!play) {
      play = true;
    }
    start();
    while( image == null) {
      try {
        Thread.sleep(5);
      } catch (InterruptedException e) { }
    }
    pixels = image.pixels;
    width = image.width;
    height = image.height;
  }


  public void repeat() {
    loop = true;
    if(!play) {
      play = true;
    }
    start();
    while( image == null) {
      try {
        Thread.sleep(5);
      } catch (InterruptedException e) { }
    }
    pixels = image.pixels;
    width = image.width;
    height = image.height;
  }


  public void pause() {
    play = false;
  }
  */


  public void run() {
    //System.out.println("entering thread");
    while (Thread.currentThread() == runner) {
      //System.out.print("<");
      try {
        //Thread.sleep(5);
        Thread.sleep(1000 / fps);
      } catch (InterruptedException e) { }
      //System.out.print(">");

      // this could be a lie, but..
      if (play) {
        //read();
        //System.out.println("play");
        available = true;

        if (movieEventMethod == null) {
          // If no special handling, then automatically read from the movie.
          read();

        } else {
          try {
            movieEventMethod.invoke(parent, new Object[] { this });
          } catch (Exception e) {
            System.err.println("error, disabling movieEvent() for " +
                               filename);
            e.printStackTrace();
            movieEventMethod = null;
          }
        }

        try {
          if (movie.isDone() && repeat) {
            movie.goToBeginning();
          }
        } catch (StdQTException e) {
          play = false;
          errorMessage("rewinding", e);
        }
        //} else {
        //System.out.println("no play");
      }

      //try {
      //read();
      //if (movie.isDone() && loop) movie.goToBeginning();

      //} catch (QTException e) {
      //System.err.println("Movie exception");
      //e.printStackTrace();
      //QTSession.close(); ??
      //}
    }
  }


  /**
   * Call this to halt the movie from running, and stop its thread.
   */
  public void dispose() {
    stop();
    runner = null;
    QTSession.close();
  }


  /**
   * General error reporting, all corraled here just in case
   * I think of something slightly more intelligent to do.
   */
  protected void errorMessage(String where, Exception e) {
    parent.die("Error inside Movie." + where + "()", e);
  }
}

