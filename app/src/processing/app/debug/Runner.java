/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-08 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package processing.app.debug;

import processing.app.*;
import processing.core.*;

import java.awt.Point;
import java.io.*;
import java.util.*;

import com.sun.jdi.*;
import com.sun.jdi.connect.*;
import com.sun.jdi.event.ExceptionEvent;


/**
 * Runs a compiled sketch. As of release 0136, all sketches are run externally
 * to the environment so that a debugging interface can be used. This opens up
 * future options for a decent debugger, but in the meantime fixes several
 * problems with output and error streams, messages getting lost on Mac OS X,
 * the run/stop buttons not working, libraries not shutting down, exceptions
 * not coming through, exceptions being printed twice, having to force quit
 * if you make a bad while() loop, and so on.
 */
public class Runner implements MessageConsumer {

  private boolean presenting;

  // Object that listens for error messages or exceptions.
  private RunnerListener listener;

  // Running remote VM
  private VirtualMachine vm;

  // Thread transferring remote error stream to our error stream
  private Thread errThread = null;

  // Thread transferring remote output stream to our output stream
  private Thread outThread = null;

  // Mode for tracing the Trace program (default= 0 off)
  private int debugTraceMode = 0;

  //  Do we want to watch assignments to fields
  private boolean watchFields = false;

  // Class patterns for which we don't want events
  private String[] excludes = {
      "java.*", "javax.*", "sun.*", "com.sun.*",
      "apple.*",
      "processing.*"
  };

  private RunnerException exception;
  //private PrintStream leechErr;

  private Editor editor;
  private Sketch sketch;
  private String appletClassName;

//  private boolean newMessage;
//  private int messageLineCount;
//  private boolean foundMessageSource;
//
//  private SystemOutSiphon processInput;
//  private OutputStream processOutput;
//  private MessageSiphon processError;


  public Runner(Sketch sketch, String appletClassName,
                boolean presenting, RunnerListener listener) {
    this.sketch = sketch;
    this.appletClassName = appletClassName;
    this.presenting = presenting;
    this.listener = listener;

    if (listener instanceof Editor) {
      this.editor = (Editor) listener;
    }
  }


  public void launch() {
    // TODO entire class is a total mess as of release 0136.
    // This will be cleaned up significantly over the next couple months.

    // all params have to be stored as separate items,
    // so a growable array needs to be used. i.e. -Xms128m -Xmx1024m
    // will throw an error if it's shoved into a single array element
    //Vector params = new Vector();

    // get around Apple's Java 1.5 bugs
    //params.add("/System/Library/Frameworks/JavaVM.framework/Versions/1.4.2/Commands/java");
    //params.add("java");
    //System.out.println("0");

    String[] machineParamList = getMachineParams();
    String[] sketchParamList = getSketchParams();

    vm = launchVirtualMachine(machineParamList, sketchParamList);
    if (vm != null) {
      generateTrace(null);
//      try {
//        generateTrace(new PrintWriter("/Users/fry/Desktop/output.txt"));
//      } catch (Exception e) {
//        e.printStackTrace();
//      }
    }
  }


  protected String[] getMachineParams() {
    ArrayList params = new ArrayList();

    //params.add("-Xint"); // interpreted mode
    //params.add("-Xprof");  // profiler
    //params.add("-Xaprof");  // allocation profiler
    //params.add("-Xrunhprof:cpu=samples");  // old-style profiler

    // TODO change this to use run.args = true, run.args.0, run.args.1, etc.
    // so that spaces can be included in the arg names
    String options = Preferences.get("run.options");
    if (options.length() > 0) {
      String pieces[] = PApplet.split(options, ' ');
      for (int i = 0; i < pieces.length; i++) {
        String p = pieces[i].trim();
        if (p.length() > 0) {
          params.add(p);
        }
      }
    }

//    params.add("-Djava.ext.dirs=nuffing");

    if (Preferences.getBoolean("run.options.memory")) {
      params.add("-Xms" + Preferences.get("run.options.memory.initial") + "m");
      params.add("-Xmx" + Preferences.get("run.options.memory.maximum") + "m");
    }

    if (Base.isMacOS()) {
      params.add("-Xdock:name=" + appletClassName);
//      params.add("-Dcom.apple.mrj.application.apple.menu.about.name=" +
//                 sketch.getMainClassName());
    }
    // sketch.libraryPath might be ""
    // librariesClassPath will always have sep char prepended
    params.add("-Djava.library.path=" +
               sketch.getLibraryPath() +
               File.pathSeparator +
               System.getProperty("java.library.path"));

    params.add("-cp");
    params.add(sketch.getClassPath());
//    params.add(sketch.getClassPath() +
//        File.pathSeparator +
//        Base.librariesClassPath);

    //PApplet.println(PApplet.split(sketch.classPath, ':'));

    String outgoing[] = new String[params.size()];
    params.toArray(outgoing);

    //PApplet.println(outgoing);
//    PApplet.println(PApplet.split(outgoing[0], ":"));
//    PApplet.println();
//    PApplet.println("class path");
//    PApplet.println(PApplet.split(outgoing[2], ":"));

    return outgoing;
    //return (String[]) params.toArray();

//  System.out.println("sketch class path");
//  PApplet.println(PApplet.split(sketch.classPath, ';'));
//  System.out.println();
//  System.out.println("libraries class path");
//  PApplet.println(PApplet.split(Base.librariesClassPath, ';'));
//  System.out.println();
  }


