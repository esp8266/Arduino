/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-06 Ben Fry and Casey Reas
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

import java.io.*;


/**
 * Slurps up messages from compiler.
 */
public class MessageSiphon implements Runnable {
  BufferedReader streamReader;
  Thread thread;
  MessageConsumer consumer;


  public MessageSiphon(InputStream stream, MessageConsumer consumer) {
    this.streamReader = new BufferedReader(new InputStreamReader(stream));
    this.consumer = consumer;

    thread = new Thread(this);
    // don't set priority too low, otherwise exceptions won't
    // bubble up in time (i.e. compile errors have a weird delay)
    //thread.setPriority(Thread.MIN_PRIORITY);
    thread.setPriority(Thread.MAX_PRIORITY-1);
    thread.start();
  }


  public void run() {
    try {
      // process data until we hit EOF; this will happily block
      // (effectively sleeping the thread) until new data comes in.
      // when the program is finally done, null will come through.
      //
      String currentLine;
      while ((currentLine = streamReader.readLine()) != null) {
        // \n is added again because readLine() strips it out
        //EditorConsole.systemOut.println("messaging in");
        consumer.message(currentLine + "\n");
        //EditorConsole.systemOut.println("messaging out");
      }
      //EditorConsole.systemOut.println("messaging thread done");
      thread = null;

    } catch (NullPointerException npe) {
      // Fairly common exception during shutdown
      thread = null;

    } catch (Exception e) {
      // On Linux and sometimes on Mac OS X, a "bad file descriptor"
      // message comes up when closing an applet that's run externally.
      // That message just gets supressed here..
      String mess = e.getMessage();
      if ((mess != null) &&
          (mess.indexOf("Bad file descriptor") != -1)) {
        //if (e.getMessage().indexOf("Bad file descriptor") == -1) {
        //System.err.println("MessageSiphon err " + e);
        //e.printStackTrace();
      } else {
        e.printStackTrace();
      }
      thread = null;
    }
  }
  
  // Wait until the MessageSiphon thread is complete.
  public void join() throws java.lang.InterruptedException {
    // Grab a temp copy in case another thread nulls the "thread" 
    // member variable
    Thread t = thread;
    if (t != null) t.join();
  }

  public Thread getThread() {
    return thread;
  }
}
