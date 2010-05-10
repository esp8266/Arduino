/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-10 Ben Fry and Casey Reas
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
 * GUI tool for font creation heaven/hell.
 */
public class CreateFont extends JFrame implements Tool {
  Editor editor;
  //Sketch sketch;

  Dimension windowSize;

  JList fontSelector;
  JTextField sizeSelector;
  JButton charsetButton;
  JCheckBox smoothBox;
  JComponent sample;
  JButton okButton;
  JTextField filenameField;

  HashMap<String,Font> table;
  boolean smooth = true;

  Font font;

  String[] list;
  int selection = -1;

  CharacterSelector charSelector;


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
    table = new HashMap<String,Font>();

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

    sample = new SampleComponent(this);
    
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

//    allBox = new JCheckBox("All Characters");
//    allBox.addActionListener(new ActionListener() {
//        public void actionPerformed(ActionEvent e) {
//          all = allBox.isSelected();
//        }
//      });
//    allBox.setSelected(all);
//    panel.add(allBox);
    charsetButton = new JButton("Characters...");
    charsetButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
        //showCharacterList();
        charSelector.setVisible(true);
        }
      });
    panel.add(charsetButton);

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

    setResizable(false);
    pack();

    // do this after pack so it doesn't affect layout
    sample.setFont(new Font(list[0], Font.PLAIN, 48));

    fontSelector.setSelectedIndex(0);

    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    windowSize = getSize();

    setLocation((screen.width - windowSize.width) / 2,
                (screen.height - windowSize.height) / 2);
    
    // create this behind the scenes
    charSelector = new CharacterSelector(); 
  }


  public void run() {
    setVisible(true);
  }


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

    String filename = filenameField.getText().trim();
    if (filename.length() == 0) {
      JOptionPane.showMessageDialog(this, "Enter a file name for the font.",
                                    "Lameness", JOptionPane.WARNING_MESSAGE);
      return;
    }
    if (!filename.endsWith(".vlw")) {
      filename += ".vlw";
    }

    // Please implement me properly. The schematic is below, but not debugged.
    // http://dev.processing.org/bugs/show_bug.cgi?id=1464
    
//    final String filename2 = filename;
//    final int fontsize2 = fontsize;
//    SwingUtilities.invokeLater(new Runnable() {
//      public void run() {
    try {
      Font instance = (Font) table.get(list[selection]);
      font = instance.deriveFont(Font.PLAIN, fontsize);
      //PFont f = new PFont(font, smooth, all ? null : PFont.CHARSET);
      PFont f = new PFont(font, smooth, charSelector.getCharacters());

//      PFont f = new PFont(font, smooth, null);
//      char[] charset = charSelector.getCharacters();
//      ProgressMonitor progressMonitor = new ProgressMonitor(CreateFont.this,
//                                            "Creating font", "", 0, charset.length);
//      progressMonitor.setProgress(0);
//      for (int i = 0; i < charset.length; i++) {
//        System.out.println(charset[i]);
//        f.index(charset[i]);  // load this char
//        progressMonitor.setProgress(i+1);
//      }

      // make sure the 'data' folder exists
      File folder = editor.getSketch().prepareDataFolder();
      f.save(new FileOutputStream(new File(folder, filename)));

    } catch (IOException e) {
      JOptionPane.showMessageDialog(CreateFont.this,
                                    "An error occurred while creating font.",
                                    "No font for you",
                                    JOptionPane.WARNING_MESSAGE);
      e.printStackTrace();
    }
//      }
//    });

    setVisible(false);
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
}


/**
 * Component that draws the sample text. This is its own subclassed component
 * because Mac OS X controls seem to reset the RenderingHints for smoothing
 * so that they cannot be overridden properly for JLabel or JTextArea. 
 * @author fry
 */
class SampleComponent extends JComponent {
  // see http://rinkworks.com/words/pangrams.shtml
  String text = 
    "Forsaking monastic tradition, twelve jovial friars gave up their " +
    "vocation for a questionable existence on the flying trapeze.";
  int high = 80;
  
  CreateFont parent;
  
