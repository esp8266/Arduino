/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  PSerial - class for serial port goodness
  Part of the Processing project - http://processing.org

  Copyright (c) 2004 Ben Fry & Casey Reas

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

package processing.app;

import jssc.*;
import processing.app.debug.MessageConsumer;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import static processing.app.I18n._;


public class Serial implements SerialPortEventListener {

  //PApplet parent;

  // properties can be passed in for default values
  // otherwise defaults to 9600 N81

  // these could be made static, which might be a solution
  // for the classloading problem.. because if code ran again,
  // the static class would have an object that could be closed

  SerialPort port;

  int rate;
  int parity;
  int databits;
  int stopbits;
  boolean monitor = false;

  byte buffer[] = new byte[32768];
  int bufferIndex;
  int bufferLast;

  MessageConsumer consumer;

  public Serial(boolean monitor) throws SerialException {
    this(Preferences.get("serial.port"),
            Preferences.getInteger("serial.debug_rate"),
            Preferences.get("serial.parity").charAt(0),
            Preferences.getInteger("serial.databits"),
            new Float(Preferences.get("serial.stopbits")).floatValue());
    this.monitor = monitor;
  }

  public Serial() throws SerialException {
    this(Preferences.get("serial.port"),
            Preferences.getInteger("serial.debug_rate"),
            Preferences.get("serial.parity").charAt(0),
            Preferences.getInteger("serial.databits"),
            new Float(Preferences.get("serial.stopbits")).floatValue());
  }

  public Serial(int irate) throws SerialException {
    this(Preferences.get("serial.port"), irate,
            Preferences.get("serial.parity").charAt(0),
            Preferences.getInteger("serial.databits"),
            new Float(Preferences.get("serial.stopbits")).floatValue());
  }

  public Serial(String iname, int irate) throws SerialException {
    this(iname, irate, Preferences.get("serial.parity").charAt(0),
            Preferences.getInteger("serial.databits"),
            new Float(Preferences.get("serial.stopbits")).floatValue());
  }

  public Serial(String iname) throws SerialException {
    this(iname, Preferences.getInteger("serial.debug_rate"),
            Preferences.get("serial.parity").charAt(0),
            Preferences.getInteger("serial.databits"),
            new Float(Preferences.get("serial.stopbits")).floatValue());
  }

  public static boolean touchPort(String iname, int irate) throws SerialException {
    SerialPort serialPort = new SerialPort(iname);
    try {
      serialPort.openPort();
      serialPort.setParams(irate, 8, SerialPort.STOPBITS_1, SerialPort.PARITY_NONE);
      serialPort.closePort();
      return true;
    } catch (SerialPortException e) {
      throw new SerialException(I18n.format(_("Error touching serial port ''{0}''."), iname), e);
    } finally {
      if (serialPort.isOpened()) {
        try {
          serialPort.closePort();
        } catch (SerialPortException e) {
          // noop
        }
      }
    }
  }

  public Serial(String iname, int irate, char iparity, int idatabits, float istopbits) throws SerialException {
    //if (port != null) port.close();
    //this.parent = parent;
    //parent.attach(this);

    this.rate = irate;

    parity = SerialPort.PARITY_NONE;
    if (iparity == 'E') parity = SerialPort.PARITY_EVEN;
    if (iparity == 'O') parity = SerialPort.PARITY_ODD;

    this.databits = idatabits;

    stopbits = SerialPort.STOPBITS_1;
    if (istopbits == 1.5f) stopbits = SerialPort.STOPBITS_1_5;
    if (istopbits == 2) stopbits = SerialPort.STOPBITS_2;

    try {
      port = new SerialPort(iname);
      port.openPort();
      port.setParams(rate, databits, stopbits, parity, true, true);
      port.addEventListener(this);
    } catch (Exception e) {
      throw new SerialException(I18n.format(_("Error opening serial port ''{0}''."), iname), e);
    }

    if (port == null) {
      throw new SerialNotFoundException(I18n.format(_("Serial port ''{0}'' not found. Did you select the right one from the Tools > Serial Port menu?"), iname));
    }
  }

  public void setup() {
    //parent.registerCall(this, DISPOSE);
  }

  public void dispose() throws IOException {
    if (port != null) {
      try {
        if (port.isOpened()) {
          port.closePort();  // close the port
        }
      } catch (SerialPortException e) {
        throw new IOException(e);
      } finally {
        port = null;
      }
    }
  }

