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

package processing.app.tools;

import processing.app.*;
import processing.core.*;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;


/**
 * gui interface to font creation heaven/hell.
 */
public class CreateFont extends JFrame implements Tool {
  Editor editor;
  //Sketch sketch;

  Dimension windowSize;

  JList fontSelector;
  //JComboBox styleSelector;
  JTextField sizeSelector;
  JCheckBox allBox;
  JCheckBox smoothBox;
  JTextArea sample;
  JButton okButton;
  JTextField filenameField;

  Hashtable table;
  boolean smooth = true;
  boolean all = false;

  Font font;

  String[] list;
  int selection = -1;


  //static {
  //System.out.println("yep yep yep");
  //}
  //static final String styles[] = {
  //"Plain", "Bold", "Italic", "Bold Italic"
  //};


  public CreateFont() {
    super("Create Font");
  }
  
  
  public String getMenuTitle() {
    return "Create Font...";
  }
  
  
  public void init(Editor editor) {
    this.editor = editor;
    
    Container paine = getContentPane();
    paine.setLayout(new BorderLayout()); //10, 10));

    JPanel pain = new JPanel();
    pain.setBorder(new EmptyBorder(13, 13, 13, 13));
    paine.add(pain, BorderLayout.CENTER);

    pain.setLayout(new BoxLayout(pain, BoxLayout.Y_AXIS));

    String labelText =
      "Use this tool to create bitmap fonts for your program.\n" +
      "Select a font and size, and click 'OK' to generate the font.\n" +
      "It will be added to the data folder of the current sketch.";

    JTextArea textarea = new JTextArea(labelText);
    textarea.setBorder(new EmptyBorder(10, 10, 20, 10));
    textarea.setBackground(null);
    textarea.setEditable(false);
    textarea.setHighlighter(null);
    textarea.setFont(new Font("Dialog", Font.PLAIN, 12));
    pain.add(textarea);

    // don't care about families starting with . or #
    // also ignore dialog, dialoginput, monospaced, serif, sansserif

    // getFontList is deprecated in 1.4, so this has to be used
    GraphicsEnvironment ge =
      GraphicsEnvironment.getLocalGraphicsEnvironment();

    Font fonts[] = ge.getAllFonts();
    
    String flist[] = new String[fonts.length];
    table = new Hashtable();

    int index = 0;
    for (int i = 0; i < fonts.length; i++) {
      //String psname = fonts[i].getPSName();
      //if (psname == null) System.err.println("ps name is null");

      flist[index++] = fonts[i].getPSName();
      table.put(fonts[i].getPSName(), fonts[i]);
    }

    list = new String[index];
    System.arraycopy(flist, 0, list, 0, index);

    fontSelector = new JList(list);
    fontSelector.addListSelectionListener(new ListSelectionListener() {
        public void valueChanged(ListSelectionEvent e) {
          if (e.getValueIsAdjusting() == false) {
            selection = fontSelector.getSelectedIndex();
            okButton.setEnabled(true);
            update();
          }
        }
      });

    fontSelector.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    fontSelector.setVisibleRowCount(12);
    JScrollPane fontScroller = new JScrollPane(fontSelector);
    pain.add(fontScroller);

    Dimension d1 = new Dimension(13, 13);
    pain.add(new Box.Filler(d1, d1, d1));

    // see http://rinkworks.com/words/pangrams.shtml
    sample = new JTextArea("The quick brown fox blah blah.") {
        // Forsaking monastic tradition, twelve jovial friars gave up their
        // vocation for a questionable existence on the flying trapeze.
        public void paintComponent(Graphics g) {
          //System.out.println("disabling aa");
          Graphics2D g2 = (Graphics2D) g;
          g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
                              smooth ?
                              RenderingHints.VALUE_TEXT_ANTIALIAS_ON :
                              RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);
          super.paintComponent(g2);
        }
      };
    // Seems that in some instances, no default font is set
    // http://dev.processing.org/bugs/show_bug.cgi?id=777
    sample.setFont(new Font("Dialog", Font.PLAIN, 12));

    pain.add(sample);

    Dimension d2 = new Dimension(6, 6);
    pain.add(new Box.Filler(d2, d2, d2));