  public SampleComponent(CreateFont p) {
    this.parent = p;
    
    // and yet, we still need an inner class to handle the basics. 
    // or no, maybe i'll refactor this as a separate class!
    // maybe a few getters and setters? mmm?
    addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        String input = 
          (String) JOptionPane.showInputDialog(parent,
                                               "Enter new sample text:",
                                               "Sample Text",
                                               JOptionPane.PLAIN_MESSAGE,
                                               null, // icon
                                               null, // choices
                                               text);
        if (input != null) {
          text = input;
          parent.repaint();
        }
      }
    });
  }
  
  public void paintComponent(Graphics g) {
//    System.out.println("smoothing set to " + smooth);
    Graphics2D g2 = (Graphics2D) g;
    g2.setColor(Color.WHITE);
    Dimension dim = getSize();
    g2.fillRect(0, 0, dim.width, dim.height);
    g2.setColor(Color.BLACK);
    
    g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
                        parent.smooth ?
                        RenderingHints.VALUE_TEXT_ANTIALIAS_ON :
                        RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);
    // add this one as well (after 1.0.9)
    g2.setRenderingHint(RenderingHints.KEY_ANTIALIASING,
                        parent.smooth ?
                        RenderingHints.VALUE_ANTIALIAS_ON :
                        RenderingHints.VALUE_ANTIALIAS_OFF);
    //super.paintComponent(g2);
    Font font = getFont();
    int ascent = g2.getFontMetrics().getAscent();
//    System.out.println(f.getName());
    g2.setFont(font);
    g2.drawString(text, 5, dim.height - (dim.height - ascent) / 2);
  }
  
  public Dimension getPreferredSize() {
    return new Dimension(400, high);
  }
  
  public Dimension getMaximumSize() {
    return new Dimension(10000, high);
  }
  
  public Dimension getMinimumSize() {
    return new Dimension(100, high);
  }
}


/**
 * Frame for selecting which characters will be included with the font.
 */
class CharacterSelector extends JFrame {
  JRadioButton defaultCharsButton;
  JRadioButton allCharsButton;
  JRadioButton unicodeCharsButton;
  JScrollPane unicodeBlockScroller;
  JList charsetList;


  public CharacterSelector() {
    super("Character Selector");

    charsetList = new CheckBoxList();
    DefaultListModel model = new DefaultListModel();
    charsetList.setModel(model);
    for (String item : blockNames) {
      model.addElement(new JCheckBox(item));
    }

    unicodeBlockScroller = 
      new JScrollPane(charsetList, 
                      ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS,
                      ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
    
    Container outer = getContentPane();
    outer.setLayout(new BorderLayout());

    JPanel pain = new JPanel();
    pain.setBorder(new EmptyBorder(13, 13, 13, 13));
    outer.add(pain, BorderLayout.CENTER);

    pain.setLayout(new BoxLayout(pain, BoxLayout.Y_AXIS));

    String labelText =
      "Default characters will include most bitmaps for Mac OS\n" +
      "and Windows Latin scripts. Including all characters may\n" +
      "require large amounts of memory for all of the bitmaps.\n" +
      "For greater control, you can select specific Unicode blocks.";
    JTextArea textarea = new JTextArea(labelText);
    textarea.setBorder(new EmptyBorder(13, 8, 13, 8));
    textarea.setBackground(null);
    textarea.setEditable(false);
    textarea.setHighlighter(null);
    textarea.setFont(new Font("Dialog", Font.PLAIN, 12));
    pain.add(textarea);

    ActionListener listener = new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        //System.out.println("action " + unicodeCharsButton.isSelected());
        //unicodeBlockScroller.setEnabled(unicodeCharsButton.isSelected());
        charsetList.setEnabled(unicodeCharsButton.isSelected());
      }
    };
    defaultCharsButton = new JRadioButton("Default Characters");
    allCharsButton = new JRadioButton("All Characters");
    unicodeCharsButton = new JRadioButton("Specific Unicode Blocks");

    defaultCharsButton.addActionListener(listener);
    allCharsButton.addActionListener(listener);
    unicodeCharsButton.addActionListener(listener);

