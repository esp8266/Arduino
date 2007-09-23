/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-05 Ben Fry and Casey Reas
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
  
  $Id$
*/

package processing.app;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;


/**
 * Panel just below the editing area that contains status messages.
 */
public class EditorStatus extends JPanel implements ActionListener {
  static Color bgcolor[];
  static Color fgcolor[];

  static final int NOTICE = 0;
  static final int ERR    = 1;
  static final int PROMPT = 2;
  static final int EDIT   = 3;
  static final int SERIAL = 4;

  static final int YES    = 1;
  static final int NO     = 2;
  static final int CANCEL = 3;
  static final int OK     = 4;
  static final int SEND   = 5;

  static final String NO_MESSAGE = "";

  Editor editor;

  int mode;
  String message;

  Font font;
  FontMetrics metrics;
  int ascent;

  Image offscreen;
  int sizeW, sizeH;
  int imageW, imageH;

  JButton yesButton;
  JButton noButton;
  JButton cancelButton;
  JButton okButton;
  JButton sendButton;
  JTextField editField;
  JTextField serialField;
  JComboBox serialRates;

  //Thread promptThread;
  int response;


  public EditorStatus(Editor editor) {
    this.editor = editor;
    empty();

    if (bgcolor == null) {
      bgcolor = new Color[5];
	  // Arduino 0003 switched to a blue color scheme to visually distinguish
	  // itself from Processing.  Because the image files for certain interface
	  // elements (e.g. buttons and tabs) are distributed with the application
	  // while the preference file that specifies the IDE colors is stored in
	  // the user's home directory and shared across all versions of Arduino,
	  // we need to hardcode certain colors here to match the images.
	  // Otherwise, users who used different multiple versions of the software
	  // would sometimes see colors that didn't match the interface elements.
	  // This is a hack and prevents users from customizing the IDE colors,
	  // however, it obviates the need to provide for version-specific
	  // preferences.
      //bgcolor[0] = Preferences.getColor("status.notice.bgcolor");
      bgcolor[0] = new Color(0x54, 0x91, 0x9e);
      bgcolor[1] = Preferences.getColor("status.error.bgcolor");
      bgcolor[2] = Preferences.getColor("status.prompt.bgcolor");
      bgcolor[3] = Preferences.getColor("status.prompt.bgcolor");
      bgcolor[4] = new Color(0x54, 0x91, 0x9e);

      fgcolor = new Color[5];
      fgcolor[0] = Preferences.getColor("status.notice.fgcolor");
      fgcolor[1] = Preferences.getColor("status.error.fgcolor");
      fgcolor[2] = Preferences.getColor("status.prompt.fgcolor");
      fgcolor[3] = Preferences.getColor("status.prompt.fgcolor");
      fgcolor[4] = Preferences.getColor("status.notice.fgcolor");
    }
  }


  public void empty() {
    mode = NOTICE;
    message = NO_MESSAGE;
    //update();
    repaint();
  }


  public void notice(String message) {
    mode = NOTICE;
    this.message = message;
    //update();
    repaint();
  }

  public void unnotice(String unmessage) {
    if (message.equals(unmessage)) empty();
  }


  public void error(String message) {
    mode = ERR;
    this.message = message;
    repaint();
  }


  public void prompt(String message) {
    mode = PROMPT;
    this.message = message;

    response = 0;
    yesButton.setVisible(true);
    noButton.setVisible(true);
    cancelButton.setVisible(true);
    yesButton.requestFocus();

    repaint();
  }


  // prompt has been handled, re-hide the buttons
  public void unprompt() {
    yesButton.setVisible(false);
    noButton.setVisible(false);
    cancelButton.setVisible(false);
    empty();
  }


  public void edit(String message, String dflt) {
    mode = EDIT;
    this.message = message;

    response = 0;
    okButton.setVisible(true);
    cancelButton.setVisible(true);
    editField.setVisible(true);
    editField.setText(dflt);
    editField.selectAll();
    editField.requestFocus();

    repaint();
  }