  protected String[] getSketchParams() {
    ArrayList<String> params = new ArrayList<String>();

    params.add("processing.core.PApplet");

    // If there was a saved location (this guy has been run more than once)
    // then the location will be set to the last position of the sketch window.
    // This will be passed to the PApplet runner using something like
    // --location=30,20
    // Otherwise, the editor location will be passed, and the applet will
    // figure out where to place itself based on the editor location.
    // --editor-location=150,20
    if (editor != null) {  // if running processing-cmd, don't do placement
      Point windowLocation = editor.getSketchLocation();
      if (windowLocation != null) {
        params.add(PApplet.ARGS_LOCATION + "=" +
                   windowLocation.x + "," + windowLocation.y);
      } else {
        Point editorLocation = editor.getLocation();
        params.add(PApplet.ARGS_EDITOR_LOCATION + "=" +
                   editorLocation.x + "," + editorLocation.y);
      }
      params.add(PApplet.ARGS_EXTERNAL);
    }

    params.add(PApplet.ARGS_DISPLAY + "=" +
               Preferences.get("run.display"));
    params.add(PApplet.ARGS_SKETCH_FOLDER + "=" +
               sketch.getFolder().getAbsolutePath());

    if (presenting) {
      params.add(PApplet.ARGS_PRESENT);
      if (Preferences.getBoolean("run.present.exclusive")) {
        params.add(PApplet.ARGS_EXCLUSIVE);
      }
      params.add(PApplet.ARGS_STOP_COLOR + "=" +
          Preferences.get("run.present.stop.color"));
      params.add(PApplet.ARGS_BGCOLOR + "=" +
          Preferences.get("run.present.bgcolor"));
    }

    params.add(appletClassName);

//    String outgoing[] = new String[params.size()];
//    params.toArray(outgoing);
//    return outgoing;
    return (String[]) params.toArray(new String[0]);
  }


