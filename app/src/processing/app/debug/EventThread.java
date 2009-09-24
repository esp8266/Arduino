/*
 * @(#)EventThread.java	1.4 03/01/23
 *
 * Copyright 2003 Sun Microsystems, Inc. All rights reserved.
 * SUN PROPRIETARY/CONFIDENTIAL. Use is subject to license terms.
 */
/*
 * Copyright (c) 1997-2001 by Sun Microsystems, Inc. All Rights Reserved.
 * 
 * Sun grants you ("Licensee") a non-exclusive, royalty free, license to use,
 * modify and redistribute this software in source and binary code form,
 * provided that i) this copyright notice and license appear on all copies of
 * the software; and ii) Licensee does not utilize the software in a manner
 * which is disparaging to Sun.
 * 
 * This software is provided "AS IS," without a warranty of any kind. ALL
 * EXPRESS OR IMPLIED CONDITIONS, REPRESENTATIONS AND WARRANTIES, INCLUDING ANY
 * IMPLIED WARRANTY OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR
 * NON-INFRINGEMENT, ARE HEREBY EXCLUDED. SUN AND ITS LICENSORS SHALL NOT BE
 * LIABLE FOR ANY DAMAGES SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING
 * OR DISTRIBUTING THE SOFTWARE OR ITS DERIVATIVES. IN NO EVENT WILL SUN OR ITS
 * LICENSORS BE LIABLE FOR ANY LOST REVENUE, PROFIT OR DATA, OR FOR DIRECT,
 * INDIRECT, SPECIAL, CONSEQUENTIAL, INCIDENTAL OR PUNITIVE DAMAGES, HOWEVER
 * CAUSED AND REGARDLESS OF THE THEORY OF LIABILITY, ARISING OUT OF THE USE OF
 * OR INABILITY TO USE SOFTWARE, EVEN IF SUN HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES.
 * 
 * This software is not designed or intended for use in on-line control of
 * aircraft, air traffic, aircraft navigation or aircraft communications; or in
 * the design, construction, operation or maintenance of any nuclear
 * facility. Licensee represents and warrants that it will not use or
 * redistribute the Software for such purposes.
 */

package processing.app.debug;

import java.io.PrintWriter;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.Map;

import com.sun.jdi.*;
import com.sun.jdi.event.*;
import com.sun.jdi.request.*;

/**
 * This class processes incoming JDI events and displays them
 *
 * @version     @(#) EventThread.java 1.4 03/01/23 23:33:38
 * @author Robert Field
 */
public class EventThread extends Thread {

  private final Runner parent;
  private final VirtualMachine vm;   // Running VM
  private final String[] excludes;   // Packages to exclude
  private final PrintWriter writer;  // Where output goes

  static String nextBaseIndent = ""; // Starting indent for next thread

  private boolean connected = true;  // Connected to VM
  private boolean vmDied = true;     // VMDeath occurred

  // Maps ThreadReference to ThreadTrace instances
  private Map traceMap = new HashMap();

  EventThread(Runner parent, VirtualMachine vm, String[] excludes, PrintWriter writer) {
    super("event-handler");
    this.parent = parent;
    this.vm = vm;
    this.excludes = excludes;
    this.writer = writer;
  }

  /**
   * Run the event handling thread.  
   * As long as we are connected, get event sets off 
   * the queue and dispatch the events within them.
   */
  public void run() { 
    EventQueue queue = vm.eventQueue();
    while (connected) {
      try {
        EventSet eventSet = queue.remove();
        EventIterator it = eventSet.eventIterator();
        while (it.hasNext()) {
          handleEvent(it.nextEvent());
        }
        eventSet.resume();
      } catch (InterruptedException exc) {
        // Ignore
      } catch (VMDisconnectedException discExc) {
        handleDisconnectedException();
        break;
      }
    }
  }

