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

import static processing.app.I18n._;

import java.io.IOException;
import java.util.Arrays;
import java.util.List;

import jssc.SerialPort;
import jssc.SerialPortEvent;
import jssc.SerialPortEventListener;
import jssc.SerialPortException;


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

  public Serial() throws SerialException {
    this(PreferencesData.get("serial.port"),
            PreferencesData.getInteger("serial.debug_rate"),
            PreferencesData.get("serial.parity").charAt(0),
            PreferencesData.getInteger("serial.databits"),
            new Float(PreferencesData.get("serial.stopbits")).floatValue());
  }

  public Serial(int irate) throws SerialException {
    this(PreferencesData.get("serial.port"), irate,
            PreferencesData.get("serial.parity").charAt(0),
            PreferencesData.getInteger("serial.databits"),
            new Float(PreferencesData.get("serial.stopbits")).floatValue());
  }

  public Serial(String iname, int irate) throws SerialException {
    this(iname, irate, PreferencesData.get("serial.parity").charAt(0),
            PreferencesData.getInteger("serial.databits"),
            new Float(PreferencesData.get("serial.stopbits")).floatValue());
  }

  public Serial(String iname) throws SerialException {
    this(iname, PreferencesData.getInteger("serial.debug_rate"),
            PreferencesData.get("serial.parity").charAt(0),
            PreferencesData.getInteger("serial.databits"),
            new Float(PreferencesData.get("serial.stopbits")).floatValue());
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

  public synchronized void serialEvent(SerialPortEvent serialEvent) {
    if (serialEvent.isRXCHAR()) {
      try {
        byte[] buf = port.readBytes(serialEvent.getEventValue());
        if (buf.length > 0) {
          String msg = new String(buf);
          char[] chars = msg.toCharArray();
          message(chars, chars.length);
        }
      } catch (SerialPortException e) {
        errorMessage("serialEvent", e);
      }
    }
  }

  /**
   * This method is intented to be extended to receive messages
   * coming from serial port.
   *
   * @param chars
   * @param length
   */
  protected void message(char[] chars, int length) {
	// Empty
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


 
