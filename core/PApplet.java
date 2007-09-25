/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-06 Ben Fry and Casey Reas
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

import java.applet.*;
import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.io.*;
import java.lang.reflect.*;
import java.net.*;
import java.text.*;
import java.util.*;
import java.util.zip.*;


/**
 * Base class for all sketches that use processing.core.
 * <p/>
 * Note that you should not use AWT or Swing components inside a Processing
 * applet. The surface is made to automatically update itself, and will cause
 * problems with redraw of components drawn above it. If you'd like to
 * integrate other Java components, see below.
 * <p/>
 * This class extends Applet instead of JApplet because 1) we will eventually
 * be returning Java 1.1 support, which does not include Swing (without an
 * additional, sizable, download), and 2) Swing is a bloated piece of crap.
 * A Processing applet is a heavyweight AWT component, and can be used the
 * same as any other AWT component, with or without Swing.
 * <p/>
 * Similarly, Processing runs in a Frame and not a JFrame. However, there's
 * nothing to prevent you from embedding a PApplet into a JFrame, it's just
 * that the base version uses a regular AWT frame because there's simply
 * no need for swing in that context. If people want to use Swing, they can
 * embed themselves as they wish.
 * <p/>
 * It is possible to use PApplet, along with core.jar in other projects.
 * In addition to enabling you to use Java 1.5+ features with your sketch,
 * this also allows you to embed a Processing drawing area into another Java
 * application. This means you can use standard GUI controls with a Processing
 * sketch. Because AWT and Swing GUI components cannot be used on top of a
 * PApplet, you can instead embed the PApplet inside another GUI the wayyou
 * would any other Component.
 * <p/>
 * Because the default animation thread will run at 60 frames per second,
 * an embedded PApplet can make the parent sluggish. You can use frameRate()
 * to make it update less often, or you can use noLoop() and loop() to disable
 * and then re-enable looping. If you want to only update the sketch
 * intermittently, use noLoop() inside setup(), and redraw() whenever
 * the screen needs to be updated once (or loop() to re-enable the animation
 * thread). The following example embeds a sketch and also uses the noLoop()
 * and redraw() methods. You need not use noLoop() and redraw() when embedding
 * if you want your application to animate continuously.
 * <PRE>
 * public class ExampleFrame extends Frame {
 *
 *     public ExampleFrame() {
 *         super("Embedded PApplet");
 *
 *         setLayout(new BorderLayout());
 *         PApplet embed = new Embedded();
 *         add(embed, BorderLayout.CENTER);
 *
 *         // important to call this whenever embedding a PApplet.
 *         // It ensures that the animation thread is started and
 *         // that other internal variables are properly set.
 *         embed.init();
 *     }
 * }
 *
 * public class Embedded extends PApplet {
 *
 *     public void setup() {
 *         // original setup code here ...
 *         size(400, 400);
 *
 *         // prevent thread from starving everything else
 *         noLoop();
 *     }
 *
 *     public void draw() {
 *         // drawing code goes here
 *     }
 *
 *     public void mousePressed() {
 *         // do something based on mouse movement
 *
 *         // update the screen (run draw once)
 *         redraw();
 *     }
 * }
 * </PRE>
 *
 * <H2>Processing on multiple displays</H2>
 * <P>I was asked about Processing with multiple displays, and for lack of a
 * better place to document it, things will go here.</P>
 * <P>You can address both screens by making a window the width of both,
 * and the height of the maximum of both screens. In this case, do not use
 * present mode, because that's exclusive to one screen. Basically it'll
 * give you a PApplet that spans both screens. If using one half to control
 * and the other half for graphics, you'd just have to put the 'live' stuff
 * on one half of the canvas, the control stuff on the other. This works
 * better in windows because on the mac we can't get rid of the menu bar
 * unless it's running in present mode.</P>
 * <P>For more control, you need to write straight java code that uses p5.
 * You can create two windows, that are shown on two separate screens,
 * that have their own PApplet. this is just one of the tradeoffs of one of
 * the things that we don't support in p5 from within the environment
 * itself (we must draw the line somewhere), because of how messy it would
 * get to start talking about multiple screens. It's also not that tough to
 * do by hand w/ some Java code.</P>
 */
