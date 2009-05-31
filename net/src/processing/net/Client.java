/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Client - basic network client implementation
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-2007 Ben Fry and Casey Reas
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

package processing.net;
import processing.core.*;

import java.io.*;
import java.lang.reflect.*;
import java.net.*;


public class Client implements Runnable {

  PApplet parent;
  Method clientEventMethod;
  Method disconnectEventMethod;

  Thread thread;
  Socket socket;
  String ip;
  int port;
  String host;

  public InputStream input;
  public OutputStream output;

  byte buffer[] = new byte[32768];
  int bufferIndex;
  int bufferLast;


  public Client(PApplet parent, String host, int port) {
    this.parent = parent;
    this.host = host;
    this.port = port;

    try {
      socket = new Socket(this.host, this.port);
      input = socket.getInputStream();
      output = socket.getOutputStream();

      thread = new Thread(this);
      thread.start();

      parent.registerDispose(this);

      // reflection to check whether host applet has a call for
      // public void clientEvent(processing.net.Client)
      // which would be called each time an event comes in
      try {
        clientEventMethod =
          parent.getClass().getMethod("clientEvent",
                                      new Class[] { Client.class });
      } catch (Exception e) {
        // no such method, or an error.. which is fine, just ignore
      }
      // do the same for disconnectEvent(Client c);
      try {
        disconnectEventMethod =
          parent.getClass().getMethod("disconnectEvent",
                                      new Class[] { Client.class });
      } catch (Exception e) {
        // no such method, or an error.. which is fine, just ignore
      }

    } catch (ConnectException ce) {
      ce.printStackTrace();
      dispose();

    } catch (IOException e) {
      e.printStackTrace();
      dispose();
    }
  }


  public Client(PApplet parent, Socket socket) throws IOException {
    this.socket = socket;

    input = socket.getInputStream();
    output = socket.getOutputStream();

    thread = new Thread(this);
    thread.start();
  }


  /**
   * Disconnect from the server and calls disconnectEvent(Client c)
   * in the host PApplet.
   * <P/>
   * Use this to shut the connection if you're finished with it
   * while your applet is still running. Otherwise, it will be
   * automatically be shut down by the host PApplet
   * (using dispose, which is identical)
   */
  public void stop() {
    dispose();
    if (disconnectEventMethod != null) {
      try {
        disconnectEventMethod.invoke(parent, new Object[] { this });
      } catch (Exception e) {
        e.printStackTrace();
        disconnectEventMethod = null;
      }
    }
  }


  /**
   * Disconnect from the server: internal use only.
   * <P>
   * This should only be called by the internal functions in PApplet,
   * use stop() instead from within your own applets.
   */
  public void dispose() {
    thread = null;
    try {
      // do io streams need to be closed first?
      if (input != null) input.close();
      if (output != null) output.close();

    } catch (Exception e) {
      e.printStackTrace();
    }
    input = null;
    output = null;

    try {
      if (socket != null) socket.close();

    } catch (Exception e) {
      e.printStackTrace();
    }
    socket = null;
  }


  public void run() {
    while (Thread.currentThread() == thread) {
      try {
        while ((input != null) &&
               (input.available() > 0)) {  // this will block
          synchronized (buffer) {
            if (bufferLast == buffer.length) {
              byte temp[] = new byte[bufferLast << 1];
              System.arraycopy(buffer, 0, temp, 0, bufferLast);
              buffer = temp;
            }
            buffer[bufferLast++] = (byte) input.read();
          }
        }
        // now post an event
        if (clientEventMethod != null) {
          try {
            clientEventMethod.invoke(parent, new Object[] { this });
          } catch (Exception e) {
            System.err.println("error, disabling clientEvent() for " + host);
            e.printStackTrace();
            clientEventMethod = null;
          }
        }

        try {
          // uhh.. not sure what's best here.. since blocking,
          // do we need to worry about sleeping much? or is this
          // gonna try to slurp cpu away from the main applet?
          Thread.sleep(10);
        } catch (InterruptedException ex) { }

      } catch (IOException e) {
        //errorMessage("run", e);
        e.printStackTrace();
      }
    }
  }


  /**
   * Return true if this client is still active and hasn't run
   * into any trouble.
   */
  public boolean active() {
    return (thread != null);
  }


  /**
   * Returns the ip address of this feller as a String.
   */
  public String ip() {
    return socket.getInetAddress().getHostAddress();
  }


  /**
   * Returns the number of bytes that have been read from serial
   * and are waiting to be dealt with by the user.
   */
  public int available() {
    return (bufferLast - bufferIndex);
  }


  /**
   * Ignore all the bytes read so far and empty the buffer.
   */
  public void clear() {
    bufferLast = 0;
    bufferIndex = 0;
  }


  /**
   * Returns a number between 0 and 255 for the next byte that's
   * waiting in the buffer.
   * Returns -1 if there was no byte (although the user should
   * first check available() to see if things are ready to avoid this)
   */
  public int read() {
    if (bufferIndex == bufferLast) return -1;

    synchronized (buffer) {
      int outgoing = buffer[bufferIndex++] & 0xff;
      if (bufferIndex == bufferLast) {  // rewind
        bufferIndex = 0;
        bufferLast = 0;
      }
      return outgoing;
    }
  }


  /**
   * Returns the next byte in the buffer as a char.
   * Returns -1, or 0xffff, if nothing is there.
   */
  public char readChar() {
    if (bufferIndex == bufferLast) return (char)(-1);
    return (char) read();
  }


  /**
   * Return a byte array of anything that's in the serial buffer.
   * Not particularly memory/speed efficient, because it creates
   * a byte array on each read, but it's easier to use than
   * readBytes(byte b[]) (see below).
   */
  public byte[] readBytes() {
    if (bufferIndex == bufferLast) return null;

    synchronized (buffer) {
      int length = bufferLast - bufferIndex;
      byte outgoing[] = new byte[length];
      System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

      bufferIndex = 0;  // rewind
      bufferLast = 0;
      return outgoing;
    }
  }