    ButtonGroup group = new ButtonGroup();
    group.add(defaultCharsButton);
    group.add(allCharsButton);
    group.add(unicodeCharsButton);

    JPanel radioPanel = new JPanel();
    //radioPanel.setBackground(Color.red);
    radioPanel.setLayout(new BoxLayout(radioPanel, BoxLayout.Y_AXIS));
    radioPanel.add(defaultCharsButton);
    radioPanel.add(allCharsButton);
    radioPanel.add(unicodeCharsButton);
    
    JPanel rightStuff = new JPanel();
    rightStuff.setLayout(new BoxLayout(rightStuff, BoxLayout.X_AXIS));
    rightStuff.add(radioPanel);
    rightStuff.add(Box.createHorizontalGlue());
    pain.add(rightStuff);
    pain.add(Box.createVerticalStrut(13));

//    pain.add(radioPanel);
    
//    pain.add(defaultCharsButton);
//    pain.add(allCharsButton);
//    pain.add(unicodeCharsButton);

    defaultCharsButton.setSelected(true);
    charsetList.setEnabled(false);

    //frame.getContentPane().add(scroller);
    pain.add(unicodeBlockScroller);
    pain.add(Box.createVerticalStrut(8));

    JPanel buttons = new JPanel();
    JButton okButton = new JButton("OK");
    okButton.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          setVisible(false);
        }
      });
    okButton.setEnabled(true);
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

    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    Dimension windowSize = getSize();

    setLocation((screen.width - windowSize.width) / 2,
                (screen.height - windowSize.height) / 2);
  }
  
  
  protected char[] getCharacters() {
    if (defaultCharsButton.isSelected()) {
      return PFont.CHARSET;
    }
    
    char[] charset = new char[65536];
    if (allCharsButton.isSelected()) {
      for (int i = 0; i < 0xFFFF; i++) {
        charset[i] = (char) i;
      }
    } else {
      DefaultListModel model = (DefaultListModel) charsetList.getModel();
      int index = 0;
      for (int i = 0; i < BLOCKS.length; i++) {
        if (((JCheckBox) model.get(i)).isSelected()) {
          for (int j = blockStart[i]; j <= blockStop[i]; j++) {
            charset[index++] = (char) j;
          }
        }
      }
      charset = PApplet.subset(charset, 0, index);
    }
    //System.out.println("Creating font with " + charset.length + " characters.");
    return charset;
  }


  // http://www.unicode.org/Public/UNIDATA/Blocks.txt  
  static final String[] BLOCKS = {
    "0000..007F; Basic Latin",
    "0080..00FF; Latin-1 Supplement",
    "0100..017F; Latin Extended-A",
    "0180..024F; Latin Extended-B",
    "0250..02AF; IPA Extensions",
    "02B0..02FF; Spacing Modifier Letters",
    "0300..036F; Combining Diacritical Marks",
    "0370..03FF; Greek and Coptic",
    "0400..04FF; Cyrillic",
    "0500..052F; Cyrillic Supplement",
    "0530..058F; Armenian",
    "0590..05FF; Hebrew",
    "0600..06FF; Arabic",
    "0700..074F; Syriac",
    "0750..077F; Arabic Supplement",
    "0780..07BF; Thaana",
    "07C0..07FF; NKo",
    "0800..083F; Samaritan",
    "0900..097F; Devanagari",
    "0980..09FF; Bengali",
    "0A00..0A7F; Gurmukhi",
    "0A80..0AFF; Gujarati",
    "0B00..0B7F; Oriya",
    "0B80..0BFF; Tamil",
    "0C00..0C7F; Telugu",
    "0C80..0CFF; Kannada",
    "0D00..0D7F; Malayalam",
    "0D80..0DFF; Sinhala",
    "0E00..0E7F; Thai",
    "0E80..0EFF; Lao",
    "0F00..0FFF; Tibetan",
    "1000..109F; Myanmar",
    "10A0..10FF; Georgian",
    "1100..11FF; Hangul Jamo",
    "1200..137F; Ethiopic",
    "1380..139F; Ethiopic Supplement",
    "13A0..13FF; Cherokee",
    "1400..167F; Unified Canadian Aboriginal Syllabics",
    "1680..169F; Ogham",
    "16A0..16FF; Runic",
    "1700..171F; Tagalog",
    "1720..173F; Hanunoo",
    "1740..175F; Buhid",
    "1760..177F; Tagbanwa",
    "1780..17FF; Khmer",
    "1800..18AF; Mongolian",
    "18B0..18FF; Unified Canadian Aboriginal Syllabics Extended",
    "1900..194F; Limbu",
    "1950..197F; Tai Le",
    "1980..19DF; New Tai Lue",
    "19E0..19FF; Khmer Symbols",
    "1A00..1A1F; Buginese",
    "1A20..1AAF; Tai Tham",
    "1B00..1B7F; Balinese",
    "1B80..1BBF; Sundanese",
    "1C00..1C4F; Lepcha",
    "1C50..1C7F; Ol Chiki",
    "1CD0..1CFF; Vedic Extensions",
    "1D00..1D7F; Phonetic Extensions",
    "1D80..1DBF; Phonetic Extensions Supplement",
    "1DC0..1DFF; Combining Diacritical Marks Supplement",
    "1E00..1EFF; Latin Extended Additional",
    "1F00..1FFF; Greek Extended",
    "2000..206F; General Punctuation",
    "2070..209F; Superscripts and Subscripts",
    "20A0..20CF; Currency Symbols",
    "20D0..20FF; Combining Diacritical Marks for Symbols",
    "2100..214F; Letterlike Symbols",
    "2150..218F; Number Forms",
    "2190..21FF; Arrows",
    "2200..22FF; Mathematical Operators",
    "2300..23FF; Miscellaneous Technical",
    "2400..243F; Control Pictures",
    "2440..245F; Optical Character Recognition",
    "2460..24FF; Enclosed Alphanumerics",
    "2500..257F; Box Drawing",
    "2580..259F; Block Elements",
    "25A0..25FF; Geometric Shapes",
    "2600..26FF; Miscellaneous Symbols",
    "2700..27BF; Dingbats",
    "27C0..27EF; Miscellaneous Mathematical Symbols-A",
    "27F0..27FF; Supplemental Arrows-A",
    "2800..28FF; Braille Patterns",
    "2900..297F; Supplemental Arrows-B",
    "2980..29FF; Miscellaneous Mathematical Symbols-B",
    "2A00..2AFF; Supplemental Mathematical Operators",
    "2B00..2BFF; Miscellaneous Symbols and Arrows",
    "2C00..2C5F; Glagolitic",
    "2C60..2C7F; Latin Extended-C",
    "2C80..2CFF; Coptic",
    "2D00..2D2F; Georgian Supplement",
    "2D30..2D7F; Tifinagh",
    "2D80..2DDF; Ethiopic Extended",
    "2DE0..2DFF; Cyrillic Extended-A",
    "2E00..2E7F; Supplemental Punctuation",
    "2E80..2EFF; CJK Radicals Supplement",
    "2F00..2FDF; Kangxi Radicals",
    "2FF0..2FFF; Ideographic Description Characters",
    "3000..303F; CJK Symbols and Punctuation",
    "3040..309F; Hiragana",
    "30A0..30FF; Katakana",
    "3100..312F; Bopomofo",
    "3130..318F; Hangul Compatibility Jamo",
    "3190..319F; Kanbun",
    "31A0..31BF; Bopomofo Extended",
    "31C0..31EF; CJK Strokes",
    "31F0..31FF; Katakana Phonetic Extensions",
    "3200..32FF; Enclosed CJK Letters and Months",
    "3300..33FF; CJK Compatibility",
    "3400..4DBF; CJK Unified Ideographs Extension A",
    "4DC0..4DFF; Yijing Hexagram Symbols",
    "4E00..9FFF; CJK Unified Ideographs",
    "A000..A48F; Yi Syllables",
    "A490..A4CF; Yi Radicals",
    "A4D0..A4FF; Lisu",
    "A500..A63F; Vai",
    "A640..A69F; Cyrillic Extended-B",
    "A6A0..A6FF; Bamum",
    "A700..A71F; Modifier Tone Letters",
    "A720..A7FF; Latin Extended-D",
    "A800..A82F; Syloti Nagri",
    "A830..A83F; Common Indic Number Forms",
    "A840..A87F; Phags-pa",
    "A880..A8DF; Saurashtra",
    "A8E0..A8FF; Devanagari Extended",
    "A900..A92F; Kayah Li",
    "A930..A95F; Rejang",
    "A960..A97F; Hangul Jamo Extended-A",
    "A980..A9DF; Javanese",
    "AA00..AA5F; Cham",
    "AA60..AA7F; Myanmar Extended-A",
    "AA80..AADF; Tai Viet",
    "ABC0..ABFF; Meetei Mayek",
    "AC00..D7AF; Hangul Syllables",
    "D7B0..D7FF; Hangul Jamo Extended-B",
    "D800..DB7F; High Surrogates",
    "DB80..DBFF; High Private Use Surrogates",
    "DC00..DFFF; Low Surrogates",
    "E000..F8FF; Private Use Area",
    "F900..FAFF; CJK Compatibility Ideographs",
    "FB00..FB4F; Alphabetic Presentation Forms",
    "FB50..FDFF; Arabic Presentation Forms-A",
    "FE00..FE0F; Variation Selectors",
    "FE10..FE1F; Vertical Forms",
    "FE20..FE2F; Combining Half Marks",
    "FE30..FE4F; CJK Compatibility Forms",
    "FE50..FE6F; Small Form Variants",
    "FE70..FEFF; Arabic Presentation Forms-B",
    "FF00..FFEF; Halfwidth and Fullwidth Forms",
    "FFF0..FFFF; Specials"
  };

  static String[] blockNames;
  static int[] blockStart;
  static int[] blockStop;
  static {
    int count = BLOCKS.length;
    blockNames = new String[count];
    blockStart = new int[count];
    blockStop = new int[count];
    for (int i = 0; i < count; i++) {
      String line = BLOCKS[i];
      blockStart[i] = PApplet.unhex(line.substring(0, 4));
      blockStop[i] = PApplet.unhex(line.substring(6, 10));
      blockNames[i] = line.substring(12);
    }
//    PApplet.println(codePointStop);
//    PApplet.println(codePoints);
  }
}