  /*
  protected VirtualMachine launchVirtualMachine_sun(String[] vmParams, String[] classParams) {
    //vm = launchTarget(sb.toString());
    LaunchingConnector connector =
      findLaunchingConnector("com.sun.jdi.CommandLineLaunch");
    //Map arguments = connectorArguments(connector, mainArgs);

    PApplet.println(connector);  // gets the defaults

    Map arguments = connector.defaultArguments();
    //System.out.println(arguments);

//    for (Iterator itr = arguments.keySet().iterator(); itr.hasNext(); ) {
//      Connector.Argument argument =
//        (Connector.Argument) arguments.get(itr.next());
//      System.out.println(argument);
//    }

    //connector.transport().

    Connector.Argument mainArg =
      (Connector.Argument)arguments.get("main");
    if (mainArg == null) {
      throw new Error("Bad launching connector");
    }
    String mainArgs = "";
    //mainArgs = addArgument(mainArgs, className);
    if (classParams != null) {
      for (int i = 0; i < classParams.length; i++) {
        mainArgs = addArgument(mainArgs, classParams[i], ' ');
      }
    }
    mainArg.setValue(mainArgs);

    //System.out.println("main args are: ");
    //System.out.println(mainArgs);

//    if (watchFields) {
//      // We need a VM that supports watchpoints
//      Connector.Argument optionArg =
//        (Connector.Argument)arguments.get("options");
//      if (optionArg == null) {
//        throw new Error("Bad launching connector");
//      }
//      optionArg.setValue("-classic");
//    }
    String optionArgs = "";
    for (int i = 0; i < vmParams.length; i++) {
      optionArgs = addArgument(optionArgs, vmParams[i], ' ');
    }
    // prevent any incorrect transport address b.s. from being added
    // -Xrunjdwp:transport=dt_socket,address=cincinnati118.ipcorporate.com:55422,suspend=y
    //optionArgs = addArgument(optionArgs, "-agentlib:jdwp=transport=dt_socket,address=localhost:12345,suspend=y", ' ');
    //optionArgs += " -Xrunjdwp:transport=dt_socket,address=localhost:55422,suspend=y";
    //optionArgs = optionArgs + " -agentlib:jdwp=transport=dt_socket";
    //optionArgs = addArgument(optionArgs, "-Xrunjdwp:transport=dt_socket,address=localhost:55422,suspend=y", ' ');

    //optionArgs = addArgument(optionArgs, "address=127.0.0.1:54321", ' ');
    //optionArgs = addArgument(optionArgs, "localAddress", ' ');

    Connector.Argument optionArg =
      (Connector.Argument)arguments.get("options");
    optionArg.setValue(optionArgs);

//    Connector.Argument addressArg =
//      (Connector.Argument)arguments.get("address");
    //arguments.put("raw.address", new Connector.Argument("blah"));
    //PApplet.println("it's gonna be " + addressArg);

    //arguments.put("address", "localhost");

//    Connector.Argument addressArg =
//      (Connector.Argument)arguments.get("address");
//    addressArg.setValue("localhost");

//    System.out.println("option args are: ");
//    System.out.println(arguments.get("options"));

    System.out.println("args are " + arguments);

    // com.sun.tools.jdi.SunCommandLineLauncher

    // http://java.sun.com/j2se/1.5.0/docs/guide/jpda/conninv.html#sunlaunch
    try {
      return connector.launch(arguments);
    } catch (IOException exc) {
      throw new Error("Unable to launch target VM: " + exc);
    } catch (IllegalConnectorArgumentsException exc) {
      throw new Error("Internal error: " + exc);
    } catch (VMStartException exc) {
      exc.printStackTrace();
      System.err.println();
      System.err.println("Could not run the sketch.");
      System.err.println("Make sure that you haven't set the maximum available memory too high.");
      System.err.println("For more information, read revisions.txt and Help -> Troubleshooting.");
      //System.err.println("Target VM failed to initialize:");
      //System.err.println("msg is " + exc.getMessage());
      //exc.printStackTrace();
      //throw new Error("Target VM failed to initialize: " +
      //exc.getMessage());
      //throw new Error(exc.getMessage());
      //throw new RunnerException(exc.getMessage());
      //editor.error(exc);
      editor.error("Could not run the sketch.");
      return null;
    }
  }
  */