  /**
   * Grab whatever is in the serial buffer, and stuff it into a
   * byte buffer passed in by the user. This is more memory/time
   * efficient than readBytes() returning a byte[] array.
   *
   * Returns an int for how many bytes were read. If more bytes
   * are available than can fit into the byte array, only those
   * that will fit are read.
   */
  public int readBytes(byte outgoing[]) {
    if (bufferIndex == bufferLast) return 0;

    synchronized (buffer) {
      int length = bufferLast - bufferIndex;
      if (length > outgoing.length) length = outgoing.length;
      System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

      bufferIndex += length;
      if (bufferIndex == bufferLast) {
        bufferIndex = 0;  // rewind
        bufferLast = 0;
      }
      return length;
    }
  }


  /**
   * Reads from the serial port into a buffer of bytes up to and
   * including a particular character. If the character isn't in
   * the serial buffer, then 'null' is returned.
   */
  public byte[] readBytesUntil(int interesting) {
    if (bufferIndex == bufferLast) return null;
    byte what = (byte)interesting;

    synchronized (buffer) {
      int found = -1;
      for (int k = bufferIndex; k < bufferLast; k++) {
        if (buffer[k] == what) {
          found = k;
          break;
        }
      }
      if (found == -1) return null;

      int length = found - bufferIndex + 1;
      byte outgoing[] = new byte[length];
      System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

      bufferIndex += length;
      if (bufferIndex == bufferLast) {
        bufferIndex = 0; // rewind
        bufferLast = 0;
      }
      return outgoing;
    }
  }


  /**
   * Reads from the serial port into a buffer of bytes until a
   * particular character. If the character isn't in the serial
   * buffer, then 'null' is returned.
   *
   * If outgoing[] is not big enough, then -1 is returned,
   *   and an error message is printed on the console.
   * If nothing is in the buffer, zero is returned.
   * If 'interesting' byte is not in the buffer, then 0 is returned.
   */
  public int readBytesUntil(int interesting, byte outgoing[]) {
    if (bufferIndex == bufferLast) return 0;
    byte what = (byte)interesting;

    synchronized (buffer) {
      int found = -1;
      for (int k = bufferIndex; k < bufferLast; k++) {
        if (buffer[k] == what) {
          found = k;
          break;
        }
      }
      if (found == -1) return 0;

      int length = found - bufferIndex + 1;
      if (length > outgoing.length) {
        System.err.println("readBytesUntil() byte buffer is" +
                           " too small for the " + length +
                           " bytes up to and including char " + interesting);
        return -1;
      }
      //byte outgoing[] = new byte[length];
      System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

      bufferIndex += length;
      if (bufferIndex == bufferLast) {
        bufferIndex = 0;  // rewind
        bufferLast = 0;
      }
      return length;
    }
  }


  /**
   * Return whatever has been read from the serial port so far
   * as a String. It assumes that the incoming characters are ASCII.
   *
   * If you want to move Unicode data, you can first convert the
   * String to a byte stream in the representation of your choice
   * (i.e. UTF8 or two-byte Unicode data), and send it as a byte array.
   */
  public String readString() {
    if (bufferIndex == bufferLast) return null;
    return new String(readBytes());
  }


  /**
   * Combination of readBytesUntil and readString. See caveats in
   * each function. Returns null if it still hasn't found what
   * you're looking for.
   * <p/>
   * If you want to move Unicode data, you can first convert the
   * String to a byte stream in the representation of your choice
   * (i.e. UTF8 or two-byte Unicode data), and send it as a byte array.
   */
  public String readStringUntil(int interesting) {
    byte b[] = readBytesUntil(interesting);
    if (b == null) return null;
    return new String(b);
  }


  /**
   * This will handle ints, bytes and chars transparently.
   */
  public void write(int what) {  // will also cover char
    try {
      output.write(what & 0xff);  // for good measure do the &
      output.flush();   // hmm, not sure if a good idea

    } catch (Exception e) { // null pointer or serial port dead
      //errorMessage("write", e);
      //e.printStackTrace();
      //dispose();
      //disconnect(e);
      e.printStackTrace();
      stop();
    }
  }


  public void write(byte bytes[]) {
    try {
      output.write(bytes);
      output.flush();   // hmm, not sure if a good idea

    } catch (Exception e) { // null pointer or serial port dead
      //errorMessage("write", e);
      //e.printStackTrace();
      //disconnect(e);
      e.printStackTrace();
      stop();
    }
  }


  /**
   * Write a String to the output. Note that this doesn't account
   * for Unicode (two bytes per char), nor will it send UTF8
   * characters.. It assumes that you mean to send a byte buffer
   * (most often the case for networking and serial i/o) and
   * will only use the bottom 8 bits of each char in the string.
   * (Meaning that internally it uses String.getBytes)
   *
   * If you want to move Unicode data, you can first convert the
   * String to a byte stream in the representation of your choice
   * (i.e. UTF8 or two-byte Unicode data), and send it as a byte array.
   */
  public void write(String what) {
    write(what.getBytes());
  }


  /**
   * Handle disconnect due to an Exception being thrown.
   */
  /*
    protected void disconnect(Exception e) {
    dispose();
    if (e != null) {
    e.printStackTrace();
    }
    }
  */


  /**
   * General error reporting, all corraled here just in case
   * I think of something slightly more intelligent to do.
   */
  //public void errorMessage(String where, Exception e) {
  //parent.die("Error inside Client." + where + "()", e);
  //e.printStackTrace(System.err);
  //}
}