  public void unedit() {
    okButton.setVisible(false);
    cancelButton.setVisible(false);
    editField.setVisible(false);
    empty();
  }
  
  public void serial()
  {
    mode = SERIAL;
    this.message = NO_MESSAGE;
    
    sendButton.setVisible(true);
    serialRates.setVisible(true);
    serialField.setVisible(true);
    serialField.setText("");
    serialField.requestFocus();

    repaint();
  }
  
  public void unserial()
  {
    sendButton.setVisible(false);
    serialField.setVisible(false);
    serialRates.setVisible(false);
    empty();
  }


  /*
  public void update() {
    Graphics g = this.getGraphics();
    try {
      setBackground(bgcolor[mode]);
    } catch (NullPointerException e) { } // if not ready yet
    if (g != null) paint(g);
  }

  public void update(Graphics g) {
    paint(g);
  }
  */


  public void paintComponent(Graphics screen) {
    //if (screen == null) return;
    if (yesButton == null) setup();

    //System.out.println("status.paintComponent");

    Dimension size = getSize();
    if ((size.width != sizeW) || (size.height != sizeH)) {
      // component has been resized

      if ((size.width > imageW) || (size.height > imageH)) {
        // nix the image and recreate, it's too small
        offscreen = null;

      } else {
        // who cares, just resize
        sizeW = size.width;
        sizeH = size.height;
        setButtonBounds();
      }
    }

    if (offscreen == null) {
      sizeW = size.width;
      sizeH = size.height;
      setButtonBounds();
      imageW = sizeW;
      imageH = sizeH;
      offscreen = createImage(imageW, imageH);
    }

    Graphics g = offscreen.getGraphics();
    if (font == null) {
      font = Preferences.getFont("status.font");
      //new Font("SansSerif", Font.PLAIN, 12));
      g.setFont(font);
      metrics = g.getFontMetrics();
      ascent = metrics.getAscent();
    }

    //setBackground(bgcolor[mode]);  // does nothing

    g.setColor(bgcolor[mode]);
    g.fillRect(0, 0, imageW, imageH);

    g.setColor(fgcolor[mode]);
    g.setFont(font); // needs to be set each time on osx
    g.drawString(message, Preferences.GUI_SMALL, (sizeH + ascent) / 2);

    screen.drawImage(offscreen, 0, 0, null);
  }