  /**
   * Create the desired event requests, and enable 
   * them so that we will get events.
   * @param excludes     Class patterns for which we don't want events
   * @param watchFields  Do we want to watch assignments to fields
   */
  void setEventRequests(boolean watchFields) {
    EventRequestManager mgr = vm.eventRequestManager();

//    VMDeathRequest deathReq = mgr.createVMDeathRequest();
//    deathReq.setSuspendPolicy(EventRequest.SUSPEND_ALL);
//    deathReq.enable();
    
    // get only the uncaught exceptions
    ExceptionRequest excReq = mgr.createExceptionRequest(null, false, true);
    // this version reports all exceptions, caught or uncaught
    //ExceptionRequest excReq = mgr.createExceptionRequest(null, true, true);
    // suspend so we can step
    excReq.setSuspendPolicy(EventRequest.SUSPEND_ALL);
    excReq.enable();

    /*
    MethodEntryRequest menr = mgr.createMethodEntryRequest();
    for (int i=0; i<excludes.length; ++i) {
      menr.addClassExclusionFilter(excludes[i]);
    }
    menr.setSuspendPolicy(EventRequest.SUSPEND_NONE);
    menr.enable();

    MethodExitRequest mexr = mgr.createMethodExitRequest();
    for (int i=0; i<excludes.length; ++i) {
      mexr.addClassExclusionFilter(excludes[i]);
    }
    mexr.setSuspendPolicy(EventRequest.SUSPEND_NONE);
    mexr.enable();

    ThreadDeathRequest tdr = mgr.createThreadDeathRequest();
    // Make sure we sync on thread death
    tdr.setSuspendPolicy(EventRequest.SUSPEND_ALL);
    tdr.enable();
    */

    // turn on field watching (waaay slow)
    /*
//    if (watchFields) {
      ClassPrepareRequest cpr = mgr.createClassPrepareRequest();
      for (int i=0; i<excludes.length; ++i) {
        cpr.addClassExclusionFilter(excludes[i]);
      }
      cpr.setSuspendPolicy(EventRequest.SUSPEND_ALL);
      cpr.enable();
//    }
    */
  }

  /**
   * This class keeps context on events in one thread.
   * In this implementation, context is the indentation prefix.
   */
  class ThreadTrace {
    final ThreadReference thread;
    final String baseIndent;
    static final String threadDelta = "                     ";
    StringBuffer indent;

    ThreadTrace(ThreadReference thread) {
      this.thread = thread;
      this.baseIndent = nextBaseIndent;
      indent = new StringBuffer(baseIndent);
      nextBaseIndent += threadDelta;
      println("====== " + thread.name() + " ======");
    }

    private void println(String str) {
      if (writer != null) {
        writer.print(indent);
        writer.println(str);
        writer.flush();
      }
    }

    void methodEntryEvent(MethodEntryEvent event)  {
      println(event.method().name() + "  --  " 
          + event.method().declaringType().name());
      indent.append("| ");
    }

    void methodExitEvent(MethodExitEvent event)  {
      indent.setLength(indent.length()-2);
    }

    void fieldWatchEvent(ModificationWatchpointEvent event)  {
      Field field = event.field();
      Value value = event.valueToBe();
      println("    " + field.name() + " = " + value);
    }

    void exceptionEvent(ExceptionEvent event) {
      println("Exception: " + event.exception() + 
          " catch: " + event.catchLocation());
//      System.out.println("Exception: " + event.exception() + 
//                         " catch: " + event.catchLocation());

      // Step to the catch
      EventRequestManager mgr = vm.eventRequestManager();
      StepRequest req = mgr.createStepRequest(thread, 
          StepRequest.STEP_MIN,
          StepRequest.STEP_INTO);
      req.addCountFilter(1);  // next step only
      req.setSuspendPolicy(EventRequest.SUSPEND_ALL);
      req.enable();
    }

    // Step to exception catch
    void stepEvent(StepEvent event)  {
      // Adjust call depth
      int cnt = 0;
      indent = new StringBuffer(baseIndent);
      try {
        cnt = thread.frameCount();
      } catch (IncompatibleThreadStateException exc) {
      }
      while (cnt-- > 0) {
        indent.append("| ");
      }

      EventRequestManager mgr = vm.eventRequestManager();
      mgr.deleteEventRequest(event.request());
    }

    void threadDeathEvent(ThreadDeathEvent event)  {
      indent = new StringBuffer(baseIndent);
      println("====== " + thread.name() + " end ======");
    }
  }	

  /**
   * Returns the ThreadTrace instance for the specified thread,
   * creating one if needed.
   */
  ThreadTrace threadTrace(ThreadReference thread) {
    ThreadTrace trace = (ThreadTrace)traceMap.get(thread);
    if (trace == null) {
      trace = new ThreadTrace(thread);
      traceMap.put(thread, trace);
    }
    return trace;
  }

  /**
   * Dispatch incoming events
   */
  private void handleEvent(Event event) {
    if (event instanceof ExceptionEvent) {
      exceptionEvent((ExceptionEvent)event);
    } else if (event instanceof ModificationWatchpointEvent) {
      fieldWatchEvent((ModificationWatchpointEvent)event);
    } else if (event instanceof MethodEntryEvent) {
      methodEntryEvent((MethodEntryEvent)event);
    } else if (event instanceof MethodExitEvent) {
      methodExitEvent((MethodExitEvent)event);
    } else if (event instanceof StepEvent) {
      stepEvent((StepEvent)event);
    } else if (event instanceof ThreadDeathEvent) {
      threadDeathEvent((ThreadDeathEvent)event);
    } else if (event instanceof ClassPrepareEvent) {
      classPrepareEvent((ClassPrepareEvent)event);
    } else if (event instanceof VMStartEvent) {
      vmStartEvent((VMStartEvent)event);
    } else if (event instanceof VMDeathEvent) {
      vmDeathEvent((VMDeathEvent)event);
    } else if (event instanceof VMDisconnectEvent) {
      vmDisconnectEvent((VMDisconnectEvent)event);
    } else {
      throw new Error("Unexpected event type");
    }
  }