  protected VirtualMachine launchVirtualMachine(String[] vmParams,
                                                String[] classParams) {
    //vm = launchTarget(sb.toString());
    LaunchingConnector connector =
      findLaunchingConnector("com.sun.jdi.RawCommandLineLaunch");
    //PApplet.println(connector);  // gets the defaults

    //Map arguments = connectorArguments(connector, mainArgs);
    Map arguments = connector.defaultArguments();

    Connector.Argument commandArg =
      (Connector.Argument)arguments.get("command");
    // Using localhost instead of 127.0.0.1 sometimes causes a
    // "Transport Error 202" error message when trying to run.
    // http://dev.processing.org/bugs/show_bug.cgi?id=895
    String addr = "127.0.0.1:" + (8000 + (int) (Math.random() * 1000));
    //String addr = "localhost:" + (8000 + (int) (Math.random() * 1000));
    //String addr = "" + (8000 + (int) (Math.random() * 1000));

    String commandArgs = Base.isWindows() ?
      "java -Xrunjdwp:transport=dt_shmem,address=" + addr + ",suspend=y " :
      "java -Xrunjdwp:transport=dt_socket,address=" + addr + ",suspend=y ";
    //String commandArgs = "java -agentlib:jdwp=transport=dt_socket,address=" + addr + ",suspend=y ";
    //String commandArgs = "java -agentlib:jdwp=transport=dt_socket,address=" + addr + ",server=n,suspend=y ";
    for (int i = 0; i < vmParams.length; i++) {
      commandArgs = addArgument(commandArgs, vmParams[i], ' ');
    }
    if (classParams != null) {
      for (int i = 0; i < classParams.length; i++) {
        commandArgs = addArgument(commandArgs, classParams[i], ' ');
      }
    }
    commandArg.setValue(commandArgs);

    Connector.Argument addressArg =
      (Connector.Argument)arguments.get("address");
    addressArg.setValue(addr);

    //PApplet.println(connector);  // prints the current
    //com.sun.tools.jdi.AbstractLauncher al;
    //com.sun.tools.jdi.RawCommandLineLauncher rcll;

    //System.out.println(PApplet.javaVersion);
    // http://java.sun.com/j2se/1.5.0/docs/guide/jpda/conninv.html#sunlaunch
    try {
      return connector.launch(arguments);
    } catch (IOException exc) {
      throw new Error("Unable to launch target VM: " + exc);
    } catch (IllegalConnectorArgumentsException exc) {
      throw new Error("Internal error: " + exc);
    } catch (VMStartException exc) {
      Process p = exc.process();
      //System.out.println(p);
      String[] errorStrings = PApplet.loadStrings(p.getErrorStream());
      /*String[] inputStrings =*/ PApplet.loadStrings(p.getInputStream());

      if (errorStrings != null && errorStrings.length > 1) {
        if (errorStrings[0].indexOf("Invalid maximum heap size") != -1) {
          Base.showWarning("Way Too High",
                           "Please lower the value for \u201Cmaximum available memory\u201D in the\n" +
                           "Preferences window. For more information, read Help \u2192 Troubleshooting.",
                           exc);
        } else {
          PApplet.println(errorStrings);
          System.err.println("Using startup command:");
          PApplet.println(arguments);
        }
      } else {
        exc.printStackTrace();
        System.err.println("Could not run the sketch (Target VM failed to initialize).");
        if (Preferences.getBoolean("run.options.memory")) {
          // Only mention this if they've even altered the memory setup
          System.err.println("Make sure that you haven't set the maximum available memory too high.");
        }
        System.err.println("For more information, read revisions.txt and Help \u2192 Troubleshooting.");
      }
      if (editor != null) {
        listener.statusError("Could not run the sketch.");
      }
      return null;
    }
  }


  private static boolean hasWhitespace(String string) {
    int length = string.length();
    for (int i = 0; i < length; i++) {
      if (Character.isWhitespace(string.charAt(i))) {
        return true;
      }
    }
    return false;
  }


  private static String addArgument(String string, String argument, char sep) {
    if (hasWhitespace(argument) || argument.indexOf(',') != -1) {
      // Quotes were stripped out for this argument, add 'em back.
      StringBuffer buffer = new StringBuffer(string);
      buffer.append('"');
      for (int i = 0; i < argument.length(); i++) {
        char c = argument.charAt(i);
        if (c == '"') {
          buffer.append('\\');
        }
        buffer.append(c);
      }
      buffer.append('\"');
      buffer.append(sep);
      return buffer.toString();
    } else {
      return string + argument + String.valueOf(sep);
    }
  }


  /**
   * Generate the trace.
   * Enable events, start thread to display events,
   * start threads to forward remote error and output streams,
   * resume the remote VM, wait for the final event, and shutdown.
   */
  void generateTrace(PrintWriter writer) {
    vm.setDebugTraceMode(debugTraceMode);

    EventThread eventThread = null;
    //if (writer != null) {
    eventThread = new EventThread(this, vm, excludes, writer);
    eventThread.setEventRequests(watchFields);
    eventThread.start();
    //}

    //redirectOutput();

    Process process = vm.process();

//  processInput = new SystemOutSiphon(process.getInputStream());
//  processError = new MessageSiphon(process.getErrorStream(), this);

    // Copy target's output and error to our output and error.
//    errThread = new StreamRedirectThread("error reader",
//        process.getErrorStream(),
//        System.err);
    MessageSiphon ms = new MessageSiphon(process.getErrorStream(), this);
    errThread = ms.thread;

    outThread = new StreamRedirectThread("output reader",
        process.getInputStream(),
        System.out);

    errThread.start();
    outThread.start();

    vm.resume();
    //System.out.println("done with resume");

    // Shutdown begins when event thread terminates
    try {
      if (eventThread != null) eventThread.join();
      // Bug #852 tracked to this next line in the code.
      // http://dev.processing.org/bugs/show_bug.cgi?id=852
      errThread.join(); // Make sure output is forwarded
      outThread.join(); // before we exit
      //System.out.println("out of it");

      // At this point, disable the run button.
      // This happens when the sketch is exited by hitting ESC,
      // or the user manually closes the sketch window.
      // TODO this should be handled better, should it not?
      if (editor != null) {
        editor.internalRunnerClosed();
      }

    } catch (InterruptedException exc) {
      // we don't interrupt
    }
    //System.out.println("and leaving");
    if (writer != null) writer.close();
  }