  protected void setup() {
    if (yesButton == null) {
      yesButton    = new JButton(Preferences.PROMPT_YES);
      noButton     = new JButton(Preferences.PROMPT_NO);
      cancelButton = new JButton(Preferences.PROMPT_CANCEL);
      okButton     = new JButton(Preferences.PROMPT_OK);
      sendButton   = new JButton(Preferences.PROMPT_SEND);

      // !@#(* aqua ui #($*(( that turtle-neck wearing #(** (#$@)(
      // os9 seems to work if bg of component is set, but x still a bastard
      if (Base.isMacOS()) {
        yesButton.setBackground(bgcolor[PROMPT]);
        noButton.setBackground(bgcolor[PROMPT]);
        cancelButton.setBackground(bgcolor[PROMPT]);
        okButton.setBackground(bgcolor[PROMPT]);
        sendButton.setBackground(bgcolor[SERIAL]);
      }
      setLayout(null);

      yesButton.addActionListener(this);
      noButton.addActionListener(this);
      cancelButton.addActionListener(this);
      okButton.addActionListener(this);
      sendButton.addActionListener(this);

      add(yesButton);
      add(noButton);
      add(cancelButton);
      add(okButton);
      add(sendButton);

      yesButton.setVisible(false);
      noButton.setVisible(false);
      cancelButton.setVisible(false);
      okButton.setVisible(false);
      sendButton.setVisible(false);

      editField = new JTextField();
      editField.addActionListener(this);

      //if (Base.platform != Base.MACOSX) {
      editField.addKeyListener(new KeyAdapter() {
          // no-op implemented because of a jikes bug
          //protected void noop() { }

          //public void keyPressed(KeyEvent event) {
          //System.out.println("pressed " + event + "  " + KeyEvent.VK_SPACE);
          //}

          // use keyTyped to catch when the feller is actually
          // added to the text field. with keyTyped, as opposed to
          // keyPressed, the keyCode will be zero, even if it's
          // enter or backspace or whatever, so the keychar should
          // be used instead. grr.
          public void keyTyped(KeyEvent event) {
            //System.out.println("got event " + event + "  " +
            // KeyEvent.VK_SPACE);
            int c = event.getKeyChar();
            
            if (mode == EDIT) {
              if (c == KeyEvent.VK_ENTER) {  // accept the input
                String answer = editField.getText();
                editor.sketch.nameCode(answer);
                unedit();
                event.consume();

                // easier to test the affirmative case than the negative
              } else if ((c == KeyEvent.VK_BACK_SPACE) ||
                         (c == KeyEvent.VK_DELETE) ||
                         (c == KeyEvent.VK_RIGHT) ||
                         (c == KeyEvent.VK_LEFT) ||
                         (c == KeyEvent.VK_UP) ||
                         (c == KeyEvent.VK_DOWN) ||
                         (c == KeyEvent.VK_HOME) ||
                         (c == KeyEvent.VK_END) ||
                         (c == KeyEvent.VK_SHIFT)) {
                //System.out.println("nothing to see here");
                //noop();

              } else if (c == KeyEvent.VK_ESCAPE) {
                unedit();
                editor.buttons.clear();
                event.consume();

              } else if (c == KeyEvent.VK_SPACE) {
                //System.out.println("got a space");
                // if a space, insert an underscore
                //editField.insert("_", editField.getCaretPosition());
                /* tried to play nice and see where it got me
                   editField.dispatchEvent(new KeyEvent(editField,
                   KeyEvent.KEY_PRESSED,
                   System.currentTimeMillis(),
                   0, 45, '_'));
                */
                //System.out.println("start/end = " +
                //                 editField.getSelectionStart() + " " +
                //                 editField.getSelectionEnd());
                String t = editField.getText();
                //int p = editField.getCaretPosition();
                //editField.setText(t.substring(0, p) + "_" + t.substring(p));
                //editField.setCaretPosition(p+1);
                int start = editField.getSelectionStart();
                int end = editField.getSelectionEnd();
                editField.setText(t.substring(0, start) + "_" +
                                  t.substring(end));
                editField.setCaretPosition(start+1);
                //System.out.println("consuming event");
                event.consume();

              } else if ((c == '_') || (c == '.') ||  // allow .pde and .java
                         ((c >= 'A') && (c <= 'Z')) ||
                         ((c >= 'a') && (c <= 'z'))) {
                // everything fine, catches upper and lower
                //noop();

              } else if ((c >= '0') && (c <= '9')) {
                // getCaretPosition == 0 means that it's the first char
                // and the field is empty.
                // getSelectionStart means that it *will be* the first
                // char, because the selection is about to be replaced
                // with whatever is typed.
                if ((editField.getCaretPosition() == 0) ||
                    (editField.getSelectionStart() == 0)) {
                  // number not allowed as first digit
                  //System.out.println("bad number bad");
                  event.consume();
                }
              } else {
                event.consume();
                //System.out.println("code is " + code + "  char = " + c);
              }
            } 
            //System.out.println("code is " + code + "  char = " + c);
          }
        });
      add(editField);
      editField.setVisible(false);
            
      serialField = new JTextField();
      serialField.addActionListener(this);

      serialField.addKeyListener(new KeyAdapter() {
        public void keyTyped(KeyEvent event) {
          int c = event.getKeyChar();
            
          if (c == KeyEvent.VK_ENTER) {  // accept the input
            editor.serialPort.write(serialField.getText());
            event.consume();
            serialField.setText("");
          } 
        }});
        
      add(serialField);
      serialField.setVisible(false);

      String[] serialRateStrings = {
        "300","1200","2400","4800","9600","14400",
        "19200","28800","38400","57600","115200"
      };
      
      serialRates = new JComboBox();

      if (Base.isMacOS())
        serialRates.setBackground(bgcolor[SERIAL]);
        
      for (int i = 0; i < serialRateStrings.length; i++)
        serialRates.addItem(serialRateStrings[i] + " baud");

      serialRates.setSelectedItem(
        Preferences.get("serial.debug_rate") + " baud");
      serialRates.addActionListener(this);      
      add(serialRates);
      serialRates.setVisible(false);
    }
  }


