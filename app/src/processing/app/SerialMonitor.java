/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
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

package processing.app;

import processing.app.debug.MessageConsumer;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;

public class SerialMonitor extends JFrame implements MessageConsumer {
  private Serial serial;
  private String port;
  private JTextArea textArea;
  private JTextField textField;
  private JButton sendButton;
  private JComboBox serialRates;

  public SerialMonitor() {
    addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
          closeSerialPort();
        }
      });  
  
    getContentPane().setLayout(new BorderLayout());
    
    Font font = Theme.getFont("console.font");

    textArea = new JTextArea(16, 40);
    textArea.setEditable(false);    
    textArea.setFont(font);
    
    getContentPane().add(new JScrollPane(textArea), BorderLayout.CENTER);
    
    JPanel pane = new JPanel(new BorderLayout(4, 0));
    pane.setBorder(new EmptyBorder(4, 4, 4, 4));

    textField = new JTextField(40);
    textField.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        send(textField.getText());
        textField.setText("");
      }});

    sendButton = new JButton("Send");
    sendButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        send(textField.getText());
        textField.setText("");
      }});
    
    pane.add(textField, BorderLayout.CENTER);
    pane.add(sendButton, BorderLayout.EAST);
    
    getContentPane().add(pane, BorderLayout.NORTH);
    
    pane = new JPanel(new FlowLayout(FlowLayout.TRAILING, 4, 4));
  
    String[] serialRateStrings = {
      "300","1200","2400","4800","9600","14400",
      "19200","28800","38400","57600","115200"
    };
    
    serialRates = new JComboBox();
    for (int i = 0; i < serialRateStrings.length; i++)
      serialRates.addItem(serialRateStrings[i] + " baud");

    serialRates.setSelectedItem(Preferences.get("serial.debug_rate") + " baud");
    serialRates.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        String wholeString = (String) serialRates.getSelectedItem();
        String rateString = wholeString.substring(0, wholeString.indexOf(' '));
        int rate = Integer.parseInt(rateString);
        Preferences.set("serial.debug_rate", rateString);
        closeSerialPort();
        Editor.activeEditor.handleSerial(true);
      }});
    
    pane.add(serialRates);
    
    getContentPane().add(pane, BorderLayout.SOUTH);

    pack();
  }
  
  private void send(String s) {
    if (serial != null)
      serial.write(s);
  }
  
  public void openSerialPort(String port) throws SerialException {
    if (port.equals(this.port))
      return;
    
    closeSerialPort();
    
    serial = new Serial(port);
    serial.addListener(this);
    setTitle(port);
    this.port = port;
  }
  
  public void closeSerialPort() {
    if (serial != null) {
      setTitle("closing...");
      textArea.setText("");
      serial.dispose();
      serial = null;
      port = null;
    }
  }
  
  public void message(final String s) {
    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        textArea.append(s);
      }});
  }
}