  /**
   * Find a com.sun.jdi.CommandLineLaunch connector
   */
  LaunchingConnector findLaunchingConnector(String connectorName) {
    //VirtualMachineManager mgr = Bootstrap.virtualMachineManager();

    // Get the default connector.
    // Not useful here since they all need different args.
//      System.out.println(Bootstrap.virtualMachineManager().defaultConnector());
//      return Bootstrap.virtualMachineManager().defaultConnector();

    List connectors = Bootstrap.virtualMachineManager().allConnectors();

    // code to list available connectors
//    Iterator iter2 = connectors.iterator();
//    while (iter2.hasNext()) {
//      Connector connector = (Connector)iter2.next();
//      System.out.println("connector name is " + connector.name());
//    }

    Iterator iter = connectors.iterator();
    while (iter.hasNext()) {
      Connector connector = (Connector)iter.next();
      if (connector.name().equals(connectorName)) {
        return (LaunchingConnector)connector;
      }
    }
    throw new Error("No launching connector");
  }


  public void exception(ExceptionEvent event) {
    ObjectReference or = event.exception();
    ReferenceType rt = or.referenceType();
    String exceptionName = rt.name();
    //Field messageField = Throwable.class.getField("detailMessage");
    Field messageField = rt.fieldByName("detailMessage");
//    System.out.println("field " + messageField);
    Value messageValue = or.getValue(messageField);
//    System.out.println("mess val " + messageValue);

    int last = exceptionName.lastIndexOf('.');
    String message = exceptionName.substring(last + 1);
    if (messageValue != null) {
      String messageStr = messageValue.toString();
      if (messageStr.startsWith("\"")) {
        messageStr = messageStr.substring(1, messageStr.length() - 1);
      }
      message += ": " + messageStr;
    }
//    System.out.println("mess type " + messageValue.type());
    //StringReference messageReference = (StringReference) messageValue.type();

//    System.out.println(or.referenceType().fields());
//    if (name.startsWith("java.lang.")) {
//      name = name.substring(10);
    if (exceptionName.equals("java.lang.OutOfMemoryError")) {
      listener.statusError("OutOfMemoryError: You may need to increase the memory setting in Preferences.");
      System.err.println("An OutOfMemoryError means that your code is either using up too much memory");
      System.err.println("because of a bug (e.g. creating an array that's too large, or unintentionally");
      System.err.println("loading thousands of images), or that your sketch may need more memory to run.");
      System.err.println("If your sketch uses a lot of memory (for instance if it loads a lot of data files)");
      System.err.println("you can increase the memory available to your sketch using the Preferences window.");

    } else if (exceptionName.equals("java.lang.StackOverflowError")) {
      listener.statusError("StackOverflowError: This sketch is attempting too much recursion.");
      System.err.println("A StackOverflowError means that you have a bug that's causing a function");
      System.err.println("to be called recursively (it's calling itself and going in circles),");
      System.err.println("or you're intentionally calling a recursive function too much,");
      System.err.println("and your code should be rewritten in a more efficient manner.");

    } else if (exceptionName.equals("java.lang.UnsupportedClassVersionError")) {
      listener.statusError("UnsupportedClassVersionError: A library is using code compiled with an unsupported version of Java.");
      System.err.println("This version of Processing only supports libraries and JAR files compiled for Java 1.5.");
      System.err.println("A library used by this sketch was compiled for Java 1.6 or later, ");
      System.err.println("and needs to be recompiled to be compatible with Java 1.5.");

    } else if (exceptionName.equals("java.lang.NoSuchMethodError") || exceptionName.equals("java.lang.NoSuchFieldError")) {
      listener.statusError(exceptionName.substring(10) + ": You're probably using a library that's incompatible with this version of Processing.");

    } else if (message.equals("ClassNotFoundException: quicktime.std.StdQTException")) {
      listener.statusError("Could not find QuickTime, please reinstall QuickTime 7 or later.");

    } else {
      reportException(message, event.thread());
    }
    editor.internalRunnerClosed();
  }