    JPanel panel = new JPanel();
    panel.add(new JLabel("Size:"));
    sizeSelector = new JTextField(" 48 ");
    sizeSelector.getDocument().addDocumentListener(new DocumentListener() {
        public void insertUpdate(DocumentEvent e) { update(); }
        public void removeUpdate(DocumentEvent e) { update(); }
        public void changedUpdate(DocumentEvent e) { }
      });
    panel.add(sizeSelector);

    smoothBox = new JCheckBox("Smooth");
    smoothBox.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          smooth = smoothBox.isSelected();
          update();
        }
      });
    smoothBox.setSelected(smooth);
    panel.add(smoothBox);

    allBox = new JCheckBox("All Characters");
    allBox.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          all = allBox.isSelected();
        }
      });
    allBox.setSelected(all);
    panel.add(allBox);

    pain.add(panel);

    JPanel filestuff = new JPanel();
    filestuff.add(new JLabel("Filename:"));
    filestuff.add(filenameField = new JTextField(20));
    filestuff.add(new JLabel(".vlw"));
    pain.add(filestuff);

    JPanel buttons = new JPanel();
    JButton cancelButton = new JButton("Cancel");
    cancelButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          setVisible(false);
        }
      });
    okButton = new JButton("OK");
    okButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          build();
        }
      });
    okButton.setEnabled(false);

    buttons.add(cancelButton);
    buttons.add(okButton);
    pain.add(buttons);

    JRootPane root = getRootPane();
    root.setDefaultButton(okButton);
    ActionListener disposer = new ActionListener() {
        public void actionPerformed(ActionEvent actionEvent) {
          setVisible(false);
        }
      };
    Base.registerWindowCloseKeys(root, disposer);
    Base.setIcon(this);

    pack();

    // do this after pack so it doesn't affect layout
    sample.setFont(new Font(list[0], Font.PLAIN, 48));

    fontSelector.setSelectedIndex(0);

    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    windowSize = getSize();

    setLocation((screen.width - windowSize.width) / 2,
                (screen.height - windowSize.height) / 2);
  }


  public void run() {
    setVisible(true);
  }


  /**
   * make the window vertically resizable
   */
  public Dimension getMaximumSize() {
    return new Dimension(windowSize.width, 2000);
  }

  public Dimension getMinimumSize() {
    return windowSize;
  }


  /*
  public void show(File targetFolder) {
    this.targetFolder = targetFolder;
    show();
  }
  */


  public void update() {
    int fontsize = 0;
    try {
      fontsize = Integer.parseInt(sizeSelector.getText().trim());
      //System.out.println("'" + sizeSelector.getText() + "'");
    } catch (NumberFormatException e2) { }

    // if a deselect occurred, selection will be -1
    if ((fontsize > 0) && (fontsize < 256) && (selection != -1)) {
      //font = new Font(list[selection], Font.PLAIN, fontsize);
      Font instance = (Font) table.get(list[selection]);
      font = instance.deriveFont(Font.PLAIN, fontsize);
      //System.out.println("setting font to " + font);
      sample.setFont(font);

      String filenameSuggestion = list[selection].replace(' ', '_');
      filenameSuggestion += "-" + fontsize;
      filenameField.setText(filenameSuggestion);
    }
  }


  public void build() {
    int fontsize = 0;
    try {
      fontsize = Integer.parseInt(sizeSelector.getText().trim());
    } catch (NumberFormatException e) { }

    if (fontsize <= 0) {
      JOptionPane.showMessageDialog(this, "Bad font size, try again.",
                                    "Badness", JOptionPane.WARNING_MESSAGE);
      return;
    }

    String filename = filenameField.getText();
    if (filename.length() == 0) {
      JOptionPane.showMessageDialog(this, "Enter a file name for the font.",
                                    "Lameness", JOptionPane.WARNING_MESSAGE);
      return;
    }
    if (!filename.endsWith(".vlw")) {
      filename += ".vlw";
    }

    try {
      Font instance = (Font) table.get(list[selection]);
      font = instance.deriveFont(Font.PLAIN, fontsize);
      PFont f = new PFont(font, smooth, all ? null : PFont.DEFAULT_CHARSET);

      // make sure the 'data' folder exists
      File folder = editor.getSketch().prepareDataFolder();
      f.save(new FileOutputStream(new File(folder, filename)));

    } catch (IOException e) {
      JOptionPane.showMessageDialog(this,
                                    "An error occurred while creating font.",
                                    "No font for you",
                                    JOptionPane.WARNING_MESSAGE);
      e.printStackTrace();
    }

    setVisible(false);
  }
}