  /***
   * A VMDisconnectedException has happened while dealing with
   * another event. We need to flush the event queue, dealing only
   * with exit events (VMDeath, VMDisconnect) so that we terminate
   * correctly.
   */
  synchronized void handleDisconnectedException() {
    EventQueue queue = vm.eventQueue();
    while (connected) {
      try {
        EventSet eventSet = queue.remove();
        EventIterator iter = eventSet.eventIterator();
        while (iter.hasNext()) {
          Event event = iter.nextEvent();
          if (event instanceof VMDeathEvent) {
            vmDeathEvent((VMDeathEvent)event);
          } else if (event instanceof VMDisconnectEvent) {
            vmDisconnectEvent((VMDisconnectEvent)event);
          } 
        }
        eventSet.resume(); // Resume the VM
      } catch (InterruptedException exc) {
        // ignore
      }
    }
  }

  private void vmStartEvent(VMStartEvent event)  {
    if (writer != null) writer.println("-- VM Started --");
  }

  // Forward event for thread specific processing
  private void methodEntryEvent(MethodEntryEvent event)  {
    threadTrace(event.thread()).methodEntryEvent(event);
  }

  // Forward event for thread specific processing
  private void methodExitEvent(MethodExitEvent event)  {
    threadTrace(event.thread()).methodExitEvent(event);
  }

  // Forward event for thread specific processing
  private void stepEvent(StepEvent event)  {
    threadTrace(event.thread()).stepEvent(event);
  }

  // Forward event for thread specific processing
  private void fieldWatchEvent(ModificationWatchpointEvent event)  {
    threadTrace(event.thread()).fieldWatchEvent(event);
  }

  void threadDeathEvent(ThreadDeathEvent event)  {
    ThreadTrace trace = (ThreadTrace)traceMap.get(event.thread());
    if (trace != null) {  // only want threads we care about
      trace.threadDeathEvent(event);   // Forward event
    }
  }

  /**
   * A new class has been loaded.  
   * Set watchpoints on each of its fields
   */
  private void classPrepareEvent(ClassPrepareEvent event)  {
//    System.out.println(event);
//    List list = event.referenceType().methodsByName("stop");
//    Object o = list.get(0);
//    System.out.println("stop methods = " + list);
//    System.out.println(o.getClass());
    
    EventRequestManager mgr = vm.eventRequestManager();
    List fields = event.referenceType().visibleFields();
    for (Iterator it = fields.iterator(); it.hasNext(); ) {
      Field field = (Field)it.next();
      ModificationWatchpointRequest req = 
        mgr.createModificationWatchpointRequest(field);
      for (int i=0; i<excludes.length; ++i) {
        req.addClassExclusionFilter(excludes[i]);
      }
      req.setSuspendPolicy(EventRequest.SUSPEND_NONE);
      req.enable();
    }
  }

  private void exceptionEvent(ExceptionEvent event) {
//    com.sun.jdi.ObjectReference or = event.exception();
//    System.out.println("exceptionEvent() fired " + or);
//    System.out.println("catch location " + event.catchLocation());
    
    parent.exception(event);
    
    /*
    ObjectReference or = event.exception();
    ThreadReference thread = event.thread();
    ThreadTrace trace = (ThreadTrace)traceMap.get(thread);
    if (trace != null) {  // only want threads we care about
      trace.exceptionEvent(event);  // Forward event
    }
    try {
      List frames = thread.frames();
      for (Object item : frames) {
        System.out.println("got " + item);
      }
      //System.out.println(frames);
    } catch (IncompatibleThreadStateException e) {
      e.printStackTrace();
    }
    */
  }

  public void vmDeathEvent(VMDeathEvent event) {
    //System.err.println("vm is dead! dead!");
    vmDied = true;
    if (writer != null) {
      writer.println("-- The application exited --");
    }
  }

  public void vmDisconnectEvent(VMDisconnectEvent event) {
    connected = false;
    if (!vmDied) {
      if (writer != null) {
        writer.println("-- The application has been disconnected --");
      }
    }
  }
}