  protected void setButtonBounds() {
    int top = (sizeH - Preferences.BUTTON_HEIGHT) / 2;
    int eachButton = Preferences.GUI_SMALL + Preferences.BUTTON_WIDTH;

    int cancelLeft = sizeW      - eachButton;
    int noLeft     = cancelLeft - eachButton;
    int yesLeft    = noLeft     - eachButton;

    yesButton.setLocation(yesLeft, top);
    noButton.setLocation(noLeft, top);
    cancelButton.setLocation(cancelLeft, top);
    editField.setLocation(yesLeft - Preferences.BUTTON_WIDTH, top);
    serialField.setLocation(yesLeft - Preferences.BUTTON_WIDTH, top);
    okButton.setLocation(noLeft, top);
    serialRates.setLocation(0, top);
    sendButton.setLocation(cancelLeft, top);

    yesButton.setSize(      Preferences.BUTTON_WIDTH, Preferences.BUTTON_HEIGHT);
    noButton.setSize(       Preferences.BUTTON_WIDTH, Preferences.BUTTON_HEIGHT);
    cancelButton.setSize(   Preferences.BUTTON_WIDTH, Preferences.BUTTON_HEIGHT);
    okButton.setSize(       Preferences.BUTTON_WIDTH, Preferences.BUTTON_HEIGHT);
    sendButton.setSize(     Preferences.BUTTON_WIDTH, Preferences.BUTTON_HEIGHT);
    serialRates.setSize(  3*Preferences.BUTTON_WIDTH/2, Preferences.BUTTON_HEIGHT);
    editField.setSize(    2*Preferences.BUTTON_WIDTH, Preferences.BUTTON_HEIGHT);
    serialField.setSize(  3*Preferences.BUTTON_WIDTH, Preferences.BUTTON_HEIGHT);
  }


  public Dimension getPreferredSize() {
    return getMinimumSize();
  }

  public Dimension getMinimumSize() {
    return new Dimension(300, Preferences.GRID_SIZE);
  }

  public Dimension getMaximumSize() {
    return new Dimension(3000, Preferences.GRID_SIZE);
  }


  public void actionPerformed(ActionEvent e) {
    if (e.getSource() == noButton) {
      // shut everything down, clear status, and return
      unprompt();
      // don't need to save changes
      editor.checkModified2();

    } else if (e.getSource() == yesButton) {
      // answer was in response to "save changes?"
      unprompt();
      editor.handleSave(true);
      editor.checkModified2();

    } else if (e.getSource() == cancelButton) {
      // don't do anything, don't continue with checkModified2
      if (mode == PROMPT) unprompt();
      else if (mode == EDIT) unedit();
      editor.buttons.clear();

    } else if (e.getSource() == okButton) {
      // answering to "save as..." question
      String answer = editField.getText();
      //editor.handleSaveAs2(answer);
      editor.sketch.nameCode(answer);
      unedit();
    } else if (e.getSource() == sendButton) {
      editor.serialPort.write(serialField.getText());
      serialField.setText("");
    } else if (e.getSource() == serialRates) {
      String wholeString = (String) serialRates.getSelectedItem();
      String rateString = wholeString.substring(0, wholeString.indexOf(' '));
      int rate = Integer.parseInt(rateString);
      Preferences.set("serial.debug_rate", rateString);
      editor.serialPort.dispose();
      editor.serialPort = new Serial(true);
    }
  }
}