// Code for this CheckBoxList class found on the net, though I've lost the 
// link. If you run across the original version, please let me know so that 
// the original author can be credited properly. It was from a snippet 
// collection, but it seems to have been picked up so many places with others
// placing their copyright on it, that I haven't been able to determine the
// original author. [fry 20100216]
class CheckBoxList extends JList {
  protected static Border noFocusBorder = new EmptyBorder(1, 1, 1, 1);

  public CheckBoxList() {
    setCellRenderer(new CellRenderer());

    addMouseListener(new MouseAdapter() {
      public void mousePressed(MouseEvent e) {
        if (isEnabled()) {
          int index = locationToIndex(e.getPoint());

          if (index != -1) {
            JCheckBox checkbox = (JCheckBox)
            getModel().getElementAt(index);
            checkbox.setSelected(!checkbox.isSelected());
            repaint();
          }
        }
      }
    });
    setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
  }


  protected class CellRenderer implements ListCellRenderer {
    public Component getListCellRendererComponent(JList list, Object value, 
                                                  int index, boolean isSelected, 
                                                  boolean cellHasFocus) {
      JCheckBox checkbox = (JCheckBox) value;
      checkbox.setBackground(isSelected ? getSelectionBackground() : getBackground());
      checkbox.setForeground(isSelected ? getSelectionForeground() : getForeground());
      //checkbox.setEnabled(isEnabled());
      checkbox.setEnabled(list.isEnabled());
      checkbox.setFont(getFont());
      checkbox.setFocusPainted(false);
      checkbox.setBorderPainted(true);
      checkbox.setBorder(isSelected ? UIManager.getBorder("List.focusCellHighlightBorder") : noFocusBorder);
      return checkbox;
    }
  }
}