public class PApplet extends Applet
  implements PConstants, Runnable,
             MouseListener, MouseMotionListener, KeyListener, FocusListener
{
  /**
   * Full name of the Java version (i.e. 1.5.0_11).
   * Prior to 0125, this was only the first three digits.
   */
  public static final String javaVersionName =
    System.getProperty("java.version");

  /**
   * Version of Java that's in use, whether 1.1 or 1.3 or whatever,
   * stored as a float.
   * <P>
   * Note that because this is stored as a float, the values may
   * not be <EM>exactly</EM> 1.3 or 1.4. Instead, make sure you're
   * comparing against 1.3f or 1.4f, which will have the same amount
   * of error (i.e. 1.40000001). This could just be a double, but
   * since Processing only uses floats, it's safer for this to be a float
   * because there's no good way to specify a double with the preproc.
   */
  public static final float javaVersion =
    new Float(javaVersionName.substring(0, 3)).floatValue();

  /**
   * Current platform in use, one of the
   * PConstants WINDOWS, MACOSX, MACOS9, LINUX or OTHER.
   */
  static public int platform;

  /**
   * Current platform in use.
   * <P>
   * Equivalent to System.getProperty("os.name"), just used internally.
   */
  static public String platformName =
    System.getProperty("os.name");

  static {
    // figure out which operating system
    // this has to be first, since editor needs to know

    if (platformName.toLowerCase().indexOf("mac") != -1) {
      // can only check this property if running on a mac
      // on a pc it throws a security exception and kills the applet
      // (but on the mac it does just fine)
      if (System.getProperty("mrj.version") != null) {  // running on a mac
        platform = (platformName.equals("Mac OS X")) ?
          MACOSX : MACOS9;
      }

    } else {
      String osname = System.getProperty("os.name");

      if (osname.indexOf("Windows") != -1) {
        platform = WINDOWS;

      } else if (osname.equals("Linux")) {  // true for the ibm vm
        platform = LINUX;

      } else {
        platform = OTHER;
      }
    }
  }


  /** The PGraphics renderer associated with this PApplet */
  public PGraphics g;

  //protected Object glock = new Object(); // for sync

  /** The frame containing this applet (if any) */
  public Frame frame;

  /**
   * Message of the Exception thrown when size() is called the first time.
   * <P>
   * This is used internally so that setup() is forced to run twice
   * when the renderer is changed. This is the only way for us to handle
   * invoking the new renderer while also in the midst of rendering.
   */
  static final String NEW_RENDERER = "new renderer";

  /**
   * The screen size when the applet was started.
   * <P>
   * Access this via screen.width and screen.height. To make an applet
   * run at full screen, use size(screen.width, screen.height).
   * <P>
   * If you have multiple displays, this will be the size of the main
   * display. Running full screen across multiple displays isn't
   * particularly supported, and requires more monkeying with the values.
   * This probably can't/won't be fixed until/unless I get a dual head
   * system.
   * <P>
   * Note that this won't update if you change the resolution
   * of your screen once the the applet is running.
   */
  public Dimension screen =
    Toolkit.getDefaultToolkit().getScreenSize();

  /**
   * A leech graphics object that is echoing all events.
   */
  public PGraphics recorder;

  /**
   * Command line options passed in from main().
   * <P>
   * This does not include the arguments passed in to PApplet itself.
   */
  public String args[];

  /** Path to sketch folder */
  public String sketchPath; //folder;

  /** When debugging headaches */
  static final boolean THREAD_DEBUG = false;

  private Object blocker = new Object();

  /** Default width and height for applet when not specified */
  static public final int DEFAULT_WIDTH = 100;
  static public final int DEFAULT_HEIGHT = 100;

  /**
   * Minimum dimensions for the window holding an applet.
   * This varies between platforms, Mac OS X 10.3 can do any height
   * but requires at least 128 pixels width. Windows XP has another
   * set of limitations. And for all I know, Linux probably lets you
   * make windows with negative sizes.
   */
  static public final int MIN_WINDOW_WIDTH = 128;
  static public final int MIN_WINDOW_HEIGHT = 128;

  /**
   * true if no size() command has been executed. This is used to wait until
   * a size has been set before placing in the window and showing it.
   */
  public boolean defaultSize;

  /**
   * Pixel buffer from this applet's PGraphics.
   * <P>
   * When used with OpenGL or Java2D, this value will
   * be null until loadPixels() has been called.
   */
  public int pixels[];

  /** width of this applet's associated PGraphics */
  public int width;

  /** height of this applet's associated PGraphics */
  public int height;

  /** current x position of the mouse */
  public int mouseX;

  /** current y position of the mouse */
  public int mouseY;

  /**
   * Previous x/y position of the mouse. This will be a different value
   * when inside a mouse handler (like the mouseMoved() method) versus
   * when inside draw(). Inside draw(), pmouseX is updated once each
   * frame, but inside mousePressed() and friends, it's updated each time
   * an event comes through. Be sure to use only one or the other type of
   * means for tracking pmouseX and pmouseY within your sketch, otherwise
   * you're gonna run into trouble.
   */
  public int pmouseX, pmouseY;

  /**
   * previous mouseX/Y for the draw loop, separated out because this is
   * separate from the pmouseX/Y when inside the mouse event handlers.
   */
  protected int dmouseX, dmouseY;

  /**
   * pmouseX/Y for the event handlers (mousePressed(), mouseDragged() etc)
   * these are different because mouse events are queued to the end of
   * draw, so the previous position has to be updated on each event,
   * as opposed to the pmouseX/Y that's used inside draw, which is expected
   * to be updated once per trip through draw().
   */
  protected int emouseX, emouseY;

  /**
   * Used to set pmouseX/Y to mouseX/Y the first time mouseX/Y are used,
   * otherwise pmouseX/Y are always zero, causing a nasty jump.
   * <P>
   * Just using (frameCount == 0) won't work since mouseXxxxx()
   * may not be called until a couple frames into things.
   */
  public boolean firstMouse;

  /**
   * Last mouse button pressed, one of LEFT, CENTER, or RIGHT.
   * <P>
   * If running on Mac OS, a ctrl-click will be interpreted as
   * the righthand mouse button (unlike Java, which reports it as
   * the left mouse).
   */
  public int mouseButton;

  public boolean mousePressed;
  public MouseEvent mouseEvent;

  /**
   * Last key pressed.
   * <P>
   * If it's a coded key, i.e. UP/DOWN/CTRL/SHIFT/ALT,
   * this will be set to CODED (0xffff or 65535).
   */
  public char key;

  /**
   * When "key" is set to CODED, this will contain a Java key code.
   * <P>
   * For the arrow keys, keyCode will be one of UP, DOWN, LEFT and RIGHT.
   * Also available are ALT, CONTROL and SHIFT. A full set of constants
   * can be obtained from java.awt.event.KeyEvent, from the VK_XXXX variables.
   */
  public int keyCode;

  /**
   * true if the mouse is currently pressed.
   */
  public boolean keyPressed;

  /**
   * the last KeyEvent object passed into a mouse function.
   */
  public KeyEvent keyEvent;

  /**
   * Gets set to true/false as the applet gains/loses focus.
   */
  public boolean focused = false;

  /**
   * true if the applet is online.
   * <P>
   * This can be used to test how the applet should behave
   * since online situations are different (no file writing, etc).
   */
  public boolean online = false;

  /**
   * Time in milliseconds when the applet was started.
   * <P>
   * Used by the millis() function.
   */
  long millisOffset;

  /**
   * The current value of frames per second.
   * <P>
   * The initial value will be 10 fps, and will be updated with each
   * frame thereafter. The value is not instantaneous (since that
   * wouldn't be very useful since it would jump around so much),
   * but is instead averaged (integrated) over several frames.
   * As such, this value won't be valid until after 5-10 frames.
   */
  public float frameRate = 10;
  protected long frameRateLastMillis = 0;

  /** Last time in milliseconds that a frameRate delay occurred */
  protected long frameRateLastDelayTime = 0;
  /** As of release 0116, frameRate(60) is called as a default */
  protected float frameRateTarget = 60;

  protected boolean looping;

  /** flag set to true when a redraw is asked for by the user */
  protected boolean redraw;

  /**
   * How many frames have been displayed since the applet started.
   * <P>
   * This value is read-only <EM>do not</EM> attempt to set it,
   * otherwise bad things will happen.
   * <P>
   * Inside setup(), frameCount is 0.
   * For the first iteration of draw(), frameCount will equal 1.
   */
  public int frameCount;

  /**
   * true if this applet has had it.
   */
  public boolean finished;

  /**
   * true if exit() has been called so that things shut down
   * once the main thread kicks off.
   */
  protected boolean exit;

  Thread thread;

  /**
   * Set to the an exception that occurs inside run() and is not
   * caught. <P> Used by PdeRuntime to determine what happened and
   * report back to the user.
   */
  public Exception exception;
  //public Throwable exception;

  protected RegisteredMethods sizeMethods;
  protected RegisteredMethods preMethods, drawMethods, postMethods;
  protected RegisteredMethods mouseEventMethods, keyEventMethods;
  protected RegisteredMethods disposeMethods;

  // this text isn't seen unless PApplet is used on its
  // own and someone takes advantage of leechErr.. not likely
  static public final String LEECH_WAKEUP = "Error while running applet.";
  public PrintStream leechErr;

  // messages to send if attached as an external vm

  /**
   * Position of the upper-lefthand corner of the editor window
   * that launched this applet.
   */
  static public final String ARGS_EDITOR_LOCATION = "--editor-location";

  /**
   * Location for where to position the applet window on screen.
   * <P>
   * This is used by the editor to when saving the previous applet
   * location, or could be used by other classes to launch at a
   * specific position on-screen.
   */
  static public final String ARGS_EXTERNAL = "--external";

  static public final String ARGS_LOCATION = "--location";

  static public final String ARGS_DISPLAY = "--display";

  static public final String ARGS_BGCOLOR = "--bgcolor";

  static public final String ARGS_PRESENT = "--present";

  static public final String ARGS_STOP_COLOR = "--stop-color";

  static public final String ARGS_HIDE_STOP = "--hide-stop";

  /**
   * Allows the user or PdeEditor to set a specific sketch folder path.
   * <P>
   * Used by PdeEditor to pass in the location where saveFrame()
   * and all that stuff should write things.
   */
  static public final String ARGS_SKETCH_FOLDER = "--sketch-path";

  /**
   * Message from parent editor (when run as external) to quit.
   */
  static public final char EXTERNAL_STOP = 's';

  /**
   * When run externally to a PdeEditor,
   * this is sent by the applet when it quits.
   */
  static public final String EXTERNAL_QUIT = "__QUIT__";

  /**
   * When run externally to a PdeEditor, this is sent by the applet
   * whenever the window is moved.
   * <P>
   * This is used so that the editor can re-open the sketch window
   * in the same position as the user last left it.
   */
  static public final String EXTERNAL_MOVE = "__MOVE__";


  // during rev 0100 dev cycle, working on new threading model,
  // but need to disable and go conservative with changes in order
  // to get pdf and audio working properly first.
  // for 0116, the CRUSTY_THREADS are being disabled to fix lots of bugs.
  static final boolean CRUSTY_THREADS = false; //true;


  public void init() {
    // first get placed size in case it's non-zero
    Dimension initialSize = getSize();

    // send tab keys through to the PApplet
    try {
      if (javaVersion >= 1.4f) {
        //setFocusTraversalKeysEnabled(false);  // 1.4-only function
        Method defocus =
          Component.class.getMethod("setFocusTraversalKeysEnabled",
                                    new Class[] { Boolean.TYPE });
        defocus.invoke(this, new Object[] { Boolean.FALSE });
      }
    } catch (Exception e) { }  // oh well

    millisOffset = System.currentTimeMillis();

    finished = false; // just for clarity

    // this will be cleared by draw() if it is not overridden
    looping = true;
    redraw = true;  // draw this guy once
    firstMouse = true;

    // these need to be inited before setup
    sizeMethods = new RegisteredMethods();
    preMethods = new RegisteredMethods();
    drawMethods = new RegisteredMethods();
    postMethods = new RegisteredMethods();
    mouseEventMethods = new RegisteredMethods();
    keyEventMethods = new RegisteredMethods();
    disposeMethods = new RegisteredMethods();

    try {
      getAppletContext();
      online = true;
    } catch (NullPointerException e) {
      online = false;
    }

    if (javaVersion < 1.3f) {
      addMouseListener(new MouseAdapter() {
          public void mousePressed(MouseEvent e) {
            link("http://java.com/");
          }
        });
      // no init to do, so don't cause no trouble, boy
      return;
      // call this after making the methods to minimize the
      // number of places needing the javaVersion crap
      // (also needs to check online first and create empty
      // stop method register list)
    }

    try {
      if (sketchPath == null) {
        sketchPath = System.getProperty("user.dir");
      }
    } catch (Exception e) { }  // may be a security problem

    // create a dummy graphics context
    if ((initialSize.width != 0) && (initialSize.height != 0)) {
      size(initialSize.width, initialSize.height);
    } else {
      //System.out.println("setting default");
      size(DEFAULT_WIDTH, DEFAULT_HEIGHT);
      this.defaultSize = true;
      //System.out.println("zeroing");
      //this.width = 0;  // use this to flag whether the width/height are valid
      //this.height = 0;
      // need to set width/height otherwise
      // they won't work for static mode apps
      //defaultSize = true;
    }

    // this is automatically called in applets
    // though it's here for applications anyway
    start();
  }


  /**
   * Called by the browser or applet viewer to inform this applet that it
   * should start its execution. It is called after the init method and
   * each time the applet is revisited in a Web page.
   * <p/>
   * Called explicitly via the first call to PApplet.paint(), because
   * PAppletGL needs to have a usable screen before getting things rolling.
   */
  public void start() {
    if (javaVersion < 1.3f) return;

    if (thread != null) return;
    thread = new Thread(this);
    thread.start();
  }


  /**
   * Called by the browser or applet viewer to inform
   * this applet that it should stop its execution.
   * <p/>
   * Unfortunately, there are no guarantees from the Java spec
   * when or if stop() will be called (i.e. on browser quit,
   * or when moving between web pages), and it's not always called.
   */
   public void stop() {
     // maybe start should also be used as the method for kicking
     // the thread on, instead of doing it inside paint()

     // bringing this back for 0111, hoping it'll help opengl shutdown
     finished = true;  // why did i comment this out?

     //System.out.println("stopping applet " + thread);

    // don't run stop and disposers twice
    if (thread == null) return;
    thread = null;

    // call to shut down renderer, in case it needs it (pdf does)
    if (g != null) g.dispose();

    // maybe this should be done earlier? might help ensure it gets called
    // before the vm just craps out since 1.5 craps out so aggressively.
    disposeMethods.handle();
  }


  /**
   * Called by the browser or applet viewer to inform this applet
   * that it is being reclaimed and that it should destroy
   * any resources that it has allocated.
   * <p/>
   * This also attempts to call PApplet.stop(), in case there
   * was an inadvertent override of the stop() function by a user.
   * <p/>
   * destroy() supposedly gets called as the applet viewer
   * is shutting down the applet. stop() is called
   * first, and then destroy() to really get rid of things.
   * no guarantees on when they're run (on browser quit, or
   * when moving between pages), though.
   */
  public void destroy() {
    ((PApplet)this).stop();
  }


  /**
   * This returns the last width and height specified by the user
   * via the size() command.
   */
  public Dimension getPreferredSize() {
    return new Dimension(width, height);
  }


  //////////////////////////////////////////////////////////////


  public class RegisteredMethods {
    int count;
    Object objects[];
    Method methods[];


    // convenience version for no args
    public void handle() {
      handle(new Object[] { });
    }

    public void handle(Object oargs[]) {
      for (int i = 0; i < count; i++) {
        try {
          //System.out.println(objects[i] + " " + args);
          methods[i].invoke(objects[i], oargs);
        } catch (Exception e) {
          e.printStackTrace();
        }
      }
    }

    public void add(Object object, Method method) {
      if (objects == null) {
        objects = new Object[5];
        methods = new Method[5];
      }
      if (count == objects.length) {
        Object otemp[] = new Object[count << 1];
        System.arraycopy(objects, 0, otemp, 0, count);
        objects = otemp;
        Method mtemp[] = new Method[count << 1];
        System.arraycopy(methods, 0, mtemp, 0, count);
        methods = mtemp;
      }
      objects[count] = object;
      methods[count] = method;
      count++;
    }
  }


  public void registerSize(Object o) {
    Class methodArgs[] = new Class[] { Integer.TYPE, Integer.TYPE };
    registerWithArgs(sizeMethods, "size", o, methodArgs);
  }

  public void registerPre(Object o) {
    registerNoArgs(preMethods, "pre", o);
  }

  public void registerDraw(Object o) {
    registerNoArgs(drawMethods, "draw", o);
  }

  public void registerPost(Object o) {
    registerNoArgs(postMethods, "post", o);
  }

  public void registerMouseEvent(Object o) {
    Class methodArgs[] = new Class[] { MouseEvent.class };
    registerWithArgs(mouseEventMethods, "mouseEvent", o, methodArgs);
  }


  public void registerKeyEvent(Object o) {
    Class methodArgs[] = new Class[] { KeyEvent.class };
    registerWithArgs(keyEventMethods, "keyEvent", o, methodArgs);
  }

  public void registerDispose(Object o) {
    registerNoArgs(disposeMethods, "dispose", o);
  }


  protected void registerNoArgs(RegisteredMethods meth,
                                String name, Object o) {
    Class c = o.getClass();
    try {
      Method method = c.getMethod(name, new Class[] {});
      meth.add(o, method);

    } catch (Exception e) {
      die("Could not register " + name + " + () for " + o, e);
    }
  }


  protected void registerWithArgs(RegisteredMethods meth,
                                  String name, Object o, Class cargs[]) {
    Class c = o.getClass();
    try {
      Method method = c.getMethod(name, cargs);
      meth.add(o, method);

    } catch (Exception e) {
      die("Could not register " + name + " + () for " + o, e);
    }
  }


  //////////////////////////////////////////////////////////////


  public void setup() {
  }


  public void draw() {
    // if no draw method, then shut things down
    //System.out.println("no draw method, goodbye");
    finished = true;
  }


  //////////////////////////////////////////////////////////////


  /**
   * Starts up and creates a two-dimensional drawing surface,
   * or resizes the current drawing surface.
   * <P>
   * This should be the first thing called inside of setup().
   * <P>
   * If using Java 1.3 or later, this will default to using
   * PGraphics2, the Java2D-based renderer. If using Java 1.1,
   * or if PGraphics2 is not available, then PGraphics will be used.
   * To set your own renderer, use the other version of the size()
   * method that takes a renderer as its last parameter.
   * <P>
   * If called once a renderer has already been set, this will
   * use the previous renderer and simply resize it.
   */
  public void size(int iwidth, int iheight) {
    if (g != null) {
      // just resize the current renderer
      size(iwidth, iheight, g.getClass().getName());

    } else {
      // create a JAVA2D renderer (the current default)
      size(iwidth, iheight, JAVA2D);

      /*
      if (PApplet.javaVersion >= 1.3f) {
        try {
          Class c = Class.forName(JAVA2D);
          size(iwidth, iheight, JAVA2D);
          return;

        } catch (ClassNotFoundException e) { }
        size(iwidth, iheight, P2D);  // fall-through case
      }
      */
    }
  }


  public void size(int iwidth, int iheight, String irenderer) {
    size(iwidth, iheight, irenderer, null);
  }


  /**
   * Creates a new PGraphics object and sets it to the specified size.
   * <P>
   * Note that you cannot change the renderer once outside of setup().
   * In most cases, you can call size() to give it a new size,
   * but you need to always ask for the same renderer, otherwise
   * you're gonna run into trouble.
   * <P>
   * XXXX Also note that this calls defaults(), which will reset any
   * XXXX settings for the font, stroke, fill, colorMode, lights, etc.
   */
  public void size(int iwidth, int iheight,
                   String irenderer, String ipath) {
    String currentRenderer =
      (g == null) ? null : g.getClass().getName();
    // ensure that this is an absolute path
    if (ipath != null) ipath = savePath(ipath);

    if (currentRenderer != null) {
      if (currentRenderer.equals(irenderer)) {
        if ((iwidth == g.width) && (iheight == g.height)) {
          // in this case, size() is being called a second time because
          // setup() is being called a second time, since the first time
          // that setup was called, the renderer was changed so an
          // exception was thrown and setup() didn't complete. but this
          // time around, g is the proper size and the proper class.

          // that or size() is being called again for no good reason,
          // in which case we just ignore it anyway.

          // so all that needs to be done is to set the defaults
          // (clear the background, set default strokeWeight, etc).
          //g.defaults();
          // removed this in favor of calling defaults() from beginDraw()

          // this will happen when P3D or OPENGL are used with size()
          // inside of setup. it's also safe to call defaults() now,
          // because it's happening inside setup, which is just frame 0,
          // meaning that the graphics context is proper and visible.

        } else {  // just resizing, no need to create new graphics object
          g.resize(iwidth, iheight);
          updateSize(iwidth, iheight);
          redraw(); // changed for rev 0100
        }
        // in either case, the renderer is unchanged, so return
        //return;

      } else {  // renderer is being changed
        if (frameCount > 0) {
          throw new RuntimeException("size() cannot be called to change " +
                                     "the renderer outside of setup()");
        }
        // otherwise ok to fall through and create renderer below
        // the renderer is changing, so need to create a new object
        g = PApplet.createGraphics(iwidth, iheight, irenderer, ipath, this);
        //g.setMainDrawingSurface();
        //if (g != null) {
        updateSize(iwidth, iheight);
        //}

        // throw an exception so that setup() is called again
        // but with a properly sized render
        // this is for opengl, which needs a valid, properly sized
        // display before calling anything inside setup().
        throw new RuntimeException(NEW_RENDERER);
      }
    } else {  // none exists, just create a freshy
      g = PApplet.createGraphics(iwidth, iheight, irenderer, ipath, this);
      //g.setMainDrawingSurface();
      updateSize(iwidth, iheight);
    }

    /*
    // the renderer is changing, so need to create a new object
    g = createGraphics(iwidth, iheight, irenderer);
    //if (g != null) {
    updateSize(iwidth, iheight);
    //}

    //if ((currentRenderer != null) &&
    //  !currentRenderer.equals(irenderer)) {
    if (currentRenderer != null) {
      // throw an exception so that setup() is called again
      // but with a properly sized render
      // this is for opengl, which needs a valid, properly sized
      // display before calling anything inside setup().
      throw new RuntimeException(NEW_RENDERER);
    }
    */
  }


  /**
   * Sets this.width and this.height, unsets defaultSize, and calls
   * the size() methods inside any libraries.
   */
  protected void updateSize(int iwidth, int iheight) {
    this.width = iwidth;
    this.height = iheight;
    defaultSize = false;

    // make the applet itself larger.. it's a subclass of Component,
    // so this is important for when it's embedded inside another app.
    setSize(width, height);

    // probably needs to mess with the parent frame here?
    // TODO wait for a "legitimate size" flag to be set
    // (meaning that setup has finished properly)
    // at which time the parent frame will do its thing.

    // if the default renderer is just being resized,
    // restore it to its default values
    //g.defaults();
    // no, otherwise fonts that were set in setup() will go away

    // this has to be called after the exception is thrown,
    // otherwise the supporting libs won't have a valid context to draw to
    Object methodArgs[] =
      new Object[] { new Integer(width), new Integer(height) };
    sizeMethods.handle(methodArgs);
  }


  /**
   * Create an offscreen PGraphics object for drawing. This can be used
   * for bitmap or vector images drawing or rendering.
   *
   * <UL>
   * <LI>Do not use "new PGraphicsXxxx()", use this method. This method
   * ensures that internal variables are set up properly that tie the
   * new graphics context back to its parent PApplet.
   * <LI>The basic way to create bitmap images is to use the <A
   * HREF="http://processing.org/reference/saveFrame_.html">saveFrame()</A>
   * function.
   * <LI>If you want to create a really large scene and write that,
   * first make sure that you've allocated a lot of memory in the Preferences.
   * <LI>If you want to create images that are larger than the screen,
   * you should create your own PGraphics object, draw to that, and use
   * <A HREF="http://processing.org/reference/save_.html">save()</A>.
   * For now, it's best to use <A HREF="http://dev.processing.org/reference/everything/javadoc/processing/core/PGraphics3D.html">P3D</A> in this scenario.
   * P2D is currently disabled, and the JAVA2D default will give mixed
   * results. An example of using P3D:
   * <PRE>
   *
   * PGraphics big;
   *
   * void setup() {
   *   big = createGraphics(3000, 3000, P3D);
   *
   *   big.beginDraw();
   *   big.background(128);
   *   big.line(20, 1800, 1800, 900);
   *   // etc..
   *   big.endDraw();
   *
   *   // make sure the file is written to the sketch folder
   *   big.save("big.tif");
   * }
   *
   * </PRE>
   * <LI>It's important to always wrap drawing to createGraphics() with
   * beginDraw() and endDraw() (beginFrame() and endFrame() prior to
   * revision 0115). The reason is that the renderer needs to know when
   * drawing has stopped, so that it can update itself internally.
   * This also handles calling the defaults() method, for people familiar
   * with that.
   * <LI>It's not possible to use createGraphics() with the OPENGL renderer,
   * because it doesn't allow offscreen use.
   * <LI>With Processing 0115 and later, it's possible to write images in
   * formats other than the default .tga and .tiff. The exact formats and
   * background information can be found in the developer's reference for
   * <A HREF="http://dev.processing.org/reference/core/javadoc/processing/core/PImage.html#save(java.lang.String)">PImage.save()</A>.
   * </UL>
   */
  public PGraphics createGraphics(int iwidth, int iheight,
                                  String irenderer) {
    PGraphics pg =
      PApplet.createGraphics(iwidth, iheight, irenderer, null, null);
    pg.parent = this;  // make save() work
    return pg;
  }


  /**
   * Create an offscreen graphics surface for drawing, in this case
   * for a renderer that writes to a file (such as PDF or DXF).
   * @param ipath can be an absolute or relative path
   */
  public PGraphics createGraphics(int iwidth, int iheight,
                                  String irenderer, String ipath) {
    if (ipath != null) {
      ipath = savePath(ipath);
    }
    PGraphics pg =
      PApplet.createGraphics(iwidth, iheight, irenderer, ipath, null);
    pg.parent = this;  // make save() work
    return pg;
  }


  /**
   * Version of createGraphics() used internally.
   *
   * @param ipath must be an absolute path, usually set via savePath()
   * @oaram applet the parent applet object, this should only be non-null
   *               in cases where this is the main drawing surface object.
   */
  static protected PGraphics createGraphics(int iwidth, int iheight,
                                            String irenderer, String ipath,
                                            PApplet applet) {
    if (irenderer.equals(OPENGL)) {
      if (PApplet.platform == WINDOWS) {
        String s = System.getProperty("java.version");
        if (s != null) {
          if (s.equals("1.5.0_10")) {
            System.err.println("OpenGL support is broken with Java 1.5.0_10");
            System.err.println("See http://dev.processing.org" +
                               "/bugs/show_bug.cgi?id=513 for more info.");
            throw new RuntimeException("Please update your Java " +
                                       "installation (see bug #513)");
          }
        }
      }
    }

    /*
    if (irenderer.equals(P2D)) {
      throw new RuntimeException("P2D is not yet implemented, " +
                                 "use JAVA2D or P3D instead.");
    }
    */

    /*
    // ok when calling size, but not really with createGraphics()
    if (renderer.equals(OPENGL)) {
      throw new RuntimeException("createGraphics() with OPENGL is not " +
                                 "supported. Use P3D instead.");
    }
    */

    String openglError =
      "Before using OpenGL, first select " +
      "Import Library > opengl from the Sketch menu.";

    try {
      Class rendererClass = Class.forName(irenderer);
      Class constructorParams[] = null;
      Object constructorValues[] = null;

      if (ipath == null) {
        constructorParams = new Class[] {
          Integer.TYPE, Integer.TYPE, PApplet.class
        };
        constructorValues = new Object[] {
          new Integer(iwidth), new Integer(iheight), applet
        };
      } else {
        // first make sure that this in a nice, full, absolute path
        //ipath = applet.savePath(ipath);

        constructorParams = new Class[] {
          Integer.TYPE, Integer.TYPE, PApplet.class, String.class
        };
        constructorValues = new Object[] {
          new Integer(iwidth), new Integer(iheight), applet, ipath
        };
      }

      Constructor constructor =
        rendererClass.getConstructor(constructorParams);
      PGraphics pg = (PGraphics) constructor.newInstance(constructorValues);
      return pg;

    } catch (InvocationTargetException ite) {
      String msg = ite.getTargetException().getMessage();
      if ((msg != null) &&
          (msg.indexOf("no jogl in java.library.path") != -1)) {
        throw new RuntimeException(openglError);

      } else {
        ite.getTargetException().printStackTrace();
        Throwable target = ite.getTargetException();
        if (platform == MACOSX) target.printStackTrace(System.out);  // bug
        // neither of these help, or work
        //target.printStackTrace(System.err);
        //System.err.flush();
        //System.out.println(System.err);  // and the object isn't null
        throw new RuntimeException(target.getMessage());
      }

    } catch (ClassNotFoundException cnfe) {
      if (cnfe.getMessage().indexOf("processing.opengl.PGraphicsGL") != -1) {
        throw new RuntimeException(openglError);
      } else {
        throw new RuntimeException("You need to use \"Import Library\" " +
                                   "to add " + irenderer + " to your sketch.");
      }

    } catch (Exception e) {
      //System.out.println("ex3");
      if ((e instanceof IllegalArgumentException) ||
          (e instanceof NoSuchMethodException) ||
          (e instanceof IllegalAccessException)) {

        String msg = "public " +
          irenderer.substring(irenderer.lastIndexOf('.') + 1) +
          "(int width, int height, PApplet parent" +
          ((ipath == null) ? "" : ", String filename") +
          ") does not exist.";
        throw new RuntimeException(msg);

      } else {
        if (platform == MACOSX) e.printStackTrace(System.out);
        throw new RuntimeException(e.getMessage());
      }
    }
  }


  /**
   * Preferred method of creating new PImage objects, ensures that a
   * reference to the parent PApplet is included, which makes save() work
   * without needing an absolute path.
   */
  public PImage createImage(int wide, int high, int format) {
    PImage image = new PImage(wide, high, format);
    image.parent = this;  // make save() work
    return image;
  }

  public void update(Graphics screen) {
    //System.out.println("PApplet.update()");
    if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                              "    4 update() external");
    paint(screen);
  }


  synchronized public void paint(Graphics screen) {
    if (javaVersion < 1.3f) {
      screen.setColor(new Color(64, 64, 64));
      Dimension size = getSize();
      screen.fillRect(0, 0, size.width, size.height);
      screen.setColor(Color.white);
      screen.setFont(new Font("Dialog", Font.PLAIN, 9));
      screen.drawString("You need to install", 3, 15);
      screen.drawString("Java 1.3 or later", 3, 28);
      screen.drawString("to view this content.", 3, 41);
      screen.drawString("Click here to visit", 3, 59);
      screen.drawString("java.com and install.", 3, 72);
      return;
    }

    //System.out.println("PApplet.paint()");
    if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                              "     5a enter paint");

    // ignore the very first call to paint, since it's coming
    // from the o.s., and the applet will soon update itself anyway.
    //if (firstFrame) return;
    if (frameCount == 0) {
      // paint() may be called more than once before things
      // are finally painted to the screen and the thread gets going
      //System.out.println("not painting");
      /*
      if (thread == null) {
        initGraphics();
        start();
      }
      */
      return;
    }

    // without ignoring the first call, the first several frames
    // are confused because paint() gets called in the midst of
    // the initial nextFrame() call, so there are multiple
    // updates fighting with one another.

    // g.image is synchronized so that draw/loop and paint don't
    // try to fight over it. this was causing a randomized slowdown
    // that would cut the frameRate into a third on macosx,
    // and is probably related to the windows sluggishness bug too
    if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                              "     5b enter paint sync");

    //synchronized (g) {
    //synchronized (glock) {
      if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                                "     5c inside paint sync");
      //System.out.println("5b paint has sync");
      //Exception e = new Exception();
      //e.printStackTrace();

      // moving this into PGraphics caused weird sluggishness on win2k
      //g.mis.newPixels(pixels, g.cm, 0, width); // must call this

      // make sure the screen is visible and usable
      // (also prevents over-drawing when using PGraphicsGL)
      if ((g != null) && (g.image != null)) {
        screen.drawImage(g.image, 0, 0, null);
      }
      //if (THREAD_DEBUG) println("notifying all");
      //notifyAll();
      //thread.notify();
      //System.out.println("      6 exit paint");
    //}
    if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                              "      6 exit paint");
    //updated = true;
  }


  synchronized public void handleDisplay() {
    if (PApplet.THREAD_DEBUG) println(Thread.currentThread().getName() +
                                      " formerly nextFrame()");
    if (looping || redraw) {
      /*
        if (frameCount == 0) {  // needed here for the sync
        //createGraphics();
        // set up a dummy graphics in case size() is never
        // called inside setup
        size(INITIAL_WIDTH, INITIAL_HEIGHT);
        }
      */

      // g may be rebuilt inside here, so turning of the sync
      //synchronized (g) {
      // use a different sync object
      //synchronized (glock) {
        if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                                  " 1a beginDraw");
        g.beginDraw();
        if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                                  " 1b draw");

        //boolean recorderNull = true;
        //boolean recorderRawNull = true;

        if (frameCount == 0) {
          try {
            //System.out.println("attempting setup");
            //System.out.println("into try");
            setup();
            //g.defaults();

            //System.out.println("done attempting setup");
            //System.out.println("out of try");
            //g.postSetup();  // FIXME

          } catch (RuntimeException e) {
            //System.out.println("runtime extends " + e);
            //System.out.println("catching a cold " + e.getMessage());
            String msg = e.getMessage();
            if ((msg != null) &&
                (e.getMessage().indexOf(NEW_RENDERER) != -1)) {
              //System.out.println("got new renderer");
              return;
              //continue;  // will this work?

            } else {
              //e.printStackTrace(System.out);
              //System.out.println("re-throwing");
              throw e;
            }
          }
          // if depth() is called inside setup, pixels/width/height
          // will be ok by the time it's back out again

          //this.pixels = g.pixels;  // make em call loadPixels
          // now for certain that we've got a valid size
          this.width = g.width;
          this.height = g.height;
          this.defaultSize = false;

        } else {  // frameCount > 0, meaning an actual draw()
          // update the current frameRate
          if (frameRateLastMillis != 0) {
            float elapsed = (float)
              (System.currentTimeMillis() - frameRateLastMillis);
            if (elapsed != 0) {
              frameRate =
                (frameRate * 0.9f) + ((1.0f / (elapsed / 1000.0f)) * 0.1f);
            }
          }
          frameRateLastMillis = System.currentTimeMillis();

          /*
          if (frameRateTarget != 0) {
            //System.out.println("delaying");
            if (frameRateLastDelayTime == 0) {
              frameRateLastDelayTime = System.currentTimeMillis();

            } else {
              long timeToLeave =
                frameRateLastDelayTime + (long)(1000.0f / frameRateTarget);
              long now = System.currentTimeMillis();
              int napTime = (int) (timeToLeave - now);
              if (napTime > 0) {
                frameRateLastDelayTime = timeToLeave;
                delay(napTime);
              } else {
                // nap time is negative, need to reset clock (bug #336)
                frameRateLastDelayTime = now;
              }
            }
          }
          */

          preMethods.handle();

          pmouseX = dmouseX;
          pmouseY = dmouseY;

          //synchronized (glock) {
          //synchronized (this) {
          //try {
          draw();
            /*
              // seems to catch, but then blanks out
          } catch (Exception e) {
            if (e instanceof InvocationTargetException) {
              System.out.println("found poo");
              ((InvocationTargetException)e).getTargetException().printStackTrace(System.out);
            }
          }
            */
          //}
          //}

          // set a flag regarding whether the recorders were non-null
          // as of draw().. this will prevent the recorder from being
          // reset if recordShape() is called in an event method, such
          // as mousePressed()
          //recorderNull = (recorder == null);
          //recorderRawNull = (g.recorderRaw == null);

          // dmouseX/Y is updated only once per frame
          dmouseX = mouseX;
          dmouseY = mouseY;

          // these are called *after* loop so that valid
          // drawing commands can be run inside them. it can't
          // be before, since a call to background() would wipe
          // out anything that had been drawn so far.
          dequeueMouseEvents();

          dequeueKeyEvents();
          if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                                    " 2b endDraw");

          drawMethods.handle();
          //for (int i = 0; i < libraryCount; i++) {
          //if (libraryCalls[i][PLibrary.DRAW]) libraries[i].draw();
          //}

          redraw = false;  // unset 'redraw' flag in case it was set
          // (only do this once draw() has run, not just setup())
        }

        g.endDraw();
        /*
        if (!recorderNull) {
          if (recorder != null) {
            recorder.endDraw();
            recorder = null;
          }
        }
        if (!recorderRawNull) {
          if (g.recorderRaw != null) {
            g.recorderRaw.endDraw();
            g.recorderRaw = null;
          }
        }
        */

        //}  // older end sync

        //update();
        // formerly 'update'
        //if (firstFrame) firstFrame = false;
        // internal frame counter
        frameCount++;
        if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                                  "   3a calling repaint() " + frameCount);
        repaint();
        if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                                  "   3b calling Toolkit.sync " + frameCount);
        getToolkit().sync();  // force repaint now (proper method)
        if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                                  "   3c done " + frameCount);
        //if (THREAD_DEBUG) println("   3d waiting");
        //wait();
        //if (THREAD_DEBUG) println("   3d out of wait");
        //frameCount++;

        postMethods.handle();
        //for (int i = 0; i < libraryCount; i++) {
        //if (libraryCalls[i][PLibrary.POST]) libraries[i].post();
        //}
        //}  // end of synchronize
    }
  }


  //////////////////////////////////////////////////////////////


  public void run() {  // not good to make this synchronized, locks things up
    try {
      while ((Thread.currentThread() == thread) && !finished) {
        // render a single frame
        g.requestDisplay(this);

        // wait for update & paint to happen before drawing next frame
        // this is necessary since the drawing is sometimes in a
        // separate thread, meaning that the next frame will start
        // before the update/paint is completed

        try {
          // Windows doesn't like Thread.yield(), acts as though it hasn't
          // even been called and starves the CPU anyway. So have to sleep
          // (or wait) at least for some small amount of time (below).
          //Thread.yield();

          // Can't remember when/why I changed the generic nap time to '1'
          // (rather than 3 or 5, like back in the day), but I have a feeling
          // that some platforms aren't gonna like that.

          // If !looping, sleeps for a nice long time, or until an
          // interrupt or notify from a call to loop/noLoop/redraw

          int nap = (looping || finished) ? 1 : 10000;

          // don't nap after setup, because if noLoop() is called this
          // will make the first draw wait 10 seconds before showing up
          if (frameCount == 1) {
            nap = 1;

          } else if (finished) {
            nap = 0;

          } else if (looping) {
            if (frameRateTarget != 0) {
              if (frameRateLastDelayTime == 0) {
                frameRateLastDelayTime = System.currentTimeMillis();

              } else {
                long timeToLeave =
                  frameRateLastDelayTime + (long)(1000.0f / frameRateTarget);
                long now = System.currentTimeMillis();
                nap = (int) (timeToLeave - now);
                if (nap > 0) {
                  frameRateLastDelayTime = timeToLeave;
                  //delay(napTime);
                  //nap = napTime;

                } else {
                  // nap time is negative, need to reset clock (bug #336)
                  frameRateLastDelayTime = now;
                }
              }
            } else {
              nap = 1;
            }
          }

          if (CRUSTY_THREADS) {
            Thread.sleep(nap);
          } else {
            synchronized (blocker) {
              if (nap > 0) blocker.wait(nap);
            }
          }
        } catch (InterruptedException e) { }
      }

    } catch (Exception e) {
      // note that this will not catch errors inside setup()
      // those are caught by the PdeRuntime

      //System.out.println("exception occurred (if you don't see a stack " +
      //                 "trace below this message, we've got a bug)");
      finished = true;
      if (e instanceof InvocationTargetException) {
        //System.out.println("target problem");
        e = (Exception) (((InvocationTargetException) e).getTargetException());
      }
      exception = e;
      //e.printStackTrace(System.out);

      if (leechErr != null) {
        // if draw() mode, make sure that ui stops waiting
        // and the run button quits out
        leechErr.println(LEECH_WAKEUP);
        e.printStackTrace(leechErr);
        e.printStackTrace(System.out);

      } else {
        System.err.println(LEECH_WAKEUP);
        e.printStackTrace();
        e.printStackTrace(System.out);
      }
    }
    if (THREAD_DEBUG) println(Thread.currentThread().getName() +
                              " thread finished");

    // this may not be safe? this will get triggered with exit()
    // but need to see if this is it
    //if ((leechErr == null) && !online) {
    //System.exit(0);
    //}

    //System.out.println("exiting run " + finished);
    stop();  // call to shutdown libs?

    if (exit) {  // user called exit() function
      if ((leechErr == null) && !online) {
        // don't want to call System.exit() when an applet,
        // or running inside the PDE (would kill the PDE)
        System.exit(0);
      }
    }
  }


  synchronized public void redraw() {
    if (!looping) {
      redraw = true;
      if (thread != null) {
        // wake from sleep (necessary otherwise it'll be
        // up to 10 seconds before update)
        if (CRUSTY_THREADS) {
          thread.interrupt();
        } else {
          synchronized (blocker) {
            blocker.notifyAll();
          }
        }
      }
    }
  }


  synchronized public void loop() {
    if (!looping) {
      looping = true;
      if (thread != null) {
        // wake from sleep (necessary otherwise it'll be
        // up to 10 seconds before update)
        if (CRUSTY_THREADS) {
          thread.interrupt();
        } else {
          synchronized (blocker) {
            blocker.notifyAll();
          }
        }
      }
    }
  }


  synchronized public void noLoop() {
    if (looping) {
      looping = false;

      // reset frameRate delay times
      frameRateLastDelayTime = 0;
      frameRateLastMillis = 0;

      if (thread != null) {
        if (CRUSTY_THREADS) {
          thread.interrupt();  // wake from sleep
        } else {
          synchronized (blocker) {
            blocker.notifyAll();
          }
          /*
            try {
              wait();  // until a notify
            } catch (InterruptedException e) { }
          */
        }
      }
    }
  }


  //////////////////////////////////////////////////////////////


  protected boolean listenersAdded;

  public void addListeners() {
    if (!listenersAdded) {
      addMouseListener(this);
      addMouseMotionListener(this);
      addKeyListener(this);
      addFocusListener(this);

      addComponentListener(new ComponentAdapter() {
          public void componentResized(ComponentEvent e) {
            Component c = e.getComponent();
            Rectangle bounds = c.getBounds();
            //System.out.println("componentResized()");
            //System.out.println("  " + c.getClass().getName());
            //println("  visible " + isVisible());
            //System.out.println("  " + e);
            //System.out.println("  bounds: " + bounds);
            //int newWidth = bounds.width - bounds.x * 2;
            //int newHeight = bounds.height - (bounds.y + bounds.x);
            //System.out.println("  new: " + newWidth + " " + newHeight);

            size(bounds.width, bounds.height);

            //if (c == PApplet.this) {
            //Container con = (Container) c;
            //Dimension newSize = getSize();
            //System.out.println("resizing to " + newSize + " ");
            //System.out.println(c.getBounds());
            //System.out.println(e);
            //System.out.println(c);
            //System.out.println("insets " + con.getInsets());
            //size(newSize.width, newSize.height);
            //}
          }
        });

      listenersAdded = true;
    }
  }


  //////////////////////////////////////////////////////////////


  MouseEvent mouseEventQueue[] = new MouseEvent[10];
  int mouseEventCount;

  protected void enqueueMouseEvent(MouseEvent e) {
    synchronized (mouseEventQueue) {
      if (mouseEventCount == mouseEventQueue.length) {
        MouseEvent temp[] = new MouseEvent[mouseEventCount << 1];
        System.arraycopy(mouseEventQueue, 0, temp, 0, mouseEventCount);
        mouseEventQueue = temp;
      }
      mouseEventQueue[mouseEventCount++] = e;
    }
  }

  protected void dequeueMouseEvents() {
    synchronized (mouseEventQueue) {
      for (int i = 0; i < mouseEventCount; i++) {
        mouseEvent = mouseEventQueue[i];
        handleMouseEvent(mouseEvent);
      }
      mouseEventCount = 0;
    }
  }


  /**
   * Actually take action based on a mouse event.
   * Internally updates mouseX, mouseY, mousePressed, and mouseEvent.
   * Then it calls the event type with no params,
   * i.e. mousePressed() or mouseReleased() that the user may have
   * overloaded to do something more useful.
   */
  protected void handleMouseEvent(MouseEvent event) {
    int id = event.getID();

    // http://dev.processing.org/bugs/show_bug.cgi?id=170
    // also prevents mouseExited() on the mac from hosing the mouse
    // position, because x/y are bizarre values on the exit event.
    // see also the id check below.. both of these go together
    if ((id == MouseEvent.MOUSE_DRAGGED) ||
        (id == MouseEvent.MOUSE_MOVED)) {
      pmouseX = emouseX;
      pmouseY = emouseY;
      mouseX = event.getX();
      mouseY = event.getY();
    }

    mouseEvent = event;

    int modifiers = event.getModifiers();
    if ((modifiers & InputEvent.BUTTON1_MASK) != 0) {
      mouseButton = LEFT;
    } else if ((modifiers & InputEvent.BUTTON2_MASK) != 0) {
      mouseButton = CENTER;
    } else if ((modifiers & InputEvent.BUTTON3_MASK) != 0) {
      mouseButton = RIGHT;
    }
    // if running on macos, allow ctrl-click as right mouse
    if ((platform == MACOSX) || (platform == MACOS9)) {
      if (mouseEvent.isPopupTrigger()) {
        mouseButton = RIGHT;
      }
    }

    mouseEventMethods.handle(new Object[] { event });

    // this used to only be called on mouseMoved and mouseDragged
    // change it back if people run into trouble
    if (firstMouse) {
      pmouseX = mouseX;
      pmouseY = mouseY;
      dmouseX = mouseX;
      dmouseY = mouseY;
      firstMouse = false;
    }

    //println(event);

    switch (id) {
    case MouseEvent.MOUSE_PRESSED:
      mousePressed = true;
      mousePressed();
      break;
    case MouseEvent.MOUSE_RELEASED:
      mousePressed = false;
      mouseReleased();
      break;
    case MouseEvent.MOUSE_CLICKED:
      mouseClicked();
      break;
    case MouseEvent.MOUSE_DRAGGED:
      mouseDragged();
      break;
    case MouseEvent.MOUSE_MOVED:
      mouseMoved();
      break;
    }

    if ((id == MouseEvent.MOUSE_DRAGGED) ||
        (id == MouseEvent.MOUSE_MOVED)) {
      emouseX = mouseX;
      emouseY = mouseY;
    }
  }


  /**
   * Figure out how to process a mouse event. When loop() has been
   * called, the events will be queued up until drawing is complete.
   * If noLoop() has been called, then events will happen immediately.
   */
  protected void checkMouseEvent(MouseEvent event) {
    if (looping) {
      enqueueMouseEvent(event);
    } else {
      handleMouseEvent(event);
    }
  }


  /**
   * If you override this or any function that takes a "MouseEvent e"
   * without calling its super.mouseXxxx() then mouseX, mouseY,
   * mousePressed, and mouseEvent will no longer be set.
   */
  public void mousePressed(MouseEvent e) {
    checkMouseEvent(e);
  }

  public void mouseReleased(MouseEvent e) {
    checkMouseEvent(e);
  }

  public void mouseClicked(MouseEvent e) {
    checkMouseEvent(e);
  }

  public void mouseEntered(MouseEvent e) {
    checkMouseEvent(e);
  }

  public void mouseExited(MouseEvent e) {
    checkMouseEvent(e);
  }

  public void mouseDragged(MouseEvent e) {
    checkMouseEvent(e);
  }

  public void mouseMoved(MouseEvent e) {
    checkMouseEvent(e);
  }


  /**
   * Mouse has been pressed, and should be considered "down"
   * until mouseReleased() is called. If you must, use
   * int button = mouseEvent.getButton();
   * to figure out which button was clicked. It will be one of:
   * MouseEvent.BUTTON1, MouseEvent.BUTTON2, MouseEvent.BUTTON3
   * Note, however, that this is completely inconsistent across
   * platforms.
   */
  public void mousePressed() { }

  /**
   * Mouse button has been released.
   */
  public void mouseReleased() { }

  /**
   * When the mouse is clicked, mousePressed() will be called,
   * then mouseReleased(), then mouseClicked(). Note that
   * mousePressed is already false inside of mouseClicked().
   */
  public void mouseClicked() { }

  /**
   * Mouse button is pressed and the mouse has been dragged.
   */
  public void mouseDragged() { }

  /**
   * Mouse button is not pressed but the mouse has changed locations.
   */
  public void mouseMoved() { }


  //////////////////////////////////////////////////////////////


  KeyEvent keyEventQueue[] = new KeyEvent[10];
  int keyEventCount;

  protected void enqueueKeyEvent(KeyEvent e) {
    synchronized (keyEventQueue) {
      if (keyEventCount == keyEventQueue.length) {
        KeyEvent temp[] = new KeyEvent[keyEventCount << 1];
        System.arraycopy(keyEventQueue, 0, temp, 0, keyEventCount);
        keyEventQueue = temp;
      }
      keyEventQueue[keyEventCount++] = e;
    }
  }

  protected void dequeueKeyEvents() {
    synchronized (keyEventQueue) {
      for (int i = 0; i < keyEventCount; i++) {
        keyEvent = keyEventQueue[i];
        handleKeyEvent(keyEvent);
      }
      keyEventCount = 0;
    }
  }


  protected void handleKeyEvent(KeyEvent event) {
    keyEvent = event;
    key = event.getKeyChar();
    keyCode = event.getKeyCode();

    keyEventMethods.handle(new Object[] { event });
    /*
    for (int i = 0; i < libraryCount; i++) {
      if (libraryCalls[i][PLibrary.KEY]) {
        libraries[i].key(event);  // endNet/endSerial etc
      }
    }
    */

    switch (event.getID()) {
    case KeyEvent.KEY_PRESSED:
      keyPressed = true;
      keyPressed();
      break;
    case KeyEvent.KEY_RELEASED:
      keyPressed = false;
      keyReleased();
      break;
    case KeyEvent.KEY_TYPED:
      keyTyped();
      break;
    }

    // if someone else wants to intercept the key, they should
    // set key to zero (or something besides the ESC).
    if ((event.getID() == KeyEvent.KEY_PRESSED) &&
        (key == KeyEvent.VK_ESCAPE)) {
      exit();
    }
  }


  protected void checkKeyEvent(KeyEvent event) {
    if (looping) {
      enqueueKeyEvent(event);
    } else {
      handleKeyEvent(event);
    }
  }


  /**
   * Overriding keyXxxxx(KeyEvent e) functions will cause the 'key',
   * 'keyCode', and 'keyEvent' variables to no longer work;
   * key events will no longer be queued until the end of draw();
   * and the keyPressed(), keyReleased() and keyTyped() methods
   * will no longer be called.
   */
  public void keyPressed(KeyEvent e) { checkKeyEvent(e); }
  public void keyReleased(KeyEvent e) { checkKeyEvent(e); }
  public void keyTyped(KeyEvent e) { checkKeyEvent(e); }


  /**
   * Called each time a single key on the keyboard is pressed.
   * Because of how operating systems handle key repeats, holding
   * down a key will cause multiple calls to keyPressed(), because
   * the OS repeat takes over.
   * <P>
   * Examples for key handling:
   * (Tested on Windows XP, please notify if different on other
   * platforms, I have a feeling Mac OS and Linux may do otherwise)
   * <PRE>
   * 1. Pressing 'a' on the keyboard:
   *    keyPressed  with key == 'a' and keyCode == 'A'
   *    keyTyped    with key == 'a' and keyCode ==  0
   *    keyReleased with key == 'a' and keyCode == 'A'
   *
   * 2. Pressing 'A' on the keyboard:
   *    keyPressed  with key == 'A' and keyCode == 'A'
   *    keyTyped    with key == 'A' and keyCode ==  0
   *    keyReleased with key == 'A' and keyCode == 'A'
   *
   * 3. Pressing 'shift', then 'a' on the keyboard (caps lock is off):
   *    keyPressed  with key == CODED and keyCode == SHIFT
   *    keyPressed  with key == 'A'   and keyCode == 'A'
   *    keyTyped    with key == 'A'   and keyCode == 0
   *    keyReleased with key == 'A'   and keyCode == 'A'
   *    keyReleased with key == CODED and keyCode == SHIFT
   *
   * 4. Holding down the 'a' key.
   *    The following will happen several times,
   *    depending on your machine's "key repeat rate" settings:
   *    keyPressed  with key == 'a' and keyCode == 'A'
   *    keyTyped    with key == 'a' and keyCode ==  0
   *    When you finally let go, you'll get:
   *    keyReleased with key == 'a' and keyCode == 'A'
   *
   * 5. Pressing and releasing the 'shift' key
   *    keyPressed  with key == CODED and keyCode == SHIFT
   *    keyReleased with key == CODED and keyCode == SHIFT
   *    (note there is no keyTyped)
   *
   * 6. Pressing the tab key in an applet with Java 1.4 will
   *    normally do nothing, but PApplet dynamically shuts
   *    this behavior off if Java 1.4 is in use (tested 1.4.2_05 Windows).
   *    Java 1.1 (Microsoft VM) passes the TAB key through normally.
   *    Not tested on other platforms or for 1.3.
   * </PRE>
   */
  public void keyPressed() { }


  /**
   * See keyPressed().
   */
  public void keyReleased() { }


  /**
   * Only called for "regular" keys like letters,
   * see keyPressed() for full documentation.
   */
  public void keyTyped() { }


  //////////////////////////////////////////////////////////////

  // i am focused man, and i'm not afraid of death.
  // and i'm going all out. i circle the vultures in a van
  // and i run the block.


  public void focusGained() { }

  public void focusGained(FocusEvent e) {
    focused = true;
    focusGained();
  }


  public void focusLost() { }

  public void focusLost(FocusEvent e) {
    focused = false;
    focusLost();
  }


  //////////////////////////////////////////////////////////////

  // getting the time


  /**
   * Get the number of milliseconds since the applet started.
   * <P>
   * This is a function, rather than a variable, because it may
   * change multiple times per frame.
   */
  public int millis() {
    return (int) (System.currentTimeMillis() - millisOffset);
  }

  /** Seconds position of the current time. */
  static public int second() {
    return Calendar.getInstance().get(Calendar.SECOND);
  }

  /** Minutes position of the current time. */
  static public int minute() {
    return Calendar.getInstance().get(Calendar.MINUTE);
  }

  /**
   * Hour position of the current time in international format (0-23).
   * <P>
   * To convert this value to American time: <BR>
   * <PRE>int yankeeHour = (hour() % 12);
   * if (yankeeHour == 0) yankeeHour = 12;</PRE>
   */
  static public int hour() {
    return Calendar.getInstance().get(Calendar.HOUR_OF_DAY);
  }

  /**
   * Get the current day of the month (1 through 31).
   * <P>
   * If you're looking for the day of the week (M-F or whatever)
   * or day of the year (1..365) then use java's Calendar.get()
   */
  static public int day() {
    return Calendar.getInstance().get(Calendar.DAY_OF_MONTH);
  }

  /**
   * Get the current month in range 1 through 12.
   */
  static public int month() {
    // months are number 0..11 so change to colloquial 1..12
    return Calendar.getInstance().get(Calendar.MONTH) + 1;
  }

  /**
   * Get the current year.
   */
  static public int year() {
    return Calendar.getInstance().get(Calendar.YEAR);
  }


  //////////////////////////////////////////////////////////////

  // controlling time (playing god)


  /**
   * The delay() function causes the program to halt for a specified time.
   * Delay times are specified in thousandths of a second. For example,
   * running delay(3000) will stop the program for three seconds and
   * delay(500) will stop the program for a half-second. Remember: the
   * display window is updated only at the end of draw(), so putting more
   * than one delay() inside draw() will simply add them together and the new
   * frame will be drawn when the total delay is over.
   * <br/> <br/>
   * I'm not sure if this is even helpful anymore, as the screen isn't
   * updated before or after the delay, meaning which means it just
   * makes the app lock up temporarily.
   */
  public void delay(int napTime) {
    if (frameCount == 0) return;
    if (napTime > 0) {
      try {
        if (CRUSTY_THREADS) {
          Thread.sleep(napTime);
        } else {
          wait(napTime);
        }
      } catch (InterruptedException e) { }
    }
  }


  /**
   * Set a target frameRate. This will cause delay() to be called
   * after each frame so that the sketch synchronizes to a particular speed.
   * Note that this only sets the maximum frame rate, it cannot be used to
   * make a slow sketch go faster. Sketches have no default frame rate
   * setting, and will attempt to use maximum processor power to achieve
   * maximum speed.
   */
  public void frameRate(float newRateTarget) {
    this.frameRateTarget = newRateTarget;
  }


  //////////////////////////////////////////////////////////////


  /**
   * Get a param from the web page, or (eventually)
   * from a properties file.
   */
  public String param(String what) {
    if (online) {
      return getParameter(what);

    } else {
      System.err.println("param() only works inside a web browser");
    }
    return null;
  }


  /**
   * Show status in the status bar of a web browser, or in the
   * System.out console. Eventually this might show status in the
   * p5 environment itself, rather than relying on the console.
   */
  public void status(String what) {
    if (online) {
      showStatus(what);

    } else {
      System.out.println(what);  // something more interesting?
    }
  }


  public void link(String here) {
    link(here, null);
  }


  /**
   * Link to an external page without all the muss.
   * <P>
   * When run with an applet, uses the browser to open the url,
   * for applications, attempts to launch a browser with the url.
   * <P>
   * Works on Mac OS X and Windows. For Linux, use:
   * <PRE>open(new String[] { "firefox", url });</PRE>
   * or whatever you want as your browser, since Linux doesn't
   * yet have a standard method for launching URLs.
   */
  public void link(String url, String frameTitle) {
    if (online) {
      try {
        if (frameTitle == null) {
          getAppletContext().showDocument(new URL(url));
        } else {
          getAppletContext().showDocument(new URL(url), frameTitle);
        }
      } catch (Exception e) {
        e.printStackTrace();
        throw new RuntimeException("Could not open " + url);
      }
    } else {
      try {
        if (platform == WINDOWS) {
          // the following uses a shell execute to launch the .html file
          // note that under cygwin, the .html files have to be chmodded +x
          // after they're unpacked from the zip file. i don't know why,
          // and don't understand what this does in terms of windows
          // permissions. without the chmod, the command prompt says
          // "Access is denied" in both cygwin and the "dos" prompt.
          //Runtime.getRuntime().exec("cmd /c " + currentDir + "\\reference\\" +
          //                    referenceFile + ".html");

          // replace ampersands with control sequence for DOS.
          // solution contributed by toxi on the bugs board.
          url = url.replaceAll("&","^&");

          // open dos prompt, give it 'start' command, which will
          // open the url properly. start by itself won't work since
          // it appears to need cmd
          Runtime.getRuntime().exec("cmd /c start " + url);

        } else if ((platform == MACOSX) || (platform == MACOS9)) {
          //com.apple.mrj.MRJFileUtils.openURL(url);
          try {
            Class mrjFileUtils = Class.forName("com.apple.mrj.MRJFileUtils");
            Method openMethod =
              mrjFileUtils.getMethod("openURL", new Class[] { String.class });
            openMethod.invoke(null, new Object[] { url });
          } catch (Exception e) {
            e.printStackTrace();
          }
        } else {
          //throw new RuntimeException("Can't open URLs for this platform");
          // Just pass it off to open() and hope for the best
          open(url);
        }
      } catch (IOException e) {
        e.printStackTrace();
        throw new RuntimeException("Could not open " + url);
      }
    }
  }


  /**
   * Attempt to open a file using the platform's shell.
   */
  public void open(String filename) {
    open(new String[] { filename });
  }


  static String openLauncher;

  /**
   * Launch a process using a platforms shell. This version uses an array
   * to make it easier to deal with spaces in the individual elements.
   * (This avoids the situation of trying to put single or double quotes
   * around different bits).
   */
  static public Process open(String argv[]) {
    String[] params = null;

    if (platform == WINDOWS) {
      // just launching the .html file via the shell works
      // but make sure to chmod +x the .html files first
      // also place quotes around it in case there's a space
      // in the user.dir part of the url
      params = new String[] { "cmd", "/c" };

    } else if (platform == MACOSX) {
      params = new String[] { "open" };

    } else if (platform == LINUX) {
      if (openLauncher == null) {
        // Attempt to use gnome-open
        try {
          Process p = Runtime.getRuntime().exec(new String[] { "gnome-open" });
          /*int result =*/ p.waitFor();
          // Not installed will throw an IOException (JDK 1.4.2, Ubuntu 7.04)
          openLauncher = "gnome-open";
        } catch (Exception e) { }
      }
      if (openLauncher == null) {
        // Attempt with kde-open
        try {
          Process p = Runtime.getRuntime().exec(new String[] { "kde-open" });
          /*int result =*/ p.waitFor();
          openLauncher = "kde-open";
        } catch (Exception e) { }
      }
      if (openLauncher == null) {
        System.err.println("Could not find gnome-open or kde-open, " +
                           "the open() command may not work.");
      }
      if (openLauncher != null) {
        params = new String[] { openLauncher };
      }
    //} else {  // give up and just pass it to Runtime.exec()
      //open(new String[] { filename });
      //params = new String[] { filename };
    }
    if (params != null) {
      // If the 'open', 'gnome-open' or 'cmd' are already included
      if (params[0].equals(argv[0])) {
        // then don't prepend those params again
        return exec(argv);
      } else {
        params = concat(params, argv);
        return exec(params);
      }
    } else {
      return exec(argv);
    }
  }


  static public Process exec(String[] argv) {
    try {
      return Runtime.getRuntime().exec(argv);
    } catch (Exception e) {
      e.printStackTrace();
      throw new RuntimeException("Could not open " + join(argv, ' '));
    }
  }

    /*
    try {
      Runtime.getRuntime().exec("cmd /c \"" + filename + "\"");
    } catch (IOException e) {
      e.printStackTrace();
      throw new RuntimeException("Could not open " + filename);
    }

    try {
      return Runtime.getRuntime().exec(argv);
    } catch (Exception e) {
      e.printStackTrace();
      throw new RuntimeException("Could not open " + join(argv, ' '));
    }
  }

  /*
  static protected String findLinuxLauncher() {
    if (linuxLauncher == null) {
      // Attempt to use gnome-open
      try {
        Process p = Runtime.getRuntime().exec(new String[] { "gnome-open" });
        int result = p.waitFor();
        // Not installed will throw an IOException (JDK 1.4.2, Ubuntu 7.04)
        linuxLauncher = "gnome-open";
      } catch (Exception e) { }

      // Attempt with kde-open
      try {
        Process p = Runtime.getRuntime().exec(new String[] { "kde-open" });
        int result = p.waitFor();
        linuxLauncher = "kde-open";
      } catch (Exception e) { }
    }
    if (linuxLauncher == null) {
      System.err.println("Could not find gnome-open or kde-open, " +
                         "the open() command may not work.");
    }
    return linuxLauncher;
  }
  */


  //////////////////////////////////////////////////////////////


  /**
   * Function for an applet/application to kill itself and
   * display an error. Mostly this is here to be improved later.
   */
  public void die(String what) {
    stop();
    throw new RuntimeException(what);
    /*
    if (online) {
      System.err.println("i'm dead.. " + what);

    } else {
      System.err.println(what);
      System.exit(1);
    }
    */
  }


  /**
   * Same as above but with an exception. Also needs work.
   */
  public void die(String what, Exception e) {
    if (e != null) e.printStackTrace();
    die(what);
  }


  /**
   * Call to safely exit the sketch when finished. For instance,
   * to render a single frame, save it, and quit.
   */
  public void exit() {
    if (thread == null) {
      // exit immediately, stop() has already been called,
      // meaning that the main thread has long since exited
      if ((leechErr == null) && !online) {
        // don't want to call System.exit() when an applet,
        // or running inside the PDE (would kill the PDE)
        System.exit(0);
      }
    } else {
      finished = true;  // stop() will be called as the thread exits
      //stop();
      exit = true;
    }
  }


  //////////////////////////////////////////////////////////////

  // SCREEN GRABASS


  /**
   * Intercepts any relative paths to make them absolute (relative
   * to the sketch folder) before passing to save() in PImage.
   * (Changed in 0100)
   */
  public void save(String filename) {
    g.save(savePath(filename));
  }


  /**
   * Grab an image of what's currently in the drawing area and save it
   * as a .tif or .tga file.
   * <P>
   * Best used just before endDraw() at the end of your draw().
   * This can only create .tif or .tga images, so if neither extension
   * is specified it defaults to writing a tiff and adds a .tif suffix.
   */
  public void saveFrame() {
    if (online) {
      System.err.println("Can't use saveFrame() when running in a browser.");
      return;
    }

    //File file = new File(folder, "screen-" + nf(frame, 4) + ".tif");
    //save(savePath("screen-" + nf(frameCount, 4) + ".tif"));
    //save("screen-" + nf(frame, 4) + ".tif");
    g.save(savePath("screen-" + nf(frameCount, 4) + ".tif"));
  }


  /**
   * Save the current frame as a .tif or .tga image.
   * <P>
   * The String passed in can contain a series of # signs
   * that will be replaced with the screengrab number.
   * <PRE>
   * i.e. saveFrame("blah-####.tif");
   *      // saves a numbered tiff image, replacing the
   *      // #### signs with zeros and the frame number </PRE>
   */
  public void saveFrame(String what) {
    if (online) {
      System.err.println("Can't use saveFrame() when running in a browser.");
      return;
    }

    g.save(savePath(insertFrame(what)));
    /*
    int first = what.indexOf('#');
    int last = what.lastIndexOf('#');

    if (first == -1) {
      g.save(savePath(what));

    } else {
      String prefix = what.substring(0, first);
      int count = last - first + 1;
      String suffix = what.substring(last + 1);
      g.save(savePath(prefix + nf(frameCount, count) + suffix));
    }
    */
  }



  //////////////////////////////////////////////////////////////

  // CURSOR

  //


  int cursorType = ARROW; // cursor type
  boolean cursorVisible = true; // cursor visibility flag
  PImage invisibleCursor;


  /**
   * Set the cursor type
   */
  public void cursor(int _cursor_type) {
    setCursor(Cursor.getPredefinedCursor(_cursor_type));
    cursorVisible = true;
    cursorType = _cursor_type;
  }


  /**
   * Replace the cursor with the specified PImage. The x- and y-
   * coordinate of the center will be the center of the image.
   */
  public void cursor(PImage image) {
    cursor(image, image.width/2, image.height/2);
  }


  /**
   * Set a custom cursor to an image with a specific hotspot.
   * Only works with JDK 1.2 and later.
   * Currently seems to be broken on Java 1.4 for Mac OS X
   * <P>
   * Based on code contributed by Amit Pitaru, plus additional
   * code to handle Java versions via reflection by Jonathan Feinberg.
   */
  public void cursor(PImage image, int hotspotX, int hotspotY) {
    if (javaVersion < 1.2f) {
      System.err.println("Java 1.2 or higher is required to use cursor()");
      System.err.println("(You're using version " + javaVersionName + ")");
      return;
    }

    // don't set this as cursor type, instead use cursor_type
    // to save the last cursor used in case cursor() is called
    //cursor_type = Cursor.CUSTOM_CURSOR;
    Image jimage =
      createImage(new MemoryImageSource(image.width, image.height,
                                        image.pixels, 0, image.width));

    Point hotspot = new Point(hotspotX, hotspotY);
    try {
      Method mCustomCursor =
        Toolkit.class.getMethod("createCustomCursor",
                                new Class[] { Image.class,
                                              Point.class,
                                              String.class, });
      Cursor cursor =
        (Cursor)mCustomCursor.invoke(Toolkit.getDefaultToolkit(),
                                     new Object[] { jimage,
                                                    hotspot,
                                                    "no cursor" });
      setCursor(cursor);
      cursorVisible = true;

    } catch (NoSuchMethodError e) {
      System.err.println("cursor() is not available " +
                         "when using Java " + javaVersionName);
    } catch (IndexOutOfBoundsException e) {
      System.err.println("cursor() error: the hotspot " + hotspot +
                         " is out of bounds for the given image.");
    } catch (Exception e) {
      System.err.println(e);
    }
  }


  /**
   * Show the cursor after noCursor() was called.
   * Notice that the program remembers the last set cursor type
   */
  public void cursor() {
    // maybe should always set here? seems dangerous, since
    // it's likely that java will set the cursor to something
    // else on its own, and the applet will be stuck b/c bagel
    // thinks that the cursor is set to one particular thing
    if (!cursorVisible) {
      cursorVisible = true;
      setCursor(Cursor.getPredefinedCursor(cursorType));
    }
  }


  /**
   * Hide the cursor by creating a transparent image
   * and using it as a custom cursor.
   */
  public void noCursor() {
    if (!cursorVisible) return;  // don't hide if already hidden.

    if (invisibleCursor == null) {
      invisibleCursor = new PImage(16, 16, ARGB);
    }
    // was formerly 16x16, but the 0x0 was added by jdf as a fix
    // for macosx, which didn't wasn't honoring the invisible cursor
    cursor(invisibleCursor, 0, 0);
    cursorVisible = false;
  }


  //////////////////////////////////////////////////////////////


  static public void print(byte what) {
    System.out.print(what);
    System.out.flush();
  }

  static public void print(boolean what) {
    System.out.print(what);
    System.out.flush();
  }

  static public void print(char what) {
    System.out.print(what);
    System.out.flush();
  }

  static public void print(int what) {
    System.out.print(what);
    System.out.flush();
  }

  static public void print(float what) {
    System.out.print(what);
    System.out.flush();
  }

  /*
  static public void print(double what) {
    System.out.print(what);
    System.out.flush();
  }
  */

  static public void print(String what) {
    System.out.print(what);
    System.out.flush();
  }

  static public void print(Object what) {
    if (what == null) {
      // special case since this does fuggly things on > 1.1
      System.out.print("null");
    } else {
      System.out.println(what.toString());
    }

    /*
      String name = what.getClass().getName();
      if (name.charAt(0) == '[') {
        switch (name.charAt(1)) {
        case '[':
          // don't even mess with multi-dimensional arrays (case '[')
          // or anything else that's not int, float, boolean, char
          System.out.print(what);
          System.out.print(' ');
          break;

        case 'L':
          // print a 1D array of objects as individual elements
          Object poo[] = (Object[]) what;
          for (int i = 0; i < poo.length; i++) {
            System.out.print(poo[i]);
            System.out.print(' ');
          }
          break;

        case 'Z':  // boolean
          boolean zz[] = (boolean[]) what;
          for (int i = 0; i < zz.length; i++) {
            System.out.print(zz[i]);
            System.out.print(' ');
          }
          break;

        case 'B':  // byte
          byte bb[] = (byte[]) what;
          for (int i = 0; i < bb.length; i++) {
            System.out.print(bb[i]);
            System.out.print(' ');
          }
          break;

        case 'C':  // char
          char cc[] = (char[]) what;
          for (int i = 0; i < cc.length; i++) {
            System.out.print(cc[i]);
            System.out.print(' ');
          }
          break;

        case 'I':  // int
          int ii[] = (int[]) what;
          for (int i = 0; i < ii.length; i++) {
            System.out.print(ii[i]);
            System.out.print(' ');
          }
          break;

        case 'F':  // float
          float ff[] = (float[]) what;
          for (int i = 0; i < ff.length; i++) {
            System.out.print(ff[i]);
            System.out.print(' ');
          }
          break;

        case 'D':  // double
          double dd[] = (double[]) what;
          for (int i = 0; i < dd.length; i++) {
            System.out.print(dd[i]);
            System.out.print(' ');
          }
          break;

        default:
          System.out.print(what);
        }
      } else {
        System.out.print(what); //.toString());
      }
    */
  }

  //

  static public void println() {
    System.out.println();
  }

  //

  static public void println(byte what) {
    print(what); System.out.println();
  }

  static public void println(boolean what) {
    print(what); System.out.println();
  }

  static public void println(char what) {
    print(what); System.out.println();
  }

  static public void println(int what) {
    print(what); System.out.println();
  }

  static public void println(float what) {
    print(what); System.out.println();
  }

  /*
  static public void println(double what) {
    print(what); System.out.println();
  }
  */

  static public void println(String what) {
    print(what); System.out.println();
  }

  static public void println(Object what) {
    if (what == null) {
      // special case since this does fuggly things on > 1.1
      System.out.println("null");

    } else {
      String name = what.getClass().getName();
      if (name.charAt(0) == '[') {
        switch (name.charAt(1)) {
        case '[':
          // don't even mess with multi-dimensional arrays (case '[')
          // or anything else that's not int, float, boolean, char
          System.out.println(what);
          break;

        case 'L':
          // print a 1D array of objects as individual elements
          Object poo[] = (Object[]) what;
          for (int i = 0; i < poo.length; i++) {
            if (poo[i] instanceof String) {
              System.out.println("[" + i + "] \"" + poo[i] + "\"");
            } else {
              System.out.println("[" + i + "] " + poo[i]);
            }
          }
          break;

        case 'Z':  // boolean
          boolean zz[] = (boolean[]) what;
          for (int i = 0; i < zz.length; i++) {
            System.out.println("[" + i + "] " + zz[i]);
          }
          break;

        case 'B':  // byte
          byte bb[] = (byte[]) what;
          for (int i = 0; i < bb.length; i++) {
            System.out.println("[" + i + "] " + bb[i]);
          }
          break;

        case 'C':  // char
          char cc[] = (char[]) what;
          for (int i = 0; i < cc.length; i++) {
            System.out.println("[" + i + "] '" + cc[i] + "'");
          }
          break;

        case 'I':  // int
          int ii[] = (int[]) what;
          for (int i = 0; i < ii.length; i++) {
            System.out.println("[" + i + "] " + ii[i]);
          }
          break;

        case 'F':  // float
          float ff[] = (float[]) what;
          for (int i = 0; i < ff.length; i++) {
            System.out.println("[" + i + "] " + ff[i]);
          }
          break;

          /*
        case 'D':  // double
          double dd[] = (double[]) what;
          for (int i = 0; i < dd.length; i++) {
            System.out.println("[" + i + "] " + dd[i]);
          }
          break;
          */

        default:
          System.out.println(what);
        }
      } else {  // not an array
        System.out.println(what);
      }
    }
  }

  //

  /*
  // not very useful, because it only works for public (and protected?)
  // fields of a class, not local variables to methods
  public void printvar(String name) {
    try {
      Field field = getClass().getDeclaredField(name);
      println(name + " = " + field.get(this));
    } catch (Exception e) {
      e.printStackTrace();
    }
  }
  */


  //////////////////////////////////////////////////////////////

  // MATH

  // lots of convenience methods for math with floats.
  // doubles are overkill for processing applets, and casting
  // things all the time is annoying, thus the functions below.


  static public final float abs(float n) {
    return (n < 0) ? -n : n;
  }

  static public final int abs(int n) {
    return (n < 0) ? -n : n;
  }

  static public final float sq(float a) {
    return a*a;
  }

  static public final float sqrt(float a) {
    return (float)Math.sqrt(a);
  }

  static public final float log(float a) {
    return (float)Math.log(a);
  }

  static public final float exp(float a) {
    return (float)Math.exp(a);
  }

  static public final float pow(float a, float b) {
    return (float)Math.pow(a, b);
  }


  static public final int max(int a, int b) {
    return (a > b) ? a : b;
  }

  static public final float max(float a, float b) {
    return (a > b) ? a : b;
  }


  static public final int max(int a, int b, int c) {
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
  }

  static public final float max(float a, float b, float c) {
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
  }


  /**
   * Find the maximum value in an array.
   * @param list the source array
   * @return The maximum value, or 0 if the array is length zero.
   */
  static public final int max(int[] list) {
    if (list.length == 0) {
      return 0;
    }
    int max = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] > max) max = list[i];
    }
    return max;
  }

  /**
   * Find the maximum value in an array.
   * @param list the source array
   * @return The maximum value, or Float.NaN if the array is length zero.
   */
  static public final float max(float[] list) {
    if (list.length == 0) {
      return Float.NaN;
    }
    float max = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] > max) max = list[i];
    }
    return max;
  }


  static public final int min(int a, int b) {
    return (a < b) ? a : b;
  }

  static public final float min(float a, float b) {
    return (a < b) ? a : b;
  }


  static public final int min(int a, int b, int c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
  }

  static public final float min(float a, float b, float c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
  }


  /**
   * Find the minimum value in an array.
   * @param list the source array
   * @return The minimum value, or 0 if the array is length zero.
   */
  static public final int min(int[] list) {
    if (list.length == 0) {
      return 0;
    }
    int min = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] < min) min = list[i];
    }
    return min;
  }
  /**
   * Find the minimum value in an array.
   * @param list the source array
   * @return The minimum value, or Float.NaN if the array is length zero.
   */
  static public final float min(float[] list) {
    if (list.length == 0) {
      return Float.NaN;
    }
    float min = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] < min) min = list[i];
    }
    return min;
  }


  static public final int constrain(int amt, int low, int high) {
    return (amt < low) ? low : ((amt > high) ? high : amt);
  }

  static public final float constrain(float amt, float low, float high) {
    return (amt < low) ? low : ((amt > high) ? high : amt);
  }


  static public final float sin(float angle) {
    return (float)Math.sin(angle);
  }

  static public final float cos(float angle) {
    return (float)Math.cos(angle);
  }

  static public final float tan(float angle) {
    return (float)Math.tan(angle);
  }


  static public final float asin(float value) {
    return (float)Math.asin(value);
  }

  static public final float acos(float value) {
    return (float)Math.acos(value);
  }

  static public final float atan(float value) {
    return (float)Math.atan(value);
  }

  static public final float atan2(float a, float b) {
    return (float)Math.atan2(a, b);
  }


  static public final float degrees(float radians) {
    return radians * RAD_TO_DEG;
  }

  static public final float radians(float degrees) {
    return degrees * DEG_TO_RAD;
  }


  static public final int ceil(float what) {
    return (int) Math.ceil(what);
  }

  static public final int floor(float what) {
    return (int) Math.floor(what);
  }

  static public final int round(float what) {
    return (int) Math.round(what);
  }


  static public final float mag(float a, float b) {
    return (float)Math.sqrt(a*a + b*b);
  }

  static public final float mag(float a, float b, float c) {
    return (float)Math.sqrt(a*a + b*b + c*c);
  }


  static public final float dist(float x1, float y1, float x2, float y2) {
    return sqrt(sq(x2-x1) + sq(y2-y1));
  }

  static public final float dist(float x1, float y1, float z1,
                                 float x2, float y2, float z2) {
    return sqrt(sq(x2-x1) + sq(y2-y1) + sq(z2-z1));
  }


  static public final float lerp(float start, float stop, float amt) {
    return start + (stop-start) * amt;
  }

  /**
   * Normalize a value to exist between 0 and 1 (inclusive).
   * Mathematically the opposite of lerp(), figures out what proportion
   * a particular value is relative to start and stop coordinates.
   */
  static public final float norm(float value, float start, float stop) {
    return (value - start) / (stop - start);
  }

  /**
   * Convenience function to map a variable from one coordinate space
   * to another. Equivalent to unlerp() followed by lerp().
   */
  static public final float map(float value,
                                float istart, float istop,
                                float ostart, float ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
  }



  //////////////////////////////////////////////////////////////

  // RANDOM NUMBERS


  Random internalRandom;

  /**
   * Return a random number in the range [0, howbig).
   * <P>
   * The number returned will range from zero up to
   * (but not including) 'howbig'.
   */
  public final float random(float howbig) {
    // for some reason (rounding error?) Math.random() * 3
    // can sometimes return '3' (once in ~30 million tries)
    // so a check was added to avoid the inclusion of 'howbig'

    // avoid an infinite loop
    if (howbig == 0) return 0;

    // internal random number object
    if (internalRandom == null) internalRandom = new Random();

    float value = 0;
    do {
      //value = (float)Math.random() * howbig;
      value = internalRandom.nextFloat() * howbig;
    } while (value == howbig);
    return value;
  }


  /**
   * Return a random number in the range [howsmall, howbig).
   * <P>
   * The number returned will range from 'howsmall' up to
   * (but not including 'howbig'.
   * <P>
   * If howsmall is >= howbig, howsmall will be returned,
   * meaning that random(5, 5) will return 5 (useful)
   * and random(7, 4) will return 7 (not useful.. better idea?)
   */
  public final float random(float howsmall, float howbig) {
    if (howsmall >= howbig) return howsmall;
    float diff = howbig - howsmall;
    return random(diff) + howsmall;
  }


  public final void randomSeed(long what) {
    // internal random number object
    if (internalRandom == null) internalRandom = new Random();
    internalRandom.setSeed(what);
  }



  //////////////////////////////////////////////////////////////

  // PERLIN NOISE

  // [toxi 040903]
  // octaves and amplitude amount per octave are now user controlled
  // via the noiseDetail() function.

  // [toxi 030902]
  // cleaned up code and now using bagel's cosine table to speed up

  // [toxi 030901]
  // implementation by the german demo group farbrausch
  // as used in their demo "art": http://www.farb-rausch.de/fr010src.zip

  static final int PERLIN_YWRAPB = 4;
  static final int PERLIN_YWRAP = 1<<PERLIN_YWRAPB;
  static final int PERLIN_ZWRAPB = 8;
  static final int PERLIN_ZWRAP = 1<<PERLIN_ZWRAPB;
  static final int PERLIN_SIZE = 4095;

  int perlin_octaves = 4; // default to medium smooth
  float perlin_amp_falloff = 0.5f; // 50% reduction/octave

  // [toxi 031112]
  // new vars needed due to recent change of cos table in PGraphics
  int perlin_TWOPI, perlin_PI;
  float[] perlin_cosTable;
  float[] perlin;

  Random perlinRandom;


  /**
   * Computes the Perlin noise function value at point x.
   */
  public float noise(float x) {
    // is this legit? it's a dumb way to do it (but repair it later)
    return noise(x, 0f, 0f);
  }

  /**
   * Computes the Perlin noise function value at the point x, y.
   */
  public float noise(float x, float y) {
    return noise(x, y, 0f);
  }

  /**
   * Computes the Perlin noise function value at x, y, z.
   */
  public float noise(float x, float y, float z) {
    if (perlin == null) {
      if (perlinRandom == null) {
        perlinRandom = new Random();
      }
      perlin = new float[PERLIN_SIZE + 1];
      for (int i = 0; i < PERLIN_SIZE + 1; i++) {
        perlin[i] = perlinRandom.nextFloat(); //(float)Math.random();
      }
      // [toxi 031112]
      // noise broke due to recent change of cos table in PGraphics
      // this will take care of it
      perlin_cosTable = PGraphics.cosLUT;
      perlin_TWOPI = perlin_PI = PGraphics.SINCOS_LENGTH;
      perlin_PI >>= 1;
    }

    if (x<0) x=-x;
    if (y<0) y=-y;
    if (z<0) z=-z;

    int xi=(int)x, yi=(int)y, zi=(int)z;
    float xf = (float)(x-xi);
    float yf = (float)(y-yi);
    float zf = (float)(z-zi);
    float rxf, ryf;

    float r=0;
    float ampl=0.5f;

    float n1,n2,n3;

    for (int i=0; i<perlin_octaves; i++) {
      int of=xi+(yi<<PERLIN_YWRAPB)+(zi<<PERLIN_ZWRAPB);

      rxf=noise_fsc(xf);
      ryf=noise_fsc(yf);

      n1  = perlin[of&PERLIN_SIZE];
      n1 += rxf*(perlin[(of+1)&PERLIN_SIZE]-n1);
      n2  = perlin[(of+PERLIN_YWRAP)&PERLIN_SIZE];
      n2 += rxf*(perlin[(of+PERLIN_YWRAP+1)&PERLIN_SIZE]-n2);
      n1 += ryf*(n2-n1);

      of += PERLIN_ZWRAP;
      n2  = perlin[of&PERLIN_SIZE];
      n2 += rxf*(perlin[(of+1)&PERLIN_SIZE]-n2);
      n3  = perlin[(of+PERLIN_YWRAP)&PERLIN_SIZE];
      n3 += rxf*(perlin[(of+PERLIN_YWRAP+1)&PERLIN_SIZE]-n3);
      n2 += ryf*(n3-n2);

      n1 += noise_fsc(zf)*(n2-n1);

      r += n1*ampl;
      ampl *= perlin_amp_falloff;
      xi<<=1; xf*=2;
      yi<<=1; yf*=2;
      zi<<=1; zf*=2;

      if (xf>=1.0f) { xi++; xf--; }
      if (yf>=1.0f) { yi++; yf--; }
      if (zf>=1.0f) { zi++; zf--; }
    }
    return r;
  }

  // [toxi 031112]
  // now adjusts to the size of the cosLUT used via
  // the new variables, defined above
  private float noise_fsc(float i) {
    // using bagel's cosine table instead
    return 0.5f*(1.0f-perlin_cosTable[(int)(i*perlin_PI)%perlin_TWOPI]);
  }

  // [toxi 040903]
  // make perlin noise quality user controlled to allow
  // for different levels of detail. lower values will produce
  // smoother results as higher octaves are surpressed

  public void noiseDetail(int lod) {
    if (lod>0) perlin_octaves=lod;
  }

  public void noiseDetail(int lod, float falloff) {
    if (lod>0) perlin_octaves=lod;
    if (falloff>0) perlin_amp_falloff=falloff;
  }

  public void noiseSeed(long what) {
    if (perlinRandom == null) perlinRandom = new Random();
    perlinRandom.setSeed(what);
    // force table reset after changing the random number seed [0122]
    perlin = null;
  }



  //////////////////////////////////////////////////////////////

  // SOUND I/O

  /*
  public PSound loadSound(String filename) {
    if (PApplet.javaVersion >= 1.3f) {
      return new PSound2(this, openStream(filename));
    }
    return new PSound(this, openStream(filename));
  }
  */


  //////////////////////////////////////////////////////////////

  // IMAGE I/O


  /*
  Hashtable imageTable;
  */

  /**
   * Draw an image based on its filename. This is less efficient than
   * using loadImage because there's no way to unload it from memory,
   * but it's useful for beginners.
   */
  /*
  public void image(String filename, float x, float y) {
    image(tableImage(filename), x, y);
  }
  */

  /**
   * Draw an image based on its filename. This is less than efficient
   * than using loadImage because there's no way to unload it from memory,
   * but it's useful for beginners.
   */
  /*
  public void image(String filename,
                    float x, float y, float c, float d) {
    image(tableImage(filename), x, y, c, d);
  }
  */

  /**
   * Draw an image based on its filename. This is less than efficient
   * than using loadImage because there's no way to unload it from memory,
   * but it's useful for beginners.
   */
  /*
  public void image(String filename,
                    float a, float b, float c, float d,
                    int u1, int v1, int u2, int v2) {
    image(tableImage(filename), a, b, c, d, u1, v1, u2, v2);
  }
  */


  /**
   * Load an image and store it in a table based on its name.
   */
  /*
  protected PImage tableImage(String filename) {
    if (imageTable == null) imageTable = new Hashtable();

    PImage image = (PImage) imageTable.get(filename);
    if (image != null) return image;

    image = loadImage(filename);
    return image;
  }
  */


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  protected String[] loadImageFormats;


  /**
   * Load an image from the data folder or a local directory.
   * Supports .gif (including transparency), .tga, and .jpg images.
   * In Java 1.3 or later, .png images are
   * <A HREF="http://java.sun.com/j2se/1.3/docs/guide/2d/new_features.html">
   * also supported</A>.
   * <P>
   * Generally, loadImage() should only be used during setup, because
   * re-loading images inside draw() is likely to cause a significant
   * delay while memory is allocated and the thread blocks while waiting
   * for the image to load because loading is not asynchronous.
   * <P>
   * To load several images asynchronously, see more information in the
   * FAQ about writing your own threaded image loading method.
   * <P>
   * As of 0096, returns null if no image of that name is found,
   * rather than an error.
   * <P>
   * Release 0115 also provides support for reading TIFF and RLE-encoded
   * Targa (.tga) files written by Processing via save() and saveFrame().
   * Other TIFF and Targa files will probably not load, use a different
   * format (gif, jpg and png are safest bets) when creating images with
   * another application to use with Processing.
   * <P>
   * Also in release 0115, more image formats (BMP and others) can
   * be read when using Java 1.4 and later. Because many people still
   * use Java 1.1 and 1.3, these formats are not recommended for
   * work that will be posted on the web. To get a list of possible
   * image formats for use with Java 1.4 and later, use the following:
   * <TT>println(javax.imageio.ImageIO.getReaderFormatNames())</TT>
   * <P>
   * Images are loaded via a byte array that is passed to
   * Toolkit.createImage(). Unfortunately, we cannot use Applet.getImage()
   * because it takes a URL argument, which would be a pain in the a--
   * to make work consistently for online and local sketches.
   * Sometimes this causes problems, resulting in issues like
   * <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=279">Bug 279</A>
   * and
   * <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=305">Bug 305</A>.
   * In release 0115, everything was instead run through javax.imageio,
   * but that turned out to be very slow, see
   * <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=392">Bug 392</A>.
   * As a result, starting with 0116, the following happens:
   * <UL>
   * <LI>TGA and TIFF images are loaded using the internal load methods.
   * <LI>JPG, GIF, and PNG images are loaded via loadBytes().
   * <LI>If the image still isn't loaded, it's passed to javax.imageio.
   * </UL>
   * For releases 0116 and later, if you have problems such as those seen
   * in Bugs 279 and 305, use Applet.getImage() instead. You'll be stuck
   * with the limitations of getImage() (the headache of dealing with
   * online/offline use). Set up your own MediaTracker, and pass the resulting
   * java.awt.Image to the PImage constructor that takes an AWT image.
   * You can also use the loadImageSync() function (added in 0116) that
   * takes an AWT image and loads it synchronously inside PApplet.
   * <PRE>
   * public PImage loadImageAlt(String filename) {
   *   java.awt.Image img = getImage(getCodeBase(), filename);
   *   return loadImageSync(img);
   * }
   * </PRE>
   * This isn't much fun, but this will have to do unless we find the
   * actual culprit, which may still be a threading issue.
   */
  public PImage loadImage(String filename) {
    String lower = filename.toLowerCase();
    int dot = filename.lastIndexOf('.');
    if (dot == -1) {
      // no extension found
      return loadImage(filename, "unknown");
    }
    String extension = lower.substring(dot + 1);

    // check for, and strip any parameters on the url, i.e.
    // filename.jpg?blah=blah&something=that
    int question = extension.indexOf('?');
    if (question != -1) {
      extension = extension.substring(0, question);
    }

    return loadImage(filename, extension);
  }


  /**
   * Identical to loadImage, but allows you to specify the type of
   * image by its extension. Especially useful when downloading from
   * CGI scripts.
   * <br/> <br/>
   * Use 'unknown' as the extension to pass off to the default
   * image loader that handles gif, jpg, and png.
   */
  public PImage loadImage(String filename, String extension) {
    // just in case. them users will try anything!
    extension = extension.toLowerCase();

    if (extension.equals("tga")) {
      try {
        return loadImageTGA(filename);
      } catch (IOException e) {
        e.printStackTrace();
        return null;
      }
    }

    //if (lower.endsWith(".tif") || lower.endsWith(".tiff")) {
    if (extension.equals("tif") || extension.equals("tiff")) {
      byte bytes[] = loadBytes(filename);
      return (bytes == null) ? null : PImage.loadTIFF(bytes);
    }

    // Make sure that PNG images aren't being loaded by Java 1.1
    //if (lower.endsWith(".png") && PApplet.javaVersion < 1.3f) {
    if (extension.equals("png") && PApplet.javaVersion < 1.3f) {
      System.err.println("PNG images can only be loaded when " +
                         "using Java 1.3 and later.");
      return null;
    }

    // For jpeg, gif, and png, load them using createImage(),
    // because the javax.imageio code was found to be much slower, see
    // <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=392">Bug 392</A>.
    try {
      //if (lower.endsWith(".jpg") || lower.endsWith(".jpeg") ||
      //  lower.endsWith(".gif") || lower.endsWith(".png")) {
      if (extension.equals("jpg") || extension.equals("jpeg") ||
          extension.equals("gif") || extension.equals("png") ||
          extension.equals("unknown")) {
        byte bytes[] = loadBytes(filename);
        if (bytes == null) {
          return null;
        } else {
          Image awtImage = Toolkit.getDefaultToolkit().createImage(bytes);
          PImage image = loadImageSync(awtImage);
          // if it's a .gif image, test to see if it has transparency
          //if ((lower.endsWith(".gif")) || (lower.endsWith(".png"))) {
          if (extension.equals("gif") || extension.equals("png")) {
            image.checkAlpha();
          }
          return image;
        }
      }
    } catch (Exception e) {
      // show error, but move on to the stuff below, see if it'll work
      e.printStackTrace();
    }

    if (PApplet.javaVersion >= 1.4f) {
      if (loadImageFormats == null) {
        //loadImageFormats = javax.imageio.ImageIO.getReaderFormatNames();
        try {
          Class ioClass = Class.forName("javax.imageio.ImageIO");
          Method getFormatNamesMethod =
            ioClass.getMethod("getReaderFormatNames", (Class[]) null);
          loadImageFormats = (String[])
            getFormatNamesMethod.invoke((Class[]) null, (Object[]) null);
        } catch (Exception e) {
          e.printStackTrace();
        }
      }
      if (loadImageFormats != null) {
        for (int i = 0; i < loadImageFormats.length; i++) {
          //if (filename.endsWith("." + loadImageFormats[i])) {
          if (extension.equals(loadImageFormats[i])) {
            return loadImageIO(filename);
          }
        }
      }
    }

    // failed, could not load image after all those attempts
    System.err.println("Could not find a method to load " + filename);
    return null;
  }


  /**
   * Load an AWT image synchronously.
   */
  public PImage loadImageSync(Image awtImage) {
    MediaTracker tracker = new MediaTracker(this);
    tracker.addImage(awtImage, 0);
    try {
      tracker.waitForAll();
    } catch (InterruptedException e) {
      //e.printStackTrace();  // non-fatal, right?
    }

    PImage image = new PImage(awtImage);
    image.parent = this;
    return image;
  }


  /**
   * Use Java 1.4 ImageIO methods to load an image. All done via reflection
   * in order to maintain compatability with previous releases.
   */
  protected PImage loadImageIO(String filename) {
    InputStream stream = openStream(filename);
    if (stream == null) {
      System.err.println("The image " + filename + " could not be found.");
      return null;
    }

    try {
      Class ioClass = Class.forName("javax.imageio.ImageIO");
      Method readMethod =
        ioClass.getMethod("read", new Class[] { InputStream.class });
      Object bimage = readMethod.invoke(null, new Object[] { stream });

      // need to get width and height, then create pixels[] at that size
      //int px[] = null;

      Class biClass =
        Class.forName("java.awt.image.BufferedImage");

      Method getHeightMethod =
        biClass.getMethod("getHeight", (Class[]) null);
      Integer hi = (Integer) getHeightMethod.invoke(bimage, (Object[]) null);

      Method getWidthMethod =
        biClass.getMethod("getWidth", (Class[]) null);
      Integer wi = (Integer) getWidthMethod.invoke(bimage, (Object[]) null);

      // was gonna call getType() on the image to see if RGB or ARGB,
      // but it's not actually useful, since gif images will come through
      // as TYPE_BYTE_INDEXED, which means it'll still have to check for
      // the transparency. also, would have to iterate through all the other
      // types and guess whether alpha was in there, so.. just gonna stick
      // with the old method.

      PImage outgoing = new PImage(wi.intValue(), hi.intValue());
      outgoing.parent = this;

      Method getRgbMethod =
        biClass.getMethod("getRGB", new Class[] {
            Integer.TYPE, Integer.TYPE, Integer.TYPE, Integer.TYPE,
            outgoing.pixels.getClass(), Integer.TYPE, Integer.TYPE
          });
      getRgbMethod.invoke(bimage, new Object[] {
          new Integer(0), new Integer(0),
          new Integer(outgoing.width), new Integer(outgoing.height),
          outgoing.pixels, new Integer(0), new Integer(outgoing.width)
        });

      // check the alpha for this image
      outgoing.checkAlpha();

      // return the image
      return outgoing;

    } catch (Exception e) {
      e.printStackTrace();
      return null;
    }
  }


  /**
   * Targa image loader for RLE-compressed TGA files.
   * <P>
   * Rewritten for 0115 to read/write RLE-encoded targa images.
   * For 0125, non-RLE encoded images are now supported, along with
   * images whose y-order is reversed (which is standard for TGA files).
   */
  protected PImage loadImageTGA(String filename) throws IOException {
    InputStream is = openStream(filename);
    if (is == null) return null;

    byte header[] = new byte[18];
    int offset = 0;
    do {
      int count = is.read(header, offset, header.length - offset);
      if (count == -1) return null;
      offset += count;
    } while (offset < 18);

    /*
      header[2] image type code
      2  (0x02) - Uncompressed, RGB images.
      3  (0x03) - Uncompressed, black and white images.
      10 (0x0A) - Runlength encoded RGB images.
      11 (0x0B) - Compressed, black and white images. (grayscale?)

      header[16] is the bit depth (8, 24, 32)

      header[17] image descriptor (packed bits)
      0x20 is 32 = origin upper-left
      0x28 is 32 + 8 = origin upper-left + 32 bits

        7  6  5  4  3  2  1  0
      128 64 32 16  8  4  2  1
    */

    int format = 0;

    if (((header[2] == 3) || (header[2] == 11)) &&  // B&W, plus RLE or not
        (header[16] == 8) &&  // 8 bits
        ((header[17] == 0x8) || (header[17] == 0x28))) {  // origin, 32 bit
      format = ALPHA;

    } else if (((header[2] == 2) || (header[2] == 10)) &&  // RGB, RLE or not
               (header[16] == 24) &&  // 24 bits
               ((header[17] == 0x20) || (header[17] == 0))) {  // origin
      format = RGB;

    } else if (((header[2] == 2) || (header[2] == 10)) &&
               (header[16] == 32) &&
               ((header[17] == 0x8) || (header[17] == 0x28))) {  // origin, 32
      format = ARGB;
    }

    if (format == 0) {
      System.err.println("Unknown .tga file format for " + filename);
                         //" (" + header[2] + " " +
                         //(header[16] & 0xff) + " " +
                         //hex(header[17], 2) + ")");
      return null;
    }

    int w = ((header[13] & 0xff) << 8) + (header[12] & 0xff);
    int h = ((header[15] & 0xff) << 8) + (header[14] & 0xff);
    PImage outgoing = createImage(w, h, format);

    boolean reversed = (header[17] & 0x20) != 0;

    if ((header[2] == 2) || (header[2] == 3)) {  // not RLE encoded
      if (reversed) {
        int index = (h-1) * w;
        switch (format) {
        case ALPHA:
          for (int y = h-1; y >= 0; y--) {
            for (int x = 0; x < w; x++) {
              outgoing.pixels[index + x] = is.read();
            }
            index -= w;
          }
          break;
        case RGB:
          for (int y = h-1; y >= 0; y--) {
            for (int x = 0; x < w; x++) {
              outgoing.pixels[index + x] =
                is.read() | (is.read() << 8) | (is.read() << 16) |
                0xff000000;
            }
            index -= w;
          }
          break;
        case ARGB:
          for (int y = h-1; y >= 0; y--) {
            for (int x = 0; x < w; x++) {
              outgoing.pixels[index + x] =
                is.read() | (is.read() << 8) | (is.read() << 16) |
                (is.read() << 24);
            }
            index -= w;
          }
        }
      } else {  // not reversed
        int count = w * h;
        switch (format) {
        case ALPHA:
          for (int i = 0; i < count; i++) {
            outgoing.pixels[i] = is.read();
          }
          break;
        case RGB:
          for (int i = 0; i < count; i++) {
            outgoing.pixels[i] =
              is.read() | (is.read() << 8) | (is.read() << 16) |
              0xff000000;
          }
          break;
        case ARGB:
          for (int i = 0; i < count; i++) {
            outgoing.pixels[i] =
              is.read() | (is.read() << 8) | (is.read() << 16) |
              (is.read() << 24);
          }
          break;
        }
      }

    } else {  // header[2] is 10 or 11
      int index = 0;
      int px[] = outgoing.pixels;

      while (index < px.length) {
        int num = is.read();
        boolean isRLE = (num & 0x80) != 0;
        if (isRLE) {
          num -= 127;  // (num & 0x7F) + 1
          int pixel = 0;
          switch (format) {
          case ALPHA:
            pixel = is.read();
            break;
          case RGB:
            pixel = 0xFF000000 |
              is.read() | (is.read() << 8) | (is.read() << 16);
            //(is.read() << 16) | (is.read() << 8) | is.read();
            break;
          case ARGB:
            pixel = is.read() |
              (is.read() << 8) | (is.read() << 16) | (is.read() << 24);
            break;
          }
          for (int i = 0; i < num; i++) {
            px[index++] = pixel;
            if (index == px.length) break;
          }
        } else {  // write up to 127 bytes as uncompressed
          num += 1;
          switch (format) {
          case ALPHA:
            for (int i = 0; i < num; i++) {
              px[index++] = is.read();
            }
            break;
          case RGB:
            for (int i = 0; i < num; i++) {
              px[index++] = 0xFF000000 |
                is.read() | (is.read() << 8) | (is.read() << 16);
              //(is.read() << 16) | (is.read() << 8) | is.read();
            }
            break;
          case ARGB:
            for (int i = 0; i < num; i++) {
              px[index++] = is.read() | //(is.read() << 24) |
                (is.read() << 8) | (is.read() << 16) | (is.read() << 24);
              //(is.read() << 16) | (is.read() << 8) | is.read();
            }
            break;
          }
        }
      }

      if (reversed) {
        int[] temp = new int[w];
        for (int y = 0; y < h/2; y++) {
          int z = (h-1) - y;
          System.arraycopy(px, y*w, temp, 0, w);
          System.arraycopy(px, z*w, px, y*w, w);
          System.arraycopy(temp, 0, px, z*w, w);
        }
      }
    }

    return outgoing;
  }



  //////////////////////////////////////////////////////////////

  // FONT I/O


  /*
  Hashtable fontTable;
  */

  /**
   * Set the font based on its filename. This is less than efficient
   * than using loadFont because there's no way to unload it from memory,
   * but it's useful for beginners.
   */
  /*
  public void textFont(String filename) {
    if (filename.toLowerCase().indexOf(".vlw") == -1) {
      System.err.println("textFont() needs the filename of a .vlw font");
    } else {
      textFont(tableFont(filename));
    }
  }
  */


  /**
   * Set the font based on its filename. This is less than efficient
   * than using loadFont because there's no way to unload it from memory,
   * but it's useful for beginners.
   */
  /*
  public void textFont(String filename, float size) {
    if (filename.toLowerCase().indexOf(".vlw") == -1) {
      System.err.println("textFont() needs the filename of a .vlw font");
    } else {
      textFont(tableFont(filename), size);
    }
  }
  */


  /*
  protected PFont tableFont(String filename) {
    if (fontTable == null) fontTable = new Hashtable();

    PFont font = (PFont) fontTable.get(filename);
    if (font != null) return font;

    font = loadFont(filename);
    return font;
  }
  */


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  public PFont loadFont(String filename) {
    //if (g == null) {  // just for good measure
    //die("loadFont() only be used inside setup() or draw()");
    //}

    try {
      String lower = filename.toLowerCase();
      InputStream input = openStream(filename);

      // For compatability with earlier releases of Processing
      if (lower.endsWith(".vlw.gz")) {
        input = new GZIPInputStream(input);

      } else if (!lower.endsWith(".vlw")) {
        // this gets thrown down below
        throw new IOException("I don't know how to load a font named " +
                              filename);
      }
      return new PFont(input);

    } catch (Exception e) {
      die("Could not load font " + filename + ". " +
          "Make sure that the font has been copied " +
          "to the data folder of your sketch.", e);
    }
    return null;
  }


  public PFont createFont(String name, float size) {
    return createFont(name, size, true, PFont.DEFAULT_CHARSET);
  }


  public PFont createFont(String name, float size, boolean smooth) {
    return createFont(name, size, smooth, PFont.DEFAULT_CHARSET);
  }


  /**
   * Create a .vlw font on the fly from either a font name that's
   * installed on the system, or from a .ttf or .otf that's inside
   * the data folder of this sketch.
   * <P/>
   * Only works with Java 1.3 or later. Many .otf fonts don't seem
   * to be supported by Java, perhaps because they're CFF based?
   * <P/>
   * Font names are inconsistent across platforms and Java versions.
   * On Mac OS X, Java 1.3 uses the font menu name of the font,
   * whereas Java 1.4 uses the PostScript name of the font. Java 1.4
   * on OS X will also accept the font menu name as well. On Windows,
   * it appears that only the menu names are used, no matter what
   * Java version is in use. Naming system unknown/untested for 1.5.
   * <P/>
   * Use 'null' for the charset if you want to use any of the 65,536
   * unicode characters that exist in the font. Note that this can
   * produce an enormous file or may cause an OutOfMemoryError.
   */
  public PFont createFont(String name, float size,
                          boolean smooth, char charset[]) {
    if (PApplet.javaVersion < 1.3f) {
      throw new RuntimeException("Can only create fonts with " +
                                 "Java 1.3 or higher");
    }

    String lowerName = name.toLowerCase();
    Font font = null;

    try {
      Method deriveFontMethod =
        Font.class.getMethod("deriveFont",
                             new Class[] { Float.TYPE });
      Float floatSize = new Float(size);

      if (lowerName.endsWith(".otf") || lowerName.endsWith(".ttf")) {
        InputStream stream = openStream(name);
        if (stream == null) {
          System.err.println("The font \"" + name + "\" " +
                             "is missing or inaccessible, make sure " +
                             "the URL is valid or that the file has been " +
                             "added to your sketch and is readable.");
          return null;
        }

        //font = Font.createFont(Font.TRUETYPE_FONT, openStream(name));
        Method createFontMethod =
          Font.class.getMethod("createFont",
                               new Class[] { Integer.TYPE,
                                             InputStream.class });
        Field ttf = Font.class.getField("TRUETYPE_FONT");
        Integer ttfInteger = new Integer(ttf.getInt(ttf));
        Font baseFont = (Font)
          createFontMethod.invoke(name,
                                  new Object[] { ttfInteger,
                                                 openStream(name) });
        font = (Font) deriveFontMethod.invoke(baseFont,
                                              new Object[] { floatSize });
      } else {
        Font baseFont = new Font(name, Font.PLAIN, 1);
        font = (Font)
          deriveFontMethod.invoke(baseFont, new Object[] { floatSize });
      }

    } catch (Exception e) {
      e.printStackTrace();
      throw new RuntimeException("Problem using createFont() " +
                                 "with the file " + name);
    }
    return new PFont(font, smooth, charset);
  }



  //////////////////////////////////////////////////////////////

  // FILE INPUT


  public File inputFile() {
    return inputFile("Select a file...");
  }


  public File inputFile(String prompt) {
    Frame parentFrame = null;
    Component comp = getParent();
    while (comp != null) {
      if (comp instanceof Frame) {
        parentFrame = (Frame) comp;
        break;
      }
      comp = comp.getParent();
    }
    return inputFile(prompt, parentFrame);
  }


  static public File inputFile(Frame parent) {
    return inputFile("Select a file...", parent);
  }


  /**
   * static version of inputFile usable by external classes.
   * <P>
   * The parentFrame is the Frame that will guide the placement of
   * the prompt window. If no Frame is available, just pass in null.
   */
  // can't be static because it wants a host component
  static public File inputFile(String prompt, Frame parentFrame) {
    if (parentFrame == null) parentFrame = new Frame();
    FileDialog fd = new FileDialog(parentFrame, prompt, FileDialog.LOAD);
    fd.setVisible(true);

    String directory = fd.getDirectory();
    String filename = fd.getFile();
    if (filename == null) return null;
    return new File(directory, filename);
  }


  public File outputFile() {
    return outputFile("Save as...");
  }


  public File outputFile(String prompt) {
    Frame parentFrame = null;
    Component comp = getParent();
    while (comp != null) {
      //System.out.println(comp + " " + comp.getClass());
      if (comp instanceof Frame) {
        parentFrame = (Frame) comp;
        break;
      }
      comp = comp.getParent();
    }
    return outputFile(prompt, parentFrame);
  }



  static public File outputFile(Frame parentFrame) {
    return outputFile("Save as...", parentFrame);
  }


  /**
   * static version of outputFile usable by external classes.
   * <P>
   * The parentFrame is the Frame that will guide the placement of
   * the prompt window. If no Frame is available, just pass in null.
   */
  static public File outputFile(String prompt, Frame parentFrame) {
    if (parentFrame == null) parentFrame = new Frame();
    FileDialog fd = new FileDialog(parentFrame, prompt, FileDialog.SAVE);
    fd.setVisible(true);

    String directory = fd.getDirectory();
    String filename = fd.getFile();
    if (filename == null) return null;
    return new File(directory, filename);
  }


  /**
   * I want to read lines from a file. I have RSI from typing these
   * eight lines of code so many times.
   */
  public BufferedReader createReader(String filename) {
    try {
      InputStream is = openStream(filename);
      if (is == null) {
        System.err.println(filename + " does not exist or could not be read");
        return null;
      }
      return createReader(is);

    } catch (Exception e) {
      if (filename == null) {
        System.err.println("Filename passed to reader() was null");
      } else {
        System.err.println("Couldn't create a reader for " + filename);
      }
    }
    return null;
  }


  /**
   * I want to read lines from a file. And I'm still annoyed.
   */
  static public BufferedReader createReader(File file) {
    try {
      return createReader(new FileInputStream(file));

    } catch (Exception e) {
      if (file == null) {
        throw new RuntimeException("File passed to reader() was null");
      } else {
        e.printStackTrace();
        throw new RuntimeException("Couldn't create a reader for " +
                                   file.getAbsolutePath());
      }
    }
    //return null;
  }


  /**
   * I want to read lines from a stream. If I have to type the
   * following lines any more I'm gonna send Sun my medical bills.
   */
  static public BufferedReader createReader(InputStream input) {
    InputStreamReader isr = new InputStreamReader(input);
    return new BufferedReader(isr);
  }


  /**
   * decode a gzip input stream
   */
  static public InputStream gzipInput(InputStream input) {
    try {
      return new GZIPInputStream(input);
    } catch (IOException e) {
      e.printStackTrace();
      throw new RuntimeException("Problem with gzip input");
    }
    //return null;
  }


  /**
   * decode a gzip output stream
   */
  static public OutputStream gzipOutput(OutputStream output) {
    try {
      return new GZIPOutputStream(output);
    } catch (IOException e) {
      e.printStackTrace();
      throw new RuntimeException("Problem with gzip output");
    }
    //return null;
  }


  /**
   * I want to print lines to a file. Why can't I?
   */
  public PrintWriter createWriter(String filename) {
    try {
      return createWriter(new FileOutputStream(savePath(filename)));

    } catch (Exception e) {
      if (filename == null) {
        die("Filename passed to writer() was null", e);
      } else {
        die("Couldn't create a writer for " + filename, e);
      }
    }
    return null;
  }


  /**
   * I want to print lines to a file. I have RSI from typing these
   * eight lines of code so many times.
   */
  static public PrintWriter createWriter(File file) {
    try {
      return createWriter(new FileOutputStream(file));

    } catch (Exception e) {
      if (file == null) {
        throw new RuntimeException("File passed to writer() was null");
      } else {
        e.printStackTrace();
        throw new RuntimeException("Couldn't create a writer for " +
                                   file.getAbsolutePath());
      }
    }
    //return null;
  }


  /**
   * I want to print lines to a file. Why am I always explaining myself?
   * It's the JavaSoft API engineers who need to explain themselves.
   */
  static public PrintWriter createWriter(OutputStream output) {
    OutputStreamWriter osw = new OutputStreamWriter(output);
    return new PrintWriter(osw);
  }


  /**
   * Simplified method to open a Java InputStream.
   * <P>
   * This method is useful if you want to use the facilities provided
   * by PApplet to easily open things from the data folder or from a URL,
   * but want an InputStream object so that you can use other Java
   * methods to take more control of how the stream is read.
   * <P>
   * If the requested item doesn't exist, null is returned.
   * (Prior to 0096, die() would be called, killing the applet)
   * <P>
   * For 0096+, the "data" folder is exported intact with subfolders,
   * and openStream() properly handles subdirectories from the data folder
   * <P>
   * If not online, this will also check to see if the user is asking
   * for a file whose name isn't properly capitalized. This helps prevent
   * issues when a sketch is exported to the web, where case sensitivity
   * matters, as opposed to Windows and the Mac OS default where
   * case sensitivity is preserved but ignored.
   * <P>
   * It is strongly recommended that libraries use this method to open
   * data files, so that the loading sequence is handled in the same way
   * as functions like loadBytes(), loadImage(), etc.
   * <P>
   * The filename passed in can be:
   * <UL>
   * <LI>A URL, for instance openStream("http://processing.org/");
   * <LI>A file in the sketch's data folder
   * <LI>Another file to be opened locally (when running as an application)
   * </UL>
   */
  public InputStream openStream(String filename) {
    InputStream stream = null;

    if (filename == null) return null;

    if (filename.length() == 0) {
      // an error will be called by the parent function
      //System.err.println("The filename passed to openStream() was empty.");
      return null;
    }

    // safe to check for this as a url first. this will prevent online
    // access logs from being spammed with GET /sketchfolder/http://blahblah
    try {
      URL url = new URL(filename);
      stream = url.openStream();
      return stream;

    } catch (MalformedURLException mfue) {
      // not a url, that's fine

    } catch (FileNotFoundException fnfe) {
      // Java 1.5 likes to throw this when URL not available. (fix for 0119)
      // http://dev.processing.org/bugs/show_bug.cgi?id=403

    } catch (IOException e) {
      // changed for 0117, shouldn't be throwing exception
      e.printStackTrace();
      //System.err.println("Error downloading from URL " + filename);
      return null;
      //throw new RuntimeException("Error downloading from URL " + filename);
    }

    // using getClassLoader() prevents java from converting dots
    // to slashes or requiring a slash at the beginning.
    // (a slash as a prefix means that it'll load from the root of
    // the jar, rather than trying to dig into the package location)
    ClassLoader cl = getClass().getClassLoader();

    // by default, data files are exported to the root path of the jar.
    // (not the data folder) so check there first.
    stream = cl.getResourceAsStream("data/" + filename);
    if (stream != null) {
      String cn = stream.getClass().getName();
      // this is an irritation of sun's java plug-in, which will return
      // a non-null stream for an object that doesn't exist. like all good
      // things, this is probably introduced in java 1.5. awesome!
      // http://dev.processing.org/bugs/show_bug.cgi?id=359
      if (!cn.equals("sun.plugin.cache.EmptyInputStream")) {
        return stream;
      }
    }

    // when used with an online script, also need to check without the
    // data folder, in case it's not in a subfolder called 'data'
    // http://dev.processing.org/bugs/show_bug.cgi?id=389
    stream = cl.getResourceAsStream(filename);
    if (stream != null) {
      String cn = stream.getClass().getName();
      if (!cn.equals("sun.plugin.cache.EmptyInputStream")) {
        return stream;
      }
    }

    // handle case sensitivity check
    if (!online) {
      try {
        // first see if it's in a data folder
        File file = new File(dataPath(filename));
        if (!file.exists()) {
          // next see if it's just in this folder
          file = new File(sketchPath, filename);
        }
        if (file.exists()) {
          try {
            String filePath = file.getCanonicalPath();
            String filenameActual = new File(filePath).getName();
            // make sure there isn't a subfolder prepended to the name
            String filenameShort = new File(filename).getName();
            // if the actual filename is the same, but capitalized
            // differently, warn the user.
            //if (filenameActual.equalsIgnoreCase(filenameShort) &&
            //!filenameActual.equals(filenameShort)) {
            if (!filenameActual.equals(filenameShort)) {
              throw new RuntimeException("This file is named " +
                                         filenameActual + " not " +
                                         filename + ". Re-name it " +
                                         "or change your code.");
            }
          } catch (IOException e) { }
        }

        // if this file is ok, may as well just load it
        stream = new FileInputStream(file);
        if (stream != null) return stream;

        // have to break these out because a general Exception might
        // catch the RuntimeException being thrown above
      } catch (IOException ioe) {
      } catch (SecurityException se) { }
    }

    try {
      // attempt to load from a local file, used when running as
      // an application, or as a signed applet
      try {  // first try to catch any security exceptions
        try {
          stream = new FileInputStream(dataPath(filename));
          if (stream != null) return stream;
        } catch (IOException e2) { }

        try {
          stream = new FileInputStream(sketchPath(filename));
          if (stream != null) return stream;
        } catch (Exception e) { }  // ignored

        try {
          stream = new FileInputStream(filename);
          if (stream != null) return stream;
        } catch (IOException e1) { }

      } catch (SecurityException se) { }  // online, whups

    } catch (Exception e) {
      //die(e.getMessage(), e);
      e.printStackTrace();
    }
    return null;
  }


  static public InputStream openStream(File file) {
    try {
      return new FileInputStream(file);

    } catch (IOException e) {
      if (file == null) {
        throw new RuntimeException("File passed to openStream() was null");

      } else {
        e.printStackTrace();
        throw new RuntimeException("Couldn't openStream() for " +
                                   file.getAbsolutePath());
      }
    }
  }


  public InputStream openStreamGZ(String filename) {
    try {
      return new GZIPInputStream(openStream(filename));
    } catch (IOException e) {
      e.printStackTrace();
      throw new RuntimeException("Couldn't openStreamGZ() for " +
                                 filename);
    }
  }


  static public InputStream openStreamGZ(File file) {
    try {
      return new GZIPInputStream(openStream(file));
    } catch (IOException e) {
      e.printStackTrace();
      throw new RuntimeException("Couldn't openStreamGZ() for " +
                                 file.getAbsolutePath());
    }
  }


  public byte[] loadBytes(String filename) {
    InputStream is = openStream(filename);
    if (is != null) return loadBytes(is);

    System.err.println("The file \"" + filename + "\" " +
                       "is missing or inaccessible, make sure " +
                       "the URL is valid or that the file has been " +
                       "added to your sketch and is readable.");
    return null;
  }


  static public byte[] loadBytes(InputStream input) {
    try {
      BufferedInputStream bis = new BufferedInputStream(input);
      ByteArrayOutputStream out = new ByteArrayOutputStream();

      int c = bis.read();
      while (c != -1) {
        out.write(c);
        c = bis.read();
      }
      return out.toByteArray();

    } catch (IOException e) {
      e.printStackTrace();
      //throw new RuntimeException("Couldn't load bytes from stream");
    }
    return null;
  }


  static public String[] loadStrings(File file) {
    InputStream is = openStream(file);
    if (is != null) return loadStrings(is);
    return null;
  }


  /**
   * Load data from a file and shove it into a String array.
   * <P>
   * Exceptions are handled internally, when an error, occurs, an
   * exception is printed to the console and 'null' is returned,
   * but the program continues running. This is a tradeoff between
   * 1) showing the user that there was a problem but 2) not requiring
   * that all i/o code is contained in try/catch blocks, for the sake
   * of new users (or people who are just trying to get things done
   * in a "scripting" fashion. If you want to handle exceptions,
   * use Java methods for I/O.
   */
  public String[] loadStrings(String filename) {
    InputStream is = openStream(filename);
    if (is != null) return loadStrings(is);

    System.err.println("The file \"" + filename + "\" " +
                       "is missing or inaccessible, make sure " +
                       "the URL is valid or that the file has been " +
                       "added to your sketch and is readable.");
    return null;
  }


  static public String[] loadStrings(InputStream input) {
    try {
      BufferedReader reader =
        new BufferedReader(new InputStreamReader(input));

      String lines[] = new String[100];
      int lineCount = 0;
      String line = null;
      while ((line = reader.readLine()) != null) {
        if (lineCount == lines.length) {
          String temp[] = new String[lineCount << 1];
          System.arraycopy(lines, 0, temp, 0, lineCount);
          lines = temp;
        }
        lines[lineCount++] = line;
      }
      reader.close();

      if (lineCount == lines.length) {
        return lines;
      }

      // resize array to appropriate amount for these lines
      String output[] = new String[lineCount];
      System.arraycopy(lines, 0, output, 0, lineCount);
      return output;

    } catch (IOException e) {
      e.printStackTrace();
      //throw new RuntimeException("Error inside loadStrings()");
    }
    return null;
  }



  //////////////////////////////////////////////////////////////

  // FILE OUTPUT


  /**
   * Save the contents of a stream to a file in the sketch folder.
   * This is basically saveBytes(loadBytes(), blah), but done
   * in a less confusing manner.
   */
  public void saveStream(String filename, String stream) {
    saveBytes(filename, loadBytes(stream));
  }


  /**
   * Identical to the other saveStream(), but writes to a File
   * object, for greater control over the file location.
   */
  public void saveStream(File file, String stream) {
    saveBytes(file, loadBytes(stream));
  }


  /**
   * Saves bytes to a file to inside the sketch folder.
   * The filename can be a relative path, i.e. "poo/bytefun.txt"
   * would save to a file named "bytefun.txt" to a subfolder
   * called 'poo' inside the sketch folder. If the in-between
   * subfolders don't exist, they'll be created.
   */
  public void saveBytes(String filename, byte buffer[]) {
    try {
      String location = savePath(filename);
      FileOutputStream fos = new FileOutputStream(location);
      saveBytes(fos, buffer);
      fos.close();

    } catch (IOException e) {
      System.err.println("error saving bytes to " + filename);
      e.printStackTrace();
    }
  }

  /**
   * Saves bytes to a specific File location specified by the user.
   */
  static public void saveBytes(File file, byte buffer[]) {
    try {
      String filename = file.getAbsolutePath();
      createPath(filename);
      FileOutputStream fos = new FileOutputStream(file);
      saveBytes(fos, buffer);
      fos.close();

    } catch (IOException e) {
      System.err.println("error saving bytes to " + file);
      e.printStackTrace();
    }
  }


  /**
   * Spews a buffer of bytes to an OutputStream.
   */
  static public void saveBytes(OutputStream output, byte buffer[]) {
    try {
      //BufferedOutputStream bos = new BufferedOutputStream(output);
      output.write(buffer);
      output.flush();

    } catch (IOException e) {
      e.printStackTrace();
      throw new RuntimeException("Couldn't save bytes");
    }
  }

  //

  public void saveStrings(String filename, String strings[]) {
    try {
      String location = savePath(filename);
      FileOutputStream fos = new FileOutputStream(location);
      saveStrings(fos, strings);
      fos.close();

    } catch (IOException e) {
      e.printStackTrace();
      throw new RuntimeException("saveStrings() failed: " + e.getMessage());
    }
  }


  static public void saveStrings(File file, String strings[]) {
    try {
      String location = file.getAbsolutePath();
      createPath(location);
      FileOutputStream fos = new FileOutputStream(location);
      saveStrings(fos, strings);
      fos.close();

    } catch (IOException e) {
      System.err.println("error while saving strings");
      e.printStackTrace();
    }
  }


  static public void saveStrings(OutputStream output, String strings[]) {
    PrintWriter writer =
      new PrintWriter(new OutputStreamWriter(output));
    for (int i = 0; i < strings.length; i++) {
      writer.println(strings[i]);
    }
    writer.flush();
  }


  //////////////////////////////////////////////////////////////


  /**
   * Prepend the sketch folder path to the filename (or path) that is
   * passed in. External libraries should use this function to save to
   * the sketch folder.
   * <p/>
   * Note that when running as an applet inside a web browser,
   * the sketchPath will be set to null, because security restrictions
   * prevent applets from accessing that information.
   * <p/>
   * This will also cause an error if the sketch is not inited properly,
   * meaning that init() was never called on the PApplet when hosted
   * my some other main() or by other code. For proper use of init(),
   * see the examples in the main description text for PApplet.
   */
  public String sketchPath(String where) {
    if (sketchPath == null) {
      throw new RuntimeException("The applet was not inited properly, " +
                                 "or security restrictions prevented " +
                                 "it from determining its path.");
    }
    // isAbsolute() could throw an access exception, but so will writing
    // to the local disk using the sketch path, so this is safe here.
    // for 0120, added a try/catch anyways.
    try {
      if (new File(where).isAbsolute()) return where;
    } catch (Exception e) { }

    return sketchPath + File.separator + where;
  }


  /**
   * Returns a path inside the applet folder to save to,
   * just like sketchPath(), but also creates any in-between
   * folders so that things save properly.
   * <p/>
   * All saveXxxx() functions use the path to the sketch folder, rather than
   * its data folder. Once exported, the data folder will be found inside the
   * jar file of the exported application or applet. In this case, it's not
   * possible to save data into the jar file, because it will often be running
   * from a server, or marked in-use if running from a local file system.
   * With this in mind, saving to the data path doesn't make sense anyway.
   * If you know you're running locally, and want to save to the data folder,
   * use <TT>saveXxxx("data/blah.dat")</TT>.
   */
  public String savePath(String where) {
    String filename = sketchPath(where);
    createPath(filename);
    return filename;
  }


  /**
   * Return a full path to an item in the data folder.
   * <p>
   * In this method, the data path is defined not as the applet's actual
   * data path, but a folder titled "data" in the sketch's working
   * directory. This is because in an application, the "data" folder is
   * exported as part of the jar file, and it's not as though you're gonna
   * write into the jar file itself. If you need to get things out of
   * the jar file, you should use openStream().
   */
  public String dataPath(String where) {
    // isAbsolute() could throw an access exception, but so will writing
    // to the local disk using the sketch path, so this is safe here.
    if (new File(where).isAbsolute()) return where;

    return sketchPath + File.separator + "data" + File.separator + where;
  }


  /**
   * Takes a path and creates any in-between folders if they don't
   * already exist. Useful when trying to save to a subfolder that
   * may not actually exist.
   */
  static public void createPath(String filename) {
    File file = new File(filename);
    String parent = file.getParent();
    if (parent != null) {
      File unit = new File(parent);
      if (!unit.exists()) unit.mkdirs();
    }
  }


  //////////////////////////////////////////////////////////////

  // SORT

  int sort_mode;

  static final int BYTES   = 1;
  static final int CHARS   = 2;
  static final int INTS    = 3;
  static final int FLOATS  = 4;
  static final int STRINGS = 5;
  static final int OBJECTS = 6;

  byte sort_bytes[];
  char sort_chars[];
  int sort_ints[];
  float sort_floats[];
  String sort_strings[];

  Object sortObject;
  Method swapMethod;
  Method compareMethod;

  public byte[] sort(byte what[]) {
    return sort(what, what.length);
  }

  public char[] sort(char what[]) {
    return sort(what, what.length);
  }

  public int[] sort(int what[]) {
    return sort(what, what.length);
  }

  public float[] sort(float what[]) {
    return sort(what, what.length);
  }

  public String[] sort(String what[]) {
    return sort(what, what.length);
  }

  //

  public byte[] sort(byte what[], int count) {
    if (count == 0) return null;
    sort_mode = BYTES;
    sort_bytes = new byte[count];
    System.arraycopy(what, 0, sort_bytes, 0, count);
    sort_internal(0, count-1);
    return sort_bytes;
  }

  public char[] sort(char what[], int count) {
    if (count == 0) return null;
    sort_mode = CHARS;
    sort_chars = new char[count];
    System.arraycopy(what, 0, sort_chars, 0, count);
    sort_internal(0, count-1);
    return sort_chars;
  }

  public int[] sort(int what[], int count) {
    if (count == 0) return null;
    sort_mode = INTS;
    sort_ints = new int[count];
    System.arraycopy(what, 0, sort_ints, 0, count);
    sort_internal(0, count-1);
    return sort_ints;
  }

  public float[] sort(float what[], int count) {
    if (count == 0) return null;
    sort_mode = FLOATS;
    sort_floats = new float[count];
    System.arraycopy(what, 0, sort_floats, 0, count);
    sort_internal(0, count-1);
    return sort_floats;
  }

  public String[] sort(String what[], int count) {
    if (count == 0) return null;
    sort_mode = STRINGS;
    sort_strings = new String[count];
    System.arraycopy(what, 0, sort_strings, 0, count);
    sort_internal(0, count-1);
    return sort_strings;
  }

  /*
  public void sort(Object what, int count) {
    if (count == 0) return null;
    sort_mode = OBJECTS;
    sort_strings = new String[count];
    System.arraycopy(what, 0, sort_strings, 0, count);
    sort_internal(0, count-1);
    return sort_strings;
  }
  */

  //

  protected void sort_internal(int i, int j) {
    int pivotIndex = (i+j)/2;
    sort_swap(pivotIndex, j);
    int k = sort_partition(i-1, j);
    sort_swap(k, j);
    if ((k-i) > 1) sort_internal(i, k-1);
    if ((j-k) > 1) sort_internal(k+1, j);
  }


  protected int sort_partition(int left, int right) {
    int pivot = right;
    do {
      while (sort_compare(++left, pivot) < 0) { }
      while ((right != 0) && (sort_compare(--right, pivot) > 0)) { }
      sort_swap(left, right);
    } while (left < right);
    sort_swap(left, right);
    return left;
  }


  protected void sort_swap(int a, int b) {
    switch (sort_mode) {
    case BYTES:
      byte btemp = sort_bytes[a];
      sort_bytes[a] = sort_bytes[b];
      sort_bytes[b] = btemp;
      break;
    case CHARS:
      char ctemp = sort_chars[a];
      sort_chars[a] = sort_chars[b];
      sort_chars[b] = ctemp;
      break;
    case INTS:
      int itemp = sort_ints[a];
      sort_ints[a] = sort_ints[b];
      sort_ints[b] = itemp;
      break;
    case FLOATS:
      float ftemp = sort_floats[a];
      sort_floats[a] = sort_floats[b];
      sort_floats[b] = ftemp;
      break;
    case STRINGS:
      String stemp = sort_strings[a];
      sort_strings[a] = sort_strings[b];
      sort_strings[b] = stemp;
      break;
    case OBJECTS:
      try {
        Object[] params = new Object[] { new Integer(a), new Integer(b) };
        swapMethod.invoke(sortObject, params);
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
  }


  protected float sort_compare(int a, int b) {
    switch (sort_mode) {
    case BYTES:
      return sort_bytes[a] - sort_bytes[b];
    case CHARS:
      return sort_chars[a] - sort_chars[b];
    case INTS:
      return sort_ints[a] - sort_ints[b];
    case FLOATS:
      // can't just cast to an int because 0.2 and 0.4 would
      // just appear to be the same thing. no good.
      //if (sort_floats[a] < sort_floats[b]) return -1;
      //return (sort_floats[a] == sort_floats[b]) ? 0 : 1;
      return sort_floats[a] - sort_floats[b];
    case STRINGS:
      return sort_strings[a].compareTo(sort_strings[b]);
    case OBJECTS:
      try {
        Object[] params = new Object[] { new Integer(a), new Integer(b) };
        Float output = (Float) compareMethod.invoke(sortObject, params);
        return output.floatValue();
      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    return 0;
  }


  //////////////////////////////////////////////////////////////


  public void sort(Object o, int count) {
    Class c = o.getClass();
    try {
      Class[] params = new Class[] { Integer.TYPE, Integer.TYPE };
      // takes two ints, returns a float
      compareMethod = c.getMethod("sortCompare", params);
      // takes two ints, returns void
      swapMethod = c.getMethod("sortSwap", params);
      // start the sort
      sortObject = o;
      sort_mode = OBJECTS;
      sort_internal(0, count-1);

    } catch (Exception e) {
      e.printStackTrace();
    }
  }


  //////////////////////////////////////////////////////////////

  // ARRAY UTILITIES


  /**
   * Calls System.arraycopy(), included here so that we can
   * avoid people needing to learn about the System object
   * before they can just copy an array.
   */
  static public void arraycopy(Object src, int srcPosition,
                               Object dst, int dstPosition,
                               int length) {
    System.arraycopy(src, srcPosition, dst, dstPosition, length);
  }


  /**
   * Convenience method for arraycopy().
   * Identical to <CODE>arraycopy(src, 0, dst, 0, length);</CODE>
   */
  static public void arraycopy(Object src, Object dst, int length) {
    System.arraycopy(src, 0, dst, 0, length);
  }


  /**
   * Shortcut to copy the entire contents of
   * the source into the destination array.
   * Identical to <CODE>arraycopy(src, 0, dst, 0, src.length);</CODE>
   */
  static public void arraycopy(Object src, Object dst) {
    System.arraycopy(src, 0, dst, 0, Array.getLength(src));
  }

  //

  static public boolean[] expand(boolean list[]) {
    return expand(list, list.length << 1);
  }

  static public boolean[] expand(boolean list[], int newSize) {
    boolean temp[] = new boolean[newSize];
    System.arraycopy(list, 0, temp, 0, Math.min(newSize, list.length));
    return temp;
  }


  static public byte[] expand(byte list[]) {
    return expand(list, list.length << 1);
  }

  static public byte[] expand(byte list[], int newSize) {
    byte temp[] = new byte[newSize];
    System.arraycopy(list, 0, temp, 0, Math.min(newSize, list.length));
    return temp;
  }


  static public char[] expand(char list[]) {
    return expand(list, list.length << 1);
  }

  static public char[] expand(char list[], int newSize) {
    char temp[] = new char[newSize];
    System.arraycopy(list, 0, temp, 0, Math.min(newSize, list.length));
    return temp;
  }


  static public int[] expand(int list[]) {
    return expand(list, list.length << 1);
  }

  static public int[] expand(int list[], int newSize) {
    int temp[] = new int[newSize];
    System.arraycopy(list, 0, temp, 0, Math.min(newSize, list.length));
    return temp;
  }


  static public float[] expand(float list[]) {
    return expand(list, list.length << 1);
  }

  static public float[] expand(float list[], int newSize) {
    float temp[] = new float[newSize];
    System.arraycopy(list, 0, temp, 0, Math.min(newSize, list.length));
    return temp;
  }


  static public String[] expand(String list[]) {
    return expand(list, list.length << 1);
  }

  static public String[] expand(String list[], int newSize) {
    String temp[] = new String[newSize];
    // in case the new size is smaller than list.length
    System.arraycopy(list, 0, temp, 0, Math.min(newSize, list.length));
    return temp;
  }


  static public Object expand(Object array) {
    return expand(array, Array.getLength(array) << 1);
  }

  static public Object expand(Object list, int newSize) {
    Class type = list.getClass().getComponentType();
    Object temp = Array.newInstance(type, newSize);
    System.arraycopy(list, 0, temp, 0,
                     Math.min(Array.getLength(list), newSize));
    return temp;
  }

  //

  // contract() has been removed in revision 0124, use subset() instead.
  // (expand() is also functionally equivalent)

  //

  static public byte[] append(byte b[], byte value) {
    b = expand(b, b.length + 1);
    b[b.length-1] = value;
    return b;
  }

  static public char[] append(char b[], char value) {
    b = expand(b, b.length + 1);
    b[b.length-1] = value;
    return b;
  }

  static public int[] append(int b[], int value) {
    b = expand(b, b.length + 1);
    b[b.length-1] = value;
    return b;
  }

  static public float[] append(float b[], float value) {
    b = expand(b, b.length + 1);
    b[b.length-1] = value;
    return b;
  }

  static public String[] append(String b[], String value) {
    b = expand(b, b.length + 1);
    b[b.length-1] = value;
    return b;
  }

  static public Object append(Object b, Object value) {
    int length = Array.getLength(b);
    b = expand(b, length + 1);
    Array.set(b, length, value);
    return b;
  }

  //

  static public boolean[] shorten(boolean list[]) {
    return subset(list, 0, list.length-1);
  }

  static public byte[] shorten(byte list[]) {
    return subset(list, 0, list.length-1);
  }

  static public char[] shorten(char list[]) {
    return subset(list, 0, list.length-1);
  }

  static public int[] shorten(int list[]) {
    return subset(list, 0, list.length-1);
  }

  static public float[] shorten(float list[]) {
    return subset(list, 0, list.length-1);
  }

  static public String[] shorten(String list[]) {
    return subset(list, 0, list.length-1);
  }

  static public Object shorten(Object list) {
    int length = Array.getLength(list);
    return subset(list, 0, length - 1);
  }

  //

  static final public boolean[] splice(boolean list[],
                                       boolean v, int index) {
    boolean outgoing[] = new boolean[list.length + 1];
    System.arraycopy(list, 0, outgoing, 0, index);
    outgoing[index] = v;
    System.arraycopy(list, index, outgoing, index + 1,
                     list.length - index);
    return outgoing;
  }

  static final public boolean[] splice(boolean list[],
                                       boolean v[], int index) {
    boolean outgoing[] = new boolean[list.length + v.length];
    System.arraycopy(list, 0, outgoing, 0, index);
    System.arraycopy(v, 0, outgoing, index, v.length);
    System.arraycopy(list, index, outgoing, index + v.length,
                     list.length - index);
    return outgoing;
  }


  static final public byte[] splice(byte list[],
                                    byte v, int index) {
    byte outgoing[] = new byte[list.length + 1];
    System.arraycopy(list, 0, outgoing, 0, index);
    outgoing[index] = v;
    System.arraycopy(list, index, outgoing, index + 1,
                     list.length - index);
    return outgoing;
  }

  static final public byte[] splice(byte list[],
                                    byte v[], int index) {
    byte outgoing[] = new byte[list.length + v.length];
    System.arraycopy(list, 0, outgoing, 0, index);
    System.arraycopy(v, 0, outgoing, index, v.length);
    System.arraycopy(list, index, outgoing, index + v.length,
                     list.length - index);
    return outgoing;
  }


  static final public char[] splice(char list[],
                                    char v, int index) {
    char outgoing[] = new char[list.length + 1];
    System.arraycopy(list, 0, outgoing, 0, index);
    outgoing[index] = v;
    System.arraycopy(list, index, outgoing, index + 1,
                     list.length - index);
    return outgoing;
  }

  static final public char[] splice(char list[],
                                    char v[], int index) {
    char outgoing[] = new char[list.length + v.length];
    System.arraycopy(list, 0, outgoing, 0, index);
    System.arraycopy(v, 0, outgoing, index, v.length);
    System.arraycopy(list, index, outgoing, index + v.length,
                     list.length - index);
    return outgoing;
  }


  static final public int[] splice(int list[],
                                   int v, int index) {
    int outgoing[] = new int[list.length + 1];
    System.arraycopy(list, 0, outgoing, 0, index);
    outgoing[index] = v;
    System.arraycopy(list, index, outgoing, index + 1,
                     list.length - index);
    return outgoing;
  }

  static final public int[] splice(int list[],
                                   int v[], int index) {
    int outgoing[] = new int[list.length + v.length];
    System.arraycopy(list, 0, outgoing, 0, index);
    System.arraycopy(v, 0, outgoing, index, v.length);
    System.arraycopy(list, index, outgoing, index + v.length,
                     list.length - index);
    return outgoing;
  }


  static final public float[] splice(float list[],
                                     float v, int index) {
    float outgoing[] = new float[list.length + 1];
    System.arraycopy(list, 0, outgoing, 0, index);
    outgoing[index] = v;
    System.arraycopy(list, index, outgoing, index + 1,
                     list.length - index);
    return outgoing;
  }

  static final public float[] splice(float list[],
                                     float v[], int index) {
    float outgoing[] = new float[list.length + v.length];
    System.arraycopy(list, 0, outgoing, 0, index);
    System.arraycopy(v, 0, outgoing, index, v.length);
    System.arraycopy(list, index, outgoing, index + v.length,
                     list.length - index);
    return outgoing;
  }


  static final public String[] splice(String list[],
                                      String v, int index) {
    String outgoing[] = new String[list.length + 1];
    System.arraycopy(list, 0, outgoing, 0, index);
    outgoing[index] = v;
    System.arraycopy(list, index, outgoing, index + 1,
                     list.length - index);
    return outgoing;
  }

  static final public String[] splice(String list[],
                                      String v[], int index) {
    String outgoing[] = new String[list.length + v.length];
    System.arraycopy(list, 0, outgoing, 0, index);
    System.arraycopy(v, 0, outgoing, index, v.length);
    System.arraycopy(list, index, outgoing, index + v.length,
                     list.length - index);
    return outgoing;
  }


  static final public Object splice(Object list, Object v, int index) {
    Object[] outgoing = null;
    int length = Array.getLength(list);

    // check whether is an array or not, and if so, treat as such
    if (list.getClass().getName().charAt(0) == '[') {
      int vlength = Array.getLength(v);
      outgoing = new Object[length + vlength];
      System.arraycopy(list, 0, outgoing, 0, index);
      System.arraycopy(v, 0, outgoing, index, vlength);
      System.arraycopy(list, index, outgoing, index + vlength, length - index);

    } else {
      outgoing = new Object[length + 1];
      System.arraycopy(list, 0, outgoing, 0, index);
      Array.set(outgoing, index, v);
      System.arraycopy(list, index, outgoing, index + 1, length - index);
    }
    return outgoing;
  }

  //

  static public boolean[] subset(boolean list[], int start) {
    return subset(list, start, list.length - start);
  }

  static public boolean[] subset(boolean list[], int start, int count) {
    boolean output[] = new boolean[count];
    System.arraycopy(list, start, output, 0, count);
    return output;
  }


  static public byte[] subset(byte list[], int start) {
    return subset(list, start, list.length - start);
  }

  static public byte[] subset(byte list[], int start, int count) {
    byte output[] = new byte[count];
    System.arraycopy(list, start, output, 0, count);
    return output;
  }


  static public char[] subset(char list[], int start) {
    return subset(list, start, list.length - start);
  }

  static public char[] subset(char list[], int start, int count) {
    char output[] = new char[count];
    System.arraycopy(list, start, output, 0, count);
    return output;
  }


  static public int[] subset(int list[], int start) {
    return subset(list, start, list.length - start);
  }

  static public int[] subset(int list[], int start, int count) {
    int output[] = new int[count];
    System.arraycopy(list, start, output, 0, count);
    return output;
  }


  static public float[] subset(float list[], int start) {
    return subset(list, start, list.length - start);
  }

  static public float[] subset(float list[], int start, int count) {
    float output[] = new float[count];
    System.arraycopy(list, start, output, 0, count);
    return output;
  }


  static public String[] subset(String list[], int start) {
    return subset(list, start, list.length - start);
  }

  static public String[] subset(String list[], int start, int count) {
    String output[] = new String[count];
    System.arraycopy(list, start, output, 0, count);
    return output;
  }


  static public Object subset(Object list, int start) {
    int length = Array.getLength(list);
    int count = length - start;
    Class type = list.getClass().getComponentType();
    Object outgoing = Array.newInstance(type, count);
    System.arraycopy(list, 0, outgoing, 0, count);
    return outgoing;
  }

  static public Object subset(Object list, int start, int count) {
    //int length = Array.getLength(list);
    Class type = list.getClass().getComponentType();
    Object outgoing = Array.newInstance(type, count);
    System.arraycopy(list, start, outgoing, 0, count);
    return outgoing;
  }

  //

  static public boolean[] concat(boolean a[], boolean b[]) {
    boolean c[] = new boolean[a.length + b.length];
    System.arraycopy(a, 0, c, 0, a.length);
    System.arraycopy(b, 0, c, a.length, b.length);
    return c;
  }

  static public byte[] concat(byte a[], byte b[]) {
    byte c[] = new byte[a.length + b.length];
    System.arraycopy(a, 0, c, 0, a.length);
    System.arraycopy(b, 0, c, a.length, b.length);
    return c;
  }

  static public char[] concat(char a[], char b[]) {
    char c[] = new char[a.length + b.length];
    System.arraycopy(a, 0, c, 0, a.length);
    System.arraycopy(b, 0, c, a.length, b.length);
    return c;
  }

  static public int[] concat(int a[], int b[]) {
    int c[] = new int[a.length + b.length];
    System.arraycopy(a, 0, c, 0, a.length);
    System.arraycopy(b, 0, c, a.length, b.length);
    return c;
  }

  static public float[] concat(float a[], float b[]) {
    float c[] = new float[a.length + b.length];
    System.arraycopy(a, 0, c, 0, a.length);
    System.arraycopy(b, 0, c, a.length, b.length);
    return c;
  }

  static public String[] concat(String a[], String b[]) {
    String c[] = new String[a.length + b.length];
    System.arraycopy(a, 0, c, 0, a.length);
    System.arraycopy(b, 0, c, a.length, b.length);
    return c;
  }

  static public Object concat(Object a, Object b) {
    Class type = a.getClass().getComponentType();
    int alength = Array.getLength(a);
    int blength = Array.getLength(b);
    Object outgoing = Array.newInstance(type, alength + blength);
    System.arraycopy(a, 0, outgoing, 0, alength);
    System.arraycopy(b, 0, outgoing, alength, blength);
    return outgoing;
  }

  //

  static public boolean[] reverse(boolean list[]) {
    boolean outgoing[] = new boolean[list.length];
    int length1 = list.length - 1;
    for (int i = 0; i < list.length; i++) {
      outgoing[i] = list[length1 - i];
    }
    return outgoing;
  }

  static public byte[] reverse(byte list[]) {
    byte outgoing[] = new byte[list.length];
    int length1 = list.length - 1;
    for (int i = 0; i < list.length; i++) {
      outgoing[i] = list[length1 - i];
    }
    return outgoing;
  }

  static public char[] reverse(char list[]) {
    char outgoing[] = new char[list.length];
    int length1 = list.length - 1;
    for (int i = 0; i < list.length; i++) {
      outgoing[i] = list[length1 - i];
    }
    return outgoing;
  }

  static public int[] reverse(int list[]) {
    int outgoing[] = new int[list.length];
    int length1 = list.length - 1;
    for (int i = 0; i < list.length; i++) {
      outgoing[i] = list[length1 - i];
    }
    return outgoing;
  }

  static public float[] reverse(float list[]) {
    float outgoing[] = new float[list.length];
    int length1 = list.length - 1;
    for (int i = 0; i < list.length; i++) {
      outgoing[i] = list[length1 - i];
    }
    return outgoing;
  }

  static public String[] reverse(String list[]) {
    String outgoing[] = new String[list.length];
    int length1 = list.length - 1;
    for (int i = 0; i < list.length; i++) {
      outgoing[i] = list[length1 - i];
    }
    return outgoing;
  }

  static public Object reverse(Object list) {
    Class type = list.getClass().getComponentType();
    int length = Array.getLength(list);
    Object outgoing = Array.newInstance(type, length);
    for (int i = 0; i < length; i++) {
      Array.set(outgoing, i, Array.get(list, (length - 1) - i));
    }
    return outgoing;
  }



  //////////////////////////////////////////////////////////////

  // STRINGS


  /**
   * Remove whitespace characters from the beginning and ending
   * of a String. Works like String.trim() but includes the
   * unicode nbsp character as well.
   */
  static public String trim(String str) {
    return str.replace('\u00A0', ' ').trim();

    /*
    int left = 0;
    int right = str.length() - 1;

    while ((left <= right) &&
           (WHITESPACE.indexOf(str.charAt(left)) != -1)) left++;
    if (left == right) return "";

    while (WHITESPACE.indexOf(str.charAt(right)) != -1) --right;

    return str.substring(left, right-left+1);
    */
  }


  /**
   * Trim the whitespace from a String array. This returns a new
   * array and does not affect the passed-in array.
   */
  static public String[] trim(String[] array) {
    String[] outgoing = new String[array.length];
    for (int i = 0; i < array.length; i++) {
      outgoing[i] = array[i].replace('\u00A0', ' ').trim();
    }
    return outgoing;
  }


  /**
   * Join an array of Strings together as a single String,
   * separated by the whatever's passed in for the separator.
   */
  static public String join(String str[], char separator) {
    return join(str, String.valueOf(separator));
  }


  /**
   * Join an array of Strings together as a single String,
   * separated by the whatever's passed in for the separator.
   * <P>
   * To use this on numbers, first pass the array to nf() or nfs()
   * to get a list of String objects, then use join on that.
   * <PRE>
   * e.g. String stuff[] = { "apple", "bear", "cat" };
   *      String list = join(stuff, ", ");
   *      // list is now "apple, bear, cat"</PRE>
   */
  static public String join(String str[], String separator) {
    StringBuffer buffer = new StringBuffer();
    for (int i = 0; i < str.length; i++) {
      if (i != 0) buffer.append(separator);
      buffer.append(str[i]);
    }
    return buffer.toString();
  }


  /**
   * Split the provided String at wherever whitespace occurs.
   * Multiple whitespace (extra spaces or tabs or whatever)
   * between items will count as a single break.
   * <P>
   * The whitespace characters are "\t\n\r\f", which are the defaults
   * for java.util.StringTokenizer, plus the unicode non-breaking space
   * character, which is found commonly on files created by or used
   * in conjunction with Mac OS X (character 160, or 0x00A0 in hex).
   * <PRE>
   * i.e. splitTokens("a b") -> { "a", "b" }
   *      splitTokens("a    b") -> { "a", "b" }
   *      splitTokens("a\tb") -> { "a", "b" }
   *      splitTokens("a \t  b  ") -> { "a", "b" }</PRE>
   */
  static public String[] splitTokens(String what) {
    return splitTokens(what, WHITESPACE);
  }


  /**
   * Splits a string into pieces, using any of the chars in the
   * String 'delim' as separator characters. For instance,
   * in addition to white space, you might want to treat commas
   * as a separator. The delimeter characters won't appear in
   * the returned String array.
   * <PRE>
   * i.e. splitTokens("a, b", " ,") -> { "a", "b" }
   * </PRE>
   * To include all the whitespace possibilities, use the variable
   * WHITESPACE, found in PConstants:
   * <PRE>
   * i.e. splitTokens("a   | b", WHITESPACE + "|");  ->  { "a", "b" }</PRE>
   */
  static public String[] splitTokens(String what, String delim) {
    StringTokenizer toker = new StringTokenizer(what, delim);
    String pieces[] = new String[toker.countTokens()];

    int index = 0;
    while (toker.hasMoreTokens()) {
      pieces[index++] = toker.nextToken();
    }
    return pieces;
  }


  /**
   * Split a string into pieces along a specific character.
   * Most commonly used to break up a String along tab characters.
   * <P>
   * This operates differently than the others, where the
   * single delimeter is the only breaking point, and consecutive
   * delimeters will produce an empty string (""). This way,
   * one can split on tab characters, but maintain the column
   * alignments (of say an excel file) where there are empty columns.
   */
  static public String[] split(String what, char delim) {
    // do this so that the exception occurs inside the user's
    // program, rather than appearing to be a bug inside split()
    if (what == null) return null;
    //return split(what, String.valueOf(delim));  // huh

    char chars[] = what.toCharArray();
    int splitCount = 0; //1;
    for (int i = 0; i < chars.length; i++) {
      if (chars[i] == delim) splitCount++;
    }
    // make sure that there is something in the input string
    //if (chars.length > 0) {
      // if the last char is a delimeter, get rid of it..
      //if (chars[chars.length-1] == delim) splitCount--;
      // on second thought, i don't agree with this, will disable
    //}
    if (splitCount == 0) {
      String splits[] = new String[1];
      splits[0] = new String(what);
      return splits;
    }
    //int pieceCount = splitCount + 1;
    String splits[] = new String[splitCount + 1];
    int splitIndex = 0;
    int startIndex = 0;
    for (int i = 0; i < chars.length; i++) {
      if (chars[i] == delim) {
        splits[splitIndex++] =
          new String(chars, startIndex, i-startIndex);
        startIndex = i + 1;
      }
    }
    //if (startIndex != chars.length) {
      splits[splitIndex] =
        new String(chars, startIndex, chars.length-startIndex);
    //}
    return splits;
  }


  /**
   * FIXME this is only temporary
   */
  static public String[] split(String what, String delim) {
    return what.split(delim);
  }



  //////////////////////////////////////////////////////////////

  // CASTING FUNCTIONS, INSERTED BY PREPROC


  /**
   * Convert a char to a boolean. 'T', 't', and '1' will become the
   * boolean value true, while 'F', 'f', or '0' will become false.
   */
  /*
  static final public boolean parseBoolean(char what) {
    return ((what == 't') || (what == 'T') || (what == '1'));
  }
  */

  /**
   * <p>Convert an integer to a boolean. Because of how Java handles upgrading
   * numbers, this will also cover byte and char (as they will upgrade to
   * an int without any sort of explicit cast).</p>
   * <p>The preprocessor will convert boolean(what) to parseBoolean(what).</p>
   * @return false if 0, true if any other number
   */
  static final public boolean parseBoolean(int what) {
    return (what != 0);
  }

  /*
  // removed because this makes no useful sense
  static final public boolean parseBoolean(float what) {
    return (what != 0);
  }
  */

  /**
   * Convert the string "true" or "false" to a boolean.
   * @return true if 'what' is "true" or "TRUE", false otherwise
   */
  static final public boolean parseBoolean(String what) {
    return new Boolean(what).booleanValue();
  }

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  /*
  // removed, no need to introduce strange syntax from other languages
  static final public boolean[] parseBoolean(char what[]) {
    boolean outgoing[] = new boolean[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] =
        ((what[i] == 't') || (what[i] == 'T') || (what[i] == '1'));
    }
    return outgoing;
  }
  */

  /**
   * Convert a byte array to a boolean array. Each element will be
   * evaluated identical to the integer case, where a byte equal
   * to zero will return false, and any other value will return true.
   * @return array of boolean elements
   */
  static final public boolean[] parseBoolean(byte what[]) {
    boolean outgoing[] = new boolean[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = (what[i] != 0);
    }
    return outgoing;
  }

  /**
   * Convert an int array to a boolean array. An int equal
   * to zero will return false, and any other value will return true.
   * @return array of boolean elements
   */
  static final public boolean[] parseBoolean(int what[]) {
    boolean outgoing[] = new boolean[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = (what[i] != 0);
    }
    return outgoing;
  }

  /*
  // removed, not necessary... if necessary, convert to int array first
  static final public boolean[] parseBoolean(float what[]) {
    boolean outgoing[] = new boolean[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = (what[i] != 0);
    }
    return outgoing;
  }
  */

  static final public boolean[] parseBoolean(String what[]) {
    boolean outgoing[] = new boolean[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = new Boolean(what[i]).booleanValue();
    }
    return outgoing;
  }

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  static final public byte parseByte(boolean what) {
    return what ? (byte)1 : 0;
  }

  static final public byte parseByte(char what) {
    return (byte) what;
  }

  static final public byte parseByte(int what) {
    return (byte) what;
  }

  static final public byte parseByte(float what) {
    return (byte) what;
  }

  /*
  // nixed, no precedent
  static final public byte[] parseByte(String what) {  // note: array[]
    return what.getBytes();
  }
  */

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  static final public byte[] parseByte(boolean what[]) {
    byte outgoing[] = new byte[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = what[i] ? (byte)1 : 0;
    }
    return outgoing;
  }

  static final public byte[] parseByte(char what[]) {
    byte outgoing[] = new byte[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = (byte) what[i];
    }
    return outgoing;
  }

  static final public byte[] parseByte(int what[]) {
    byte outgoing[] = new byte[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = (byte) what[i];
    }
    return outgoing;
  }

  static final public byte[] parseByte(float what[]) {
    byte outgoing[] = new byte[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = (byte) what[i];
    }
    return outgoing;
  }

  /*
  static final public byte[][] parseByte(String what[]) {  // note: array[][]
    byte outgoing[][] = new byte[what.length][];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = what[i].getBytes();
    }
    return outgoing;
  }
  */

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  /*
  static final public char parseChar(boolean what) {  // 0/1 or T/F ?
    return what ? 't' : 'f';
  }
  */

  static final public char parseChar(byte what) {
    return (char) (what & 0xff);
  }

  static final public char parseChar(int what) {
    return (char) what;
  }

  /*
  static final public char parseChar(float what) {  // nonsensical
    return (char) what;
  }

  static final public char[] parseChar(String what) {  // note: array[]
    return what.toCharArray();
  }
  */

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  /*
  static final public char[] parseChar(boolean what[]) {  // 0/1 or T/F ?
    char outgoing[] = new char[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = what[i] ? 't' : 'f';
    }
    return outgoing;
  }
  */

  static final public char[] parseChar(byte what[]) {
    char outgoing[] = new char[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = (char) (what[i] & 0xff);
    }
    return outgoing;
  }

  static final public char[] parseChar(int what[]) {
    char outgoing[] = new char[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = (char) what[i];
    }
    return outgoing;
  }

  /*
  static final public char[] parseChar(float what[]) {  // nonsensical
    char outgoing[] = new char[what.length];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = (char) what[i];
    }
    return outgoing;
  }

  static final public char[][] parseChar(String what[]) {  // note: array[][]
    char outgoing[][] = new char[what.length][];
    for (int i = 0; i < what.length; i++) {
      outgoing[i] = what[i].toCharArray();
    }
    return outgoing;
  }
  */

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  static final public int parseInt(boolean what) {
    return what ? 1 : 0;
  }

  /**
   * Note that parseInt() will un-sign a signed byte value.
   */
  static final public int parseInt(byte what) {
    return what & 0xff;
  }

  /**
   * Note that parseInt('5') is unlike String in the sense that it
   * won't return 5, but the ascii value. This is because ((int) someChar)
   * returns the ascii value, and parseInt() is just longhand for the cast.
   */
  static final public int parseInt(char what) {
    return what;
  }

  /**
   * Same as floor(), or an (int) cast.
   */
  static final public int parseInt(float what) {
    return (int) what;
  }

  /**
   * Parse a String into an int value. Returns 0 if the value is bad.
   */
  static final public int parseInt(String what) {
    return parseInt(what, 0);
  }

  /**
   * Parse a String to an int, and provide an alternate value that
   * should be used when the number is invalid.
   */
  static final public int parseInt(String what, int otherwise) {
    try {
      int offset = what.indexOf('.');
      if (offset == -1) {
        return Integer.parseInt(what);
      } else {
        return Integer.parseInt(what.substring(0, offset));
      }
    } catch (NumberFormatException e) { }
    return otherwise;
  }

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  static final public int[] parseInt(boolean what[]) {
    int list[] = new int[what.length];
    for (int i = 0; i < what.length; i++) {
      list[i] = what[i] ? 1 : 0;
    }
    return list;
  }

  static final public int[] parseInt(byte what[]) {  // note this unsigns
    int list[] = new int[what.length];
    for (int i = 0; i < what.length; i++) {
      list[i] = (what[i] & 0xff);
    }
    return list;
  }

  static final public int[] parseInt(char what[]) {
    int list[] = new int[what.length];
    for (int i = 0; i < what.length; i++) {
      list[i] = what[i];
    }
    return list;
  }

  static public int[] parseInt(float what[]) {
    int inties[] = new int[what.length];
    for (int i = 0; i < what.length; i++) {
      inties[i] = (int)what[i];
    }
    return inties;
  }

  /**
   * Make an array of int elements from an array of String objects.
   * If the String can't be parsed as a number, it will be set to zero.
   *
   * String s[] = { "1", "300", "44" };
   * int numbers[] = parseInt(s);
   *
   * numbers will contain { 1, 300, 44 }
   */
  static public int[] parseInt(String what[]) {
    return parseInt(what, 0);
  }

  /**
   * Make an array of int elements from an array of String objects.
   * If the String can't be parsed as a number, its entry in the
   * array will be set to the value of the "missing" parameter.
   *
   * String s[] = { "1", "300", "apple", "44" };
   * int numbers[] = parseInt(s, 9999);
   *
   * numbers will contain { 1, 300, 9999, 44 }
   */
  static public int[] parseInt(String what[], int missing) {
    int output[] = new int[what.length];
    for (int i = 0; i < what.length; i++) {
      try {
        output[i] = Integer.parseInt(what[i]);
      } catch (NumberFormatException e) {
        output[i] = missing;
      }
    }
    return output;
  }

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  /*
  static final public float parseFloat(boolean what) {
    return what ? 1 : 0;
  }
  */

  /**
   * Convert an int to a float value. Also handles bytes because of
   * Java's rules for upgrading values.
   */
  static final public float parseFloat(int what) {  // also handles byte
    return (float)what;
  }

  static final public float parseFloat(String what) {
    return parseFloat(what, Float.NaN);
  }

  static final public float parseFloat(String what, float otherwise) {
    try {
      return new Float(what).floatValue();
    } catch (NumberFormatException e) { }

    return otherwise;
  }

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  /*
  static final public float[] parseFloat(boolean what[]) {
    float floaties[] = new float[what.length];
    for (int i = 0; i < what.length; i++) {
      floaties[i] = what[i] ? 1 : 0;
    }
    return floaties;
  }

  static final public float[] parseFloat(char what[]) {
    float floaties[] = new float[what.length];
    for (int i = 0; i < what.length; i++) {
      floaties[i] = (char) what[i];
    }
    return floaties;
  }
  */

  static final public float[] parseByte(byte what[]) {
    float floaties[] = new float[what.length];
    for (int i = 0; i < what.length; i++) {
      floaties[i] = what[i];
    }
    return floaties;
  }

  static final public float[] parseFloat(int what[]) {
    float floaties[] = new float[what.length];
    for (int i = 0; i < what.length; i++) {
      floaties[i] = what[i];
    }
    return floaties;
  }

  static final public float[] parseFloat(String what[]) {
    return parseFloat(what, 0);
  }

  static final public float[] parseFloat(String what[], float missing) {
    float output[] = new float[what.length];
    for (int i = 0; i < what.length; i++) {
      try {
        output[i] = new Float(what[i]).floatValue();
      } catch (NumberFormatException e) {
        output[i] = missing;
      }
    }
    return output;
  }

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  static final public String str(boolean x) {
    return String.valueOf(x);
  }

  static final public String str(byte x) {
    return String.valueOf(x);
  }

  static final public String str(char x) {
    return String.valueOf(x);
  }

  static final public String str(int x) {
    return String.valueOf(x);
  }

  static final public String str(float x) {
    return String.valueOf(x);
  }

  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  static final public String[] str(boolean x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x);
    return s;
  }

  static final public String[] str(byte x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x);
    return s;
  }

  static final public String[] str(char x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x);
    return s;
  }

  static final public String[] str(int x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x);
    return s;
  }

  static final public String[] str(float x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x);
    return s;
  }


  //////////////////////////////////////////////////////////////

  // INT NUMBER FORMATTING


  /**
   * Integer number formatter.
   */
  static private NumberFormat int_nf;
  static private int int_nf_digits;
  static private boolean int_nf_commas;


  static public String[] nf(int num[], int digits) {
    String formatted[] = new String[num.length];
    for (int i = 0; i < formatted.length; i++) {
      formatted[i] = nf(num[i], digits);
    }
    return formatted;
  }


  static public String nf(int num, int digits) {
    if ((int_nf != null) &&
        (int_nf_digits == digits) &&
        !int_nf_commas) {
      return int_nf.format(num);
    }

    int_nf = NumberFormat.getInstance();
    int_nf.setGroupingUsed(false); // no commas
    int_nf_commas = false;
    int_nf.setMinimumIntegerDigits(digits);
    int_nf_digits = digits;
    return int_nf.format(num);
  }


  static public String[] nfc(int num[]) {
    String formatted[] = new String[num.length];
    for (int i = 0; i < formatted.length; i++) {
      formatted[i] = nfc(num[i]);
    }
    return formatted;
  }


  static public String nfc(int num) {
    if ((int_nf != null) &&
        (int_nf_digits == 0) &&
        int_nf_commas) {
      return int_nf.format(num);
    }

    int_nf = NumberFormat.getInstance();
    int_nf.setGroupingUsed(true);
    int_nf_commas = true;
    int_nf.setMinimumIntegerDigits(0);
    int_nf_digits = 0;
    return int_nf.format(num);
  }


  /**
   * number format signed (or space)
   * Formats a number but leaves a blank space in the front
   * when it's positive so that it can be properly aligned with
   * numbers that have a negative sign in front of them.
   */
  static public String nfs(int num, int digits) {
    return (num < 0) ? nf(num, digits) : (' ' + nf(num, digits));
  }

  static public String[] nfs(int num[], int digits) {
    String formatted[] = new String[num.length];
    for (int i = 0; i < formatted.length; i++) {
      formatted[i] = nfs(num[i], digits);
    }
    return formatted;
  }

  //

  /**
   * number format positive (or plus)
   * Formats a number, always placing a - or + sign
   * in the front when it's negative or positive.
   */
  static public String nfp(int num, int digits) {
    return (num < 0) ? nf(num, digits) : ('+' + nf(num, digits));
  }

  static public String[] nfp(int num[], int digits) {
    String formatted[] = new String[num.length];
    for (int i = 0; i < formatted.length; i++) {
      formatted[i] = nfp(num[i], digits);
    }
    return formatted;
  }



  //////////////////////////////////////////////////////////////

  // FLOAT NUMBER FORMATTING


  static private NumberFormat float_nf;
  static private int float_nf_left, float_nf_right;
  static private boolean float_nf_commas;


  static public String[] nf(float num[], int left, int right) {
    String formatted[] = new String[num.length];
    for (int i = 0; i < formatted.length; i++) {
      formatted[i] = nf(num[i], left, right);
    }
    return formatted;
  }


  static public String nf(float num, int left, int right) {
    if ((float_nf != null) &&
        (float_nf_left == left) &&
        (float_nf_right == right) &&
        !float_nf_commas) {
      return float_nf.format(num);
    }

    float_nf = NumberFormat.getInstance();
    float_nf.setGroupingUsed(false);
    float_nf_commas = false;

    if (left != 0) float_nf.setMinimumIntegerDigits(left);
    if (right != 0) {
      float_nf.setMinimumFractionDigits(right);
      float_nf.setMaximumFractionDigits(right);
    }
    float_nf_left = left;
    float_nf_right = right;
    return float_nf.format(num);
  }


  static public String[] nfc(float num[], int right) {
    String formatted[] = new String[num.length];
    for (int i = 0; i < formatted.length; i++) {
      formatted[i] = nfc(num[i], right);
    }
    return formatted;
  }


  static public String nfc(float num, int right) {
    if ((float_nf != null) &&
        (float_nf_left == 0) &&
        (float_nf_right == right) &&
        float_nf_commas) {
      return float_nf.format(num);
    }

    float_nf = NumberFormat.getInstance();
    float_nf.setGroupingUsed(true);
    float_nf_commas = true;

    if (right != 0) {
      float_nf.setMinimumFractionDigits(right);
      float_nf.setMaximumFractionDigits(right);
    }
    float_nf_left = 0;
    float_nf_right = right;
    return float_nf.format(num);
  }


  /**
   * Number formatter that takes into account whether the number
   * has a sign (positive, negative, etc) in front of it.
   */
  static public String[] nfs(float num[], int left, int right) {
    String formatted[] = new String[num.length];
    for (int i = 0; i < formatted.length; i++) {
      formatted[i] = nfs(num[i], left, right);
    }
    return formatted;
  }

  static public String nfs(float num, int left, int right) {
    return (num < 0) ? nf(num, left, right) :  (' ' + nf(num, left, right));
  }


  static public String[] nfp(float num[], int left, int right) {
    String formatted[] = new String[num.length];
    for (int i = 0; i < formatted.length; i++) {
      formatted[i] = nfp(num[i], left, right);
    }
    return formatted;
  }

  static public String nfp(float num, int left, int right) {
    return (num < 0) ? nf(num, left, right) :  ('+' + nf(num, left, right));
  }



  //////////////////////////////////////////////////////////////

  // HEX/BINARY CONVERSION


  static final public String hex(byte what) {
    return hex(what, 2);
  }

  static final public String hex(char what) {
    return hex(what, 4);
  }

  static final public String hex(int what) {
    return hex(what, 8);
  }

  static final public String hex(int what, int digits) {
    String stuff = Integer.toHexString(what).toUpperCase();

    int length = stuff.length();
    if (length > digits) {
      return stuff.substring(length - digits);

    } else if (length < digits) {
      return "00000000".substring(8 - (digits-length)) + stuff;
    }
    return stuff;
  }

  static final public int unhex(String what) {
    // has to parse as a Long so that it'll work for numbers bigger than 2^31
    return (int) (Long.parseLong(what, 16));
  }

  //

  /**
   * Returns a String that contains the binary value of a byte.
   * The returned value will always have 8 digits.
   */
  static final public String binary(byte what) {
    return binary(what, 8);
  }

  /**
   * Returns a String that contains the binary value of a char.
   * The returned value will always have 16 digits because chars
   * are two bytes long.
   */
  static final public String binary(char what) {
    return binary(what, 16);
  }

  /**
   * Returns a String that contains the binary value of an int.
   * The length depends on the size of the number itself.
   * An int can be up to 32 binary digits, but that seems like
   * overkill for almost any situation, so this function just
   * auto-size. If you want a specific number of digits (like all 32)
   * use binary(int what, int digits) to specify how many digits.
   */
  static final public String binary(int what) {
    return Integer.toBinaryString(what);
    //return binary(what, 32);
  }

  /**
   * Returns a String that contains the binary value of an int.
   * The digits parameter determines how many digits will be used.
   */
  static final public String binary(int what, int digits) {
    String stuff = Integer.toBinaryString(what);

    int length = stuff.length();
    if (length > digits) {
      return stuff.substring(length - digits);

    } else if (length < digits) {
      int offset = 32 - (digits-length);
      return "00000000000000000000000000000000".substring(offset) + stuff;
    }
    return stuff;
  }


  /**
   * Unpack a binary String into an int.
   * i.e. unbinary("00001000") would return 8.
   */
  static final public int unbinary(String what) {
    return Integer.parseInt(what, 2);
  }



  //////////////////////////////////////////////////////////////

  // COLOR FUNCTIONS

  // moved here so that they can work without
  // the graphics actually being instantiated (outside setup)


  public final int color(int gray) {
    if (g == null) {
      if (gray > 255) gray = 255; else if (gray < 0) gray = 0;
      return 0xff000000 | (gray << 16) | (gray << 8) | gray;
    }
    return g.color(gray);
  }


  public final int color(float fgray) {
    if (g == null) {
      int gray = (int) fgray;
      if (gray > 255) gray = 255; else if (gray < 0) gray = 0;
      return 0xff000000 | (gray << 16) | (gray << 8) | gray;
    }
    return g.color(fgray);
  }


  /**
   * As of 0116 this also takes color(#FF8800, alpha)
   */
  public final int color(int gray, int alpha) {
    if (g == null) {
      if (alpha > 255) alpha = 255; else if (alpha < 0) alpha = 0;
      if (gray > 255) {
        // then assume this is actually a #FF8800
        return (alpha << 24) | (gray & 0xFFFFFF);
      } else {
        //if (gray > 255) gray = 255; else if (gray < 0) gray = 0;
        return (alpha << 24) | (gray << 16) | (gray << 8) | gray;
      }
    }
    return g.color(gray, alpha);
  }


  public final int color(float fgray, float falpha) {
    if (g == null) {
      int gray = (int) fgray;
      int alpha = (int) falpha;
      if (gray > 255) gray = 255; else if (gray < 0) gray = 0;
      if (alpha > 255) alpha = 255; else if (alpha < 0) alpha = 0;
      return 0xff000000 | (gray << 16) | (gray << 8) | gray;
    }
    return g.color(fgray, falpha);
  }


  public final int color(int x, int y, int z) {
    if (g == null) {
      if (x > 255) x = 255; else if (x < 0) x = 0;
      if (y > 255) y = 255; else if (y < 0) y = 0;
      if (z > 255) z = 255; else if (z < 0) z = 0;

      return 0xff000000 | (x << 16) | (y << 8) | z;
    }
    return g.color(x, y, z);
  }


  public final int color(float x, float y, float z) {
    if (g == null) {
      if (x > 255) x = 255; else if (x < 0) x = 0;
      if (y > 255) y = 255; else if (y < 0) y = 0;
      if (z > 255) z = 255; else if (z < 0) z = 0;

      return 0xff000000 | ((int)x << 16) | ((int)y << 8) | (int)z;
    }
    return g.color(x, y, z);
  }


  public final int color(int x, int y, int z, int a) {
    if (g == null) {
      if (a > 255) a = 255; else if (a < 0) a = 0;
      if (x > 255) x = 255; else if (x < 0) x = 0;
      if (y > 255) y = 255; else if (y < 0) y = 0;
      if (z > 255) z = 255; else if (z < 0) z = 0;

      return (a << 24) | (x << 16) | (y << 8) | z;
    }
    return g.color(x, y, z, a);
  }


  public final int color(float x, float y, float z, float a) {
    if (g == null) {
      if (a > 255) a = 255; else if (a < 0) a = 0;
      if (x > 255) x = 255; else if (x < 0) x = 0;
      if (y > 255) y = 255; else if (y < 0) y = 0;
      if (z > 255) z = 255; else if (z < 0) z = 0;

      return ((int)a << 24) | ((int)x << 16) | ((int)y << 8) | (int)z;
    }
    return g.color(x, y, z, a);
  }



  //////////////////////////////////////////////////////////////

  // MAIN


  private static class WorkerVar {
    private Thread thread;
    WorkerVar(Thread t) { thread = t; }
    synchronized Thread get() { return thread; }
    synchronized void clear() { thread = null; }
  }

  /**
   * Class to help external communication run as a separate class.
   * <p/>
   * From a software engineering standpoint, using the stderr stream
   * is highly problematic because of its tendency to die or act
   * funny, especially on Windows. Threading issues can cause the
   * buffers to get full or the applet to not run properly.
   * Formerly known as the "code folder bug", this has been fixed
   * through the use of this class, however it remains a tenuous
   * situation that could perhaps break in a future JDK release.
   */
  class Worker {
    private Object value;
    private WorkerVar workerVar;

    protected synchronized Object getValue() {
      return value;
    }

    private synchronized void setValue(Object x) {
      value = x;
    }

    public Object construct() {
      try {
        int anything = System.in.read();
        if (anything == EXTERNAL_STOP) {

          // adding this for 0073.. need to stop libraries
          // when the stop button is hit.
          PApplet.this.stop();
          finished = true;
        }
      } catch (IOException e) {
        finished = true;
      }
      try {
        Thread.sleep(250);
        //Thread.sleep(100);  // kick up latency for 0075?
      } catch (InterruptedException e) { }
      return null;
    }

    // removing this from SwingWorker
    //public void finished() { }

    public void interrupt() {
      Thread t = workerVar.get();
      if (t != null) {
        t.interrupt();
      }
      workerVar.clear();
    }

    public Object get() {
      while (true) {
        Thread t = workerVar.get();
        if (t == null) {
          return getValue();
        }
        try {
          t.join();
        } catch (InterruptedException e) {
          Thread.currentThread().interrupt(); // propagate
          return null;
        }
      }
    }

    public Worker() {
      // removing this from SwingWorker
      //final Runnable doFinished = new Runnable() {
      //    public void run() { finished(); }
      //  };

      Runnable doConstruct = new Runnable() {
          public void run() {
            try {
              setValue(construct());

            } finally {
              workerVar.clear();
            }
            // removing this from SwingWorker to avoid swing
            //javax.swing.SwingUtilities.invokeLater(doFinished);
          }
        };

      Thread t = new Thread(doConstruct);
      workerVar = new WorkerVar(t);
    }

    public void start() {
      Thread t = workerVar.get();
      if (t != null) t.start();
    }
  }


  /**
   * Set this sketch to communicate its state back to the PDE.
   * <p/>
   * This uses the stderr stream to write positions of the window
   * (so that it will be saved by the PDE for the next run) and
   * notify on quit. See more notes in the Worker class.
   */
  public void setupExternalMessages() {  //Frame parentFrame) {
    //final Worker worker = new Worker();

    //parentFrame.addComponentListener(new ComponentAdapter() {
    frame.addComponentListener(new ComponentAdapter() {
        public void componentMoved(ComponentEvent e) {
          Point where = ((Frame) e.getSource()).getLocation();
          System.err.println(PApplet.EXTERNAL_MOVE + " " +
                             where.x + " " + where.y);
          System.err.flush();  // doesn't seem to help or hurt
        }
      });

    //parentFrame.addWindowListener(new WindowAdapter() {
    frame.addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
          System.err.println(PApplet.EXTERNAL_QUIT);
          System.err.flush();  // important
          System.exit(0);
        }
      });
  }


  /**
   * Set up a listener that will fire proper component resize events
   * in cases where frame.setResizable(true) is called.
   */
  public void setupFrameResizeListener() {
    frame.addComponentListener(new ComponentAdapter() {

        public void componentResized(ComponentEvent e) {
          // Ignore bad resize events fired during setup to fix
          // http://dev.processing.org/bugs/show_bug.cgi?id=341
          // This should also fix the blank screen on Linux bug
          // http://dev.processing.org/bugs/show_bug.cgi?id=282
          if (frame.isResizable()) {
            // might be multiple resize calls before visible (i.e. first
            // when pack() is called, then when it's resized for use).
            // ignore them because it's not the user resizing things.
            Frame farm = (Frame) e.getComponent();
            if (farm.isVisible()) {
              Insets insets = farm.getInsets();
              Dimension windowSize = farm.getSize();
              int usableW = windowSize.width - insets.left - insets.right;
              int usableH = windowSize.height - insets.top - insets.bottom;

              // the ComponentListener in PApplet will handle calling size()
              setBounds(insets.left, insets.top, usableW, usableH);
            }
          }
        }
      });
  }


  /**
   * main() method for running this class from the command line.
   * <P>
   * <B>The options shown here are not yet finalized and will be
   * changing over the next several releases.</B>
   * <P>
   * The simplest way to turn and applet into an application is to
   * add the following code to your program:
   * <PRE>static public void main(String args[]) {
   *   PApplet.main(new String[] { "YourSketchName" });
   * }</PRE>
   * This will properly launch your applet from a double-clickable
   * .jar or from the command line.
   * <PRE>
   * Parameters useful for launching or also used by the PDE:
   *
   * --location=x,y        upper-lefthand corner of where the applet
   *                       should appear on screen. if not used,
   *                       the default is to center on the main screen.
   *
   * --present             put the applet into full screen presentation
   *                       mode. requires java 1.4 or later.
   *
   * --hide-stop           use to hide the stop button in situations where
   *                       you don't want to allow users to exit. also
   *                       see the FAQ on information for capturing the ESC
   *                       key when running in presentation mode.
   *
   * --stop-color          color of the 'stop' text used to quit an
   *                       sketch when it's in present mode.
   *
   * --bgcolor=#xxxxxx     background color of the window.
   *
   * --sketch-path         location of where to save files from functions
   *                       like saveStrings() or saveFrame(). defaults to
   *                       the folder that the java application was
   *                       launched from, which means if this isn't set by
   *                       the pde, everything goes into the same folder
   *                       as processing.exe.
   *
   * --display=n           set what display should be used by this applet.
   *                       displays are numbered starting from 1.
   *
   *
   * Parameters used by Processing when running via the PDE
   *
   * --external            set when the applet is being used by the PDE
   *
   * --editor-location=x,y position of the upper-lefthand corner of the
   *                       editor window, for placement of applet window
   * </PRE>
   */
  static public void main(String args[]) {
    if (args.length < 1) {
      System.err.println("Usage: PApplet <appletname>");
      System.err.println("For additional options, " +
                         "see the javadoc for PApplet");
      System.exit(1);
    }

    try {
      // true if this sketch is being run by the PDE
      boolean external = false;
      int location[] = null;
      int editorLocation[] = null;

      String name = null;
      boolean present = false;
      Color backgroundColor = Color.black; //BLACK;
      Color stopColor = Color.gray; //GRAY;
      GraphicsDevice displayDevice = null;
      boolean hideStop = false;

      String param = null, value = null;

      // try to get the user folder. if running under java web start,
      // this may cause a security exception if the code is not signed.
      // http://processing.org/discourse/yabb_beta/YaBB.cgi?board=Integrate;action=display;num=1159386274
      String folder = null;
      try {
        folder = System.getProperty("user.dir");
      } catch (Exception e) { }

      int argIndex = 0;
      while (argIndex < args.length) {
        int equals = args[argIndex].indexOf('=');
        if (equals != -1) {
          param = args[argIndex].substring(0, equals);
          value = args[argIndex].substring(equals + 1);

          if (param.equals(ARGS_EDITOR_LOCATION)) {
            external = true;
            editorLocation = parseInt(split(value, ','));

          } else if (param.equals(ARGS_DISPLAY)) {
            int deviceIndex = Integer.parseInt(value) - 1;

            //DisplayMode dm = device.getDisplayMode();
            //if ((dm.getWidth() == 1024) && (dm.getHeight() == 768)) {

            GraphicsEnvironment environment =
              GraphicsEnvironment.getLocalGraphicsEnvironment();
            GraphicsDevice devices[] = environment.getScreenDevices();
            if ((deviceIndex >= 0) && (deviceIndex < devices.length)) {
              displayDevice = devices[deviceIndex];
            } else {
              System.err.println("Display " + value + " does not exist, " +
                                 "using the default display instead.");
            }

          } else if (param.equals(ARGS_BGCOLOR)) {
            if (value.charAt(0) == '#') value = value.substring(1);
            backgroundColor = new Color(Integer.parseInt(value, 16));

          } else if (param.equals(ARGS_STOP_COLOR)) {
            if (value.charAt(0) == '#') value = value.substring(1);
            stopColor = new Color(Integer.parseInt(value, 16));

          } else if (param.equals(ARGS_SKETCH_FOLDER)) {
            folder = value;

          } else if (param.equals(ARGS_LOCATION)) {
            location = parseInt(split(value, ','));
          }

        } else {
          if (args[argIndex].equals(ARGS_PRESENT)) {
            present = true;

          } else if (args[argIndex].equals(ARGS_HIDE_STOP)) {
            hideStop = true;

          } else if (args[argIndex].equals(ARGS_EXTERNAL)) {
            external = true;

          } else {
            name = args[argIndex];
            break;
          }
        }
        argIndex++;
      }

      if (displayDevice == null) {
        GraphicsEnvironment environment =
          GraphicsEnvironment.getLocalGraphicsEnvironment();
        displayDevice = environment.getDefaultScreenDevice();
      }

      Frame frame = new Frame(displayDevice.getDefaultConfiguration());
      /*
      Frame frame = null;
      if (displayDevice != null) {
        frame = new Frame(displayDevice.getDefaultConfiguration());
      } else {
        frame = new Frame();
      }
      */

      Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();

      // remove the grow box by default
      // users who want it back can call frame.setResizable(true)
      frame.setResizable(false);

      Class c = Class.forName(name);
      PApplet applet = (PApplet) c.newInstance();

      // these are needed before init/start
      applet.frame = frame;
      applet.sketchPath = folder;
      applet.args = PApplet.subset(args, 1);

      applet.init();

      // wait until the applet has figured out its width
      // hoping that this won't hang if the applet has an exception
      while (applet.defaultSize && !applet.finished) {
        try {
          Thread.sleep(5);

        } catch (InterruptedException e) { }
      }

      if (present) {
        frame.setUndecorated(true);
        frame.setBackground(backgroundColor);
        displayDevice.setFullScreenWindow(frame);

        frame.add(applet);
        Dimension fullscreen = frame.getSize();
        applet.setBounds((fullscreen.width - applet.width) / 2,
                         (fullscreen.height - applet.height) / 2,
                         applet.width, applet.height);

        if (!hideStop) {
          Label label = new Label("stop");
          label.setForeground(stopColor);
          label.addMouseListener(new MouseAdapter() {
              public void mousePressed(MouseEvent e) {
                System.exit(0);
              }
            });
          frame.add(label);

          Dimension labelSize = label.getPreferredSize();
          // sometimes shows up truncated on mac
          //System.out.println("label width is " + labelSize.width);
          labelSize = new Dimension(100, labelSize.height);
          label.setSize(labelSize);
          label.setLocation(20, fullscreen.height - labelSize.height - 20);
        }

        // not always running externally when in present mode
        if (external) {
          applet.setupExternalMessages();
        }

      } else {  // if not presenting
        // can't do pack earlier cuz present mode don't like it
        // (can't go full screen with a frame after calling pack)
        frame.pack();  // get insets. get more.
        Insets insets = frame.getInsets();

        int windowW = Math.max(applet.width, MIN_WINDOW_WIDTH) +
          insets.left + insets.right;
        int windowH = Math.max(applet.height, MIN_WINDOW_HEIGHT) +
          insets.top + insets.bottom;

        frame.setSize(windowW, windowH);

        if (location != null) {
          // a specific location was received from PdeRuntime
          // (applet has been run more than once, user placed window)
          frame.setLocation(location[0], location[1]);

        } else if (external) {
          int locationX = editorLocation[0] - 20;
          int locationY = editorLocation[1];

          if (locationX - windowW > 10) {
            // if it fits to the left of the window
            frame.setLocation(locationX - windowW, locationY);

          } else {  // doesn't fit
            // if it fits inside the editor window,
            // offset slightly from upper lefthand corner
            // so that it's plunked inside the text area
            locationX = editorLocation[0] + 66;
            locationY = editorLocation[1] + 66;

            if ((locationX + windowW > screen.width - 33) ||
                (locationY + windowH > screen.height - 33)) {
              // otherwise center on screen
              locationX = (screen.width - windowW) / 2;
              locationY = (screen.height - windowH) / 2;
            }
            frame.setLocation(locationX, locationY);
          }
        } else {  // just center on screen
          frame.setLocation((screen.width - applet.width) / 2,
                            (screen.height - applet.height) / 2);
        }

        frame.setLayout(null);
        frame.add(applet);

        if (backgroundColor == Color.black) {  //BLACK) {
          // this means no bg color unless specified
          backgroundColor = SystemColor.control;
        }
        frame.setBackground(backgroundColor);

        int usableWindowH = windowH - insets.top - insets.bottom;
        applet.setBounds((windowW - applet.width)/2,
                         insets.top + (usableWindowH - applet.height)/2,
                         applet.width, applet.height);

        if (external) {
          applet.setupExternalMessages();

        } else {  // !external
          frame.addWindowListener(new WindowAdapter() {
              public void windowClosing(WindowEvent e) {
                System.exit(0);
              }
            });
        }

        // handle frame resizing events
        applet.setupFrameResizeListener();

        // all set for rockin
        if (applet.displayable()) {
          frame.setVisible(true);
        }
      }

      //System.out.println("showing frame");
      //System.out.println("applet requesting focus");
      applet.requestFocus(); // ask for keydowns
      //System.out.println("exiting main()");

    } catch (Exception e) {
      e.printStackTrace();
      System.exit(1);
    }
  }


  //////////////////////////////////////////////////////////////


  public PGraphics beginRecord(String renderer, String filename) {
    filename = insertFrame(filename);
    //filename = savePath(filename);
    PGraphics rec = createGraphics(width, height, renderer, filename);
    beginRecord(rec);
    return rec;
  }


  public void beginRecord(PGraphics recorder) {
    this.recorder = recorder;
    recorder.beginDraw();
  }


  public void endRecord() {
    //println("endRecord()");
    //if (!recorderNull) {
    if (recorder != null) {
      //recorder.endRecord();
      recorder.endDraw();
      recorder.dispose();
      recorder = null;
    }
  }


  public PGraphics beginRaw(String renderer, String filename) {
    filename = insertFrame(filename);
    //filename = savePath(filename);  // ensure an absolute path
    PGraphics rec = createGraphics(width, height, renderer, filename);
    //g.recordRaw(rec);
    g.beginRaw(rec);
    return rec;
  }


  /**
   * Check a string for #### signs to see if the frame number should be
   * inserted. Used for functions like saveFrame() and beginRecord() to
   * replace the # marks with the frame number. If only one # is used,
   * it will be ignored, under the assumption that it's probably not
   * intended to be the frame number.
   */
  public String insertFrame(String what) {
    int first = what.indexOf('#');
    int last = what.lastIndexOf('#');

    if ((first != -1) && (last - first > 0)) {
      String prefix = what.substring(0, first);
      int count = last - first + 1;
      String suffix = what.substring(last + 1);
      return prefix + nf(frameCount, count) + suffix;
    }
    return what;  // no change
  }


  //////////////////////////////////////////////////////////////


  /**
   * Override the g.pixels[] function to set the pixels[] array
   * that's part of the PApplet object. Allows the use of
   * pixels[] in the code, rather than g.pixels[].
   */
  public void loadPixels() {
    g.loadPixels();
    pixels = g.pixels;
  }


  //////////////////////////////////////////////////////////////

  // everything below this line is automatically generated. no touch.
  // public functions for processing.core


  public void imageMode(int mode) {
    if (recorder != null) recorder.imageMode(mode);
    g.imageMode(mode);
  }


  public void smooth() {
    if (recorder != null) recorder.smooth();
    g.smooth();
  }


  public void noSmooth() {
    if (recorder != null) recorder.noSmooth();
    g.noSmooth();
  }


  public void updatePixels() {
    if (recorder != null) recorder.updatePixels();
    g.updatePixels();
  }


  public void updatePixels(int x1, int y1, int x2, int y2) {
    if (recorder != null) recorder.updatePixels(x1, y1, x2, y2);
    g.updatePixels(x1, y1, x2, y2);
  }


  public int get(int x, int y) {
    return g.get(x, y);
  }


  public PImage get(int x, int y, int w, int h) {
    return g.get(x, y, w, h);
  }


  public PImage get() {
    return g.get();
  }


  public void set(int x, int y, int c) {
    if (recorder != null) recorder.set(x, y, c);
    g.set(x, y, c);
  }


  public void set(int dx, int dy, PImage src) {
    if (recorder != null) recorder.set(dx, dy, src);
    g.set(dx, dy, src);
  }


  public void mask(int alpha[]) {
    if (recorder != null) recorder.mask(alpha);
    g.mask(alpha);
  }


  public void mask(PImage alpha) {
    if (recorder != null) recorder.mask(alpha);
    g.mask(alpha);
  }


  public void filter(int kind) {
    if (recorder != null) recorder.filter(kind);
    g.filter(kind);
  }


  public void filter(int kind, float param) {
    if (recorder != null) recorder.filter(kind, param);
    g.filter(kind, param);
  }


  public void copy(int sx1, int sy1, int sx2, int sy2,
                   int dx1, int dy1, int dx2, int dy2) {
    if (recorder != null) recorder.copy(sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2);
    g.copy(sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2);
  }


  public void copy(PImage src,
                   int sx1, int sy1, int sx2, int sy2,
                   int dx1, int dy1, int dx2, int dy2) {
    if (recorder != null) recorder.copy(src, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2);
    g.copy(src, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2);
  }


  static public int blendColor(int c1, int c2, int mode) {
    return PGraphics.blendColor(c1, c2, mode);
  }


  public void blend(int sx1, int sy1, int sx2, int sy2,
                    int dx1, int dy1, int dx2, int dy2, int mode) {
    if (recorder != null) recorder.blend(sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2, mode);
    g.blend(sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2, mode);
  }


  public void blend(PImage src,
                    int sx1, int sy1, int sx2, int sy2,
                    int dx1, int dy1, int dx2, int dy2, int mode) {
    if (recorder != null) recorder.blend(src, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2, mode);
    g.blend(src, sx1, sy1, sx2, sy2, dx1, dy1, dx2, dy2, mode);
  }


  public void hint(int which) {
    if (recorder != null) recorder.hint(which);
    g.hint(which);
  }


  public void noHint(int which) {
    if (recorder != null) recorder.noHint(which);
    g.noHint(which);
  }


  public void beginShape() {
    if (recorder != null) recorder.beginShape();
    g.beginShape();
  }


  public void beginShape(int kind) {
    if (recorder != null) recorder.beginShape(kind);
    g.beginShape(kind);
  }


  public void normal(float nx, float ny, float nz) {
    if (recorder != null) recorder.normal(nx, ny, nz);
    g.normal(nx, ny, nz);
  }


  public void textureMode(int mode) {
    if (recorder != null) recorder.textureMode(mode);
    g.textureMode(mode);
  }


  public void texture(PImage image) {
    if (recorder != null) recorder.texture(image);
    g.texture(image);
  }


  public void vertex(float x, float y) {
    if (recorder != null) recorder.vertex(x, y);
    g.vertex(x, y);
  }


  public void vertex(float x, float y, float z) {
    if (recorder != null) recorder.vertex(x, y, z);
    g.vertex(x, y, z);
  }


  public void vertex(float x, float y, float u, float v) {
    if (recorder != null) recorder.vertex(x, y, u, v);
    g.vertex(x, y, u, v);
  }


  public void vertex(float x, float y, float z, float u, float v) {
    if (recorder != null) recorder.vertex(x, y, z, u, v);
    g.vertex(x, y, z, u, v);
  }


  public void bezierVertex(float x2, float y2,
                           float x3, float y3,
                           float x4, float y4) {
    if (recorder != null) recorder.bezierVertex(x2, y2, x3, y3, x4, y4);
    g.bezierVertex(x2, y2, x3, y3, x4, y4);
  }


  public void bezierVertex(float x2, float y2, float z2,
                           float x3, float y3, float z3,
                           float x4, float y4, float z4) {
    if (recorder != null) recorder.bezierVertex(x2, y2, z2, x3, y3, z3, x4, y4, z4);
    g.bezierVertex(x2, y2, z2, x3, y3, z3, x4, y4, z4);
  }


  public void curveVertex(float x, float y) {
    if (recorder != null) recorder.curveVertex(x, y);
    g.curveVertex(x, y);
  }


  public void curveVertex(float x, float y, float z) {
    if (recorder != null) recorder.curveVertex(x, y, z);
    g.curveVertex(x, y, z);
  }


  public void breakShape() {
    if (recorder != null) recorder.breakShape();
    g.breakShape();
  }


  public final void endShape() {
    if (recorder != null) recorder.endShape();
    g.endShape();
  }


  public void endShape(int mode) {
    if (recorder != null) recorder.endShape(mode);
    g.endShape(mode);
  }


  public void point(float x, float y) {
    if (recorder != null) recorder.point(x, y);
    g.point(x, y);
  }


  public void point(float x, float y, float z) {
    if (recorder != null) recorder.point(x, y, z);
    g.point(x, y, z);
  }


  public void line(float x1, float y1, float x2, float y2) {
    if (recorder != null) recorder.line(x1, y1, x2, y2);
    g.line(x1, y1, x2, y2);
  }


  public void line(float x1, float y1, float z1,
                   float x2, float y2, float z2) {
    if (recorder != null) recorder.line(x1, y1, z1, x2, y2, z2);
    g.line(x1, y1, z1, x2, y2, z2);
  }


  public void triangle(float x1, float y1, float x2, float y2,
                       float x3, float y3) {
    if (recorder != null) recorder.triangle(x1, y1, x2, y2, x3, y3);
    g.triangle(x1, y1, x2, y2, x3, y3);
  }


  public void quad(float x1, float y1, float x2, float y2,
                   float x3, float y3, float x4, float y4) {
    if (recorder != null) recorder.quad(x1, y1, x2, y2, x3, y3, x4, y4);
    g.quad(x1, y1, x2, y2, x3, y3, x4, y4);
  }


  public void rectMode(int mode) {
    if (recorder != null) recorder.rectMode(mode);
    g.rectMode(mode);
  }


  public void rect(float x1, float y1, float x2, float y2) {
    if (recorder != null) recorder.rect(x1, y1, x2, y2);
    g.rect(x1, y1, x2, y2);
  }


  public void ellipseMode(int mode) {
    if (recorder != null) recorder.ellipseMode(mode);
    g.ellipseMode(mode);
  }


  public void ellipse(float a, float b, float c, float d) {
    if (recorder != null) recorder.ellipse(a, b, c, d);
    g.ellipse(a, b, c, d);
  }


  public void arc(float a, float b, float c, float d,
                  float start, float stop) {
    if (recorder != null) recorder.arc(a, b, c, d, start, stop);
    g.arc(a, b, c, d, start, stop);
  }


  public void box(float size) {
    if (recorder != null) recorder.box(size);
    g.box(size);
  }


  public void box(float w, float h, float d) {
    if (recorder != null) recorder.box(w, h, d);
    g.box(w, h, d);
  }


  public void sphereDetail(int res) {
    if (recorder != null) recorder.sphereDetail(res);
    g.sphereDetail(res);
  }


  public void sphere(float r) {
    if (recorder != null) recorder.sphere(r);
    g.sphere(r);
  }


  public float bezierPoint(float a, float b, float c, float d, float t) {
    return g.bezierPoint(a, b, c, d, t);
  }


  public float bezierTangent(float a, float b, float c, float d, float t) {
    return g.bezierTangent(a, b, c, d, t);
  }


  public void bezierDetail(int detail) {
    if (recorder != null) recorder.bezierDetail(detail);
    g.bezierDetail(detail);
  }


  public void bezier(float x1, float y1,
                     float x2, float y2,
                     float x3, float y3,
                     float x4, float y4) {
    if (recorder != null) recorder.bezier(x1, y1, x2, y2, x3, y3, x4, y4);
    g.bezier(x1, y1, x2, y2, x3, y3, x4, y4);
  }


  public void bezier(float x1, float y1, float z1,
                     float x2, float y2, float z2,
                     float x3, float y3, float z3,
                     float x4, float y4, float z4) {
    if (recorder != null) recorder.bezier(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4);
    g.bezier(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4);
  }


  public float curvePoint(float a, float b, float c, float d, float t) {
    return g.curvePoint(a, b, c, d, t);
  }


  public float curveTangent(float a, float b, float c, float d,
                            float t) {
    return g.curveTangent(a, b, c, d, t);
  }


  public void curveDetail(int detail) {
    if (recorder != null) recorder.curveDetail(detail);
    g.curveDetail(detail);
  }


  public void curveTightness(float tightness) {
    if (recorder != null) recorder.curveTightness(tightness);
    g.curveTightness(tightness);
  }


  public void curve(float x1, float y1,
                    float x2, float y2,
                    float x3, float y3,
                    float x4, float y4) {
    if (recorder != null) recorder.curve(x1, y1, x2, y2, x3, y3, x4, y4);
    g.curve(x1, y1, x2, y2, x3, y3, x4, y4);
  }


  public void curve(float x1, float y1, float z1,
                    float x2, float y2, float z2,
                    float x3, float y3, float z3,
                    float x4, float y4, float z4) {
    if (recorder != null) recorder.curve(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4);
    g.curve(x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4);
  }


  public void image(PImage image, float x, float y) {
    if (recorder != null) recorder.image(image, x, y);
    g.image(image, x, y);
  }


  public void image(PImage image,
                    float x, float y, float c, float d) {
    if (recorder != null) recorder.image(image, x, y, c, d);
    g.image(image, x, y, c, d);
  }


  public void image(PImage image,
                    float a, float b, float c, float d,
                    int u1, int v1, int u2, int v2) {
    if (recorder != null) recorder.image(image, a, b, c, d, u1, v1, u2, v2);
    g.image(image, a, b, c, d, u1, v1, u2, v2);
  }


  public void textAlign(int align) {
    if (recorder != null) recorder.textAlign(align);
    g.textAlign(align);
  }


  public void textAlign(int alignX, int alignY) {
    if (recorder != null) recorder.textAlign(alignX, alignY);
    g.textAlign(alignX, alignY);
  }


  public float textAscent() {
    return g.textAscent();
  }


  public float textDescent() {
    return g.textDescent();
  }


  public void textFont(PFont which) {
    if (recorder != null) recorder.textFont(which);
    g.textFont(which);
  }


  public void textFont(PFont which, float size) {
    if (recorder != null) recorder.textFont(which, size);
    g.textFont(which, size);
  }


  public void textLeading(float leading) {
    if (recorder != null) recorder.textLeading(leading);
    g.textLeading(leading);
  }


  public void textMode(int mode) {
    if (recorder != null) recorder.textMode(mode);
    g.textMode(mode);
  }


  public void textSize(float size) {
    if (recorder != null) recorder.textSize(size);
    g.textSize(size);
  }


  public float textWidth(char c) {
    return g.textWidth(c);
  }


  public float textWidth(String str) {
    return g.textWidth(str);
  }


  public void text(char c) {
    if (recorder != null) recorder.text(c);
    g.text(c);
  }


  public void text(char c, float x, float y) {
    if (recorder != null) recorder.text(c, x, y);
    g.text(c, x, y);
  }


  public void text(char c, float x, float y, float z) {
    if (recorder != null) recorder.text(c, x, y, z);
    g.text(c, x, y, z);
  }


  public void text(String str) {
    if (recorder != null) recorder.text(str);
    g.text(str);
  }


  public void text(String str, float x, float y) {
    if (recorder != null) recorder.text(str, x, y);
    g.text(str, x, y);
  }


  public void text(String str, float x, float y, float z) {
    if (recorder != null) recorder.text(str, x, y, z);
    g.text(str, x, y, z);
  }


  public void text(String str, float x1, float y1, float x2, float y2) {
    if (recorder != null) recorder.text(str, x1, y1, x2, y2);
    g.text(str, x1, y1, x2, y2);
  }


  public void text(String s, float x1, float y1, float x2, float y2, float z) {
    if (recorder != null) recorder.text(s, x1, y1, x2, y2, z);
    g.text(s, x1, y1, x2, y2, z);
  }


  public void text(int num, float x, float y) {
    if (recorder != null) recorder.text(num, x, y);
    g.text(num, x, y);
  }


  public void text(int num, float x, float y, float z) {
    if (recorder != null) recorder.text(num, x, y, z);
    g.text(num, x, y, z);
  }


  public void text(float num, float x, float y) {
    if (recorder != null) recorder.text(num, x, y);
    g.text(num, x, y);
  }


  public void text(float num, float x, float y, float z) {
    if (recorder != null) recorder.text(num, x, y, z);
    g.text(num, x, y, z);
  }


  public void translate(float tx, float ty) {
    if (recorder != null) recorder.translate(tx, ty);
    g.translate(tx, ty);
  }


  public void translate(float tx, float ty, float tz) {
    if (recorder != null) recorder.translate(tx, ty, tz);
    g.translate(tx, ty, tz);
  }


  public void rotate(float angle) {
    if (recorder != null) recorder.rotate(angle);
    g.rotate(angle);
  }


  public void rotateX(float angle) {
    if (recorder != null) recorder.rotateX(angle);
    g.rotateX(angle);
  }


  public void rotateY(float angle) {
    if (recorder != null) recorder.rotateY(angle);
    g.rotateY(angle);
  }


  public void rotateZ(float angle) {
    if (recorder != null) recorder.rotateZ(angle);
    g.rotateZ(angle);
  }


  public void rotate(float angle, float vx, float vy, float vz) {
    if (recorder != null) recorder.rotate(angle, vx, vy, vz);
    g.rotate(angle, vx, vy, vz);
  }


  public void scale(float s) {
    if (recorder != null) recorder.scale(s);
    g.scale(s);
  }


  public void scale(float sx, float sy) {
    if (recorder != null) recorder.scale(sx, sy);
    g.scale(sx, sy);
  }


  public void scale(float x, float y, float z) {
    if (recorder != null) recorder.scale(x, y, z);
    g.scale(x, y, z);
  }


  public void pushMatrix() {
    if (recorder != null) recorder.pushMatrix();
    g.pushMatrix();
  }


  public void popMatrix() {
    if (recorder != null) recorder.popMatrix();
    g.popMatrix();
  }


  public void resetMatrix() {
    if (recorder != null) recorder.resetMatrix();
    g.resetMatrix();
  }


  public void applyMatrix(float n00, float n01, float n02,
                          float n10, float n11, float n12) {
    if (recorder != null) recorder.applyMatrix(n00, n01, n02, n10, n11, n12);
    g.applyMatrix(n00, n01, n02, n10, n11, n12);
  }


  public void applyMatrix(float n00, float n01, float n02, float n03,
                          float n10, float n11, float n12, float n13,
                          float n20, float n21, float n22, float n23,
                          float n30, float n31, float n32, float n33) {
    if (recorder != null) recorder.applyMatrix(n00, n01, n02, n03, n10, n11, n12, n13, n20, n21, n22, n23, n30, n31, n32, n33);
    g.applyMatrix(n00, n01, n02, n03, n10, n11, n12, n13, n20, n21, n22, n23, n30, n31, n32, n33);
  }


  public void loadMatrix() {
    if (recorder != null) recorder.loadMatrix();
    g.loadMatrix();
  }


  public void printMatrix() {
    if (recorder != null) recorder.printMatrix();
    g.printMatrix();
  }


  public void beginCamera() {
    if (recorder != null) recorder.beginCamera();
    g.beginCamera();
  }


  public void endCamera() {
    if (recorder != null) recorder.endCamera();
    g.endCamera();
  }


  public void camera() {
    if (recorder != null) recorder.camera();
    g.camera();
  }


  public void camera(float eyeX, float eyeY, float eyeZ,
                     float centerX, float centerY, float centerZ,
                     float upX, float upY, float upZ) {
    if (recorder != null) recorder.camera(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
    g.camera(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
  }


  public void printCamera() {
    if (recorder != null) recorder.printCamera();
    g.printCamera();
  }


  public void ortho() {
    if (recorder != null) recorder.ortho();
    g.ortho();
  }


  public void ortho(float left, float right,
                    float bottom, float top,
                    float near, float far) {
    if (recorder != null) recorder.ortho(left, right, bottom, top, near, far);
    g.ortho(left, right, bottom, top, near, far);
  }


  public void perspective() {
    if (recorder != null) recorder.perspective();
    g.perspective();
  }


  public void perspective(float fovy, float aspect, float zNear, float zFar) {
    if (recorder != null) recorder.perspective(fovy, aspect, zNear, zFar);
    g.perspective(fovy, aspect, zNear, zFar);
  }


  public void frustum(float left, float right, float bottom,
                      float top, float znear, float zfar) {
    if (recorder != null) recorder.frustum(left, right, bottom, top, znear, zfar);
    g.frustum(left, right, bottom, top, znear, zfar);
  }


  public void printProjection() {
    if (recorder != null) recorder.printProjection();
    g.printProjection();
  }


  public float screenX(float x, float y) {
    return g.screenX(x, y);
  }


  public float screenY(float x, float y) {
    return g.screenY(x, y);
  }


  public float screenX(float x, float y, float z) {
    return g.screenX(x, y, z);
  }


  public float screenY(float x, float y, float z) {
    return g.screenY(x, y, z);
  }


  public float screenZ(float x, float y, float z) {
    return g.screenZ(x, y, z);
  }


  public float modelX(float x, float y, float z) {
    return g.modelX(x, y, z);
  }


  public float modelY(float x, float y, float z) {
    return g.modelY(x, y, z);
  }


  public float modelZ(float x, float y, float z) {
    return g.modelZ(x, y, z);
  }


  public void colorMode(int mode) {
    if (recorder != null) recorder.colorMode(mode);
    g.colorMode(mode);
  }


  public void colorMode(int mode, float max) {
    if (recorder != null) recorder.colorMode(mode, max);
    g.colorMode(mode, max);
  }


  public void colorMode(int mode,
                        float maxX, float maxY, float maxZ) {
    if (recorder != null) recorder.colorMode(mode, maxX, maxY, maxZ);
    g.colorMode(mode, maxX, maxY, maxZ);
  }


  public void colorMode(int mode,
                        float maxX, float maxY, float maxZ, float maxA) {
    if (recorder != null) recorder.colorMode(mode, maxX, maxY, maxZ, maxA);
    g.colorMode(mode, maxX, maxY, maxZ, maxA);
  }


  public void strokeWeight(float weight) {
    if (recorder != null) recorder.strokeWeight(weight);
    g.strokeWeight(weight);
  }


  public void strokeJoin(int join) {
    if (recorder != null) recorder.strokeJoin(join);
    g.strokeJoin(join);
  }


  public void strokeCap(int cap) {
    if (recorder != null) recorder.strokeCap(cap);
    g.strokeCap(cap);
  }


  public void noStroke() {
    if (recorder != null) recorder.noStroke();
    g.noStroke();
  }


  public void stroke(int rgb) {
    if (recorder != null) recorder.stroke(rgb);
    g.stroke(rgb);
  }


  public void stroke(int rgb, float alpha) {
    if (recorder != null) recorder.stroke(rgb, alpha);
    g.stroke(rgb, alpha);
  }


  public void stroke(float gray) {
    if (recorder != null) recorder.stroke(gray);
    g.stroke(gray);
  }


  public void stroke(float gray, float alpha) {
    if (recorder != null) recorder.stroke(gray, alpha);
    g.stroke(gray, alpha);
  }


  public void stroke(float x, float y, float z) {
    if (recorder != null) recorder.stroke(x, y, z);
    g.stroke(x, y, z);
  }


  public void stroke(float x, float y, float z, float a) {
    if (recorder != null) recorder.stroke(x, y, z, a);
    g.stroke(x, y, z, a);
  }


  public void noTint() {
    if (recorder != null) recorder.noTint();
    g.noTint();
  }


  public void tint(int rgb) {
    if (recorder != null) recorder.tint(rgb);
    g.tint(rgb);
  }


  public void tint(int rgb, float alpha) {
    if (recorder != null) recorder.tint(rgb, alpha);
    g.tint(rgb, alpha);
  }


  public void tint(float gray) {
    if (recorder != null) recorder.tint(gray);
    g.tint(gray);
  }


  public void tint(float gray, float alpha) {
    if (recorder != null) recorder.tint(gray, alpha);
    g.tint(gray, alpha);
  }


  public void tint(float x, float y, float z) {
    if (recorder != null) recorder.tint(x, y, z);
    g.tint(x, y, z);
  }


  public void tint(float x, float y, float z, float a) {
    if (recorder != null) recorder.tint(x, y, z, a);
    g.tint(x, y, z, a);
  }


  public void noFill() {
    if (recorder != null) recorder.noFill();
    g.noFill();
  }


  public void fill(int rgb) {
    if (recorder != null) recorder.fill(rgb);
    g.fill(rgb);
  }


  public void fill(int rgb, float alpha) {
    if (recorder != null) recorder.fill(rgb, alpha);
    g.fill(rgb, alpha);
  }


  public void fill(float gray) {
    if (recorder != null) recorder.fill(gray);
    g.fill(gray);
  }


  public void fill(float gray, float alpha) {
    if (recorder != null) recorder.fill(gray, alpha);
    g.fill(gray, alpha);
  }


  public void fill(float x, float y, float z) {
    if (recorder != null) recorder.fill(x, y, z);
    g.fill(x, y, z);
  }


  public void fill(float x, float y, float z, float a) {
    if (recorder != null) recorder.fill(x, y, z, a);
    g.fill(x, y, z, a);
  }


  public void ambient(int rgb) {
    if (recorder != null) recorder.ambient(rgb);
    g.ambient(rgb);
  }


  public void ambient(float gray) {
    if (recorder != null) recorder.ambient(gray);
    g.ambient(gray);
  }


  public void ambient(float x, float y, float z) {
    if (recorder != null) recorder.ambient(x, y, z);
    g.ambient(x, y, z);
  }


  public void specular(int rgb) {
    if (recorder != null) recorder.specular(rgb);
    g.specular(rgb);
  }


  public void specular(float gray) {
    if (recorder != null) recorder.specular(gray);
    g.specular(gray);
  }


  public void specular(float gray, float alpha) {
    if (recorder != null) recorder.specular(gray, alpha);
    g.specular(gray, alpha);
  }


  public void specular(float x, float y, float z) {
    if (recorder != null) recorder.specular(x, y, z);
    g.specular(x, y, z);
  }


  public void specular(float x, float y, float z, float a) {
    if (recorder != null) recorder.specular(x, y, z, a);
    g.specular(x, y, z, a);
  }


  public void shininess(float shine) {
    if (recorder != null) recorder.shininess(shine);
    g.shininess(shine);
  }


  public void emissive(int rgb) {
    if (recorder != null) recorder.emissive(rgb);
    g.emissive(rgb);
  }


  public void emissive(float gray) {
    if (recorder != null) recorder.emissive(gray);
    g.emissive(gray);
  }


  public void emissive(float x, float y, float z ) {
    if (recorder != null) recorder.emissive(x, y, z);
    g.emissive(x, y, z);
  }


  public void lights() {
    if (recorder != null) recorder.lights();
    g.lights();
  }


  public void ambientLight(float red, float green, float blue) {
    if (recorder != null) recorder.ambientLight(red, green, blue);
    g.ambientLight(red, green, blue);
  }


  public void ambientLight(float red, float green, float blue,
                           float x, float y, float z) {
    if (recorder != null) recorder.ambientLight(red, green, blue, x, y, z);
    g.ambientLight(red, green, blue, x, y, z);
  }


  public void directionalLight(float red, float green, float blue,
                               float nx, float ny, float nz) {
    if (recorder != null) recorder.directionalLight(red, green, blue, nx, ny, nz);
    g.directionalLight(red, green, blue, nx, ny, nz);
  }


  public void pointLight(float red, float green, float blue,
                         float x, float y, float z) {
    if (recorder != null) recorder.pointLight(red, green, blue, x, y, z);
    g.pointLight(red, green, blue, x, y, z);
  }


  public void spotLight(float red, float green, float blue,
                        float x, float y, float z,
                        float nx, float ny, float nz,
                        float angle, float concentration) {
    if (recorder != null) recorder.spotLight(red, green, blue, x, y, z, nx, ny, nz, angle, concentration);
    g.spotLight(red, green, blue, x, y, z, nx, ny, nz, angle, concentration);
  }


  public void lightFalloff(float constant, float linear, float quadratic) {
    if (recorder != null) recorder.lightFalloff(constant, linear, quadratic);
    g.lightFalloff(constant, linear, quadratic);
  }


  public void lightSpecular(float x, float y, float z) {
    if (recorder != null) recorder.lightSpecular(x, y, z);
    g.lightSpecular(x, y, z);
  }


  public void background(int rgb) {
    if (recorder != null) recorder.background(rgb);
    g.background(rgb);
  }


  public void background(int rgb, float alpha) {
    if (recorder != null) recorder.background(rgb, alpha);
    g.background(rgb, alpha);
  }


  public void background(float gray) {
    if (recorder != null) recorder.background(gray);
    g.background(gray);
  }


  public void background(float gray, float alpha) {
    if (recorder != null) recorder.background(gray, alpha);
    g.background(gray, alpha);
  }


  public void background(float x, float y, float z) {
    if (recorder != null) recorder.background(x, y, z);
    g.background(x, y, z);
  }


  public void background(float x, float y, float z, float a) {
    if (recorder != null) recorder.background(x, y, z, a);
    g.background(x, y, z, a);
  }


  public void background(PImage image) {
    if (recorder != null) recorder.background(image);
    g.background(image);
  }


  public final float alpha(int what) {
    return g.alpha(what);
  }


  public final float red(int what) {
    return g.red(what);
  }


  public final float green(int what) {
    return g.green(what);
  }


  public final float blue(int what) {
    return g.blue(what);
  }


  public final float hue(int what) {
    return g.hue(what);
  }


  public final float saturation(int what) {
    return g.saturation(what);
  }


  public final float brightness(int what) {
    return g.brightness(what);
  }


  public int lerpColor(int c1, int c2, float amt) {
    return g.lerpColor(c1, c2, amt);
  }


  static public int lerpColor(int c1, int c2, float amt, int mode) {
    return PGraphics.lerpColor(c1, c2, amt, mode);
  }


  public void beginRaw(PGraphics rawGraphics) {
    if (recorder != null) recorder.beginRaw(rawGraphics);
    g.beginRaw(rawGraphics);
  }


  public void endRaw() {
    if (recorder != null) recorder.endRaw();
    g.endRaw();
  }


  public boolean displayable() {
    return g.displayable();
  }
}