  // This may be called more than one time per error in the VM,
  // presumably because exceptions might be wrapped inside others,
  // and this will fire for both.
  protected void reportException(String message, ThreadReference thread) {
    try {
      int codeIndex = -1;
      int lineNumber = -1;

      // Any of the thread.blah() methods can throw an AbsentInformationEx
      // if that bit of data is missing. If so, just write out the error
      // message to the console.
      List<StackFrame> frames = thread.frames();
      for (StackFrame frame : frames) {
        //System.out.println("frame: " + frame);
        Location location = frame.location();
        String filename = null;
        filename = location.sourceName();
        lineNumber = location.lineNumber();

        String appletJavaFile = appletClassName + ".java";
        SketchCode errorCode = null;
        if (filename.equals(appletJavaFile)) {
          for (SketchCode code : sketch.getCode()) {
            if (code.isExtension("pde")) {
              if (lineNumber >= code.getPreprocOffset()) {
                errorCode = code;
              }
            }
          }
        } else {
          for (SketchCode code : sketch.getCode()) {
            if (code.isExtension("java")) {
              if (filename.equals(code.getFileName())) {
                errorCode = code;
              }
            }
          }
        }
        codeIndex = sketch.getCodeIndex(errorCode);

        if (codeIndex != -1) {
          //System.out.println("got line num " + lineNumber);
          // in case this was a tab that got embedded into the main .java
          lineNumber -= sketch.getCode(codeIndex).getPreprocOffset();

          // lineNumber is 1-indexed, but editor wants zero-indexed
          lineNumber--;

          // getMessage() will be what's shown in the editor
          exception = new RunnerException(message, codeIndex, lineNumber, -1);
          exception.hideStackTrace();
          listener.statusError(exception);
          return;
        }
      }
    } catch (AbsentInformationException e) {
      //e.printStackTrace();  // not useful
      exception = new RunnerException(message);
      exception.hideStackTrace();
      listener.statusError(exception);

    } catch (IncompatibleThreadStateException e) {
      e.printStackTrace();
    }
  }


  public void close() {
    // TODO make sure stop() has already been called to exit the sketch

    // TODO actually kill off the vm here
    if (vm != null) {
      try {
        vm.exit(0);

      } catch (com.sun.jdi.VMDisconnectedException vmde) {
        // if the vm has disconnected on its own, ignore message
        //System.out.println("harmless disconnect " + vmde.getMessage());
        // TODO shouldn't need to do this, need to do more cleanup
      }
      vm = null;
    }

    //if (window != null) window.hide();
//    if (window != null) {
//      //System.err.println("disposing window");
//      window.dispose();
//      window = null;
//    }

    /*
    if (process != null) {
      try {
        process.destroy();
      } catch (Exception e) {
        //System.err.println("(ignored) error while destroying");
        //e.printStackTrace();
      }
      process = null;
    }
    */
  }


