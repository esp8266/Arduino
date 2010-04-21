/*

 * @(#)StreamRedirectThread.java	1.4 03/01/23
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
package processing.app;

import java.io.*;

/**
 * StreamRedirectThread is a thread which copies it's input to
 * it's output and terminates when it completes.
 *
 * @version     @(#) StreamRedirectThread.java 1.4 03/01/23 23:33:38
 * @author Robert Field
 */
public class StreamRedirectThread extends Thread {

  private final Reader in;
  private final Writer out;

  private static final int BUFFER_SIZE = 2048;

  
  /**
   * Set up for copy.
   * @param name  Name of the thread
   * @param in    Stream to copy from
   * @param out   Stream to copy to
   */
  public StreamRedirectThread(String name, InputStream in, OutputStream out) {
    super(name);
    this.in = new InputStreamReader(in);
    this.out = new OutputStreamWriter(out);
    setPriority(Thread.MAX_PRIORITY-1);
  }
  
  
  public StreamRedirectThread(String name, Reader in, Writer out) {
    super(name);
    this.in = in;
    this.out = out;
    setPriority(Thread.MAX_PRIORITY-1);
  }
  

  /**
   * Copy.
   */
  public void run() {
    try {
      char[] cbuf = new char[BUFFER_SIZE];
      int count;
      //System.out.println("opening streamredirectthread");
      while ((count = in.read(cbuf, 0, BUFFER_SIZE)) >= 0) {
        out.write(cbuf, 0, count);
        // had to add the flush() here.. maybe shouldn't be using writer? [fry]
        out.flush();
      }
      //System.out.println("exiting streamredirectthread");
      out.flush();
    } catch(IOException exc) {
      System.err.println("Child I/O Transfer - " + exc);
    }
  }
}