  public void addListener(MessageConsumer consumer) {
    this.consumer = consumer;
  }

  public synchronized void serialEvent(SerialPortEvent serialEvent) {
    if (serialEvent.isRXCHAR()) {
      try {
        byte[] buf = port.readBytes();
        if (buf.length > 0) {
          if (bufferLast == buffer.length) {
            byte temp[] = new byte[bufferLast << 1];
            System.arraycopy(buffer, 0, temp, 0, bufferLast);
            buffer = temp;
          }
          if (monitor) {
            System.out.print(new String(buf));
          }
          if (this.consumer != null) {
            this.consumer.message(new String(buf));
          }
        }
      } catch (SerialPortException e) {
        errorMessage("serialEvent", e);
      }
    }
  }


  /**
   * Returns the number of bytes that have been read from serial
   * and are waiting to be dealt with by the user.
   */
  public synchronized int available() {
    return (bufferLast - bufferIndex);
  }


  /**
   * Ignore all the bytes read so far and empty the buffer.
   */
  public synchronized void clear() {
    bufferLast = 0;
    bufferIndex = 0;
  }


  /**
   * Returns a number between 0 and 255 for the next byte that's
   * waiting in the buffer.
   * Returns -1 if there was no byte (although the user should
   * first check available() to see if things are ready to avoid this)
   */
  public synchronized int read() {
    if (bufferIndex == bufferLast) return -1;

    int outgoing = buffer[bufferIndex++] & 0xff;
    if (bufferIndex == bufferLast) {  // rewind
      bufferIndex = 0;
      bufferLast = 0;
    }
    return outgoing;
  }


  /**
   * Returns the next byte in the buffer as a char.
   * Returns -1, or 0xffff, if nothing is there.
   */
  public synchronized char readChar() {
    if (bufferIndex == bufferLast) return (char) (-1);
    return (char) read();
  }


  /**
   * Return a byte array of anything that's in the serial buffer.
   * Not particularly memory/speed efficient, because it creates
   * a byte array on each read, but it's easier to use than
   * readBytes(byte b[]) (see below).
   */
  public synchronized byte[] readBytes() {
    if (bufferIndex == bufferLast) return null;

    int length = bufferLast - bufferIndex;
    byte outgoing[] = new byte[length];
    System.arraycopy(buffer, bufferIndex, outgoing, 0, length);

    bufferIndex = 0;  // rewind
    bufferLast = 0;
    return outgoing;
  }