  // made synchronized for rev 87
  // attempted to remove synchronized for 0136 to fix bug #775 (no luck tho)
  // http://dev.processing.org/bugs/show_bug.cgi?id=775
  synchronized public void message(String s) {
    //System.out.println("M" + s.length() + ":" + s.trim()); // + "MMM" + s.length());

    // this eats the CRLFs on the lines.. oops.. do it later
    //if (s.trim().length() == 0) return;

    // this is PApplet sending a message (via System.out.println)
    // that signals that the applet has been quit.
    if (s.indexOf(PApplet.EXTERNAL_STOP) == 0) {
      //System.out.println("external: quit");
      editor.internalCloseRunner();
      return;
    }

    // this is the PApplet sending us a message that the applet
    // is being moved to a new window location
    if (s.indexOf(PApplet.EXTERNAL_MOVE) == 0) {
      String nums = s.substring(s.indexOf(' ') + 1).trim();
      int space = nums.indexOf(' ');
      int left = Integer.parseInt(nums.substring(0, space));
      int top = Integer.parseInt(nums.substring(space + 1));
      editor.setSketchLocation(new Point(left, top));
      //System.out.println("external: move to " + left + " " + top);
      return;
    }

    // Removed while doing cleaning for 0145,
    // it seems that this is never actually printed out.
    /*
    // this is PApplet sending a message saying "i'm about to spew
    // a stack trace because an error occurred during PApplet.run()"
    if (s.indexOf(PApplet.LEECH_WAKEUP) == 0) {
      // newMessage being set to 'true' means that the next time
      // message() is called, expect the first line of the actual
      // error message & stack trace to be sent from the applet.
      newMessage = true;
      return;  // this line ignored
    }
    */

    // these are used for debugging, in case there are concerns
    // that some errors aren't coming through properly
    /*
    if (s.length() > 2) {
      System.err.println(newMessage);
      System.err.println("message " + s.length() + ":" + s);
    }
    */
    // always shove out the mesage, since it might not fall under
    // the same setup as we're expecting
    System.err.print(s);
    //System.err.println("[" + s.length() + "] " + s);
    System.err.flush();

//    // exit here because otherwise the exception name
//    // may be titled with a blank string
//    if (s.trim().length() == 0) return;
//
//    // annoying, because it seems as though the terminators
//    // aren't being sent properly
//    //System.err.println(s);
//
//    //if (newMessage && s.length() > 2) {
//    if (newMessage) {
//      exception = new RunnerException(s);  // type of java ex
//      exception.hideStackTrace();
//      //System.out.println("setting ex type to " + s);
//      newMessage = false;
//      foundMessageSource = false;
//      messageLineCount = 0;
//
//    } else {
//      messageLineCount++;
//
//      /*
//java.lang.NullPointerException
//        at javatest.<init>(javatest.java:5)
//        at Temporary_2425_1153.draw(Temporary_2425_1153.java:11)
//        at PApplet.nextFrame(PApplet.java:481)
//        at PApplet.run(PApplet.java:428)
//        at java.lang.Thread.run(Unknown Source)
//      */
//
//      if (!foundMessageSource) {
//        //    "     at javatest.<init>(javatest.java:5)"
//        // -> "javatest.<init>(javatest.java:5)"
//        int atIndex = s.indexOf("at ");
//        if (atIndex == -1) {
//          //System.err.println(s);  // stop double-printing exceptions
//          return;
//        }
//        s = s.substring(atIndex + 3);
//
//        // added for 0124 to improve error handling
//        // not highlighting lines if it's in the p5 code
//        if (s.startsWith("processing.")) return;
//        // no highlight if it's java.lang.whatever
//        if (s.startsWith("java.")) return;
//
//        //    "javatest.<init>(javatest.java:5)"
//        // -> "javatest.<init>" and "(javatest.java:5)"
//        int startParen = s.indexOf('(');
//        // at javatest.<init>(javatest.java:5)
//        //String pkgClassFxn = null;
//        //String fileLine = null;
//        int codeIndex = -1;
//        int lineNumber = -1;
//
//        if (startParen == -1) {
//          //pkgClassFxn = s;
//
//        } else {
//          //pkgClassFxn = s.substring(0, startParen);
//
//          // "(javatest.java:5)"
//          String fileAndLine = s.substring(startParen + 1);
//          int stopParen = fileAndLine.indexOf(')');
//          //fileAndLine = fileAndLine.substring(0, fileAndLine.length() - 1);
//          fileAndLine = fileAndLine.substring(0, stopParen);
//          //System.out.println("file 'n line " + fileAndLine);
//
//          //if (!fileAndLine.equals("Unknown Source")) {
//          // "javatest.java:5"
//          int colonIndex = fileAndLine.indexOf(':');
//          if (colonIndex != -1) {
//            String filename = fileAndLine.substring(0, colonIndex);
//            // "javatest.java" and "5"
//            //System.out.println("filename = " + filename);
//            //System.out.println("pre0 = " + sketch.code[0].preprocName);
//            //for (int i = 0; i < sketch.codeCount; i++) {
//            //System.out.println(i + " " + sketch.code[i].lineOffset + " " +
//            //                   sketch.code[i].preprocName);
//            //}
//            lineNumber =
//              Integer.parseInt(fileAndLine.substring(colonIndex + 1)) - 1;
//
//            for (int i = 0; i < sketch.getCodeCount(); i++) {
//              SketchCode code = sketch.getCode(i);
//              //System.out.println(code.preprocName + " " + lineNumber + " " +
//              //                 code.preprocOffset);
//              if (((code.preprocName == null) &&
//                   (lineNumber >= code.preprocOffset)) ||
//                  ((code.preprocName != null) &&
//                   code.preprocName.equals(filename))) {
//                codeIndex = i;
//                //System.out.println("got codeindex " + codeIndex);
//                //break;
//                //} else if (
//              }
//            }
//
//            if (codeIndex != -1) {
//              //System.out.println("got line num " + lineNumber);
//              // in case this was a tab that got embedded into the main .java
//              lineNumber -= sketch.getCode(codeIndex).preprocOffset;
//
//              // this may have a paren on the end, if so need to strip
//              // down to just the digits
//              /*
//              int lastNumberIndex = colonIndex + 1;
//              while ((lastNumberIndex < fileAndLine.length()) &&
//                     Character.isDigit(fileAndLine.charAt(lastNumberIndex))) {
//                lastNumberIndex++;
//              }
//              */
//
//              // lineNumber is 1-indexed, but editor wants zero-indexed
//              // getMessage() will be what's shown in the editor
//              exception =
//                new RunnerException(exception.getMessage(),
//                                    codeIndex, lineNumber, -1);
//              exception.hideStackTrace();
//              foundMessageSource = true;
//            }
//          }
//        }
//        editor.error(exception);
//
//      /*
//      int index = s.indexOf(className + ".java");
//      if (index != -1) {
//        int len = (className + ".java").length();
//        String lineNumberStr = s.substring(index + len + 1);
//        index = lineNumberStr.indexOf(')');
//        lineNumberStr = lineNumberStr.substring(0, index);
//        try {
//          exception.line = Integer.parseInt(lineNumberStr) - 1; //2;
//        } catch (NumberFormatException e) { }
//          //e.printStackTrace();  // a recursive error waiting to happen?
//        // if nfe occurs, who cares, still send the error on up
//        editor.error(exception);
//      */
//
//        /*
//          // WARNING THESE ARE DISABLED!!
//      } else if ((index = s.indexOf(className + ".class")) != -1) {
//        // code to check for:
//        // at Temporary_484_3845.loop(Compiled Code)
//        // would also probably get:
//        // at Temporary_484_3845.loop
//        // which (i believe) is used by the mac and/or jview
//        String functionStr = s.substring(index +
//                                         (className + ".class").length() + 1);
//        index = functionStr.indexOf('(');
//        if (index != -1) {
//          functionStr = functionStr.substring(0, index);
//        }
//        exception = new RunnerException(//"inside \"" + functionStr + "()\": " +
//                                     exception.getMessage() +
//                                     " inside " + functionStr + "() " +
//                                     "[add Compiler.disable() to setup()]");
//        editor.error(exception);
//        // this will fall through in tihs example:
//        // at Temporary_4636_9696.pootie(Compiled Code)
//        // at Temporary_4636_9696.loop(Temporary_4636_9696.java:24)
//        // because pootie() (re)sets the exception title
//        // and throws it, but then the line number gets set
//        // because of the line that comes after
//        */
//
//      } else if (messageLineCount > 10) {  // 5 -> 10 for 0088
//        // this means the class name may not be mentioned
//        // in the stack trace.. this is just a general purpose
//        // error, but needs to make it through anyway.
//        // so if five lines have gone past, might as well signal
//        messageLineCount = -100;
//        exception = new RunnerException(exception.getMessage());
//        exception.hideStackTrace();
//        editor.error(exception);
//
//      } else {
//        //System.err.print(s);
//      }
//      //System.out.println("got it " + s);
//    }
  }


