/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-10 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation, version 2.1.

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
import java.util.regex.*;
import java.util.zip.*;

import javax.imageio.ImageIO;
import javax.swing.JFileChooser;
import javax.swing.SwingUtilities;

import processing.core.PShape;


/**
 * Base class for all sketches that use processing.core.
 * <p/>
 * Note that you should not use AWT or Swing components inside a Processing
 * applet. The surface is made to automatically update itself, and will cause
 * problems with redraw of components drawn above it. If you'd like to
 * integrate other Java components, see below.
 * <p/>
 * As of release 0145, Processing uses active mode rendering in all cases.
 * All animation tasks happen on the "Processing Animation Thread". The
 * setup() and draw() methods are handled by that thread, and events (like
 * mouse movement and key presses, which are fired by the event dispatch
 * thread or EDT) are queued to be (safely) handled at the end of draw().
 * For code that needs to run on the EDT, use SwingUtilities.invokeLater().
 * When doing so, be careful to synchronize between that code (since
 * invokeLater() will make your code run from the EDT) and the Processing
 * animation thread. Use of a callback function or the registerXxx() methods
 * in PApplet can help ensure that your code doesn't do something naughty.
 * <p/>
 * As of release 0136 of Processing, we have discontinued support for versions
 * of Java prior to 1.5. We don't have enough people to support it, and for a
 * project of our size, we should be focusing on the future, rather than
 * working around legacy Java code. In addition, Java 1.5 gives us access to
 * better timing facilities which will improve the steadiness of animation.
 * <p/>
 * This class extends Applet instead of JApplet because 1) historically,
 * we supported Java 1.1, which does not include Swing (without an
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
 * PApplet, you can instead embed the PApplet inside another GUI the way you
 * would any other Component.
 * <p/>
 * It is also possible to resize the Processing window by including
 * <tt>frame.setResizable(true)</tt> inside your <tt>setup()</tt> method.
 * Note that the Java method <tt>frame.setSize()</tt> will not work unless
 * you first set the frame to be resizable.
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
 * @usage Web &amp; Application
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
   * Current platform in use.
   * <P>
   * Equivalent to System.getProperty("os.name"), just used internally.
   */

  /**
   * Current platform in use, one of the
   * PConstants WINDOWS, MACOSX, MACOS9, LINUX or OTHER.
   */
  static public int platform;

  /**
   * Name associated with the current 'platform' (see PConstants.platformNames)
   */
  //static public String platformName;

  static {
    String osname = System.getProperty("os.name");

    if (osname.indexOf("Mac") != -1) {
      platform = MACOSX;

    } else if (osname.indexOf("Windows") != -1) {
      platform = WINDOWS;

    } else if (osname.equals("Linux")) {  // true for the ibm vm
      platform = LINUX;

    } else {
      platform = OTHER;
    }
  }

  /**
   * Modifier flags for the shortcut key used to trigger menus.
   * (Cmd on Mac OS X, Ctrl on Linux and Windows)
   */
  static public final int MENU_SHORTCUT =
    Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();

  /** The PGraphics renderer associated with this PApplet */
  public PGraphics g;

  //protected Object glock = new Object(); // for sync

  /** The frame containing this applet (if any) */
  public Frame frame;

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
   * <p>
   * This variable is not static, because future releases need to be better
   * at handling multiple displays.
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
   * Exception thrown when size() is called the first time.
   * <P>
   * This is used internally so that setup() is forced to run twice
   * when the renderer is changed. This is the only way for us to handle
   * invoking the new renderer while also in the midst of rendering.
   */
  static public class RendererChangeException extends RuntimeException { }

  /**
   * true if no size() command has been executed. This is used to wait until
   * a size has been set before placing in the window and showing it.
   */
  public boolean defaultSize;

  volatile boolean resizeRequest;
  volatile int resizeWidth;
  volatile int resizeHeight;

  /**
   * Array containing the values for all the pixels in the display window. These values are of the color datatype. This array is the size of the display window. For example, if the image is 100x100 pixels, there will be 10000 values and if the window is 200x300 pixels, there will be 60000 values. The <b>index</b> value defines the position of a value within the array. For example, the statment <b>color b = pixels[230]</b> will set the variable <b>b</b> to be equal to the value at that location in the array. <br><br> Before accessing this array, the data must loaded with the <b>loadPixels()</b> function. After the array data has been modified, the <b>updatePixels()</b> function must be run to update the changes. Without <b>loadPixels()</b>, running the code may (or will in future releases) result in a NullPointerException.
   * Pixel buffer from this applet's PGraphics.
   * <P>
   * When used with OpenGL or Java2D, this value will
   * be null until loadPixels() has been called.
   *
   * @webref image:pixels
   * @see processing.core.PApplet#loadPixels()
   * @see processing.core.PApplet#updatePixels()
   * @see processing.core.PApplet#get(int, int, int, int)
   * @see processing.core.PApplet#set(int, int, int)
   * @see processing.core.PImage
   */
  public int pixels[];

  /** width of this applet's associated PGraphics
   * @webref environment
   */
  public int width;

  /** height of this applet's associated PGraphics
   * @webref environment
   * */
  public int height;

  /**
   * The system variable <b>mouseX</b> always contains the current horizontal coordinate of the mouse.
   * @webref input:mouse
   * @see PApplet#mouseY
   * @see PApplet#mousePressed
   * @see PApplet#mousePressed()
   * @see PApplet#mouseReleased()
   * @see PApplet#mouseMoved()
   * @see PApplet#mouseDragged()
   *
   * */
  public int mouseX;

  /**
   * The system variable <b>mouseY</b> always contains the current vertical coordinate of the mouse.
   * @webref input:mouse
   * @see PApplet#mouseX
   * @see PApplet#mousePressed
   * @see PApplet#mousePressed()
   * @see PApplet#mouseReleased()
   * @see PApplet#mouseMoved()
   * @see PApplet#mouseDragged()
   * */
  public int mouseY;

  /**
   * Previous x/y position of the mouse. This will be a different value
   * when inside a mouse handler (like the mouseMoved() method) versus
   * when inside draw(). Inside draw(), pmouseX is updated once each
   * frame, but inside mousePressed() and friends, it's updated each time
   * an event comes through. Be sure to use only one or the other type of
   * means for tracking pmouseX and pmouseY within your sketch, otherwise
   * you're gonna run into trouble.
   * @webref input:mouse
   * @see PApplet#pmouseY
   * @see PApplet#mouseX
   * @see PApplet#mouseY
   */
  public int pmouseX;

  /**
   * @webref input:mouse
   * @see PApplet#pmouseX
   * @see PApplet#mouseX
   * @see PApplet#mouseY
   */
  public int pmouseY;

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
   * Processing automatically tracks if the mouse button is pressed and which button is pressed.
   * The value of the system variable <b>mouseButton</b> is either <b>LEFT</b>, <b>RIGHT</b>, or <b>CENTER</b> depending on which button is pressed.
   * <h3>Advanced:</h3>
   * If running on Mac OS, a ctrl-click will be interpreted as
   * the righthand mouse button (unlike Java, which reports it as
   * the left mouse).
   * @webref input:mouse
   * @see PApplet#mouseX
   * @see PApplet#mouseY
   * @see PApplet#mousePressed()
   * @see PApplet#mouseReleased()
   * @see PApplet#mouseMoved()
   * @see PApplet#mouseDragged()
   */
  public int mouseButton;

  /**
   * Variable storing if a mouse button is pressed. The value of the system variable <b>mousePressed</b> is true if a mouse button is pressed and false if a button is not pressed.
   * @webref input:mouse
   * @see PApplet#mouseX
   * @see PApplet#mouseY
   * @see PApplet#mouseReleased()
   * @see PApplet#mouseMoved()
   * @see PApplet#mouseDragged()
   */
  public boolean mousePressed;
  public MouseEvent mouseEvent;

  /**
   * The system variable <b>key</b> always contains the value of the most recent key on the keyboard that was used (either pressed or released). <br><br>
   * For non-ASCII keys, use the <b>keyCode</b> variable.
   * The keys included in the ASCII specification (BACKSPACE, TAB, ENTER, RETURN, ESC, and DELETE) do not require checking to see if they key is coded, and you should simply use the <b>key</b> variable instead of <b>keyCode</b>
   * If you're making cross-platform projects, note that the ENTER key is commonly used on PCs and Unix and the RETURN key is used instead on Macintosh.
   * Check for both ENTER and RETURN to make sure your program will work for all platforms.
   * =advanced
   *
   * Last key pressed.
   * <P>
   * If it's a coded key, i.e. UP/DOWN/CTRL/SHIFT/ALT,
   * this will be set to CODED (0xffff or 65535).
   * @webref input:keyboard
   * @see PApplet#keyCode
   * @see PApplet#keyPressed
   * @see PApplet#keyPressed()
   * @see PApplet#keyReleased()
   */
  public char key;

  /**
   * The variable <b>keyCode</b> is used to detect special keys such as the UP, DOWN, LEFT, RIGHT arrow keys and ALT, CONTROL, SHIFT.
   * When checking for these keys, it's first necessary to check and see if the key is coded. This is done with the conditional "if (key == CODED)" as shown in the example.
   * <br><br>The keys included in the ASCII specification (BACKSPACE, TAB, ENTER, RETURN, ESC, and DELETE) do not require checking to see if they key is coded, and you should simply use the <b>key</b> variable instead of <b>keyCode</b>
   * If you're making cross-platform projects, note that the ENTER key is commonly used on PCs and Unix and the RETURN key is used instead on Macintosh.
   * Check for both ENTER and RETURN to make sure your program will work for all platforms.
   * <br><br>For users familiar with Java, the values for UP and DOWN are simply shorter versions of Java's KeyEvent.VK_UP and KeyEvent.VK_DOWN.
   * Other keyCode values can be found in the Java <a href="http://java.sun.com/j2se/1.4.2/docs/api/java/awt/event/KeyEvent.html">KeyEvent</a> reference.
   *
   * =advanced
   * When "key" is set to CODED, this will contain a Java key code.
   * <P>
   * For the arrow keys, keyCode will be one of UP, DOWN, LEFT and RIGHT.
   * Also available are ALT, CONTROL and SHIFT. A full set of constants
   * can be obtained from java.awt.event.KeyEvent, from the VK_XXXX variables.
   * @webref input:keyboard
   * @see PApplet#key
   * @see PApplet#keyPressed
   * @see PApplet#keyPressed()
   * @see PApplet#keyReleased()
   */
  public int keyCode;

  /**
   * The boolean system variable <b>keyPressed</b> is <b>true</b> if any key is pressed and <b>false</b> if no keys are pressed.
   * @webref input:keyboard
   * @see PApplet#key
   * @see PApplet#keyCode
   * @see PApplet#keyPressed()
   * @see PApplet#keyReleased()
   */
  public boolean keyPressed;

  /**
   * the last KeyEvent object passed into a mouse function.
   */
  public KeyEvent keyEvent;

  /**
   * Gets set to true/false as the applet gains/loses focus.
   * @webref environment
   */
  public boolean focused = false;

  /**
   * true if the applet is online.
   * <P>
   * This can be used to test how the applet should behave
   * since online situations are different (no file writing, etc).
   * @webref environment
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
  /** Last time in nanoseconds that frameRate was checked */
  protected long frameRateLastNanos = 0;

  /** As of release 0116, frameRate(60) is called as a default */
  protected float frameRateTarget = 60;
  protected long frameRatePeriod = 1000000000L / 60L;

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
  protected boolean exitCalled;

  Thread thread;

  protected RegisteredMethods sizeMethods;
  protected RegisteredMethods preMethods, drawMethods, postMethods;
  protected RegisteredMethods mouseEventMethods, keyEventMethods;
  protected RegisteredMethods disposeMethods;

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

  static public final String ARGS_EXCLUSIVE = "--exclusive";

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
   * When run externally to a PdeEditor,
   * this is sent by the applet when it quits.
   */
  //static public final String EXTERNAL_QUIT = "__QUIT__";
  static public final String EXTERNAL_STOP = "__STOP__";

  /**
   * When run externally to a PDE Editor, this is sent by the applet
   * whenever the window is moved.
   * <P>
   * This is used so that the editor can re-open the sketch window
   * in the same position as the user last left it.
   */
  static public final String EXTERNAL_MOVE = "__MOVE__";

  /** true if this sketch is being run by the PDE */
  boolean external = false;


  static final String ERROR_MIN_MAX =
    "Cannot use min() or max() on an empty array.";


  // during rev 0100 dev cycle, working on new threading model,
  // but need to disable and go conservative with changes in order
  // to get pdf and audio working properly first.
  // for 0116, the CRUSTY_THREADS are being disabled to fix lots of bugs.
  //static final boolean CRUSTY_THREADS = false; //true;


  public void init() {
//    println("Calling init()");

    // send tab keys through to the PApplet
    setFocusTraversalKeysEnabled(false);

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

    try {
      if (sketchPath == null) {
        sketchPath = System.getProperty("user.dir");
      }
    } catch (Exception e) { }  // may be a security problem

    Dimension size = getSize();
    if ((size.width != 0) && (size.height != 0)) {
      // When this PApplet is embedded inside a Java application with other
      // Component objects, its size() may already be set externally (perhaps
      // by a LayoutManager). In this case, honor that size as the default.
      // Size of the component is set, just create a renderer.
      g = makeGraphics(size.width, size.height, getSketchRenderer(), null, true);
      // This doesn't call setSize() or setPreferredSize() because the fact
      // that a size was already set means that someone is already doing it.

    } else {
      // Set the default size, until the user specifies otherwise
      this.defaultSize = true;
      int w = getSketchWidth();
      int h = getSketchHeight();
      g = makeGraphics(w, h, getSketchRenderer(), null, true);
      // Fire component resize event
      setSize(w, h);
      setPreferredSize(new Dimension(w, h));
    }
    width = g.width;
    height = g.height;

    addListeners();

    // this is automatically called in applets
    // though it's here for applications anyway
    start();
  }


  public int getSketchWidth() {
    return DEFAULT_WIDTH;
  }


  public int getSketchHeight() {
    return DEFAULT_HEIGHT;
  }


  public String getSketchRenderer() {
    return JAVA2D;
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
    // When running inside a browser, start() will be called when someone
    // returns to a page containing this applet.
    // http://dev.processing.org/bugs/show_bug.cgi?id=581
    finished = false;

    if (thread != null) return;
    thread = new Thread(this, "Animation Thread");
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
    // bringing this back for 0111, hoping it'll help opengl shutdown
    finished = true;  // why did i comment this out?

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
//  public Dimension getPreferredSize() {
//    return new Dimension(width, height);
//  }


//  public void addNotify() {
//    super.addNotify();
//    println("addNotify()");
//  }



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
        objects = (Object[]) PApplet.expand(objects);
        methods = (Method[]) PApplet.expand(methods);
//        Object otemp[] = new Object[count << 1];
//        System.arraycopy(objects, 0, otemp, 0, count);
//        objects = otemp;
//        Method mtemp[] = new Method[count << 1];
//        System.arraycopy(methods, 0, mtemp, 0, count);
//        methods = mtemp;
      }
      objects[count] = object;
      methods[count] = method;
      count++;
    }


    /**
     * Removes first object/method pair matched (and only the first,
     * must be called multiple times if object is registered multiple times).
     * Does not shrink array afterwards, silently returns if method not found.
     */
    public void remove(Object object, Method method) {
      int index = findIndex(object, method);
      if (index != -1) {
        // shift remaining methods by one to preserve ordering
        count--;
        for (int i = index; i < count; i++) {
          objects[i] = objects[i+1];
          methods[i] = methods[i+1];
        }
        // clean things out for the gc's sake
        objects[count] = null;
        methods[count] = null;
      }
    }

    protected int findIndex(Object object, Method method) {
      for (int i = 0; i < count; i++) {
        if (objects[i] == object && methods[i].equals(method)) {
          //objects[i].equals() might be overridden, so use == for safety
          // since here we do care about actual object identity
          //methods[i]==method is never true even for same method, so must use
          // equals(), this should be safe because of object identity
          return i;
        }
      }
      return -1;
    }
  }


  public void registerSize(Object o) {
    Class<?> methodArgs[] = new Class[] { Integer.TYPE, Integer.TYPE };
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
    Class<?> methodArgs[] = new Class[] { MouseEvent.class };
    registerWithArgs(mouseEventMethods, "mouseEvent", o, methodArgs);
  }


  public void registerKeyEvent(Object o) {
    Class<?> methodArgs[] = new Class[] { KeyEvent.class };
    registerWithArgs(keyEventMethods, "keyEvent", o, methodArgs);
  }

  public void registerDispose(Object o) {
    registerNoArgs(disposeMethods, "dispose", o);
  }


  protected void registerNoArgs(RegisteredMethods meth,
                                String name, Object o) {
    Class<?> c = o.getClass();
    try {
      Method method = c.getMethod(name, new Class[] {});
      meth.add(o, method);

    } catch (NoSuchMethodException nsme) {
      die("There is no public " + name + "() method in the class " +
          o.getClass().getName());

    } catch (Exception e) {
      die("Could not register " + name + " + () for " + o, e);
    }
  }


  protected void registerWithArgs(RegisteredMethods meth,
                                  String name, Object o, Class<?> cargs[]) {
    Class<?> c = o.getClass();
    try {
      Method method = c.getMethod(name, cargs);
      meth.add(o, method);

    } catch (NoSuchMethodException nsme) {
      die("There is no public " + name + "() method in the class " +
          o.getClass().getName());

    } catch (Exception e) {
      die("Could not register " + name + " + () for " + o, e);
    }
  }


  public void unregisterSize(Object o) {
    Class<?> methodArgs[] = new Class[] { Integer.TYPE, Integer.TYPE };
    unregisterWithArgs(sizeMethods, "size", o, methodArgs);
  }

  public void unregisterPre(Object o) {
    unregisterNoArgs(preMethods, "pre", o);
  }

  public void unregisterDraw(Object o) {
    unregisterNoArgs(drawMethods, "draw", o);
  }

  public void unregisterPost(Object o) {
    unregisterNoArgs(postMethods, "post", o);
  }

  public void unregisterMouseEvent(Object o) {
    Class<?> methodArgs[] = new Class[] { MouseEvent.class };
    unregisterWithArgs(mouseEventMethods, "mouseEvent", o, methodArgs);
  }

  public void unregisterKeyEvent(Object o) {
    Class<?> methodArgs[] = new Class[] { KeyEvent.class };
    unregisterWithArgs(keyEventMethods, "keyEvent", o, methodArgs);
  }

  public void unregisterDispose(Object o) {
    unregisterNoArgs(disposeMethods, "dispose", o);
  }


  protected void unregisterNoArgs(RegisteredMethods meth,
                                  String name, Object o) {
    Class<?> c = o.getClass();
    try {
      Method method = c.getMethod(name, new Class[] {});
      meth.remove(o, method);
    } catch (Exception e) {
      die("Could not unregister " + name + "() for " + o, e);
    }
  }


  protected void unregisterWithArgs(RegisteredMethods meth,
                                    String name, Object o, Class<?> cargs[]) {
    Class<?> c = o.getClass();
    try {
      Method method = c.getMethod(name, cargs);
      meth.remove(o, method);
    } catch (Exception e) {
      die("Could not unregister " + name + "() for " + o, e);
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


  protected void resizeRenderer(int iwidth, int iheight) {
//    println("resizeRenderer request for " + iwidth + " " + iheight);
    if (width != iwidth || height != iheight) {
//      println("  former size was " + width + " " + height);
      g.setSize(iwidth, iheight);
      width = iwidth;
      height = iheight;
    }
  }


  /**
   * Defines the dimension of the display window in units of pixels. The <b>size()</b> function <em>must</em> be the first line in <b>setup()</b>. If <b>size()</b> is not called, the default size of the window is 100x100 pixels. The system variables <b>width</b> and <b>height</b> are set by the parameters passed to the <b>size()</b> function. <br><br>
   * Do not use variables as the parameters to <b>size()</b> command, because it will cause problems when exporting your sketch. When variables are used, the dimensions of your sketch cannot be determined during export. Instead, employ numeric values in the <b>size()</b> statement, and then use the built-in <b>width</b> and <b>height</b> variables inside your program when you need the dimensions of the display window are needed. <br><br>
   * The MODE parameters selects which rendering engine to use. For example, if you will be drawing 3D shapes for the web use <b>P3D</b>, if you want to export a program with OpenGL graphics acceleration use <b>OPENGL</b>. A brief description of the four primary renderers follows:<br><br><b>JAVA2D</b> - The default renderer. This renderer supports two dimensional drawing and provides higher image quality in overall, but generally slower than P2D.<br><br><b>P2D</b> (Processing 2D) - Fast 2D renderer, best used with pixel data, but not as accurate as the JAVA2D default. <br><br><b>P3D</b> (Processing 3D) - Fast 3D renderer for the web. Sacrifices rendering quality for quick 3D drawing.<br><br><b>OPENGL</b> - High speed 3D graphics renderer that makes use of OpenGL-compatible graphics hardware is available. Keep in mind that OpenGL is not magic pixie dust that makes any sketch faster (though it's close), so other rendering options may produce better results depending on the nature of your code. Also note that with OpenGL, all graphics are smoothed: the smooth() and noSmooth() commands are ignored. <br><br><b>PDF</b> - The PDF renderer draws 2D graphics directly to an Acrobat PDF file. This produces excellent results when you need vector shapes for high resolution output or printing. You must first use Import Library &rarr; PDF to make use of the library. More information can be found in the PDF library reference.
   * If you're manipulating pixels (using methods like get() or blend(), or manipulating the pixels[] array), P2D and P3D will usually be faster than the default (JAVA2D) setting, and often the OPENGL setting as well. Similarly, when handling lots of images, or doing video playback, P2D and P3D will tend to be faster.<br><br>
   * The P2D, P3D, and OPENGL renderers do not support strokeCap() or strokeJoin(), which can lead to ugly results when using strokeWeight(). (<a href="http://dev.processing.org/bugs/show_bug.cgi?id=955">Bug 955</a>) <br><br>
   * For the most elegant and accurate results when drawing in 2D, particularly when using smooth(), use the JAVA2D renderer setting. It may be slower than the others, but is the most complete, which is why it's the default. Advanced users will want to switch to other renderers as they learn the tradeoffs. <br><br>
   * Rendering graphics requires tradeoffs between speed, accuracy, and general usefulness of the available features. None of the renderers are perfect, so we provide multiple options so that you can decide what tradeoffs make the most sense for your project. We'd prefer all of them to have perfect visual accuracy, high performance, and support a wide range of features, but that's simply not possible. <br><br>
   * The maximum width and height is limited by your operating system, and is usually the width and height of your actual screen. On some machines it may simply be the number of pixels on your current screen, meaning that a screen that's 800x600 could support size(1600, 300), since it's the same number of pixels. This varies widely so you'll have to try different rendering modes and sizes until you get what you're looking for. If you need something larger, use <b>createGraphics</b> to create a non-visible drawing surface.
   * <br><br>Again, the size() method must be the first line of the code (or first item inside setup). Any code that appears before the size() command may run more than once, which can lead to confusing results.
   *
   * =advanced
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
   *
   * @webref structure
   * @param iwidth width of the display window in units of pixels
   * @param iheight height of the display window in units of pixels
   */
  public void size(int iwidth, int iheight) {
    size(iwidth, iheight, JAVA2D, null);
  }

  /**
   *
   * @param irenderer   Either P2D, P3D, JAVA2D, or OPENGL
   */
  public void size(int iwidth, int iheight, String irenderer) {
    size(iwidth, iheight, irenderer, null);
  }


  /**
   * Creates a new PGraphics object and sets it to the specified size.
   *
   * Note that you cannot change the renderer once outside of setup().
   * In most cases, you can call size() to give it a new size,
   * but you need to always ask for the same renderer, otherwise
   * you're gonna run into trouble.
   *
   * The size() method should *only* be called from inside the setup() or
   * draw() methods, so that it is properly run on the main animation thread.
   * To change the size of a PApplet externally, use setSize(), which will
   * update the component size, and queue a resize of the renderer as well.
   */
  public void size(final int iwidth, final int iheight,
                   String irenderer, String ipath) {
    // Run this from the EDT, just cuz it's AWT stuff (or maybe later Swing)
    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        // Set the preferred size so that the layout managers can handle it
        setPreferredSize(new Dimension(iwidth, iheight));
        setSize(iwidth, iheight);
      }
    });

    // ensure that this is an absolute path
    if (ipath != null) ipath = savePath(ipath);

    String currentRenderer = g.getClass().getName();
    if (currentRenderer.equals(irenderer)) {
      // Avoid infinite loop of throwing exception to reset renderer
      resizeRenderer(iwidth, iheight);
      //redraw();  // will only be called insize draw()

    } else {  // renderer is being changed
      // otherwise ok to fall through and create renderer below
      // the renderer is changing, so need to create a new object
      g = makeGraphics(iwidth, iheight, irenderer, ipath, true);
      width = iwidth;
      height = iheight;

      // fire resize event to make sure the applet is the proper size
//      setSize(iwidth, iheight);
      // this is the function that will run if the user does their own
      // size() command inside setup, so set defaultSize to false.
      defaultSize = false;

      // throw an exception so that setup() is called again
      // but with a properly sized render
      // this is for opengl, which needs a valid, properly sized
      // display before calling anything inside setup().
      throw new RendererChangeException();
    }
  }


  /**
   * Creates and returns a new <b>PGraphics</b> object of the types P2D, P3D, and JAVA2D. Use this class if you need to draw into an off-screen graphics buffer. It's not possible to use <b>createGraphics()</b> with OPENGL, because it doesn't allow offscreen use. The DXF and PDF renderers require the filename parameter.
   * <br><br>It's important to call any drawing commands between beginDraw() and endDraw() statements. This is also true for any commands that affect drawing, such as smooth() or colorMode().
   * <br><br>Unlike the main drawing surface which is completely opaque, surfaces created with createGraphics() can have transparency. This makes it possible to draw into a graphics and maintain the alpha channel. By using save() to write a PNG or TGA file, the transparency of the graphics object will be honored. Note that transparency levels are binary: pixels are either complete opaque or transparent. For the time being (as of release 0127), this means that text characters will be opaque blocks. This will be fixed in a future release (<a href="http://dev.processing.org/bugs/show_bug.cgi?id=641">Bug 641</a>).
   *
   * =advanced
   * Create an offscreen PGraphics object for drawing. This can be used
   * for bitmap or vector images drawing or rendering.
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
   *
   * @webref rendering
   * @param iwidth width in pixels
   * @param iheight height in pixels
   * @param irenderer Either P2D (not yet implemented), P3D, JAVA2D, PDF, DXF
   *
   * @see processing.core.PGraphics
   *
   */
  public PGraphics createGraphics(int iwidth, int iheight,
                                  String irenderer) {
    PGraphics pg = makeGraphics(iwidth, iheight, irenderer, null, false);
    //pg.parent = this;  // make save() work
    return pg;
  }


  /**
   * Create an offscreen graphics surface for drawing, in this case
   * for a renderer that writes to a file (such as PDF or DXF).
   * @param ipath the name of the file (can be an absolute or relative path)
   */
  public PGraphics createGraphics(int iwidth, int iheight,
                                  String irenderer, String ipath) {
    if (ipath != null) {
      ipath = savePath(ipath);
    }
    PGraphics pg = makeGraphics(iwidth, iheight, irenderer, ipath, false);
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
  protected PGraphics makeGraphics(int iwidth, int iheight,
                                   String irenderer, String ipath,
                                   boolean iprimary) {
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

//    if (irenderer.equals(P2D)) {
//      throw new RuntimeException("The P2D renderer is currently disabled, " +
//                                 "please use P3D or JAVA2D.");
//    }

    String openglError =
      "Before using OpenGL, first select " +
      "Import Library > opengl from the Sketch menu.";

    try {
      /*
      Class<?> rendererClass = Class.forName(irenderer);

      Class<?> constructorParams[] = null;
      Object constructorValues[] = null;

      if (ipath == null) {
        constructorParams = new Class[] {
          Integer.TYPE, Integer.TYPE, PApplet.class
        };
        constructorValues = new Object[] {
          new Integer(iwidth), new Integer(iheight), this
        };
      } else {
        constructorParams = new Class[] {
          Integer.TYPE, Integer.TYPE, PApplet.class, String.class
        };
        constructorValues = new Object[] {
          new Integer(iwidth), new Integer(iheight), this, ipath
        };
      }

      Constructor<?> constructor =
        rendererClass.getConstructor(constructorParams);
      PGraphics pg = (PGraphics) constructor.newInstance(constructorValues);
      */

      Class<?> rendererClass =
        Thread.currentThread().getContextClassLoader().loadClass(irenderer);

      //Class<?> params[] = null;
      //PApplet.println(rendererClass.getConstructors());
      Constructor<?> constructor = rendererClass.getConstructor(new Class[] { });
      PGraphics pg = (PGraphics) constructor.newInstance();

      pg.setParent(this);
      pg.setPrimary(iprimary);
      if (ipath != null) pg.setPath(ipath);
      pg.setSize(iwidth, iheight);

      // everything worked, return it
      return pg;

    } catch (InvocationTargetException ite) {
      String msg = ite.getTargetException().getMessage();
      if ((msg != null) &&
          (msg.indexOf("no jogl in java.library.path") != -1)) {
        throw new RuntimeException(openglError +
                                   " (The native library is missing.)");

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
        throw new RuntimeException(openglError +
                                   " (The library .jar file is missing.)");
      } else {
        throw new RuntimeException("You need to use \"Import Library\" " +
                                   "to add " + irenderer + " to your sketch.");
      }

    } catch (Exception e) {
      //System.out.println("ex3");
      if ((e instanceof IllegalArgumentException) ||
          (e instanceof NoSuchMethodException) ||
          (e instanceof IllegalAccessException)) {
        e.printStackTrace();
        /*
        String msg = "public " +
          irenderer.substring(irenderer.lastIndexOf('.') + 1) +
          "(int width, int height, PApplet parent" +
          ((ipath == null) ? "" : ", String filename") +
          ") does not exist.";
          */
        String msg = irenderer + " needs to be updated " +
          "for the current release of Processing.";
        throw new RuntimeException(msg);

      } else {
        if (platform == MACOSX) e.printStackTrace(System.out);
        throw new RuntimeException(e.getMessage());
      }
    }
  }


  /**
   * Creates a new PImage (the datatype for storing images). This provides a fresh buffer of pixels to play with. Set the size of the buffer with the <b>width</b> and <b>height</b> parameters. The <b>format</b> parameter defines how the pixels are stored. See the PImage reference for more information.
   * <br><br>Be sure to include all three parameters, specifying only the width and height (but no format) will produce a strange error.
   * <br><br>Advanced users please note that createImage() should be used instead of the syntax <tt>new PImage()</tt>.
   * =advanced
   * Preferred method of creating new PImage objects, ensures that a
   * reference to the parent PApplet is included, which makes save() work
   * without needing an absolute path.
   *
   * @webref image
   * @param wide width in pixels
   * @param high height in pixels
   * @param format Either RGB, ARGB, ALPHA (grayscale alpha channel)
   *
   * @see processing.core.PImage
   * @see processing.core.PGraphics
   */
  public PImage createImage(int wide, int high, int format) {
    PImage image = new PImage(wide, high, format);
    image.parent = this;  // make save() work
    return image;
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  public void update(Graphics screen) {
    paint(screen);
  }


  //synchronized public void paint(Graphics screen) {  // shutting off for 0146
  public void paint(Graphics screen) {
    // ignore the very first call to paint, since it's coming
    // from the o.s., and the applet will soon update itself anyway.
    if (frameCount == 0) {
//      println("Skipping frame");
      // paint() may be called more than once before things
      // are finally painted to the screen and the thread gets going
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

    // make sure the screen is visible and usable
    // (also prevents over-drawing when using PGraphicsOpenGL)
    if ((g != null) && (g.image != null)) {
//      println("inside paint(), screen.drawImage()");
      screen.drawImage(g.image, 0, 0, null);
    }
  }


  // active paint method
  protected void paint() {
    try {
      Graphics screen = this.getGraphics();
      if (screen != null) {
        if ((g != null) && (g.image != null)) {
          screen.drawImage(g.image, 0, 0, null);
        }
        Toolkit.getDefaultToolkit().sync();
      }
    } catch (Exception e) {
      // Seen on applet destroy, maybe can ignore?
      e.printStackTrace();

//    } finally {
//      if (g != null) {
//        g.dispose();
//      }
    }
  }


  //////////////////////////////////////////////////////////////


  /**
   * Main method for the primary animation thread.
   *
   * <A HREF="http://java.sun.com/products/jfc/tsc/articles/painting/">Painting in AWT and Swing</A>
   */
  public void run() {  // not good to make this synchronized, locks things up
    long beforeTime = System.nanoTime();
    long overSleepTime = 0L;

    int noDelays = 0;
    // Number of frames with a delay of 0 ms before the
    // animation thread yields to other running threads.
    final int NO_DELAYS_PER_YIELD = 15;

    /*
      // this has to be called after the exception is thrown,
      // otherwise the supporting libs won't have a valid context to draw to
      Object methodArgs[] =
        new Object[] { new Integer(width), new Integer(height) };
      sizeMethods.handle(methodArgs);
     */

    while ((Thread.currentThread() == thread) && !finished) {
      // Don't resize the renderer from the EDT (i.e. from a ComponentEvent),
      // otherwise it may attempt a resize mid-render.
      if (resizeRequest) {
        resizeRenderer(resizeWidth, resizeHeight);
        resizeRequest = false;
      }

      // render a single frame
      handleDraw();

      if (frameCount == 1) {
        // Call the request focus event once the image is sure to be on
        // screen and the component is valid. The OpenGL renderer will
        // request focus for its canvas inside beginDraw().
        // http://java.sun.com/j2se/1.4.2/docs/api/java/awt/doc-files/FocusSpec.html
        //println("requesting focus");
        requestFocus();
      }

      // wait for update & paint to happen before drawing next frame
      // this is necessary since the drawing is sometimes in a
      // separate thread, meaning that the next frame will start
      // before the update/paint is completed

      long afterTime = System.nanoTime();
      long timeDiff = afterTime - beforeTime;
      //System.out.println("time diff is " + timeDiff);
      long sleepTime = (frameRatePeriod - timeDiff) - overSleepTime;

      if (sleepTime > 0) {  // some time left in this cycle
        try {
//          Thread.sleep(sleepTime / 1000000L);  // nanoseconds -> milliseconds
          Thread.sleep(sleepTime / 1000000L, (int) (sleepTime % 1000000L));
          noDelays = 0;  // Got some sleep, not delaying anymore
        } catch (InterruptedException ex) { }

        overSleepTime = (System.nanoTime() - afterTime) - sleepTime;
        //System.out.println("  oversleep is " + overSleepTime);

      } else {    // sleepTime <= 0; the frame took longer than the period
//        excess -= sleepTime;  // store excess time value
        overSleepTime = 0L;

        if (noDelays > NO_DELAYS_PER_YIELD) {
          Thread.yield();   // give another thread a chance to run
          noDelays = 0;
        }
      }

      beforeTime = System.nanoTime();
    }

    stop();  // call to shutdown libs?

    // If the user called the exit() function, the window should close,
    // rather than the sketch just halting.
    if (exitCalled) {
      exit2();
    }
  }


  //synchronized public void handleDisplay() {
  public void handleDraw() {
    if (g != null && (looping || redraw)) {
      if (!g.canDraw()) {
        // Don't draw if the renderer is not yet ready.
        // (e.g. OpenGL has to wait for a peer to be on screen)
        return;
      }

      //System.out.println("handleDraw() " + frameCount);

      g.beginDraw();
      if (recorder != null) {
        recorder.beginDraw();
      }

      long now = System.nanoTime();

      if (frameCount == 0) {
        try {
          //println("Calling setup()");
          setup();
          //println("Done with setup()");

        } catch (RendererChangeException e) {
          // Give up, instead set the new renderer and re-attempt setup()
          return;
        }
        this.defaultSize = false;

      } else {  // frameCount > 0, meaning an actual draw()
        // update the current frameRate
        double rate = 1000000.0 / ((now - frameRateLastNanos) / 1000000.0);
        float instantaneousRate = (float) rate / 1000.0f;
        frameRate = (frameRate * 0.9f) + (instantaneousRate * 0.1f);

        preMethods.handle();

        // use dmouseX/Y as previous mouse pos, since this is the
        // last position the mouse was in during the previous draw.
        pmouseX = dmouseX;
        pmouseY = dmouseY;

        //println("Calling draw()");
        draw();
        //println("Done calling draw()");

        // dmouseX/Y is updated only once per frame (unlike emouseX/Y)
        dmouseX = mouseX;
        dmouseY = mouseY;

        // these are called *after* loop so that valid
        // drawing commands can be run inside them. it can't
        // be before, since a call to background() would wipe
        // out anything that had been drawn so far.
        dequeueMouseEvents();
        dequeueKeyEvents();

        drawMethods.handle();

        redraw = false;  // unset 'redraw' flag in case it was set
        // (only do this once draw() has run, not just setup())

      }

      g.endDraw();
      if (recorder != null) {
        recorder.endDraw();
      }

      frameRateLastNanos = now;
      frameCount++;

      // Actively render the screen
      paint();

//    repaint();
//    getToolkit().sync();  // force repaint now (proper method)

      postMethods.handle();
    }
  }


  //////////////////////////////////////////////////////////////



  synchronized public void redraw() {
    if (!looping) {
      redraw = true;
//      if (thread != null) {
//        // wake from sleep (necessary otherwise it'll be
//        // up to 10 seconds before update)
//        if (CRUSTY_THREADS) {
//          thread.interrupt();
//        } else {
//          synchronized (blocker) {
//            blocker.notifyAll();
//          }
//        }
//      }
    }
  }


  synchronized public void loop() {
    if (!looping) {
      looping = true;
    }
  }


  synchronized public void noLoop() {
    if (looping) {
      looping = false;
    }
  }


  //////////////////////////////////////////////////////////////


  public void addListeners() {
    addMouseListener(this);
    addMouseMotionListener(this);
    addKeyListener(this);
    addFocusListener(this);

    addComponentListener(new ComponentAdapter() {
      public void componentResized(ComponentEvent e) {
        Component c = e.getComponent();
        //System.out.println("componentResized() " + c);
        Rectangle bounds = c.getBounds();
        resizeRequest = true;
        resizeWidth = bounds.width;
        resizeHeight = bounds.height;
      }
    });
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
    if (platform == MACOSX) {
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
   * The <b>mousePressed()</b> function is called once after every time a mouse button is pressed. The <b>mouseButton</b> variable (see the related reference entry) can be used to determine which button has been pressed.
   * =advanced
   *
   * If you must, use
   * int button = mouseEvent.getButton();
   * to figure out which button was clicked. It will be one of:
   * MouseEvent.BUTTON1, MouseEvent.BUTTON2, MouseEvent.BUTTON3
   * Note, however, that this is completely inconsistent across
   * platforms.
   * @webref input:mouse
   * @see PApplet#mouseX
   * @see PApplet#mouseY
   * @see PApplet#mousePressed
   * @see PApplet#mouseReleased()
   * @see PApplet#mouseMoved()
   * @see PApplet#mouseDragged()
   */
  public void mousePressed() { }

  /**
   * The <b>mouseReleased()</b> function is called every time a mouse button is released.
   * @webref input:mouse
   * @see PApplet#mouseX
   * @see PApplet#mouseY
   * @see PApplet#mousePressed
   * @see PApplet#mousePressed()
   * @see PApplet#mouseMoved()
   * @see PApplet#mouseDragged()
   */
  public void mouseReleased() { }

  /**
   * The <b>mouseClicked()</b> function is called once after a mouse button has been pressed and then released.
   * =advanced
   * When the mouse is clicked, mousePressed() will be called,
   * then mouseReleased(), then mouseClicked(). Note that
   * mousePressed is already false inside of mouseClicked().
   * @webref input:mouse
   * @see PApplet#mouseX
   * @see PApplet#mouseY
   * @see PApplet#mouseButton
   * @see PApplet#mousePressed()
   * @see PApplet#mouseReleased()
   * @see PApplet#mouseMoved()
   * @see PApplet#mouseDragged()
   */
  public void mouseClicked() { }

  /**
   * The <b>mouseDragged()</b> function is called once every time the mouse moves and a mouse button is pressed.
   * @webref input:mouse
   * @see PApplet#mouseX
   * @see PApplet#mouseY
   * @see PApplet#mousePressed
   * @see PApplet#mousePressed()
   * @see PApplet#mouseReleased()
   * @see PApplet#mouseMoved()
   */
  public void mouseDragged() { }

  /**
   * The <b>mouseMoved()</b> function is called every time the mouse moves and a mouse button is not pressed.
   * @webref input:mouse
   * @see PApplet#mouseX
   * @see PApplet#mouseY
   * @see PApplet#mousePressed
   * @see PApplet#mousePressed()
   * @see PApplet#mouseReleased()
   * @see PApplet#mouseDragged()
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
    if (event.getID() == KeyEvent.KEY_PRESSED) {
      if (key == KeyEvent.VK_ESCAPE) {
        exit();
      }
      // When running tethered to the Processing application, respond to
      // Ctrl-W (or Cmd-W) events by closing the sketch. Disable this behavior
      // when running independently, because this sketch may be one component
      // embedded inside an application that has its own close behavior.
      if (external &&
          event.getModifiers() == MENU_SHORTCUT &&
          event.getKeyCode() == 'W') {
        exit();
      }
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
   *
   * The <b>keyPressed()</b> function is called once every time a key is pressed. The key that was pressed is stored in the <b>key</b> variable.
   * <br><br>For non-ASCII keys, use the <b>keyCode</b> variable.
   * The keys included in the ASCII specification (BACKSPACE, TAB, ENTER, RETURN, ESC, and DELETE) do not require checking to see if they key is coded, and you should simply use the <b>key</b> variable instead of <b>keyCode</b>
   * If you're making cross-platform projects, note that the ENTER key is commonly used on PCs and Unix and the RETURN key is used instead on Macintosh.
   * Check for both ENTER and RETURN to make sure your program will work for all platforms.<br><br>Because of how operating systems handle key repeats, holding down a key may cause multiple calls to keyPressed() (and keyReleased() as well).
   * The rate of repeat is set by the operating system and how each computer is configured.
   * =advanced
   *
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
   * @see PApplet#key
   * @see PApplet#keyCode
   * @see PApplet#keyPressed
   * @see PApplet#keyReleased()
   * @webref input:keyboard
   */
  public void keyPressed() { }


  /**
   * The <b>keyReleased()</b> function is called once every time a key is released. The key that was released will be stored in the <b>key</b> variable. See <b>key</b> and <b>keyReleased</b> for more information.
   *
   * @see PApplet#key
   * @see PApplet#keyCode
   * @see PApplet#keyPressed
   * @see PApplet#keyPressed()
   * @webref input:keyboard
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
   * Returns the number of milliseconds (thousandths of a second) since starting an applet. This information is often used for timing animation sequences.
   *
   * =advanced
   * <P>
   * This is a function, rather than a variable, because it may
   * change multiple times per frame.
   *
   * @webref input:time_date
   * @see processing.core.PApplet#second()
   * @see processing.core.PApplet#minute()
   * @see processing.core.PApplet#hour()
   * @see processing.core.PApplet#day()
   * @see processing.core.PApplet#month()
   * @see processing.core.PApplet#year()
   *
   */
  public int millis() {
    return (int) (System.currentTimeMillis() - millisOffset);
  }

  /** Seconds position of the current time.
   *
   * @webref input:time_date
   * @see processing.core.PApplet#millis()
   * @see processing.core.PApplet#minute()
   * @see processing.core.PApplet#hour()
   * @see processing.core.PApplet#day()
   * @see processing.core.PApplet#month()
   * @see processing.core.PApplet#year()
   * */
  static public int second() {
    return Calendar.getInstance().get(Calendar.SECOND);
  }

  /**
   * Processing communicates with the clock on your computer. The <b>minute()</b> function returns the current minute as a value from 0 - 59.
   *
   * @webref input:time_date
   * @see processing.core.PApplet#millis()
   * @see processing.core.PApplet#second()
   * @see processing.core.PApplet#hour()
   * @see processing.core.PApplet#day()
   * @see processing.core.PApplet#month()
   * @see processing.core.PApplet#year()
   *
   * */
  static public int minute() {
    return Calendar.getInstance().get(Calendar.MINUTE);
  }

  /**
   * Processing communicates with the clock on your computer. The <b>hour()</b> function returns the current hour as a value from 0 - 23.
   * =advanced
   * Hour position of the current time in international format (0-23).
   * <P>
   * To convert this value to American time: <BR>
   * <PRE>int yankeeHour = (hour() % 12);
   * if (yankeeHour == 0) yankeeHour = 12;</PRE>
   *
   * @webref input:time_date
   * @see processing.core.PApplet#millis()
   * @see processing.core.PApplet#second()
   * @see processing.core.PApplet#minute()
   * @see processing.core.PApplet#day()
   * @see processing.core.PApplet#month()
   * @see processing.core.PApplet#year()
   *
   */
  static public int hour() {
    return Calendar.getInstance().get(Calendar.HOUR_OF_DAY);
  }

  /**
   * Processing communicates with the clock on your computer. The <b>day()</b> function returns the current day as a value from 1 - 31.
   * =advanced
   * Get the current day of the month (1 through 31).
   * <P>
   * If you're looking for the day of the week (M-F or whatever)
   * or day of the year (1..365) then use java's Calendar.get()
   *
   * @webref input:time_date
   * @see processing.core.PApplet#millis()
   * @see processing.core.PApplet#second()
   * @see processing.core.PApplet#minute()
   * @see processing.core.PApplet#hour()
   * @see processing.core.PApplet#month()
   * @see processing.core.PApplet#year()
   */
  static public int day() {
    return Calendar.getInstance().get(Calendar.DAY_OF_MONTH);
  }

  /**
   * Processing communicates with the clock on your computer. The <b>month()</b> function returns the current month as a value from 1 - 12.
   *
   * @webref input:time_date
   * @see processing.core.PApplet#millis()
   * @see processing.core.PApplet#second()
   * @see processing.core.PApplet#minute()
   * @see processing.core.PApplet#hour()
   * @see processing.core.PApplet#day()
   * @see processing.core.PApplet#year()
   */
  static public int month() {
    // months are number 0..11 so change to colloquial 1..12
    return Calendar.getInstance().get(Calendar.MONTH) + 1;
  }

  /**
   * Processing communicates with the clock on your computer.
   * The <b>year()</b> function returns the current year as an integer (2003, 2004, 2005, etc).
   *
   * @webref input:time_date
   * @see processing.core.PApplet#millis()
   * @see processing.core.PApplet#second()
   * @see processing.core.PApplet#minute()
   * @see processing.core.PApplet#hour()
   * @see processing.core.PApplet#day()
   * @see processing.core.PApplet#month()
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
    if (frameCount != 0) {
      if (napTime > 0) {
        try {
          Thread.sleep(napTime);
        } catch (InterruptedException e) { }
      }
    }
  }


  /**
   * Specifies the number of frames to be displayed every second.
   * If the processor is not fast enough to maintain the specified rate, it will not be achieved.
   * For example, the function call <b>frameRate(30)</b> will attempt to refresh 30 times a second.
   * It is recommended to set the frame rate within <b>setup()</b>. The default rate is 60 frames per second.
   *  =advanced
   * Set a target frameRate. This will cause delay() to be called
   * after each frame so that the sketch synchronizes to a particular speed.
   * Note that this only sets the maximum frame rate, it cannot be used to
   * make a slow sketch go faster. Sketches have no default frame rate
   * setting, and will attempt to use maximum processor power to achieve
   * maximum speed.
   * @webref environment
   * @param newRateTarget number of frames per second
   * @see PApplet#delay(int)
   */
  public void frameRate(float newRateTarget) {
    frameRateTarget = newRateTarget;
    frameRatePeriod = (long) (1000000000.0 / frameRateTarget);
  }


  //////////////////////////////////////////////////////////////


  /**
   * Reads the value of a param.
   * Values are always read as a String so if you want them to be an integer or other datatype they must be converted.
   * The <b>param()</b> function will only work in a web browser.
   * The function should be called inside <b>setup()</b>,
   * otherwise the applet may not yet be initialized and connected to its parent web browser.
   *
   * @webref input:web
   * @usage Web
   *
   * @param what name of the param to read
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
   * Displays message in the browser's status area. This is the text area in the lower left corner of the browser.
   * The <b>status()</b> function will only work when the Processing program is running in a web browser.
   * =advanced
   * Show status in the status bar of a web browser, or in the
   * System.out console. Eventually this might show status in the
   * p5 environment itself, rather than relying on the console.
   *
   * @webref input:web
   * @usage Web
   * @param what any valid String
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
   * Links to a webpage either in the same window or in a new window. The complete URL must be specified.
   * =advanced
   * Link to an external page without all the muss.
   * <P>
   * When run with an applet, uses the browser to open the url,
   * for applications, attempts to launch a browser with the url.
   * <P>
   * Works on Mac OS X and Windows. For Linux, use:
   * <PRE>open(new String[] { "firefox", url });</PRE>
   * or whatever you want as your browser, since Linux doesn't
   * yet have a standard method for launching URLs.
   *
   * @webref input:web
   * @param url complete url as a String in quotes
   * @param frameTitle name of the window to load the URL as a string in quotes
   *
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

        } else if (platform == MACOSX) {
          //com.apple.mrj.MRJFileUtils.openURL(url);
          try {
//            Class<?> mrjFileUtils = Class.forName("com.apple.mrj.MRJFileUtils");
//            Method openMethod =
//              mrjFileUtils.getMethod("openURL", new Class[] { String.class });
            Class<?> eieio = Class.forName("com.apple.eio.FileManager");
            Method openMethod =
              eieio.getMethod("openURL", new Class[] { String.class });
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
   * Attempts to open an application or file using your platform's launcher. The <b>file</b> parameter is a String specifying the file name and location. The location parameter must be a full path name, or the name of an executable in the system's PATH. In most cases, using a full path is the best option, rather than relying on the system PATH. Be sure to make the file executable before attempting to open it (chmod +x).
   * <br><br>
   * The <b>args</b> parameter is a String or String array which is passed to the command line. If you have multiple parameters, e.g. an application and a document, or a command with multiple switches, use the version that takes a String array, and place each individual item in a separate element.
   * <br><br>
   * If args is a String (not an array), then it can only be a single file or application with no parameters. It's not the same as executing that String using a shell. For instance, open("jikes -help") will not work properly.
   * <br><br>
   * This function behaves differently on each platform. On Windows, the parameters are sent to the Windows shell via "cmd /c". On Mac OS X, the "open" command is used (type "man open" in Terminal.app for documentation). On Linux, it first tries gnome-open, then kde-open, but if neither are available, it sends the command to the shell without any alterations.
   * <br><br>
   * For users familiar with Java, this is not quite the same as Runtime.exec(), because the launcher command is prepended. Instead, the <b>exec(String[])</b> function is a shortcut for Runtime.getRuntime.exec(String[]).
   *
   * @webref input:files
   * @param filename name of the file
   * @usage Application
   */
  static public void open(String filename) {
    open(new String[] { filename });
  }


  static String openLauncher;

  /**
   * Launch a process using a platforms shell. This version uses an array
   * to make it easier to deal with spaces in the individual elements.
   * (This avoids the situation of trying to put single or double quotes
   * around different bits).
   *
   * @param list of commands passed to the command line
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


  //////////////////////////////////////////////////////////////


  /**
   * Function for an applet/application to kill itself and
   * display an error. Mostly this is here to be improved later.
   */
  public void die(String what) {
    stop();
    throw new RuntimeException(what);
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
      exit2();

    } else if (looping) {
      // stop() will be called as the thread exits
      finished = true;
      // tell the code to call exit2() to do a System.exit()
      // once the next draw() has completed
      exitCalled = true;

    } else if (!looping) {
      // if not looping, need to call stop explicitly,
      // because the main thread will be sleeping
      stop();

      // now get out
      exit2();
    }
  }


  void exit2() {
    try {
      System.exit(0);
    } catch (SecurityException e) {
      // don't care about applet security exceptions
    }
  }



  //////////////////////////////////////////////////////////////


  public void method(String name) {
//    final Object o = this;
//    final Class<?> c = getClass();
    try {
      Method method = getClass().getMethod(name, new Class[] {});
      method.invoke(this, new Object[] { });

    } catch (IllegalArgumentException e) {
      e.printStackTrace();
    } catch (IllegalAccessException e) {
      e.printStackTrace();
    } catch (InvocationTargetException e) {
      e.getTargetException().printStackTrace();
    } catch (NoSuchMethodException nsme) {
      System.err.println("There is no public " + name + "() method " +
                         "in the class " + getClass().getName());
    } catch (Exception e) {
      e.printStackTrace();
    }
  }


  public void thread(final String name) {
    Thread later = new Thread() {
      public void run() {
        method(name);
      }
    };
    later.start();
  }


  /*
  public void thread(String name) {
    final Object o = this;
    final Class<?> c = getClass();
    try {
      final Method method = c.getMethod(name, new Class[] {});
      Thread later = new Thread() {
        public void run() {
          try {
            method.invoke(o, new Object[] { });

          } catch (IllegalArgumentException e) {
            e.printStackTrace();
          } catch (IllegalAccessException e) {
            e.printStackTrace();
          } catch (InvocationTargetException e) {
            e.getTargetException().printStackTrace();
          }
        }
      };
      later.start();

    } catch (NoSuchMethodException nsme) {
      System.err.println("There is no " + name + "() method " +
                         "in the class " + getClass().getName());

    } catch (Exception e) {
      e.printStackTrace();
    }
  }
  */



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
    try {
      g.save(savePath("screen-" + nf(frameCount, 4) + ".tif"));
    } catch (SecurityException se) {
      System.err.println("Can't use saveFrame() when running in a browser, " +
                         "unless using a signed applet.");
    }
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
    try {
      g.save(savePath(insertFrame(what)));
    } catch (SecurityException se) {
      System.err.println("Can't use saveFrame() when running in a browser, " +
                         "unless using a signed applet.");
    }
  }


  /**
   * Check a string for #### signs to see if the frame number should be
   * inserted. Used for functions like saveFrame() and beginRecord() to
   * replace the # marks with the frame number. If only one # is used,
   * it will be ignored, under the assumption that it's probably not
   * intended to be the frame number.
   */
  protected String insertFrame(String what) {
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

  // CURSOR

  //


  int cursorType = ARROW; // cursor type
  boolean cursorVisible = true; // cursor visibility flag
  PImage invisibleCursor;


  /**
   * Set the cursor type
   * @param cursorType either ARROW, CROSS, HAND, MOVE, TEXT, WAIT
   */
  public void cursor(int cursorType) {
    setCursor(Cursor.getPredefinedCursor(cursorType));
    cursorVisible = true;
    this.cursorType = cursorType;
  }


  /**
   * Replace the cursor with the specified PImage. The x- and y-
   * coordinate of the center will be the center of the image.
   */
  public void cursor(PImage image) {
    cursor(image, image.width/2, image.height/2);
  }


  /**
   * Sets the cursor to a predefined symbol, an image, or turns it on if already hidden.
   * If you are trying to set an image as the cursor, it is recommended to make the size 16x16 or 32x32 pixels.
   * It is not possible to load an image as the cursor if you are exporting your program for the Web.
   * The values for parameters <b>x</b> and <b>y</b> must be less than the dimensions of the image.
   * =advanced
   * Set a custom cursor to an image with a specific hotspot.
   * Only works with JDK 1.2 and later.
   * Currently seems to be broken on Java 1.4 for Mac OS X
   * <P>
   * Based on code contributed by Amit Pitaru, plus additional
   * code to handle Java versions via reflection by Jonathan Feinberg.
   * Reflection removed for release 0128 and later.
   * @webref environment
   * @see       PApplet#noCursor()
   * @param image       any variable of type PImage
   * @param hotspotX    the horizonal active spot of the cursor
   * @param hotspotY    the vertical active spot of the cursor
   */
  public void cursor(PImage image, int hotspotX, int hotspotY) {
    // don't set this as cursor type, instead use cursor_type
    // to save the last cursor used in case cursor() is called
    //cursor_type = Cursor.CUSTOM_CURSOR;
    Image jimage =
      createImage(new MemoryImageSource(image.width, image.height,
                                        image.pixels, 0, image.width));
    Point hotspot = new Point(hotspotX, hotspotY);
    Toolkit tk = Toolkit.getDefaultToolkit();
    Cursor cursor = tk.createCustomCursor(jimage, hotspot, "Custom Cursor");
    setCursor(cursor);
    cursorVisible = true;
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
   * Hides the cursor from view. Will not work when running the program in a web browser.
   * =advanced
   * Hide the cursor by creating a transparent image
   * and using it as a custom cursor.
   * @webref environment
   * @see PApplet#cursor()
   * @usage Application
   */
  public void noCursor() {
    if (!cursorVisible) return;  // don't hide if already hidden.

    if (invisibleCursor == null) {
      invisibleCursor = new PImage(16, 16, ARGB);
    }
    // was formerly 16x16, but the 0x0 was added by jdf as a fix
    // for macosx, which wasn't honoring the invisible cursor
    cursor(invisibleCursor, 8, 8);
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

  /*
  static public final double max(double a, double b) {
    return (a > b) ? a : b;
  }
  */


  static public final int max(int a, int b, int c) {
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
  }

  static public final float max(float a, float b, float c) {
    return (a > b) ? ((a > c) ? a : c) : ((b > c) ? b : c);
  }


  /**
   * Find the maximum value in an array.
   * Throws an ArrayIndexOutOfBoundsException if the array is length 0.
   * @param list the source array
   * @return The maximum value
   */
  static public final int max(int[] list) {
    if (list.length == 0) {
      throw new ArrayIndexOutOfBoundsException(ERROR_MIN_MAX);
    }
    int max = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] > max) max = list[i];
    }
    return max;
  }

  /**
   * Find the maximum value in an array.
   * Throws an ArrayIndexOutOfBoundsException if the array is length 0.
   * @param list the source array
   * @return The maximum value
   */
  static public final float max(float[] list) {
    if (list.length == 0) {
      throw new ArrayIndexOutOfBoundsException(ERROR_MIN_MAX);
    }
    float max = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] > max) max = list[i];
    }
    return max;
  }


  /**
   * Find the maximum value in an array.
   * Throws an ArrayIndexOutOfBoundsException if the array is length 0.
   * @param list the source array
   * @return The maximum value
   */
  /*
  static public final double max(double[] list) {
    if (list.length == 0) {
      throw new ArrayIndexOutOfBoundsException(ERROR_MIN_MAX);
    }
    double max = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] > max) max = list[i];
    }
    return max;
  }
  */


  static public final int min(int a, int b) {
    return (a < b) ? a : b;
  }

  static public final float min(float a, float b) {
    return (a < b) ? a : b;
  }

  /*
  static public final double min(double a, double b) {
    return (a < b) ? a : b;
  }
  */


  static public final int min(int a, int b, int c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
  }

  static public final float min(float a, float b, float c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
  }

  /*
  static public final double min(double a, double b, double c) {
    return (a < b) ? ((a < c) ? a : c) : ((b < c) ? b : c);
  }
  */


  /**
   * Find the minimum value in an array.
   * Throws an ArrayIndexOutOfBoundsException if the array is length 0.
   * @param list the source array
   * @return The minimum value
   */
  static public final int min(int[] list) {
    if (list.length == 0) {
      throw new ArrayIndexOutOfBoundsException(ERROR_MIN_MAX);
    }
    int min = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] < min) min = list[i];
    }
    return min;
  }


  /**
   * Find the minimum value in an array.
   * Throws an ArrayIndexOutOfBoundsException if the array is length 0.
   * @param list the source array
   * @return The minimum value
   */
  static public final float min(float[] list) {
    if (list.length == 0) {
      throw new ArrayIndexOutOfBoundsException(ERROR_MIN_MAX);
    }
    float min = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] < min) min = list[i];
    }
    return min;
  }


  /**
   * Find the minimum value in an array.
   * Throws an ArrayIndexOutOfBoundsException if the array is length 0.
   * @param list the source array
   * @return The minimum value
   */
  /*
  static public final double min(double[] list) {
    if (list.length == 0) {
      throw new ArrayIndexOutOfBoundsException(ERROR_MIN_MAX);
    }
    double min = list[0];
    for (int i = 1; i < list.length; i++) {
      if (list[i] < min) min = list[i];
    }
    return min;
  }
  */

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


  /*
  static public final double map(double value,
                                 double istart, double istop,
                                 double ostart, double ostop) {
    return ostart + (ostop - ostart) * ((value - istart) / (istop - istart));
  }
  */



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
   */
  public PImage loadImage(String filename) {
    return loadImage(filename, null);
  }


  /**
   * Loads an image into a variable of type <b>PImage</b>. Four types of images ( <b>.gif</b>, <b>.jpg</b>, <b>.tga</b>, <b>.png</b>) images may be loaded. To load correctly, images must be located in the data directory of the current sketch. In most cases, load all images in <b>setup()</b> to preload them at the start of the program. Loading images inside <b>draw()</b> will reduce the speed of a program.
   * <br><br>The <b>filename</b> parameter can also be a URL to a file found online. For security reasons, a Processing sketch found online can only download files from the same server from which it came. Getting around this restriction requires a <a href="http://processing.org/hacks/doku.php?id=hacks:signapplet">signed applet</a>.
   * <br><br>The <b>extension</b> parameter is used to determine the image type in cases where the image filename does not end with a proper extension. Specify the extension as the second parameter to <b>loadImage()</b>, as shown in the third example on this page.
   * <br><br>If an image is not loaded successfully, the <b>null</b> value is returned and an error message will be printed to the console. The error message does not halt the program, however the null value may cause a NullPointerException if your code does not check whether the value returned from <b>loadImage()</b> is null.<br><br>Depending on the type of error, a <b>PImage</b> object may still be returned, but the width and height of the image will be set to -1. This happens if bad image data is returned or cannot be decoded properly. Sometimes this happens with image URLs that produce a 403 error or that redirect to a password prompt, because <b>loadImage()</b> will attempt to interpret the HTML as image data.
   *
   * =advanced
   * Identical to loadImage, but allows you to specify the type of
   * image by its extension. Especially useful when downloading from
   * CGI scripts.
   * <br/> <br/>
   * Use 'unknown' as the extension to pass off to the default
   * image loader that handles gif, jpg, and png.
   *
   * @webref image:loading_displaying
   * @param filename name of file to load, can be .gif, .jpg, .tga, or a handful of other image types depending on your platform.
   * @param extension the type of image to load, for example "png", "gif", "jpg"
   *
   * @see processing.core.PImage
   * @see processing.core.PApplet#image(PImage, float, float, float, float)
   * @see processing.core.PApplet#imageMode(int)
   * @see processing.core.PApplet#background(float, float, float)
   */
  public PImage loadImage(String filename, String extension) {
    if (extension == null) {
      String lower = filename.toLowerCase();
      int dot = filename.lastIndexOf('.');
      if (dot == -1) {
        extension = "unknown";  // no extension found
      }
      extension = lower.substring(dot + 1);

      // check for, and strip any parameters on the url, i.e.
      // filename.jpg?blah=blah&something=that
      int question = extension.indexOf('?');
      if (question != -1) {
        extension = extension.substring(0, question);
      }
    }

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

    if (extension.equals("tif") || extension.equals("tiff")) {
      byte bytes[] = loadBytes(filename);
      return (bytes == null) ? null : PImage.loadTIFF(bytes);
    }

    // For jpeg, gif, and png, load them using createImage(),
    // because the javax.imageio code was found to be much slower, see
    // <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=392">Bug 392</A>.
    try {
      if (extension.equals("jpg") || extension.equals("jpeg") ||
          extension.equals("gif") || extension.equals("png") ||
          extension.equals("unknown")) {
        byte bytes[] = loadBytes(filename);
        if (bytes == null) {
          return null;
        } else {
          Image awtImage = Toolkit.getDefaultToolkit().createImage(bytes);
          PImage image = loadImageMT(awtImage);
          if (image.width == -1) {
            System.err.println("The file " + filename +
                               " contains bad image data, or may not be an image.");
          }
          // if it's a .gif image, test to see if it has transparency
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

    if (loadImageFormats == null) {
      loadImageFormats = ImageIO.getReaderFormatNames();
    }
    if (loadImageFormats != null) {
      for (int i = 0; i < loadImageFormats.length; i++) {
        if (extension.equals(loadImageFormats[i])) {
          return loadImageIO(filename);
        }
      }
    }

    // failed, could not load image after all those attempts
    System.err.println("Could not find a method to load " + filename);
    return null;
  }

  public PImage requestImage(String filename) {
    return requestImage(filename, null);
  }


  /**
   * This function load images on a separate thread so that your sketch does not freeze while images load during <b>setup()</b>. While the image is loading, its width and height will be 0. If an error occurs while loading the image, its width and height will be set to -1. You'll know when the image has loaded properly because its width and height will be greater than 0. Asynchronous image loading (particularly when downloading from a server) can dramatically improve performance.<br><br>
   * The <b>extension</b> parameter is used to determine the image type in cases where the image filename does not end with a proper extension. Specify the extension as the second parameter to <b>requestImage()</b>.
   *
   * @webref image:loading_displaying
   * @param filename name of file to load, can be .gif, .jpg, .tga, or a handful of other image types depending on your platform
   * @param extension the type of image to load, for example "png", "gif", "jpg"
   *
   * @see processing.core.PApplet#loadImage(String, String)
   * @see processing.core.PImage
   */
  public PImage requestImage(String filename, String extension) {
    PImage vessel = createImage(0, 0, ARGB);
    AsyncImageLoader ail =
      new AsyncImageLoader(filename, extension, vessel);
    ail.start();
    return vessel;
  }


  /**
   * By trial and error, four image loading threads seem to work best when
   * loading images from online. This is consistent with the number of open
   * connections that web browsers will maintain. The variable is made public
   * (however no accessor has been added since it's esoteric) if you really
   * want to have control over the value used. For instance, when loading local
   * files, it might be better to only have a single thread (or two) loading
   * images so that you're disk isn't simply jumping around.
   */
  public int requestImageMax = 4;
  volatile int requestImageCount;

  class AsyncImageLoader extends Thread {
    String filename;
    String extension;
    PImage vessel;

    public AsyncImageLoader(String filename, String extension, PImage vessel) {
      this.filename = filename;
      this.extension = extension;
      this.vessel = vessel;
    }

    public void run() {
      while (requestImageCount == requestImageMax) {
        try {
          Thread.sleep(10);
        } catch (InterruptedException e) { }
      }
      requestImageCount++;

      PImage actual = loadImage(filename, extension);

      // An error message should have already printed
      if (actual == null) {
        vessel.width = -1;
        vessel.height = -1;

      } else {
        vessel.width = actual.width;
        vessel.height = actual.height;
        vessel.format = actual.format;
        vessel.pixels = actual.pixels;
      }
      requestImageCount--;
    }
  }


  /**
   * Load an AWT image synchronously by setting up a MediaTracker for
   * a single image, and blocking until it has loaded.
   */
  protected PImage loadImageMT(Image awtImage) {
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
   * Use Java 1.4 ImageIO methods to load an image.
   */
  protected PImage loadImageIO(String filename) {
    InputStream stream = createInput(filename);
    if (stream == null) {
      System.err.println("The image " + filename + " could not be found.");
      return null;
    }

    try {
      BufferedImage bi = ImageIO.read(stream);
      PImage outgoing = new PImage(bi.getWidth(), bi.getHeight());
      outgoing.parent = this;

      bi.getRGB(0, 0, outgoing.width, outgoing.height,
                outgoing.pixels, 0, outgoing.width);

      // check the alpha for this image
      // was gonna call getType() on the image to see if RGB or ARGB,
      // but it's not actually useful, since gif images will come through
      // as TYPE_BYTE_INDEXED, which means it'll still have to check for
      // the transparency. also, would have to iterate through all the other
      // types and guess whether alpha was in there, so.. just gonna stick
      // with the old method.
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
    InputStream is = createInput(filename);
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

    // where "reversed" means upper-left corner (normal for most of
    // the modernized world, but "reversed" for the tga spec)
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

      if (!reversed) {
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

  // SHAPE I/O


  /**
   * Loads vector shapes into a variable of type <b>PShape</b>. Currently, only SVG files may be loaded.
   * To load correctly, the file must be located in the data directory of the current sketch.
   * In most cases, <b>loadShape()</b> should be used inside <b>setup()</b> because loading shapes inside <b>draw()</b> will reduce the speed of a sketch.
   * <br><br>
   * The <b>filename</b> parameter can also be a URL to a file found online.
   * For security reasons, a Processing sketch found online can only download files from the same server from which it came.
   * Getting around this restriction requires a <a href="http://processing.org/hacks/doku.php?id=hacks:signapplet">signed applet</a>.
   * <br><br>
   * If a shape is not loaded successfully, the <b>null</b> value is returned and an error message will be printed to the console.
   * The error message does not halt the program, however the null value may cause a NullPointerException if your code does not check whether the value returned from <b>loadShape()</b> is null.
   *
   * @webref shape:loading_displaying
   * @see PShape
   * @see PApplet#shape(PShape)
   * @see PApplet#shapeMode(int)
   */
  public PShape loadShape(String filename) {
    if (filename.toLowerCase().endsWith(".svg")) {
      return new PShapeSVG(this, filename);
    }
    return null;
  }



  //////////////////////////////////////////////////////////////

  // FONT I/O


  public PFont loadFont(String filename) {
    try {
      InputStream input = createInput(filename);
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
    String lowerName = name.toLowerCase();
    Font baseFont = null;

    try {
      if (lowerName.endsWith(".otf") || lowerName.endsWith(".ttf")) {
        InputStream stream = createInput(name);
        if (stream == null) {
          System.err.println("The font \"" + name + "\" " +
                             "is missing or inaccessible, make sure " +
                             "the URL is valid or that the file has been " +
                             "added to your sketch and is readable.");
          return null;
        }
        baseFont = Font.createFont(Font.TRUETYPE_FONT, createInput(name));

      } else {
        //baseFont = new Font(name, Font.PLAIN, 1);
        baseFont = PFont.findFont(name);
      }
    } catch (Exception e) {
      System.err.println("Problem using createFont() with " + name);
      e.printStackTrace();
    }
    return new PFont(baseFont.deriveFont(size), smooth, charset);
  }



  //////////////////////////////////////////////////////////////

  // FILE/FOLDER SELECTION


  public File selectedFile;
  protected Frame parentFrame;


  protected void checkParentFrame() {
    if (parentFrame == null) {
      Component comp = getParent();
      while (comp != null) {
        if (comp instanceof Frame) {
          parentFrame = (Frame) comp;
          break;
        }
        comp = comp.getParent();
      }
      // Who you callin' a hack?
      if (parentFrame == null) {
        parentFrame = new Frame();
      }
    }
  }


  /**
   * Open a platform-specific file chooser dialog to select a file for input.
   * @return full path to the selected file, or null if no selection.
   */
  public String selectInput() {
    return selectInput("Select a file...");
  }


  /**
   * Opens a platform-specific file chooser dialog to select a file for input. This function returns the full path to the selected file as a <b>String</b>, or <b>null</b> if no selection.
   *
   * @webref input:files
   * @param prompt message you want the user to see in the file chooser
   * @return full path to the selected file, or null if canceled.
   *
   * @see processing.core.PApplet#selectOutput(String)
   * @see processing.core.PApplet#selectFolder(String)
   */
  public String selectInput(String prompt) {
    return selectFileImpl(prompt, FileDialog.LOAD);
  }


  /**
   * Open a platform-specific file save dialog to select a file for output.
   * @return full path to the file entered, or null if canceled.
   */
  public String selectOutput() {
    return selectOutput("Save as...");
  }


  /**
   * Open a platform-specific file save dialog to create of select a file for output.
   * This function returns the full path to the selected file as a <b>String</b>, or <b>null</b> if no selection.
   * If you select an existing file, that file will be replaced.
   * Alternatively, you can navigate to a folder and create a new file to write to.
   *
   * @param prompt message you want the user to see in the file chooser
   * @return full path to the file entered, or null if canceled.
   *
   * @webref input:files
   * @see processing.core.PApplet#selectInput(String)
   * @see processing.core.PApplet#selectFolder(String)
   */
  public String selectOutput(String prompt) {
    return selectFileImpl(prompt, FileDialog.SAVE);
  }


  protected String selectFileImpl(final String prompt, final int mode) {
    checkParentFrame();

    try {
      SwingUtilities.invokeAndWait(new Runnable() {
        public void run() {
          FileDialog fileDialog =
            new FileDialog(parentFrame, prompt, mode);
          fileDialog.setVisible(true);
          String directory = fileDialog.getDirectory();
          String filename = fileDialog.getFile();
          selectedFile =
            (filename == null) ? null : new File(directory, filename);
        }
      });
      return (selectedFile == null) ? null : selectedFile.getAbsolutePath();

    } catch (Exception e) {
      e.printStackTrace();
      return null;
    }
  }


  public String selectFolder() {
    return selectFolder("Select a folder...");
  }


  /**
   * Opens a platform-specific file chooser dialog to select a folder for input.
   * This function returns the full path to the selected folder as a <b>String</b>, or <b>null</b> if no selection.
   *
   * @webref input:files
   * @param prompt message you want the user to see in the file chooser
   * @return full path to the selected folder, or null if no selection.
   *
   * @see processing.core.PApplet#selectOutput(String)
   * @see processing.core.PApplet#selectInput(String)
   */
  public String selectFolder(final String prompt) {
    checkParentFrame();

    try {
      SwingUtilities.invokeAndWait(new Runnable() {
        public void run() {
          if (platform == MACOSX) {
            FileDialog fileDialog =
              new FileDialog(parentFrame, prompt, FileDialog.LOAD);
            System.setProperty("apple.awt.fileDialogForDirectories", "true");
            fileDialog.setVisible(true);
            System.setProperty("apple.awt.fileDialogForDirectories", "false");
            String filename = fileDialog.getFile();
            selectedFile = (filename == null) ? null :
              new File(fileDialog.getDirectory(), fileDialog.getFile());
          } else {
            JFileChooser fileChooser = new JFileChooser();
            fileChooser.setDialogTitle(prompt);
            fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

            int returned = fileChooser.showOpenDialog(parentFrame);
            System.out.println(returned);
            if (returned == JFileChooser.CANCEL_OPTION) {
              selectedFile = null;
            } else {
              selectedFile = fileChooser.getSelectedFile();
            }
          }
        }
      });
      return (selectedFile == null) ? null : selectedFile.getAbsolutePath();

    } catch (Exception e) {
      e.printStackTrace();
      return null;
    }
  }



  //////////////////////////////////////////////////////////////

  // READERS AND WRITERS


  /**
   * I want to read lines from a file. I have RSI from typing these
   * eight lines of code so many times.
   */
  public BufferedReader createReader(String filename) {
    try {
      InputStream is = createInput(filename);
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
      InputStream is = new FileInputStream(file);
      if (file.getName().toLowerCase().endsWith(".gz")) {
        is = new GZIPInputStream(is);
      }
      return createReader(is);

    } catch (Exception e) {
      if (file == null) {
        throw new RuntimeException("File passed to createReader() was null");
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
    InputStreamReader isr = null;
    try {
      isr = new InputStreamReader(input, "UTF-8");
    } catch (UnsupportedEncodingException e) { }  // not gonna happen
    return new BufferedReader(isr);
  }


  /**
   * I want to print lines to a file. Why can't I?
   */
  public PrintWriter createWriter(String filename) {
    return createWriter(saveFile(filename));
  }


  /**
   * I want to print lines to a file. I have RSI from typing these
   * eight lines of code so many times.
   */
  static public PrintWriter createWriter(File file) {
    try {
      createPath(file);  // make sure in-between folders exist
      OutputStream output = new FileOutputStream(file);
      if (file.getName().toLowerCase().endsWith(".gz")) {
        output = new GZIPOutputStream(output);
      }
      return createWriter(output);

    } catch (Exception e) {
      if (file == null) {
        throw new RuntimeException("File passed to createWriter() was null");
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
    try {
      OutputStreamWriter osw = new OutputStreamWriter(output, "UTF-8");
      return new PrintWriter(osw);
    } catch (UnsupportedEncodingException e) { }  // not gonna happen
    return null;
  }


  //////////////////////////////////////////////////////////////

  // FILE INPUT


  /**
   * @deprecated As of release 0136, use createInput() instead.
   */
  public InputStream openStream(String filename) {
    return createInput(filename);
  }


  /**
   * This is a method for advanced programmers to open a Java InputStream. The method is useful if you want to use the facilities provided by PApplet to easily open files from the data folder or from a URL, but want an InputStream object so that you can use other Java methods to take more control of how the stream is read.
   * <br><br>If the requested item doesn't exist, null is returned.
   * <br><br>In earlier releases, this method was called <b>openStream()</b>.
   * <br><br>If not online, this will also check to see if the user is asking for a file whose name isn't properly capitalized. If capitalization is different an error will be printed to the console. This helps prevent issues that appear when a sketch is exported to the web, where case sensitivity matters, as opposed to running from inside the Processing Development Environment on Windows or Mac OS, where case sensitivity is preserved but ignored.
   * <br><br>The filename passed in can be:<br>
   * - A URL, for instance openStream("http://processing.org/");<br>
   * - A file in the sketch's data folder<br>
   * - The full path to a file to be opened locally (when running as an application)
   * <br><br>
   * If the file ends with <b>.gz</b>, the stream will automatically be gzip decompressed. If you don't want the automatic decompression, use the related function <b>createInputRaw()</b>.
   *
   * =advanced
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
   *
   * @webref input:files
   * @see processing.core.PApplet#createOutput(String)
   * @see processing.core.PApplet#selectOutput(String)
   * @see processing.core.PApplet#selectInput(String)
   *
   * @param filename the name of the file to use as input
   *
   */
  public InputStream createInput(String filename) {
    InputStream input = createInputRaw(filename);
    if ((input != null) && filename.toLowerCase().endsWith(".gz")) {
      try {
        return new GZIPInputStream(input);
      } catch (IOException e) {
        e.printStackTrace();
        return null;
      }
    }
    return input;
  }


  /**
   * Call openStream() without automatic gzip decompression.
   */
  public InputStream createInputRaw(String filename) {
    InputStream stream = null;

    if (filename == null) return null;

    if (filename.length() == 0) {
      // an error will be called by the parent function
      //System.err.println("The filename passed to openStream() was empty.");
      return null;
    }

    // safe to check for this as a url first. this will prevent online
    // access logs from being spammed with GET /sketchfolder/http://blahblah
    if (filename.indexOf(":") != -1) {  // at least smells like URL
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
    }

    // Moved this earlier than the getResourceAsStream() checks, because
    // calling getResourceAsStream() on a directory lists its contents.
    // http://dev.processing.org/bugs/show_bug.cgi?id=716
    try {
      // First see if it's in a data folder. This may fail by throwing
      // a SecurityException. If so, this whole block will be skipped.
      File file = new File(dataPath(filename));
      if (!file.exists()) {
        // next see if it's just in the sketch folder
        file = new File(sketchPath, filename);
      }
      if (file.isDirectory()) {
        return null;
      }
      if (file.exists()) {
        try {
          // handle case sensitivity check
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
                                       filename + ". Rename the file " +
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

    // Using getClassLoader() prevents java from converting dots
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

    // When used with an online script, also need to check without the
    // data folder, in case it's not in a subfolder called 'data'.
    // http://dev.processing.org/bugs/show_bug.cgi?id=389
    stream = cl.getResourceAsStream(filename);
    if (stream != null) {
      String cn = stream.getClass().getName();
      if (!cn.equals("sun.plugin.cache.EmptyInputStream")) {
        return stream;
      }
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


  static public InputStream createInput(File file) {
    if (file == null) {
      throw new IllegalArgumentException("File passed to createInput() was null");
    }
    try {
      InputStream input = new FileInputStream(file);
      if (file.getName().toLowerCase().endsWith(".gz")) {
        return new GZIPInputStream(input);
      }
      return input;

    } catch (IOException e) {
      System.err.println("Could not createInput() for " + file);
      e.printStackTrace();
      return null;
    }
  }


  /**
   * Reads the contents of a file or url and places it in a byte array. If a file is specified, it must be located in the sketch's "data" directory/folder.
   * <br><br>The filename parameter can also be a URL to a file found online. For security reasons, a Processing sketch found online can only download files from the same server from which it came. Getting around this restriction requires a <a href="http://java.sun.com/developer/onlineTraining/Programming/JDCBook/signed.html">signed applet</a>.
   *
   * @webref input:files
   * @param filename name of a file in the data folder or a URL.
   *
   * @see processing.core.PApplet#loadStrings(String)
   * @see processing.core.PApplet#saveStrings(String, String[])
   * @see processing.core.PApplet#saveBytes(String, byte[])
   *
   */
  public byte[] loadBytes(String filename) {
    InputStream is = createInput(filename);
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


  static public byte[] loadBytes(File file) {
    InputStream is = createInput(file);
    return loadBytes(is);
  }


  static public String[] loadStrings(File file) {
    InputStream is = createInput(file);
    if (is != null) return loadStrings(is);
    return null;
  }


  /**
   * Reads the contents of a file or url and creates a String array of its individual lines. If a file is specified, it must be located in the sketch's "data" directory/folder.
   * <br><br>The filename parameter can also be a URL to a file found online. For security reasons, a Processing sketch found online can only download files from the same server from which it came. Getting around this restriction requires a <a href="http://java.sun.com/developer/onlineTraining/Programming/JDCBook/signed.html">signed applet</a>.
   * <br><br>If the file is not available or an error occurs, <b>null</b> will be returned and an error message will be printed to the console. The error message does not halt the program, however the null value may cause a NullPointerException if your code does not check whether the value returned is null.
   * <br><br>Starting with Processing release 0134, all files loaded and saved by the Processing API use UTF-8 encoding. In previous releases, the default encoding for your platform was used, which causes problems when files are moved to other platforms.
   *
   * =advanced
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
   *
   * @webref input:files
   * @param filename name of the file or url to load
   *
   * @see processing.core.PApplet#loadBytes(String)
   * @see processing.core.PApplet#saveStrings(String, String[])
   * @see processing.core.PApplet#saveBytes(String, byte[])
   */
  public String[] loadStrings(String filename) {
    InputStream is = createInput(filename);
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
        new BufferedReader(new InputStreamReader(input, "UTF-8"));

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
   * Similar to createInput() (formerly openStream), this creates a Java
   * OutputStream for a given filename or path. The file will be created in
   * the sketch folder, or in the same folder as an exported application.
   * <p/>
   * If the path does not exist, intermediate folders will be created. If an
   * exception occurs, it will be printed to the console, and null will be
   * returned.
   * <p/>
   * Future releases may also add support for handling HTTP POST via this
   * method (for better symmetry with createInput), however that's maybe a
   * little too clever (and then we'd have to add the same features to the
   * other file functions like createWriter). Who you callin' bloated?
   */
  public OutputStream createOutput(String filename) {
    return createOutput(saveFile(filename));
  }


  static public OutputStream createOutput(File file) {
    try {
      createPath(file);  // make sure the path exists
      FileOutputStream fos = new FileOutputStream(file);
      if (file.getName().toLowerCase().endsWith(".gz")) {
        return new GZIPOutputStream(fos);
      }
      return fos;

    } catch (IOException e) {
      e.printStackTrace();
    }
    return null;
  }


  /**
   * Save the contents of a stream to a file in the sketch folder.
   * This is basically saveBytes(blah, loadBytes()), but done
   * more efficiently (and with less confusing syntax).
   */
  public void saveStream(String targetFilename, String sourceLocation) {
    saveStream(saveFile(targetFilename), sourceLocation);
  }


  /**
   * Identical to the other saveStream(), but writes to a File
   * object, for greater control over the file location.
   * Note that unlike other api methods, this will not automatically
   * compress or uncompress gzip files.
   */
  public void saveStream(File targetFile, String sourceLocation) {
    saveStream(targetFile, createInputRaw(sourceLocation));
  }


  static public void saveStream(File targetFile, InputStream sourceStream) {
    File tempFile = null;
    try {
      File parentDir = targetFile.getParentFile();
      tempFile = File.createTempFile(targetFile.getName(), null, parentDir);

      BufferedInputStream bis = new BufferedInputStream(sourceStream, 16384);
      FileOutputStream fos = new FileOutputStream(tempFile);
      BufferedOutputStream bos = new BufferedOutputStream(fos);

      byte[] buffer = new byte[8192];
      int bytesRead;
      while ((bytesRead = bis.read(buffer)) != -1) {
        bos.write(buffer, 0, bytesRead);
      }

      bos.flush();
      bos.close();
      bos = null;

      if (!tempFile.renameTo(targetFile)) {
        System.err.println("Could not rename temporary file " +
                           tempFile.getAbsolutePath());
      }
    } catch (IOException e) {
      if (tempFile != null) {
        tempFile.delete();
      }
      e.printStackTrace();
    }
  }


  /**
   * Saves bytes to a file to inside the sketch folder.
   * The filename can be a relative path, i.e. "poo/bytefun.txt"
   * would save to a file named "bytefun.txt" to a subfolder
   * called 'poo' inside the sketch folder. If the in-between
   * subfolders don't exist, they'll be created.
   */
  public void saveBytes(String filename, byte buffer[]) {
    saveBytes(saveFile(filename), buffer);
  }


  /**
   * Saves bytes to a specific File location specified by the user.
   */
  static public void saveBytes(File file, byte buffer[]) {
    File tempFile = null;
    try {
      File parentDir = file.getParentFile();
      tempFile = File.createTempFile(file.getName(), null, parentDir);

      /*
      String filename = file.getAbsolutePath();
      createPath(filename);
      OutputStream output = new FileOutputStream(file);
      if (file.getName().toLowerCase().endsWith(".gz")) {
        output = new GZIPOutputStream(output);
      }
      */
      OutputStream output = createOutput(tempFile);
      saveBytes(output, buffer);
      output.close();
      output = null;

      if (!tempFile.renameTo(file)) {
        System.err.println("Could not rename temporary file " +
                           tempFile.getAbsolutePath());
      }

    } catch (IOException e) {
      System.err.println("error saving bytes to " + file);
      if (tempFile != null) {
        tempFile.delete();
      }
      e.printStackTrace();
    }
  }


  /**
   * Spews a buffer of bytes to an OutputStream.
   */
  static public void saveBytes(OutputStream output, byte buffer[]) {
    try {
      output.write(buffer);
      output.flush();

    } catch (IOException e) {
      e.printStackTrace();
    }
  }

  //

  public void saveStrings(String filename, String strings[]) {
    saveStrings(saveFile(filename), strings);
  }


  static public void saveStrings(File file, String strings[]) {
    saveStrings(createOutput(file), strings);
    /*
    try {
      String location = file.getAbsolutePath();
      createPath(location);
      OutputStream output = new FileOutputStream(location);
      if (file.getName().toLowerCase().endsWith(".gz")) {
        output = new GZIPOutputStream(output);
      }
      saveStrings(output, strings);
      output.close();

    } catch (IOException e) {
      e.printStackTrace();
    }
    */
  }


  static public void saveStrings(OutputStream output, String strings[]) {
    PrintWriter writer = createWriter(output);
    for (int i = 0; i < strings.length; i++) {
      writer.println(strings[i]);
    }
    writer.flush();
    writer.close();
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
      return where;
//      throw new RuntimeException("The applet was not inited properly, " +
//                                 "or security restrictions prevented " +
//                                 "it from determining its path.");
    }
    // isAbsolute() could throw an access exception, but so will writing
    // to the local disk using the sketch path, so this is safe here.
    // for 0120, added a try/catch anyways.
    try {
      if (new File(where).isAbsolute()) return where;
    } catch (Exception e) { }

    return sketchPath + File.separator + where;
  }


  public File sketchFile(String where) {
    return new File(sketchPath(where));
  }


  /**
   * Returns a path inside the applet folder to save to. Like sketchPath(),
   * but creates any in-between folders so that things save properly.
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
    if (where == null) return null;
    String filename = sketchPath(where);
    createPath(filename);
    return filename;
  }


  /**
   * Identical to savePath(), but returns a File object.
   */
  public File saveFile(String where) {
    return new File(savePath(where));
  }


  /**
   * Return a full path to an item in the data folder.
   * <p>
   * In this method, the data path is defined not as the applet's actual
   * data path, but a folder titled "data" in the sketch's working
   * directory. When running inside the PDE, this will be the sketch's
   * "data" folder. However, when exported (as application or applet),
   * sketch's data folder is exported as part of the applications jar file,
   * and it's not possible to read/write from the jar file in a generic way.
   * If you need to read data from the jar file, you should use other methods
   * such as createInput(), createReader(), or loadStrings().
   */
  public String dataPath(String where) {
    // isAbsolute() could throw an access exception, but so will writing
    // to the local disk using the sketch path, so this is safe here.
    if (new File(where).isAbsolute()) return where;

    return sketchPath + File.separator + "data" + File.separator + where;
  }


  /**
   * Return a full path to an item in the data folder as a File object.
   * See the dataPath() method for more information.
   */
  public File dataFile(String where) {
    return new File(dataPath(where));
  }


  /**
   * Takes a path and creates any in-between folders if they don't
   * already exist. Useful when trying to save to a subfolder that
   * may not actually exist.
   */
  static public void createPath(String path) {
    createPath(new File(path));
  }


  static public void createPath(File file) {
    try {
      String parent = file.getParent();
      if (parent != null) {
        File unit = new File(parent);
        if (!unit.exists()) unit.mkdirs();
      }
    } catch (SecurityException se) {
      System.err.println("You don't have permissions to create " +
                         file.getAbsolutePath());
    }
  }



  //////////////////////////////////////////////////////////////

  // SORT


  static public byte[] sort(byte what[]) {
    return sort(what, what.length);
  }


  static public byte[] sort(byte[] what, int count) {
    byte[] outgoing = new byte[what.length];
    System.arraycopy(what, 0, outgoing, 0, what.length);
    Arrays.sort(outgoing, 0, count);
    return outgoing;
  }


  static public char[] sort(char what[]) {
    return sort(what, what.length);
  }


  static public char[] sort(char[] what, int count) {
    char[] outgoing = new char[what.length];
    System.arraycopy(what, 0, outgoing, 0, what.length);
    Arrays.sort(outgoing, 0, count);
    return outgoing;
  }


  static public int[] sort(int what[]) {
    return sort(what, what.length);
  }


  static public int[] sort(int[] what, int count) {
    int[] outgoing = new int[what.length];
    System.arraycopy(what, 0, outgoing, 0, what.length);
    Arrays.sort(outgoing, 0, count);
    return outgoing;
  }


  static public float[] sort(float what[]) {
    return sort(what, what.length);
  }


  static public float[] sort(float[] what, int count) {
    float[] outgoing = new float[what.length];
    System.arraycopy(what, 0, outgoing, 0, what.length);
    Arrays.sort(outgoing, 0, count);
    return outgoing;
  }


  static public String[] sort(String what[]) {
    return sort(what, what.length);
  }


  static public String[] sort(String[] what, int count) {
    String[] outgoing = new String[what.length];
    System.arraycopy(what, 0, outgoing, 0, what.length);
    Arrays.sort(outgoing, 0, count);
    return outgoing;
  }



  //////////////////////////////////////////////////////////////

  // ARRAY UTILITIES


  /**
   * Calls System.arraycopy(), included here so that we can
   * avoid people needing to learn about the System object
   * before they can just copy an array.
   */
  static public void arrayCopy(Object src, int srcPosition,
                               Object dst, int dstPosition,
                               int length) {
    System.arraycopy(src, srcPosition, dst, dstPosition, length);
  }


  /**
   * Convenience method for arraycopy().
   * Identical to <CODE>arraycopy(src, 0, dst, 0, length);</CODE>
   */
  static public void arrayCopy(Object src, Object dst, int length) {
    System.arraycopy(src, 0, dst, 0, length);
  }


  /**
   * Shortcut to copy the entire contents of
   * the source into the destination array.
   * Identical to <CODE>arraycopy(src, 0, dst, 0, src.length);</CODE>
   */
  static public void arrayCopy(Object src, Object dst) {
    System.arraycopy(src, 0, dst, 0, Array.getLength(src));
  }

  //

  /**
   * @deprecated Use arrayCopy() instead.
   */
  static public void arraycopy(Object src, int srcPosition,
                               Object dst, int dstPosition,
                               int length) {
    System.arraycopy(src, srcPosition, dst, dstPosition, length);
  }

  /**
   * @deprecated Use arrayCopy() instead.
   */
  static public void arraycopy(Object src, Object dst, int length) {
    System.arraycopy(src, 0, dst, 0, length);
  }

  /**
   * @deprecated Use arrayCopy() instead.
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
    Class<?> type = list.getClass().getComponentType();
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

    // check whether item being spliced in is an array
    if (v.getClass().getName().charAt(0) == '[') {
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
    return subset(list, start, length - start);
  }

  static public Object subset(Object list, int start, int count) {
    Class<?> type = list.getClass().getComponentType();
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
    Class<?> type = a.getClass().getComponentType();
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
    Class<?> type = list.getClass().getComponentType();
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
   * Most commonly used to break up a String along a space or a tab
   * character.
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
   * Split a String on a specific delimiter. Unlike Java's String.split()
   * method, this does not parse the delimiter as a regexp because it's more
   * confusing than necessary, and String.split() is always available for
   * those who want regexp.
   */
  static public String[] split(String what, String delim) {
    ArrayList<String> items = new ArrayList<String>();
    int index;
    int offset = 0;
    while ((index = what.indexOf(delim, offset)) != -1) {
      items.add(what.substring(offset, index));
      offset = index + delim.length();
    }
    items.add(what.substring(offset));
    String[] outgoing = new String[items.size()];
    items.toArray(outgoing);
    return outgoing;
  }


  /**
   * Match a string with a regular expression, and returns the match as an
   * array. The first index is the matching expression, and array elements
   * [1] and higher represent each of the groups (sequences found in parens).
   *
   * This uses multiline matching (Pattern.MULTILINE) and dotall mode
   * (Pattern.DOTALL) by default, so that ^ and $ match the beginning and
   * end of any lines found in the source, and the . operator will also
   * pick up newline characters.
   */
  static public String[] match(String what, String regexp) {
    Pattern p = Pattern.compile(regexp, Pattern.MULTILINE | Pattern.DOTALL);
    Matcher m = p.matcher(what);
    if (m.find()) {
      int count = m.groupCount() + 1;
      String[] groups = new String[count];
      for (int i = 0; i < count; i++) {
        groups[i] = m.group(i);
      }
      return groups;
    }
    return null;
  }


  /**
   * Identical to match(), except that it returns an array of all matches in
   * the specified String, rather than just the first.
   */
  static public String[][] matchAll(String what, String regexp) {
    Pattern p = Pattern.compile(regexp, Pattern.MULTILINE | Pattern.DOTALL);
    Matcher m = p.matcher(what);
    ArrayList<String[]> results = new ArrayList<String[]>();
    int count = m.groupCount() + 1;
    while (m.find()) {
      String[] groups = new String[count];
      for (int i = 0; i < count; i++) {
        groups[i] = m.group(i);
      }
      results.add(groups);
    }
    if (results.isEmpty()) {
      return null;
    }
    String[][] matches = new String[results.size()][count];
    for (int i = 0; i < matches.length; i++) {
      matches[i] = (String[]) results.get(i);
    }
    return matches;
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
    return parseFloat(what, Float.NaN);
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
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x[i]);
    return s;
  }

  static final public String[] str(byte x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x[i]);
    return s;
  }

  static final public String[] str(char x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x[i]);
    return s;
  }

  static final public String[] str(int x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x[i]);
    return s;
  }

  static final public String[] str(float x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x[i]);
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
   *
   * @param gray number specifying value between white and black
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

  /**
   * Creates colors for storing in variables of the <b>color</b> datatype. The parameters are interpreted as RGB or HSB values depending on the current <b>colorMode()</b>. The default mode is RGB values from 0 to 255 and therefore, the function call <b>color(255, 204, 0)</b> will return a bright yellow color. More about how colors are stored can be found in the reference for the <a href="color_datatype.html">color</a> datatype.
   *
   * @webref color:creating_reading
   * @param x red or hue values relative to the current color range
   * @param y green or saturation values relative to the current color range
   * @param z blue or brightness values relative to the current color range
   * @param a alpha relative to current color range
   *
   * @see processing.core.PApplet#colorMode(int)
   * @ref color_datatype
   */
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


  /**
   * Set this sketch to communicate its state back to the PDE.
   * <p/>
   * This uses the stderr stream to write positions of the window
   * (so that it will be saved by the PDE for the next run) and
   * notify on quit. See more notes in the Worker class.
   */
  public void setupExternalMessages() {

    frame.addComponentListener(new ComponentAdapter() {
        public void componentMoved(ComponentEvent e) {
          Point where = ((Frame) e.getSource()).getLocation();
          System.err.println(PApplet.EXTERNAL_MOVE + " " +
                             where.x + " " + where.y);
          System.err.flush();  // doesn't seem to help or hurt
        }
      });

    frame.addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
//          System.err.println(PApplet.EXTERNAL_QUIT);
//          System.err.flush();  // important
//          System.exit(0);
          exit();  // don't quit, need to just shut everything down (0133)
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
   * GIF image of the Processing logo.
   */
  static public final byte[] ICON_IMAGE = {
    71, 73, 70, 56, 57, 97, 16, 0, 16, 0, -77, 0, 0, 0, 0, 0, -1, -1, -1, 12,
    12, 13, -15, -15, -14, 45, 57, 74, 54, 80, 111, 47, 71, 97, 62, 88, 117,
    1, 14, 27, 7, 41, 73, 15, 52, 85, 2, 31, 55, 4, 54, 94, 18, 69, 109, 37,
    87, 126, -1, -1, -1, 33, -7, 4, 1, 0, 0, 15, 0, 44, 0, 0, 0, 0, 16, 0, 16,
    0, 0, 4, 122, -16, -107, 114, -86, -67, 83, 30, -42, 26, -17, -100, -45,
    56, -57, -108, 48, 40, 122, -90, 104, 67, -91, -51, 32, -53, 77, -78, -100,
    47, -86, 12, 76, -110, -20, -74, -101, 97, -93, 27, 40, 20, -65, 65, 48,
    -111, 99, -20, -112, -117, -123, -47, -105, 24, 114, -112, 74, 69, 84, 25,
    93, 88, -75, 9, 46, 2, 49, 88, -116, -67, 7, -19, -83, 60, 38, 3, -34, 2,
    66, -95, 27, -98, 13, 4, -17, 55, 33, 109, 11, 11, -2, -128, 121, 123, 62,
    91, 120, -128, 127, 122, 115, 102, 2, 119, 0, -116, -113, -119, 6, 102,
    121, -108, -126, 5, 18, 6, 4, -102, -101, -100, 114, 15, 17, 0, 59
  };


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
   * --exclusive           use full screen exclusive mode when presenting.
   *                       disables new windows or interaction with other
   *                       monitors, this is like a "game" mode.
   *
   * --hide-stop           use to hide the stop button in situations where
   *                       you don't want to allow users to exit. also
   *                       see the FAQ on information for capturing the ESC
   *                       key when running in presentation mode.
   *
   * --stop-color=#xxxxxx  color of the 'stop' text used to quit an
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
   * Parameters used by Processing when running via the PDE
   *
   * --external            set when the applet is being used by the PDE
   *
   * --editor-location=x,y position of the upper-lefthand corner of the
   *                       editor window, for placement of applet window
   * </PRE>
   */
  static public void main(String args[]) {
    // Disable abyssmally slow Sun renderer on OS X 10.5.
    if (platform == MACOSX) {
      // Only run this on OS X otherwise it can cause a permissions error.
      // http://dev.processing.org/bugs/show_bug.cgi?id=976
      System.setProperty("apple.awt.graphics.UseQuartz", "true");
    }

    // This doesn't do anything.
//    if (platform == WINDOWS) {
//      // For now, disable the D3D renderer on Java 6u10 because
//      // it causes problems with Present mode.
//      // http://dev.processing.org/bugs/show_bug.cgi?id=1009
//      System.setProperty("sun.java2d.d3d", "false");
//    }

    if (args.length < 1) {
      System.err.println("Usage: PApplet <appletname>");
      System.err.println("For additional options, " +
                         "see the Javadoc for PApplet");
      System.exit(1);
    }

    boolean external = false;
    int[] location = null;
    int[] editorLocation = null;

    String name = null;
    boolean present = false;
    boolean exclusive = false;
    Color backgroundColor = Color.BLACK;
    Color stopColor = Color.GRAY;
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

        } else if (args[argIndex].equals(ARGS_EXCLUSIVE)) {
          exclusive = true;

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

    // Set this property before getting into any GUI init code
    //System.setProperty("com.apple.mrj.application.apple.menu.about.name", name);
    // This )*)(*@#$ Apple crap don't work no matter where you put it
    // (static method of the class, at the top of main, wherever)

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
      //Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();

    // remove the grow box by default
    // users who want it back can call frame.setResizable(true)
    frame.setResizable(false);

    // Set the trimmings around the image
    Image image = Toolkit.getDefaultToolkit().createImage(ICON_IMAGE);
    frame.setIconImage(image);
    frame.setTitle(name);

    final PApplet applet;
    try {
      Class<?> c = Thread.currentThread().getContextClassLoader().loadClass(name);
      applet = (PApplet) c.newInstance();
    } catch (Exception e) {
      throw new RuntimeException(e);
    }

    // these are needed before init/start
    applet.frame = frame;
    applet.sketchPath = folder;
    applet.args = PApplet.subset(args, 1);
    applet.external = external;

    // Need to save the window bounds at full screen,
    // because pack() will cause the bounds to go to zero.
    // http://dev.processing.org/bugs/show_bug.cgi?id=923
    Rectangle fullScreenRect = null;

    // For 0149, moving this code (up to the pack() method) before init().
    // For OpenGL (and perhaps other renderers in the future), a peer is
    // needed before a GLDrawable can be created. So pack() needs to be
    // called on the Frame before applet.init(), which itself calls size(),
    // and launches the Thread that will kick off setup().
    // http://dev.processing.org/bugs/show_bug.cgi?id=891
    // http://dev.processing.org/bugs/show_bug.cgi?id=908
    if (present) {
      frame.setUndecorated(true);
      frame.setBackground(backgroundColor);
      if (exclusive) {
        displayDevice.setFullScreenWindow(frame);
        frame.setExtendedState(java.awt.Frame.MAXIMIZED_BOTH);
        fullScreenRect = frame.getBounds();
      } else {
        DisplayMode mode = displayDevice.getDisplayMode();
        fullScreenRect = new Rectangle(0, 0, mode.getWidth(), mode.getHeight());
        frame.setBounds(fullScreenRect);
        frame.setVisible(true);
      }
    }
    frame.setLayout(null);
    frame.add(applet);
    if (present) {
      frame.invalidate();
    } else {
      frame.pack();
    }
    // insufficient, places the 100x100 sketches offset strangely
    //frame.validate();

    applet.init();

    // Wait until the applet has figured out its width.
    // In a static mode app, this will be after setup() has completed,
    // and the empty draw() has set "finished" to true.
    // TODO make sure this won't hang if the applet has an exception.
    while (applet.defaultSize && !applet.finished) {
      //System.out.println("default size");
      try {
        Thread.sleep(5);

      } catch (InterruptedException e) {
        //System.out.println("interrupt");
      }
    }
    //println("not default size " + applet.width + " " + applet.height);
    //println("  (g width/height is " + applet.g.width + "x" + applet.g.height + ")");

    if (present) {
      // After the pack(), the screen bounds are gonna be 0s
      frame.setBounds(fullScreenRect);
      applet.setBounds((fullScreenRect.width - applet.width) / 2,
                       (fullScreenRect.height - applet.height) / 2,
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
        label.setLocation(20, fullScreenRect.height - labelSize.height - 20);
      }

      // not always running externally when in present mode
      if (external) {
        applet.setupExternalMessages();
      }

    } else {  // if not presenting
      // can't do pack earlier cuz present mode don't like it
      // (can't go full screen with a frame after calling pack)
      //        frame.pack();  // get insets. get more.
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

          if ((locationX + windowW > applet.screen.width - 33) ||
              (locationY + windowH > applet.screen.height - 33)) {
            // otherwise center on screen
            locationX = (applet.screen.width - windowW) / 2;
            locationY = (applet.screen.height - windowH) / 2;
          }
          frame.setLocation(locationX, locationY);
        }
      } else {  // just center on screen
        frame.setLocation((applet.screen.width - applet.width) / 2,
                          (applet.screen.height - applet.height) / 2);
      }

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

    applet.requestFocus(); // ask for keydowns
    //System.out.println("exiting main()");
  }


  //////////////////////////////////////////////////////////////


  /**
   * Begin recording to a new renderer of the specified type, using the width
   * and height of the main drawing surface.
   */
  public PGraphics beginRecord(String renderer, String filename) {
    filename = insertFrame(filename);
    PGraphics rec = createGraphics(width, height, renderer, filename);
    beginRecord(rec);
    return rec;
  }


  /**
   * Begin recording (echoing) commands to the specified PGraphics object.
   */
  public void beginRecord(PGraphics recorder) {
    this.recorder = recorder;
    recorder.beginDraw();
  }


  public void endRecord() {
    if (recorder != null) {
      recorder.endDraw();
      recorder.dispose();
      recorder = null;
    }
  }


  /**
   * Begin recording raw shape data to a renderer of the specified type,
   * using the width and height of the main drawing surface.
   *
   * If hashmarks (###) are found in the filename, they'll be replaced
   * by the current frame number (frameCount).
   */
  public PGraphics beginRaw(String renderer, String filename) {
    filename = insertFrame(filename);
    PGraphics rec = createGraphics(width, height, renderer, filename);
    g.beginRaw(rec);
    return rec;
  }


  /**
   * Begin recording raw shape data to the specified renderer.
   *
   * This simply echoes to g.beginRaw(), but since is placed here (rather than
   * generated by preproc.pl) for clarity and so that it doesn't echo the
   * command should beginRecord() be in use.
   */
  public void beginRaw(PGraphics rawGraphics) {
    g.beginRaw(rawGraphics);
  }


  /**
   * Stop recording raw shape data to the specified renderer.
   *
   * This simply echoes to g.beginRaw(), but since is placed here (rather than
   * generated by preproc.pl) for clarity and so that it doesn't echo the
   * command should beginRecord() be in use.
   */
  public void endRaw() {
    g.endRaw();
  }


  //////////////////////////////////////////////////////////////


  /**
   * Loads the pixel data for the display window into the <b>pixels[]</b> array. This function must always be called before reading from or writing to <b>pixels[]</b>.
   * <br><br>Certain renderers may or may not seem to require <b>loadPixels()</b> or <b>updatePixels()</b>. However, the rule is that any time you want to manipulate the <b>pixels[]</b> array, you must first call <b>loadPixels()</b>, and after changes have been made, call <b>updatePixels()</b>. Even if the renderer may not seem to use this function in the current Processing release, this will always be subject to change.
   * =advanced
   * Override the g.pixels[] function to set the pixels[] array
   * that's part of the PApplet object. Allows the use of
   * pixels[] in the code, rather than g.pixels[].
   *
   * @webref image:pixels
   * @see processing.core.PApplet#pixels
   * @see processing.core.PApplet#updatePixels()
   */
  public void loadPixels() {
    g.loadPixels();
    pixels = g.pixels;
  }

  /**
   * Updates the display window with the data in the <b>pixels[]</b> array. Use in conjunction with <b>loadPixels()</b>. If you're only reading pixels from the array, there's no need to call <b>updatePixels()</b> unless there are changes.
   * <br><br>Certain renderers may or may not seem to require <b>loadPixels()</b> or <b>updatePixels()</b>. However, the rule is that any time you want to manipulate the <b>pixels[]</b> array, you must first call <b>loadPixels()</b>, and after changes have been made, call <b>updatePixels()</b>. Even if the renderer may not seem to use this function in the current Processing release, this will always be subject to change.
   * <br><br>Currently, none of the renderers use the additional parameters to <b>updatePixels()</b>, however this may be implemented in the future.
   *
   * @webref image:pixels
   *
   * @see processing.core.PApplet#loadPixels()
   * @see processing.core.PApplet#updatePixels()
   *
   */
  public void updatePixels() {
    g.updatePixels();
  }


  public void updatePixels(int x1, int y1, int x2, int y2) {
    g.updatePixels(x1, y1, x2, y2);
  }


  //////////////////////////////////////////////////////////////

  // EVERYTHING BELOW THIS LINE IS AUTOMATICALLY GENERATED. NO TOUCH!
  // This includes all of the comments, which are automatically pulled
  // from their respective functions in PGraphics or PImage.

  // public functions for processing.core


  public void flush() {
    if (recorder != null) recorder.flush();
    g.flush();
  }


  /**
   * Enable a hint option.
   * <P>
   * For the most part, hints are temporary api quirks,
   * for which a proper api hasn't been properly worked out.
   * for instance SMOOTH_IMAGES existed because smooth()
   * wasn't yet implemented, but it will soon go away.
   * <P>
   * They also exist for obscure features in the graphics
   * engine, like enabling/disabling single pixel lines
   * that ignore the zbuffer, the way they do in alphabot.
   * <P>
   * Current hint options:
   * <UL>
   * <LI><TT>DISABLE_DEPTH_TEST</TT> -
   * turns off the z-buffer in the P3D or OPENGL renderers.
   * </UL>
   */
  public void hint(int which) {
    if (recorder != null) recorder.hint(which);
    g.hint(which);
  }


  /**
   * Start a new shape of type POLYGON
   */
  public void beginShape() {
    if (recorder != null) recorder.beginShape();
    g.beginShape();
  }


  /**
   * Start a new shape.
   * <P>
   * <B>Differences between beginShape() and line() and point() methods.</B>
   * <P>
   * beginShape() is intended to be more flexible at the expense of being
   * a little more complicated to use. it handles more complicated shapes
   * that can consist of many connected lines (so you get joins) or lines
   * mixed with curves.
   * <P>
   * The line() and point() command are for the far more common cases
   * (particularly for our audience) that simply need to draw a line
   * or a point on the screen.
   * <P>
   * From the code side of things, line() may or may not call beginShape()
   * to do the drawing. In the beta code, they do, but in the alpha code,
   * they did not. they might be implemented one way or the other depending
   * on tradeoffs of runtime efficiency vs. implementation efficiency &mdash
   * meaning the speed that things run at vs. the speed it takes me to write
   * the code and maintain it. for beta, the latter is most important so
   * that's how things are implemented.
   */
  public void beginShape(int kind) {
    if (recorder != null) recorder.beginShape(kind);
    g.beginShape(kind);
  }


  /**
   * Sets whether the upcoming vertex is part of an edge.
   * Equivalent to glEdgeFlag(), for people familiar with OpenGL.
   */
  public void edge(boolean edge) {
    if (recorder != null) recorder.edge(edge);
    g.edge(edge);
  }


  /**
   * Sets the current normal vector. Only applies with 3D rendering
   * and inside a beginShape/endShape block.
   * <P/>
   * This is for drawing three dimensional shapes and surfaces,
   * allowing you to specify a vector perpendicular to the surface
   * of the shape, which determines how lighting affects it.
   * <P/>
   * For the most part, PGraphics3D will attempt to automatically
   * assign normals to shapes, but since that's imperfect,
   * this is a better option when you want more control.
   * <P/>
   * For people familiar with OpenGL, this function is basically
   * identical to glNormal3f().
   */
  public void normal(float nx, float ny, float nz) {
    if (recorder != null) recorder.normal(nx, ny, nz);
    g.normal(nx, ny, nz);
  }


  /**
   * Set texture mode to either to use coordinates based on the IMAGE
   * (more intuitive for new users) or NORMALIZED (better for advanced chaps)
   */
  public void textureMode(int mode) {
    if (recorder != null) recorder.textureMode(mode);
    g.textureMode(mode);
  }


  /**
   * Set texture image for current shape.
   * Needs to be called between @see beginShape and @see endShape
   *
   * @param image reference to a PImage object
   */
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


  /**
   * Used by renderer subclasses or PShape to efficiently pass in already
   * formatted vertex information.
   * @param v vertex parameters, as a float array of length VERTEX_FIELD_COUNT
   */
  public void vertex(float[] v) {
    if (recorder != null) recorder.vertex(v);
    g.vertex(v);
  }


  public void vertex(float x, float y, float u, float v) {
    if (recorder != null) recorder.vertex(x, y, u, v);
    g.vertex(x, y, u, v);
  }


  public void vertex(float x, float y, float z, float u, float v) {
    if (recorder != null) recorder.vertex(x, y, z, u, v);
    g.vertex(x, y, z, u, v);
  }


  /** This feature is in testing, do not use or rely upon its implementation */
  public void breakShape() {
    if (recorder != null) recorder.breakShape();
    g.breakShape();
  }


  public void endShape() {
    if (recorder != null) recorder.endShape();
    g.endShape();
  }


  public void endShape(int mode) {
    if (recorder != null) recorder.endShape(mode);
    g.endShape(mode);
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


  public void rect(float a, float b, float c, float d) {
    if (recorder != null) recorder.rect(a, b, c, d);
    g.rect(a, b, c, d);
  }


  public void ellipseMode(int mode) {
    if (recorder != null) recorder.ellipseMode(mode);
    g.ellipseMode(mode);
  }


  public void ellipse(float a, float b, float c, float d) {
    if (recorder != null) recorder.ellipse(a, b, c, d);
    g.ellipse(a, b, c, d);
  }


  /**
   * Identical parameters and placement to ellipse,
   * but draws only an arc of that ellipse.
   * <p/>
   * start and stop are always radians because angleMode() was goofy.
   * ellipseMode() sets the placement.
   * <p/>
   * also tries to be smart about start < stop.
   */
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


  /**
   * Set the detail level for approximating a sphere. The ures and vres params
   * control the horizontal and vertical resolution.
   *
   * Code for sphereDetail() submitted by toxi [031031].
   * Code for enhanced u/v version from davbol [080801].
   */
  public void sphereDetail(int ures, int vres) {
    if (recorder != null) recorder.sphereDetail(ures, vres);
    g.sphereDetail(ures, vres);
  }


  /**
   * Draw a sphere with radius r centered at coordinate 0, 0, 0.
   * <P>
   * Implementation notes:
   * <P>
   * cache all the points of the sphere in a static array
   * top and bottom are just a bunch of triangles that land
   * in the center point
   * <P>
   * sphere is a series of concentric circles who radii vary
   * along the shape, based on, er.. cos or something
   * <PRE>
   * [toxi 031031] new sphere code. removed all multiplies with
   * radius, as scale() will take care of that anyway
   *
   * [toxi 031223] updated sphere code (removed modulos)
   * and introduced sphereAt(x,y,z,r)
   * to avoid additional translate()'s on the user/sketch side
   *
   * [davbol 080801] now using separate sphereDetailU/V
   * </PRE>
   */
  public void sphere(float r) {
    if (recorder != null) recorder.sphere(r);
    g.sphere(r);
  }


  /**
   * Evalutes quadratic bezier at point t for points a, b, c, d.
   * t varies between 0 and 1, and a and d are the on curve points,
   * b and c are the control points. this can be done once with the
   * x coordinates and a second time with the y coordinates to get
   * the location of a bezier curve at t.
   * <P>
   * For instance, to convert the following example:<PRE>
   * stroke(255, 102, 0);
   * line(85, 20, 10, 10);
   * line(90, 90, 15, 80);
   * stroke(0, 0, 0);
   * bezier(85, 20, 10, 10, 90, 90, 15, 80);
   *
   * // draw it in gray, using 10 steps instead of the default 20
   * // this is a slower way to do it, but useful if you need
   * // to do things with the coordinates at each step
   * stroke(128);
   * beginShape(LINE_STRIP);
   * for (int i = 0; i <= 10; i++) {
   *   float t = i / 10.0f;
   *   float x = bezierPoint(85, 10, 90, 15, t);
   *   float y = bezierPoint(20, 10, 90, 80, t);
   *   vertex(x, y);
   * }
   * endShape();</PRE>
   */
  public float bezierPoint(float a, float b, float c, float d, float t) {
    return g.bezierPoint(a, b, c, d, t);
  }


  /**
   * Provide the tangent at the given point on the bezier curve.
   * Fix from davbol for 0136.
   */
  public float bezierTangent(float a, float b, float c, float d, float t) {
    return g.bezierTangent(a, b, c, d, t);
  }


  public void bezierDetail(int detail) {
    if (recorder != null) recorder.bezierDetail(detail);
    g.bezierDetail(detail);
  }


  /**
   * Draw a cubic bezier curve. The first and last points are
   * the on-curve points. The middle two are the 'control' points,
   * or 'handles' in an application like Illustrator.
   * <P>
   * Identical to typing:
   * <PRE>beginShape();
   * vertex(x1, y1);
   * bezierVertex(x2, y2, x3, y3, x4, y4);
   * endShape();
   * </PRE>
   * In Postscript-speak, this would be:
   * <PRE>moveto(x1, y1);
   * curveto(x2, y2, x3, y3, x4, y4);</PRE>
   * If you were to try and continue that curve like so:
   * <PRE>curveto(x5, y5, x6, y6, x7, y7);</PRE>
   * This would be done in processing by adding these statements:
   * <PRE>bezierVertex(x5, y5, x6, y6, x7, y7)
   * </PRE>
   * To draw a quadratic (instead of cubic) curve,
   * use the control point twice by doubling it:
   * <PRE>bezier(x1, y1, cx, cy, cx, cy, x2, y2);</PRE>
   */
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


  /**
   * Get a location along a catmull-rom curve segment.
   *
   * @param t Value between zero and one for how far along the segment
   */
  public float curvePoint(float a, float b, float c, float d, float t) {
    return g.curvePoint(a, b, c, d, t);
  }


  /**
   * Calculate the tangent at a t value (0..1) on a Catmull-Rom curve.
   * Code thanks to Dave Bollinger (Bug #715)
   */
  public float curveTangent(float a, float b, float c, float d, float t) {
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


  /**
   * Draws a segment of Catmull-Rom curve.
   * <P>
   * As of 0070, this function no longer doubles the first and
   * last points. The curves are a bit more boring, but it's more
   * mathematically correct, and properly mirrored in curvePoint().
   * <P>
   * Identical to typing out:<PRE>
   * beginShape();
   * curveVertex(x1, y1);
   * curveVertex(x2, y2);
   * curveVertex(x3, y3);
   * curveVertex(x4, y4);
   * endShape();
   * </PRE>
   */
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


  /**
   * If true in PImage, use bilinear interpolation for copy()
   * operations. When inherited by PGraphics, also controls shapes.
   */
  public void smooth() {
    if (recorder != null) recorder.smooth();
    g.smooth();
  }


  /**
   * Disable smoothing. See smooth().
   */
  public void noSmooth() {
    if (recorder != null) recorder.noSmooth();
    g.noSmooth();
  }


  /**
   * The mode can only be set to CORNERS, CORNER, and CENTER.
   * <p/>
   * Support for CENTER was added in release 0146.
   */
  public void imageMode(int mode) {
    if (recorder != null) recorder.imageMode(mode);
    g.imageMode(mode);
  }


  public void image(PImage image, float x, float y) {
    if (recorder != null) recorder.image(image, x, y);
    g.image(image, x, y);
  }


  public void image(PImage image, float x, float y, float c, float d) {
    if (recorder != null) recorder.image(image, x, y, c, d);
    g.image(image, x, y, c, d);
  }


  /**
   * Draw an image(), also specifying u/v coordinates.
   * In this method, the  u, v coordinates are always based on image space
   * location, regardless of the current textureMode().
   */
  public void image(PImage image,
                    float a, float b, float c, float d,
                    int u1, int v1, int u2, int v2) {
    if (recorder != null) recorder.image(image, a, b, c, d, u1, v1, u2, v2);
    g.image(image, a, b, c, d, u1, v1, u2, v2);
  }


  /**
   * Set the orientation for the shape() command (like imageMode() or rectMode()).
   * @param mode Either CORNER, CORNERS, or CENTER.
   */
  public void shapeMode(int mode) {
    if (recorder != null) recorder.shapeMode(mode);
    g.shapeMode(mode);
  }


  public void shape(PShape shape) {
    if (recorder != null) recorder.shape(shape);
    g.shape(shape);
  }


  /**
   * Convenience method to draw at a particular location.
   */
  public void shape(PShape shape, float x, float y) {
    if (recorder != null) recorder.shape(shape, x, y);
    g.shape(shape, x, y);
  }


  public void shape(PShape shape, float x, float y, float c, float d) {
    if (recorder != null) recorder.shape(shape, x, y, c, d);
    g.shape(shape, x, y, c, d);
  }


  /**
   * Sets the alignment of the text to one of LEFT, CENTER, or RIGHT.
   * This will also reset the vertical text alignment to BASELINE.
   */
  public void textAlign(int align) {
    if (recorder != null) recorder.textAlign(align);
    g.textAlign(align);
  }


  /**
   * Sets the horizontal and vertical alignment of the text. The horizontal
   * alignment can be one of LEFT, CENTER, or RIGHT. The vertical alignment
   * can be TOP, BOTTOM, CENTER, or the BASELINE (the default).
   */
  public void textAlign(int alignX, int alignY) {
    if (recorder != null) recorder.textAlign(alignX, alignY);
    g.textAlign(alignX, alignY);
  }


  /**
   * Returns the ascent of the current font at the current size.
   * This is a method, rather than a variable inside the PGraphics object
   * because it requires calculation.
   */
  public float textAscent() {
    return g.textAscent();
  }


  /**
   * Returns the descent of the current font at the current size.
   * This is a method, rather than a variable inside the PGraphics object
   * because it requires calculation.
   */
  public float textDescent() {
    return g.textDescent();
  }


  /**
   * Sets the current font. The font's size will be the "natural"
   * size of this font (the size that was set when using "Create Font").
   * The leading will also be reset.
   */
  public void textFont(PFont which) {
    if (recorder != null) recorder.textFont(which);
    g.textFont(which);
  }


  /**
   * Useful function to set the font and size at the same time.
   */
  public void textFont(PFont which, float size) {
    if (recorder != null) recorder.textFont(which, size);
    g.textFont(which, size);
  }


  /**
   * Set the text leading to a specific value. If using a custom
   * value for the text leading, you'll have to call textLeading()
   * again after any calls to textSize().
   */
  public void textLeading(float leading) {
    if (recorder != null) recorder.textLeading(leading);
    g.textLeading(leading);
  }


  /**
   * Sets the text rendering/placement to be either SCREEN (direct
   * to the screen, exact coordinates, only use the font's original size)
   * or MODEL (the default, where text is manipulated by translate() and
   * can have a textSize). The text size cannot be set when using
   * textMode(SCREEN), because it uses the pixels directly from the font.
   */
  public void textMode(int mode) {
    if (recorder != null) recorder.textMode(mode);
    g.textMode(mode);
  }


  /**
   * Sets the text size, also resets the value for the leading.
   */
  public void textSize(float size) {
    if (recorder != null) recorder.textSize(size);
    g.textSize(size);
  }


  public float textWidth(char c) {
    return g.textWidth(c);
  }


  /**
   * Return the width of a line of text. If the text has multiple
   * lines, this returns the length of the longest line.
   */
  public float textWidth(String str) {
    return g.textWidth(str);
  }


  /**
   * TODO not sure if this stays...
   */
  public float textWidth(char[] chars, int start, int length) {
    return g.textWidth(chars, start, length);
  }


  /**
   * Write text where we just left off.
   */
  public void text(char c) {
    if (recorder != null) recorder.text(c);
    g.text(c);
  }


  /**
   * Draw a single character on screen.
   * Extremely slow when used with textMode(SCREEN) and Java 2D,
   * because loadPixels has to be called first and updatePixels last.
   */
  public void text(char c, float x, float y) {
    if (recorder != null) recorder.text(c, x, y);
    g.text(c, x, y);
  }


  /**
   * Draw a single character on screen (with a z coordinate)
   */
  public void text(char c, float x, float y, float z) {
    if (recorder != null) recorder.text(c, x, y, z);
    g.text(c, x, y, z);
  }


  /**
   * Write text where we just left off.
   */
  public void text(String str) {
    if (recorder != null) recorder.text(str);
    g.text(str);
  }


  /**
   * Draw a chunk of text.
   * Newlines that are \n (Unix newline or linefeed char, ascii 10)
   * are honored, but \r (carriage return, Windows and Mac OS) are
   * ignored.
   */
  public void text(String str, float x, float y) {
    if (recorder != null) recorder.text(str, x, y);
    g.text(str, x, y);
  }


  /**
   * Method to draw text from an array of chars. This method will usually be
   * more efficient than drawing from a String object, because the String will
   * not be converted to a char array before drawing.
   */
  public void text(char[] chars, int start, int stop, float x, float y) {
    if (recorder != null) recorder.text(chars, start, stop, x, y);
    g.text(chars, start, stop, x, y);
  }


  /**
   * Same as above but with a z coordinate.
   */
  public void text(String str, float x, float y, float z) {
    if (recorder != null) recorder.text(str, x, y, z);
    g.text(str, x, y, z);
  }


  public void text(char[] chars, int start, int stop,
                   float x, float y, float z) {
    if (recorder != null) recorder.text(chars, start, stop, x, y, z);
    g.text(chars, start, stop, x, y, z);
  }


  /**
   * Draw text in a box that is constrained to a particular size.
   * The current rectMode() determines what the coordinates mean
   * (whether x1/y1/x2/y2 or x/y/w/h).
   * <P/>
   * Note that the x,y coords of the start of the box
   * will align with the *ascent* of the text, not the baseline,
   * as is the case for the other text() functions.
   * <P/>
   * Newlines that are \n (Unix newline or linefeed char, ascii 10)
   * are honored, and \r (carriage return, Windows and Mac OS) are
   * ignored.
   */
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


  /**
   * This does a basic number formatting, to avoid the
   * generally ugly appearance of printing floats.
   * Users who want more control should use their own nf() cmmand,
   * or if they want the long, ugly version of float,
   * use String.valueOf() to convert the float to a String first.
   */
  public void text(float num, float x, float y) {
    if (recorder != null) recorder.text(num, x, y);
    g.text(num, x, y);
  }


  public void text(float num, float x, float y, float z) {
    if (recorder != null) recorder.text(num, x, y, z);
    g.text(num, x, y, z);
  }


  /**
   * Push a copy of the current transformation matrix onto the stack.
   */
  public void pushMatrix() {
    if (recorder != null) recorder.pushMatrix();
    g.pushMatrix();
  }


  /**
   * Replace the current transformation matrix with the top of the stack.
   */
  public void popMatrix() {
    if (recorder != null) recorder.popMatrix();
    g.popMatrix();
  }


  /**
   * Translate in X and Y.
   */
  public void translate(float tx, float ty) {
    if (recorder != null) recorder.translate(tx, ty);
    g.translate(tx, ty);
  }


  /**
   * Translate in X, Y, and Z.
   */
  public void translate(float tx, float ty, float tz) {
    if (recorder != null) recorder.translate(tx, ty, tz);
    g.translate(tx, ty, tz);
  }


  /**
   * Two dimensional rotation.
   *
   * Same as rotateZ (this is identical to a 3D rotation along the z-axis)
   * but included for clarity. It'd be weird for people drawing 2D graphics
   * to be using rotateZ. And they might kick our a-- for the confusion.
   *
   * <A HREF="http://www.xkcd.com/c184.html">Additional background</A>.
   */
  public void rotate(float angle) {
    if (recorder != null) recorder.rotate(angle);
    g.rotate(angle);
  }


  /**
   * Rotate around the X axis.
   */
  public void rotateX(float angle) {
    if (recorder != null) recorder.rotateX(angle);
    g.rotateX(angle);
  }


  /**
   * Rotate around the Y axis.
   */
  public void rotateY(float angle) {
    if (recorder != null) recorder.rotateY(angle);
    g.rotateY(angle);
  }


  /**
   * Rotate around the Z axis.
   *
   * The functions rotate() and rotateZ() are identical, it's just that it make
   * sense to have rotate() and then rotateX() and rotateY() when using 3D;
   * nor does it make sense to use a function called rotateZ() if you're only
   * doing things in 2D. so we just decided to have them both be the same.
   */
  public void rotateZ(float angle) {
    if (recorder != null) recorder.rotateZ(angle);
    g.rotateZ(angle);
  }


  /**
   * Rotate about a vector in space. Same as the glRotatef() function.
   */
  public void rotate(float angle, float vx, float vy, float vz) {
    if (recorder != null) recorder.rotate(angle, vx, vy, vz);
    g.rotate(angle, vx, vy, vz);
  }


  /**
   * Scale in all dimensions.
   */
  public void scale(float s) {
    if (recorder != null) recorder.scale(s);
    g.scale(s);
  }


  /**
   * Scale in X and Y. Equivalent to scale(sx, sy, 1).
   *
   * Not recommended for use in 3D, because the z-dimension is just
   * scaled by 1, since there's no way to know what else to scale it by.
   */
  public void scale(float sx, float sy) {
    if (recorder != null) recorder.scale(sx, sy);
    g.scale(sx, sy);
  }


  /**
   * Scale in X, Y, and Z.
   */
  public void scale(float x, float y, float z) {
    if (recorder != null) recorder.scale(x, y, z);
    g.scale(x, y, z);
  }


  /**
   * Set the current transformation matrix to identity.
   */
  public void resetMatrix() {
    if (recorder != null) recorder.resetMatrix();
    g.resetMatrix();
  }


  public void applyMatrix(PMatrix source) {
    if (recorder != null) recorder.applyMatrix(source);
    g.applyMatrix(source);
  }


  public void applyMatrix(PMatrix2D source) {
    if (recorder != null) recorder.applyMatrix(source);
    g.applyMatrix(source);
  }


  /**
   * Apply a 3x2 affine transformation matrix.
   */
  public void applyMatrix(float n00, float n01, float n02,
                          float n10, float n11, float n12) {
    if (recorder != null) recorder.applyMatrix(n00, n01, n02, n10, n11, n12);
    g.applyMatrix(n00, n01, n02, n10, n11, n12);
  }


  public void applyMatrix(PMatrix3D source) {
    if (recorder != null) recorder.applyMatrix(source);
    g.applyMatrix(source);
  }


  /**
   * Apply a 4x4 transformation matrix.
   */
  public void applyMatrix(float n00, float n01, float n02, float n03,
                          float n10, float n11, float n12, float n13,
                          float n20, float n21, float n22, float n23,
                          float n30, float n31, float n32, float n33) {
    if (recorder != null) recorder.applyMatrix(n00, n01, n02, n03, n10, n11, n12, n13, n20, n21, n22, n23, n30, n31, n32, n33);
    g.applyMatrix(n00, n01, n02, n03, n10, n11, n12, n13, n20, n21, n22, n23, n30, n31, n32, n33);
  }


  public PMatrix getMatrix() {
    return g.getMatrix();
  }


  /**
   * Copy the current transformation matrix into the specified target.
   * Pass in null to create a new matrix.
   */
  public PMatrix2D getMatrix(PMatrix2D target) {
    return g.getMatrix(target);
  }


  /**
   * Copy the current transformation matrix into the specified target.
   * Pass in null to create a new matrix.
   */
  public PMatrix3D getMatrix(PMatrix3D target) {
    return g.getMatrix(target);
  }


  /**
   * Set the current transformation matrix to the contents of another.
   */
  public void setMatrix(PMatrix source) {
    if (recorder != null) recorder.setMatrix(source);
    g.setMatrix(source);
  }


  /**
   * Set the current transformation to the contents of the specified source.
   */
  public void setMatrix(PMatrix2D source) {
    if (recorder != null) recorder.setMatrix(source);
    g.setMatrix(source);
  }


  /**
   * Set the current transformation to the contents of the specified source.
   */
  public void setMatrix(PMatrix3D source) {
    if (recorder != null) recorder.setMatrix(source);
    g.setMatrix(source);
  }


  /**
   * Print the current model (or "transformation") matrix.
   */
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


  public void frustum(float left, float right,
                      float bottom, float top,
                      float near, float far) {
    if (recorder != null) recorder.frustum(left, right, bottom, top, near, far);
    g.frustum(left, right, bottom, top, near, far);
  }


  public void printProjection() {
    if (recorder != null) recorder.printProjection();
    g.printProjection();
  }


  /**
   * Given an x and y coordinate, returns the x position of where
   * that point would be placed on screen, once affected by translate(),
   * scale(), or any other transformations.
   */
  public float screenX(float x, float y) {
    return g.screenX(x, y);
  }


  /**
   * Given an x and y coordinate, returns the y position of where
   * that point would be placed on screen, once affected by translate(),
   * scale(), or any other transformations.
   */
  public float screenY(float x, float y) {
    return g.screenY(x, y);
  }


  /**
   * Maps a three dimensional point to its placement on-screen.
   * <P>
   * Given an (x, y, z) coordinate, returns the x position of where
   * that point would be placed on screen, once affected by translate(),
   * scale(), or any other transformations.
   */
  public float screenX(float x, float y, float z) {
    return g.screenX(x, y, z);
  }


  /**
   * Maps a three dimensional point to its placement on-screen.
   * <P>
   * Given an (x, y, z) coordinate, returns the y position of where
   * that point would be placed on screen, once affected by translate(),
   * scale(), or any other transformations.
   */
  public float screenY(float x, float y, float z) {
    return g.screenY(x, y, z);
  }


  /**
   * Maps a three dimensional point to its placement on-screen.
   * <P>
   * Given an (x, y, z) coordinate, returns its z value.
   * This value can be used to determine if an (x, y, z) coordinate
   * is in front or in back of another (x, y, z) coordinate.
   * The units are based on how the zbuffer is set up, and don't
   * relate to anything "real". They're only useful for in
   * comparison to another value obtained from screenZ(),
   * or directly out of the zbuffer[].
   */
  public float screenZ(float x, float y, float z) {
    return g.screenZ(x, y, z);
  }


  /**
   * Returns the model space x value for an x, y, z coordinate.
   * <P>
   * This will give you a coordinate after it has been transformed
   * by translate(), rotate(), and camera(), but not yet transformed
   * by the projection matrix. For instance, his can be useful for
   * figuring out how points in 3D space relate to the edge
   * coordinates of a shape.
   */
  public float modelX(float x, float y, float z) {
    return g.modelX(x, y, z);
  }


  /**
   * Returns the model space y value for an x, y, z coordinate.
   */
  public float modelY(float x, float y, float z) {
    return g.modelY(x, y, z);
  }


  /**
   * Returns the model space z value for an x, y, z coordinate.
   */
  public float modelZ(float x, float y, float z) {
    return g.modelZ(x, y, z);
  }


  public void pushStyle() {
    if (recorder != null) recorder.pushStyle();
    g.pushStyle();
  }


  public void popStyle() {
    if (recorder != null) recorder.popStyle();
    g.popStyle();
  }


  public void style(PStyle s) {
    if (recorder != null) recorder.style(s);
    g.style(s);
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


  /**
   * Set the tint to either a grayscale or ARGB value.
   * See notes attached to the fill() function.
   */
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


  /**
   * Set the tint to either a grayscale or ARGB value.
   */
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


  /**
   * Set the fill to either a grayscale value or an ARGB int.
   */
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


  public void specular(float x, float y, float z) {
    if (recorder != null) recorder.specular(x, y, z);
    g.specular(x, y, z);
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


  public void emissive(float x, float y, float z) {
    if (recorder != null) recorder.emissive(x, y, z);
    g.emissive(x, y, z);
  }


  public void lights() {
    if (recorder != null) recorder.lights();
    g.lights();
  }


  public void noLights() {
    if (recorder != null) recorder.noLights();
    g.noLights();
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


  /**
   * Set the background to a gray or ARGB color.
   * <p>
   * For the main drawing surface, the alpha value will be ignored. However,
   * alpha can be used on PGraphics objects from createGraphics(). This is
   * the only way to set all the pixels partially transparent, for instance.
   * <p>
   * Note that background() should be called before any transformations occur,
   * because some implementations may require the current transformation matrix
   * to be identity before drawing.
   */
  public void background(int rgb) {
    if (recorder != null) recorder.background(rgb);
    g.background(rgb);
  }


  /**
   * See notes about alpha in background(x, y, z, a).
   */
  public void background(int rgb, float alpha) {
    if (recorder != null) recorder.background(rgb, alpha);
    g.background(rgb, alpha);
  }


  /**
   * Set the background to a grayscale value, based on the
   * current colorMode.
   */
  public void background(float gray) {
    if (recorder != null) recorder.background(gray);
    g.background(gray);
  }


  /**
   * See notes about alpha in background(x, y, z, a).
   */
  public void background(float gray, float alpha) {
    if (recorder != null) recorder.background(gray, alpha);
    g.background(gray, alpha);
  }


  /**
   * Set the background to an r, g, b or h, s, b value,
   * based on the current colorMode.
   */
  public void background(float x, float y, float z) {
    if (recorder != null) recorder.background(x, y, z);
    g.background(x, y, z);
  }


  /**
   * Clear the background with a color that includes an alpha value. This can
   * only be used with objects created by createGraphics(), because the main
   * drawing surface cannot be set transparent.
   * <p>
   * It might be tempting to use this function to partially clear the screen
   * on each frame, however that's not how this function works. When calling
   * background(), the pixels will be replaced with pixels that have that level
   * of transparency. To do a semi-transparent overlay, use fill() with alpha
   * and draw a rectangle.
   */
  public void background(float x, float y, float z, float a) {
    if (recorder != null) recorder.background(x, y, z, a);
    g.background(x, y, z, a);
  }


  /**
   * Takes an RGB or ARGB image and sets it as the background.
   * The width and height of the image must be the same size as the sketch.
   * Use image.resize(width, height) to make short work of such a task.
   * <P>
   * Note that even if the image is set as RGB, the high 8 bits of each pixel
   * should be set opaque (0xFF000000), because the image data will be copied
   * directly to the screen, and non-opaque background images may have strange
   * behavior. Using image.filter(OPAQUE) will handle this easily.
   * <P>
   * When using 3D, this will also clear the zbuffer (if it exists).
   */
  public void background(PImage image) {
    if (recorder != null) recorder.background(image);
    g.background(image);
  }


  public void colorMode(int mode) {
    if (recorder != null) recorder.colorMode(mode);
    g.colorMode(mode);
  }


  public void colorMode(int mode, float max) {
    if (recorder != null) recorder.colorMode(mode, max);
    g.colorMode(mode, max);
  }


  /**
   * Set the colorMode and the maximum values for (r, g, b)
   * or (h, s, b).
   * <P>
   * Note that this doesn't set the maximum for the alpha value,
   * which might be confusing if for instance you switched to
   * <PRE>colorMode(HSB, 360, 100, 100);</PRE>
   * because the alpha values were still between 0 and 255.
   */
  public void colorMode(int mode, float maxX, float maxY, float maxZ) {
    if (recorder != null) recorder.colorMode(mode, maxX, maxY, maxZ);
    g.colorMode(mode, maxX, maxY, maxZ);
  }


  public void colorMode(int mode,
                        float maxX, float maxY, float maxZ, float maxA) {
    if (recorder != null) recorder.colorMode(mode, maxX, maxY, maxZ, maxA);
    g.colorMode(mode, maxX, maxY, maxZ, maxA);
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


  /**
   * Interpolate between two colors, using the current color mode.
   */
  public int lerpColor(int c1, int c2, float amt) {
    return g.lerpColor(c1, c2, amt);
  }


  /**
   * Interpolate between two colors. Like lerp(), but for the
   * individual color components of a color supplied as an int value.
   */
  static public int lerpColor(int c1, int c2, float amt, int mode) {
    return PGraphics.lerpColor(c1, c2, amt, mode);
  }


  /**
   * Return true if this renderer should be drawn to the screen. Defaults to
   * returning true, since nearly all renderers are on-screen beasts. But can
   * be overridden for subclasses like PDF so that a window doesn't open up.
   * <br/> <br/>
   * A better name? showFrame, displayable, isVisible, visible, shouldDisplay,
   * what to call this?
   */
  public boolean displayable() {
    return g.displayable();
  }


  /**
   * Store data of some kind for a renderer that requires extra metadata of
   * some kind. Usually this is a renderer-specific representation of the
   * image data, for instance a BufferedImage with tint() settings applied for
   * PGraphicsJava2D, or resized image data and OpenGL texture indices for
   * PGraphicsOpenGL.
   */
  public void setCache(Object parent, Object storage) {
    if (recorder != null) recorder.setCache(parent, storage);
    g.setCache(parent, storage);
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
    return g.getCache(parent);
  }


  /**
   * Remove information associated with this renderer from the cache, if any.
   * @param parent The PGraphics object whose cache data should be removed
   */
  public void removeCache(Object parent) {
    if (recorder != null) recorder.removeCache(parent);
    g.removeCache(parent);
  }


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
    return g.get(x, y);
  }


  /**
   * Reads the color of any pixel or grabs a group of pixels. If no parameters are specified, the entire image is returned. Get the value of one pixel by specifying an x,y coordinate. Get a section of the display window by specifing an additional <b>width</b> and <b>height</b> parameter. If the pixel requested is outside of the image window, black is returned. The numbers returned are scaled according to the current color ranges, but only RGB values are returned by this function. Even though you may have drawn a shape with <b>colorMode(HSB)</b>, the numbers returned will be in RGB.
   * <br><br>Getting the color of a single pixel with <b>get(x, y)</b> is easy, but not as fast as grabbing the data directly from <b>pixels[]</b>. The equivalent statement to "get(x, y)" using <b>pixels[]</b> is "pixels[y*width+x]". Processing requires calling <b>loadPixels()</b> to load the display window data into the <b>pixels[]</b> array before getting the values.
   * <br><br>As of release 0149, this function ignores <b>imageMode()</b>.
   *
   * @webref
   * @brief     Reads the color of any pixel or grabs a rectangle of pixels
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
    return g.get(x, y, w, h);
  }


  /**
   * Returns a copy of this PImage. Equivalent to get(0, 0, width, height).
   */
  public PImage get() {
    return g.get();
  }


  /**
   * Changes the color of any pixel or writes an image directly into the image. The <b>x</b> and <b>y</b> parameter specify the pixel or the upper-left corner of the image. The <b>color</b> parameter specifies the color value.<br><br>Setting the color of a single pixel with <b>set(x, y)</b> is easy, but not as fast as putting the data directly into <b>pixels[]</b>. The equivalent statement to "set(x, y, #000000)" using <b>pixels[]</b> is "pixels[y*width+x] = #000000". Processing requires calling <b>loadPixels()</b> to load the display window data into the <b>pixels[]</b> array before getting the values and calling <b>updatePixels()</b> to update the window.
   * <br><br>As of release 0149, this function ignores <b>imageMode()</b>.
   *
   * @webref
   * @brief     Writes a color to any pixel or writes an image into another
   * @param x x-coordinate of the pixel or upper-left corner of the image
   * @param y y-coordinate of the pixel or upper-left corner of the image
   * @param c any value of the color datatype
   *
   * @see processing.core.PImage#get(int, int, int, int)
   * @see processing.core.PImage#pixels
   * @see processing.core.PImage#copy(PImage, int, int, int, int, int, int, int, int)
   */
  public void set(int x, int y, int c) {
    if (recorder != null) recorder.set(x, y, c);
    g.set(x, y, c);
  }


  /**
   * Efficient method of drawing an image's pixels directly to this surface.
   * No variations are employed, meaning that any scale, tint, or imageMode
   * settings will be ignored.
   */
  public void set(int x, int y, PImage src) {
    if (recorder != null) recorder.set(x, y, src);
    g.set(x, y, src);
  }


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
    if (recorder != null) recorder.mask(maskArray);
    g.mask(maskArray);
  }


  /**
   * Masks part of an image from displaying by loading another image and using it as an alpha channel.
   *  This mask image should only contain grayscale data, but only the blue color channel is used.
   *  The mask image needs to be the same size as the image to which it is applied.
   *  In addition to using a mask image, an integer array containing the alpha channel data can be specified directly.
   *  This method is useful for creating dynamically generated alpha masks.
   *  This array must be of the same length as the target image's pixels array and should contain only grayscale data of values between 0-255.
   * @webref
   * @brief     Masks part of the image from displaying
   * @param maskImg any PImage object used as the alpha channel for "img", needs to be same size as "img"
   */
  public void mask(PImage maskImg) {
    if (recorder != null) recorder.mask(maskImg);
    g.mask(maskImg);
  }


  public void filter(int kind) {
    if (recorder != null) recorder.filter(kind);
    g.filter(kind);
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
    if (recorder != null) recorder.filter(kind, param);
    g.filter(kind, param);
  }


  /**
   * Copy things from one area of this image
   * to another area in the same image.
   */
  public void copy(int sx, int sy, int sw, int sh,
                   int dx, int dy, int dw, int dh) {
    if (recorder != null) recorder.copy(sx, sy, sw, sh, dx, dy, dw, dh);
    g.copy(sx, sy, sw, sh, dx, dy, dw, dh);
  }


  /**
   * Copies a region of pixels from one image into another. If the source and destination regions aren't the same size, it will automatically resize source pixels to fit the specified target region. No alpha information is used in the process, however if the source image has an alpha channel set, it will be copied as well.
   * <br><br>As of release 0149, this function ignores <b>imageMode()</b>.
   *
   * @webref
   * @brief     Copies the entire image
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
    if (recorder != null) recorder.copy(src, sx, sy, sw, sh, dx, dy, dw, dh);
    g.copy(src, sx, sy, sw, sh, dx, dy, dw, dh);
  }


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
    return PGraphics.blendColor(c1, c2, mode);
  }


  /**
   * Blends one area of this image to another area.
   *
   *
   * @see processing.core.PImage#blendColor(int,int,int)
   */
  public void blend(int sx, int sy, int sw, int sh,
                    int dx, int dy, int dw, int dh, int mode) {
    if (recorder != null) recorder.blend(sx, sy, sw, sh, dx, dy, dw, dh, mode);
    g.blend(sx, sy, sw, sh, dx, dy, dw, dh, mode);
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
    if (recorder != null) recorder.blend(src, sx, sy, sw, sh, dx, dy, dw, dh, mode);
    g.blend(src, sx, sy, sw, sh, dx, dy, dw, dh, mode);
  }
}