  /**
   * Grab whatever is in the serial buffer, and stuff it into a
   * byte buffer passed in by the user. This is more memory/time
   * efficient than readBytes() returning a byte[] array.
   * <p/>
   * Returns an int for how many bytes were read. If more bytes
   * are available than can fit into the byte array, only those
   * that will fit are read.
   */
  public synchronized int readBytes(byte outgoing[]) {
    if (bufferIndex == bufferLast) return 0;

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


  /**
   * Reads from the serial port into a buffer of bytes up to and
   * including a particular character. If the character isn't in
   * the serial buffer, then 'null' is returned.
   */
  public synchronized byte[] readBytesUntil(int interesting) {
    if (bufferIndex == bufferLast) return null;
    byte what = (byte) interesting;

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

    bufferIndex = 0;  // rewind
    bufferLast = 0;
    return outgoing;
  }


  /**
   * Reads from the serial port into a buffer of bytes until a
   * particular character. If the character isn't in the serial
   * buffer, then 'null' is returned.
   * <p/>
   * If outgoing[] is not big enough, then -1 is returned,
   * and an error message is printed on the console.
   * If nothing is in the buffer, zero is returned.
   * If 'interesting' byte is not in the buffer, then 0 is returned.
   */
  public synchronized int readBytesUntil(int interesting, byte outgoing[]) {
    if (bufferIndex == bufferLast) return 0;
    byte what = (byte) interesting;

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
      System.err.println(
              I18n.format(
                      _("readBytesUntil() byte buffer is too small for the {0}" +
                              " bytes up to and including char {1}"),
                      length,
                      interesting
              )
      );
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


  /**
   * Return whatever has been read from the serial port so far
   * as a String. It assumes that the incoming characters are ASCII.
   * <p/>
   * If you want to move Unicode data, you can first convert the
   * String to a byte stream in the representation of your choice
   * (i.e. UTF8 or two-byte Unicode data), and send it as a byte array.
   */
  public synchronized String readString() {
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
  public synchronized String readStringUntil(int interesting) {
    byte b[] = readBytesUntil(interesting);
    if (b == null) return null;
    return new String(b);
  }


  /**
   * This will handle both ints, bytes and chars transparently.
   */
  public void write(int what) {  // will also cover char
    try {
      port.writeInt(what & 0xff);
    } catch (SerialPortException e) {
      errorMessage("write", e);
    }
  }


  public void write(byte bytes[]) {
    try {
      port.writeBytes(bytes);
    } catch (SerialPortException e) {
      errorMessage("write", e);
    }
  }


  /**
   * Write a String to the output. Note that this doesn't account
   * for Unicode (two bytes per char), nor will it send UTF8
   * characters.. It assumes that you mean to send a byte buffer
   * (most often the case for networking and serial i/o) and
   * will only use the bottom 8 bits of each char in the string.
   * (Meaning that internally it uses String.getBytes)
   * <p/>
   * If you want to move Unicode data, you can first convert the
   * String to a byte stream in the representation of your choice
   * (i.e. UTF8 or two-byte Unicode data), and send it as a byte array.
   */
  public void write(String what) {
    write(what.getBytes());
  }

  public void setDTR(boolean state) {
    try {
      port.setDTR(state);
    } catch (SerialPortException e) {
      errorMessage("setDTR", e);
    }
  }

  public void setRTS(boolean state) {
    try {
      port.setRTS(state);
    } catch (SerialPortException e) {
      errorMessage("setRTS", e);
    }
  }

  static public List<String> list() {
    return Arrays.asList(SerialPortList.getPortNames());
  }


  /**
   * General error reporting, all corraled here just in case
   * I think of something slightly more intelligent to do.
   */
  static public void errorMessage(String where, Throwable e) {
    System.err.println(I18n.format(_("Error inside Serial.{0}()"), where));
    e.printStackTrace();
  }
}


  /*
  class SerialMenuListener implements ItemListener {
    //public SerialMenuListener() { }

    public void itemStateChanged(ItemEvent e) {
      int count = serialMenu.getItemCount();
      for (int i = 0; i < count; i++) {
        ((CheckboxMenuItem)serialMenu.getItem(i)).setState(false);
      }
      CheckboxMenuItem item = (CheckboxMenuItem)e.getSource();
      item.setState(true);
      String name = item.getLabel();
      //System.out.println(item.getLabel());
      PdeBase.properties.put("serial.port", name);
      //System.out.println("set to " + get("serial.port"));
    }
  }
  */


  /*
  protected Vector buildPortList() {
    // get list of names for serial ports
    // have the default port checked (if present)
    Vector list = new Vector();

    //SerialMenuListener listener = new SerialMenuListener();
    boolean problem = false;

    // if this is failing, it may be because
    // lib/javax.comm.properties is missing.
    // java is weird about how it searches for java.comm.properties
    // so it tends to be very fragile. i.e. quotes in the CLASSPATH
    // environment variable will hose things.
    try {
      //System.out.println("building port list");
      Enumeration portList = CommPortIdentifier.getPortIdentifiers();
      while (portList.hasMoreElements()) {
        CommPortIdentifier portId = 
          (CommPortIdentifier) portList.nextElement();
        //System.out.println(portId);

        if (portId.getPortType() == CommPortIdentifier.PORT_SERIAL) {
          //if (portId.getName().equals(port)) {
          String name = portId.getName();
          //CheckboxMenuItem mi = 
          //new CheckboxMenuItem(name, name.equals(defaultName));

          //mi.addItemListener(listener);
          //serialMenu.add(mi);
          list.addElement(name);
        }
      }
    } catch (UnsatisfiedLinkError e) {
      e.printStackTrace();
      problem = true;

    } catch (Exception e) {
      System.out.println("exception building serial menu");
      e.printStackTrace();
    }

    //if (serialMenu.getItemCount() == 0) {
      //System.out.println("dimming serial menu");
    //serialMenu.setEnabled(false);
    //}

    // only warn them if this is the first time
    if (problem && PdeBase.firstTime) {
      JOptionPane.showMessageDialog(this, //frame,
                                    "Serial port support not installed.\n" +
                                    "Check the readme for instructions\n" +
                                    "if you need to use the serial port.    ",
                                    "Serial Port Warning",
                                    JOptionPane.WARNING_MESSAGE);
    }
    return list;
  }
  */


 