  //////////////////////////////////////////////////////////////


  /**
   * Siphons from an InputStream of System.out (from a Process)
   * and sends it to the real System.out.
   */
  class SystemOutSiphon implements Runnable {
    InputStream input;
    Thread thread;

    public SystemOutSiphon(InputStream input) {
      this.input = input;

      thread = new Thread(this);
      // unless this is set to min, it seems to hork the app
      // since it's in charge of stuffing the editor console with strings
      // maybe it's time to get rid of/fix that friggin console
      // ...disabled for 0075, with 0074's fix for code folder hanging
      // this only seems to make the console unresponsive
      //thread.setPriority(Thread.MIN_PRIORITY);
      thread.start();
    }

    public void run() {
      byte boofer[] = new byte[256];

      while (Thread.currentThread() == thread) {
        try {
          // can't use a buffered reader here because incremental
          // print statements are interesting too.. causes some
          // disparity with how System.err gets spewed, oh well.
          int count = input.read(boofer, 0, boofer.length);
          if (count == -1) {
            thread = null;

          } else {
            System.out.print(new String(boofer, 0, count));
            //System.out.flush();
          }

        } catch (IOException e) {
          // this is prolly because the app was quit & the stream broken
          //e.printStackTrace(System.out);
          //e.printStackTrace();
          thread = null;

        } catch (Exception e) {
          //System.out.println("SystemOutSiphon: i just died in your arms tonight");
          // on mac os x, this will spew a "Bad File Descriptor" ex
          // each time an external app is shut down.
          //e.printStackTrace();
          thread = null;
          //System.out.println("");
        }
        //System.out.println("SystemOutSiphon: out");
        //thread = null;
      }
    }
  }
}
