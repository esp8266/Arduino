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

package processing.app;

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;

import javax.swing.*;

import processing.app.syntax.*;
import processing.core.*;




/**
 * Storage class for user preferences and environment settings.
 * <P>
 * This class no longer uses the Properties class, since
 * properties files are iso8859-1, which is highly likely to
 * be a problem when trying to save sketch folders and locations.
 * <p>
 * The GUI portion in here is really ugly, as it uses exact layout. This was
 * done in frustration one evening (and pre-Swing), but that's long since past,
 * and it should all be moved to a proper swing layout like BoxLayout.
 * <p>
 * This is very poorly put together, that the preferences panel and the actual
 * preferences i/o is part of the same code. But there hasn't yet been a
 * compelling reason to bother with the separation aside from concern about
 * being lectured by strangers who feel that it doesn't look like what they
 * learned in CS class.
 * <p>
 * Would also be possible to change this to use the Java Preferences API.
 * Some useful articles
 * <a href="http://www.onjava.com/pub/a/onjava/synd/2001/10/17/j2se.html">here</a> and
 * <a href="http://www.particle.kth.se/~lindsey/JavaCourse/Book/Part1/Java/Chapter10/Preferences.html">here</a>.
 * However, haven't implemented this yet for lack of time, but more
 * importantly, because it would entail writing to the registry (on Windows),
 * or an obscure file location (on Mac OS X) and make it far more difficult to
 * find the preferences to tweak them by hand (no! stay out of regedit!)
 * or to reset the preferences by simply deleting the preferences.txt file.
 */
public class Preferences {

  // what to call the feller

  static final String PREFS_FILE = "preferences.txt";


  // platform strings (used to get settings for specific platforms)

  //static final String platforms[] = {
  //  "other", "windows", "macosx", "linux"
  //};


  // prompt text stuff

  static final String PROMPT_YES     = "Yes";
  static final String PROMPT_NO      = "No";
  static final String PROMPT_CANCEL  = "Cancel";
  static final String PROMPT_OK      = "OK";
  static final String PROMPT_BROWSE  = "Browse";

  /**
   * Standardized width for buttons. Mac OS X 10.3 wants 70 as its default,
   * Windows XP needs 66, and my Ubuntu machine needs 80+, so 80 seems proper.
   */
  static public int BUTTON_WIDTH  = 80;

  /**
   * Standardized button height. Mac OS X 10.3 (Java 1.4) wants 29,
   * presumably because it now includes the blue border, where it didn't
   * in Java 1.3. Windows XP only wants 23 (not sure what default Linux
   * would be). Because of the disparity, on Mac OS X, it will be set
   * inside a static block.
   */
  static public int BUTTON_HEIGHT = 24;
  /*
  // remove this for 0121, because quaqua takes care of it
  static {
    if (Base.isMacOS()) BUTTON_HEIGHT = 29;
  }
  */

  // value for the size bars, buttons, etc

  static final int GRID_SIZE     = 33;


  // indents and spacing standards. these probably need to be modified
  // per platform as well, since macosx is so huge, windows is smaller,
  // and linux is all over the map

  static final int GUI_BIG     = 13;
  static final int GUI_BETWEEN = 10;
  static final int GUI_SMALL   = 6;

  // gui elements

  JFrame dialog;
  int wide, high;

  JTextField sketchbookLocationField;
  JCheckBox exportSeparateBox;
  JCheckBox deletePreviousBox;
//  JCheckBox closingLastQuitsBox;
  JCheckBox externalEditorBox;
  JCheckBox memoryOverrideBox;
  JTextField memoryField;
  JCheckBox checkUpdatesBox;
  JTextField fontSizeField;
  JCheckBox autoAssociateBox;
  JCheckBox menubarWorkaroundBox;


  // the calling editor, so updates can be applied

  Editor editor;


  // data model

  static Hashtable defaults;
  static Hashtable table = new Hashtable();;
  static File preferencesFile;


  static protected void init(String commandLinePrefs) {

    // start by loading the defaults, in case something
    // important was deleted from the user prefs
    try {
      load(Base.getLibStream("preferences.txt"));
    } catch (Exception e) {
      Base.showError(null, "Could not read default settings.\n" +
                           "You'll need to reinstall Processing.", e);
    }

    // check for platform-specific properties in the defaults
    String platformExt = "." + PConstants.platformNames[PApplet.platform];
    int platformExtLength = platformExt.length();
    Enumeration e = table.keys();
    while (e.hasMoreElements()) {
      String key = (String) e.nextElement();
      if (key.endsWith(platformExt)) {
        // this is a key specific to a particular platform
        String actualKey = key.substring(0, key.length() - platformExtLength);
        String value = get(key);
        table.put(actualKey, value);
      }
    }

    // clone the hash table
    defaults = (Hashtable) table.clone();

    // other things that have to be set explicitly for the defaults
    setColor("run.window.bgcolor", SystemColor.control);

    // Load a prefs file if specified on the command line
    if (commandLinePrefs != null) {
      try {
        load(new FileInputStream(commandLinePrefs));

      } catch (Exception poe) {
        Base.showError("Error",
                       "Could not read preferences from " +
                       commandLinePrefs, poe);
      }
    } else if (!Base.isCommandLine()) {
      // next load user preferences file
      preferencesFile = Base.getSettingsFile(PREFS_FILE);
      if (!preferencesFile.exists()) {
        // create a new preferences file if none exists
        // saves the defaults out to the file
        save();

      } else {
        // load the previous preferences file

        try {
          load(new FileInputStream(preferencesFile));

        } catch (Exception ex) {
          Base.showError("Error reading preferences",
                         "Error reading the preferences file. " +
                         "Please delete (or move)\n" +
                         preferencesFile.getAbsolutePath() +
                         " and restart Processing.", ex);
        }
      }

      // Theme settings always override preferences
//      try {
//        load(Base.getStream("theme/theme.txt"));
//      } catch (Exception te) {
//        Base.showError(null, "Could not read color theme settings.\n" +
//                             "You'll need to reinstall Processing.", te);
//      }
    }
  }


  public Preferences() {

    // setup dialog for the prefs

    //dialog = new JDialog(editor, "Preferences", true);
    dialog = new JFrame("Preferences");
    dialog.setResizable(false);

    Container pain = dialog.getContentPane();
    pain.setLayout(null);

    int top = GUI_BIG;
    int left = GUI_BIG;
    int right = 0;

    JLabel label;
    JButton button; //, button2;
    //JComboBox combo;
    Dimension d, d2; //, d3;
    int h, vmax;


    // [ ] Quit after closing last sketch window
    /*
    closingLastQuitsBox =
      new JCheckBox("Quit after closing last sketch window");
    pain.add(closingLastQuitsBox);
    d = closingLastQuitsBox.getPreferredSize();
    closingLastQuitsBox.setBounds(left, top, d.width + 10, d.height);
    right = Math.max(right, left + d.width);
    top += d.height + GUI_BETWEEN;
    */


    /*
    // [ ] Prompt for name and folder when creating new sketch

    sketchPromptBox =
      new JCheckBox("Prompt for name when opening or creating a sketch");
    pain.add(sketchPromptBox);
    d = sketchPromptBox.getPreferredSize();
    sketchPromptBox.setBounds(left, top, d.width + 10, d.height);
    right = Math.max(right, left + d.width);
    top += d.height + GUI_BETWEEN;
    */


    // [ ] Delete empty sketches on Quit

    /*
    sketchCleanBox = new JCheckBox("Delete empty sketches on Quit");
    pain.add(sketchCleanBox);
    d = sketchCleanBox.getPreferredSize();
    sketchCleanBox.setBounds(left, top, d.width + 10, d.height);
    right = Math.max(right, left + d.width);
    top += d.height + GUI_BETWEEN;
    */


    // Sketchbook location:
    // [...............................]  [ Browse ]

    label = new JLabel("Sketchbook location:");
    pain.add(label);
    d = label.getPreferredSize();
    label.setBounds(left, top, d.width, d.height);
    top += d.height; // + GUI_SMALL;

    sketchbookLocationField = new JTextField(40);
    pain.add(sketchbookLocationField);
    d = sketchbookLocationField.getPreferredSize();

    button = new JButton(PROMPT_BROWSE);
    button.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          /*
          JFileChooser fc = new JFileChooser();
          fc.setSelectedFile(new File(sketchbookLocationField.getText()));
          fc.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);

          int returned = fc.showOpenDialog(new JDialog());
          if (returned == JFileChooser.APPROVE_OPTION) {
            File file = fc.getSelectedFile();
            sketchbookLocationField.setText(file.getAbsolutePath());
          }
          */
          File dflt = new File(sketchbookLocationField.getText());
          File file =
            Base.selectFolder("Select new sketchbook location", dflt, dialog);
          if (file != null) {
            sketchbookLocationField.setText(file.getAbsolutePath());
          }
        }
      });
    pain.add(button);
    d2 = button.getPreferredSize();

    // take max height of all components to vertically align em
    vmax = Math.max(d.height, d2.height);
    //label.setBounds(left, top + (vmax-d.height)/2,
    //              d.width, d.height);

    //h = left + d.width + GUI_BETWEEN;
    sketchbookLocationField.setBounds(left, top + (vmax-d.height)/2,
                                      d.width, d.height);
    h = left + d.width + GUI_SMALL; //GUI_BETWEEN;
    button.setBounds(h, top + (vmax-d2.height)/2,
                     d2.width, d2.height);

    right = Math.max(right, h + d2.width + GUI_BIG);
    top += vmax + GUI_BETWEEN;


    // Editor font size [    ]

    Container box = Box.createHorizontalBox();
    label = new JLabel("Editor font size: ");
    box.add(label);
    fontSizeField = new JTextField(4);
    box.add(fontSizeField);
    label = new JLabel("  (requires restart of Processing)");
    box.add(label);
    pain.add(box);
    d = box.getPreferredSize();
    box.setBounds(left, top, d.width, d.height);
    Font editorFont = Preferences.getFont("editor.font");
    fontSizeField.setText(String.valueOf(editorFont.getSize()));
    top += d.height + GUI_BETWEEN;


    // [ ] Increase maximum available memory to [______] MB

    Container memoryBox = Box.createHorizontalBox();
    memoryOverrideBox = new JCheckBox("Increase maximum available memory to ");
    memoryBox.add(memoryOverrideBox);
    memoryField = new JTextField(4);
    memoryBox.add(memoryField);
    memoryBox.add(new JLabel(" MB"));
    pain.add(memoryBox);
    d = memoryBox.getPreferredSize();
    memoryBox.setBounds(left, top, d.width, d.height);
    top += d.height + GUI_BETWEEN;


    // [ ] Use multiple .jar files when exporting applets

    exportSeparateBox =
      new JCheckBox("Use multiple .jar files when exporting applets " +
                    "(ignored when using libraries)");
    pain.add(exportSeparateBox);
    d = exportSeparateBox.getPreferredSize();
    // adding +10 because ubuntu + jre 1.5 truncating items
    exportSeparateBox.setBounds(left, top, d.width + 10, d.height);
    right = Math.max(right, left + d.width);
    top += d.height + GUI_BETWEEN;


    // [ ] Delete previous applet or application folder on export

    deletePreviousBox =
      new JCheckBox("Delete previous applet or application folder on export");
    pain.add(deletePreviousBox);
    d = deletePreviousBox.getPreferredSize();
    deletePreviousBox.setBounds(left, top, d.width + 10, d.height);
    right = Math.max(right, left + d.width);
    top += d.height + GUI_BETWEEN;


    // [ ] Use external editor

    externalEditorBox = new JCheckBox("Use external editor");
    pain.add(externalEditorBox);
    d = externalEditorBox.getPreferredSize();
    externalEditorBox.setBounds(left, top, d.width + 10, d.height);
    right = Math.max(right, left + d.width);
    top += d.height + GUI_BETWEEN;


    // [ ] Check for updates on startup

    checkUpdatesBox = new JCheckBox("Check for updates on startup");
    pain.add(checkUpdatesBox);
    d = checkUpdatesBox.getPreferredSize();
    checkUpdatesBox.setBounds(left, top, d.width + 10, d.height);
    right = Math.max(right, left + d.width);
    top += d.height + GUI_BETWEEN;


    // [ ] Automatically associate .pde files with Processing

    if (Base.isWindows()) {
      autoAssociateBox =
        new JCheckBox("Automatically associate .pde files with Processing");
      pain.add(autoAssociateBox);
      d = autoAssociateBox.getPreferredSize();
      autoAssociateBox.setBounds(left, top, d.width + 10, d.height);
      right = Math.max(right, left + d.width);
      top += d.height + GUI_BETWEEN;
    }


    // [ ] Place menu bar inside

    if (Base.isMacOS()) {
      if (System.getProperty("os.version").startsWith("10.5")) {
        menubarWorkaroundBox =
          new JCheckBox("Place menus inside editor window to avoid " +
                        "Apple Java bug (requires restart)");
        pain.add(menubarWorkaroundBox);
        d = menubarWorkaroundBox.getPreferredSize();
        menubarWorkaroundBox.setBounds(left, top, d.width + 10, d.height);
        right = Math.max(right, left + d.width);
        top += d.height + GUI_BETWEEN;
      }
    }


    // More preferences are in the ...

    label = new JLabel("More preferences can be edited directly in the file");
    pain.add(label);
    d = label.getPreferredSize();
    label.setForeground(Color.gray);
    label.setBounds(left, top, d.width, d.height);
    right = Math.max(right, left + d.width);
    top += d.height; // + GUI_SMALL;

    label = new JLabel(preferencesFile.getAbsolutePath());
    pain.add(label);
    d = label.getPreferredSize();
    label.setBounds(left, top, d.width, d.height);
    right = Math.max(right, left + d.width);
    top += d.height;

    label = new JLabel("(edit only when Processing is not running)");
    pain.add(label);
    d = label.getPreferredSize();
    label.setForeground(Color.gray);
    label.setBounds(left, top, d.width, d.height);
    right = Math.max(right, left + d.width);
    top += d.height; // + GUI_SMALL;


    // [  OK  ] [ Cancel ]  maybe these should be next to the message?

    button = new JButton(PROMPT_OK);
    button.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          applyFrame();
          disposeFrame();
        }
      });
    pain.add(button);
    d2 = button.getPreferredSize();
    BUTTON_HEIGHT = d2.height;

    h = right - (BUTTON_WIDTH + GUI_SMALL + BUTTON_WIDTH);
    button.setBounds(h, top, BUTTON_WIDTH, BUTTON_HEIGHT);
    h += BUTTON_WIDTH + GUI_SMALL;

    button = new JButton(PROMPT_CANCEL);
    button.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          disposeFrame();
        }
      });
    pain.add(button);
    button.setBounds(h, top, BUTTON_WIDTH, BUTTON_HEIGHT);

    top += BUTTON_HEIGHT + GUI_BETWEEN;


    // finish up

    wide = right + GUI_BIG;
    high = top + GUI_SMALL;
    //setSize(wide, high);


    // closing the window is same as hitting cancel button

    dialog.addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
          disposeFrame();
        }
      });

    ActionListener disposer = new ActionListener() {
        public void actionPerformed(ActionEvent actionEvent) {
          disposeFrame();
        }
      };
    Base.registerWindowCloseKeys(dialog.getRootPane(), disposer);
    Base.setIcon(dialog);

    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    dialog.setLocation((screen.width - wide) / 2,
                      (screen.height - high) / 2);

    dialog.pack(); // get insets
    Insets insets = dialog.getInsets();
    dialog.setSize(wide + insets.left + insets.right,
                  high + insets.top + insets.bottom);


    // handle window closing commands for ctrl/cmd-W or hitting ESC.

    pain.addKeyListener(new KeyAdapter() {
        public void keyPressed(KeyEvent e) {
          //System.out.println(e);
          KeyStroke wc = Editor.WINDOW_CLOSE_KEYSTROKE;
          if ((e.getKeyCode() == KeyEvent.VK_ESCAPE) ||
              (KeyStroke.getKeyStrokeForEvent(e).equals(wc))) {
            disposeFrame();
          }
        }
      });
  }


  /*
  protected JRootPane createRootPane() {
    System.out.println("creating root pane esc received");

    ActionListener actionListener = new ActionListener() {
        public void actionPerformed(ActionEvent actionEvent) {
          //setVisible(false);
          System.out.println("esc received");
        }
      };

    JRootPane rootPane = new JRootPane();
    KeyStroke stroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0);
    rootPane.registerKeyboardAction(actionListener, stroke,
                                    JComponent.WHEN_IN_FOCUSED_WINDOW);
    return rootPane;
  }
  */


  public Dimension getPreferredSize() {
    return new Dimension(wide, high);
  }


  // .................................................................


  /**
   * Close the window after an OK or Cancel.
   */
  protected void disposeFrame() {
    dialog.dispose();
  }


  /**
   * Change internal settings based on what was chosen in the prefs,
   * then send a message to the editor saying that it's time to do the same.
   */
  protected void applyFrame() {
    // put each of the settings into the table
    setBoolean("export.applet.separate_jar_files",
               exportSeparateBox.isSelected());
    setBoolean("export.delete_target_folder",
               deletePreviousBox.isSelected());

//    setBoolean("sketchbook.closing_last_window_quits",
//               closingLastQuitsBox.isSelected());
    //setBoolean("sketchbook.prompt", sketchPromptBox.isSelected());
    //setBoolean("sketchbook.auto_clean", sketchCleanBox.isSelected());

    // if the sketchbook path has changed, rebuild the menus
    String oldPath = get("sketchbook.path");
    String newPath = sketchbookLocationField.getText();
    if (!newPath.equals(oldPath)) {
      editor.base.rebuildSketchbookMenus();
      set("sketchbook.path", newPath);
    }

    setBoolean("editor.external", externalEditorBox.isSelected());
    setBoolean("update.check", checkUpdatesBox.isSelected());

    setBoolean("run.options.memory", memoryOverrideBox.isSelected());
    int memoryMin = Preferences.getInteger("run.options.memory.initial");
    int memoryMax = Preferences.getInteger("run.options.memory.maximum");
    try {
      memoryMax = Integer.parseInt(memoryField.getText().trim());
      // make sure memory setting isn't too small
      if (memoryMax < memoryMin) memoryMax = memoryMin;
      setInteger("run.options.memory.maximum", memoryMax);
    } catch (NumberFormatException e) {
      System.err.println("Ignoring bad memory setting");
    }

    /*
      // was gonna use this to check memory settings,
      // but it quickly gets much too messy
    if (getBoolean("run.options.memory")) {
      Process process = Runtime.getRuntime().exec(new String[] {
          "java", "-Xms" + memoryMin + "m", "-Xmx" + memoryMax + "m"
        });
      processInput = new SystemOutSiphon(process.getInputStream());
      processError = new MessageSiphon(process.getErrorStream(), this);
    }
    */

    String newSizeText = fontSizeField.getText();
    try {
      int newSize = Integer.parseInt(newSizeText.trim());
      String pieces[] = PApplet.split(get("editor.font"), ',');
      pieces[2] = String.valueOf(newSize);
      set("editor.font", PApplet.join(pieces, ','));

    } catch (Exception e) {
      System.err.println("ignoring invalid font size " + newSizeText);
    }

    if (autoAssociateBox != null) {
      setBoolean("platform.auto_file_type_associations",
                 autoAssociateBox.isSelected());
    }

    if (menubarWorkaroundBox != null) {
      setBoolean("apple.laf.useScreenMenuBar",
                 !menubarWorkaroundBox.isSelected());
    }

    editor.applyPreferences();
  }


  protected void showFrame(Editor editor) {
    this.editor = editor;

    // set all settings entry boxes to their actual status
    exportSeparateBox.
      setSelected(getBoolean("export.applet.separate_jar_files"));
    deletePreviousBox.
      setSelected(getBoolean("export.delete_target_folder"));

    //closingLastQuitsBox.
    //  setSelected(getBoolean("sketchbook.closing_last_window_quits"));
    //sketchPromptBox.
    //  setSelected(getBoolean("sketchbook.prompt"));
    //sketchCleanBox.
    //  setSelected(getBoolean("sketchbook.auto_clean"));

    sketchbookLocationField.
      setText(get("sketchbook.path"));
    externalEditorBox.
      setSelected(getBoolean("editor.external"));
    checkUpdatesBox.
      setSelected(getBoolean("update.check"));
    memoryOverrideBox.
      setSelected(getBoolean("run.options.memory"));
    memoryField.
      setText(get("run.options.memory.maximum"));

    if (autoAssociateBox != null) {
      autoAssociateBox.
        setSelected(getBoolean("platform.auto_file_type_associations"));
    }

    if (menubarWorkaroundBox != null) {
      menubarWorkaroundBox.
        setSelected(!getBoolean("apple.laf.useScreenMenuBar"));
    }

    dialog.setVisible(true);
  }


  // .................................................................


  static protected void load(InputStream input) throws IOException {
    String[] lines = PApplet.loadStrings(input);  // Reads as UTF-8
    for (String line : lines) {
      if ((line.length() == 0) ||
          (line.charAt(0) == '#')) continue;

      // this won't properly handle = signs being in the text
      int equals = line.indexOf('=');
      if (equals != -1) {
        String key = line.substring(0, equals).trim();
        String value = line.substring(equals + 1).trim();
        table.put(key, value);
      }
    }
  }


  // .................................................................


  static protected void save() {
//    try {
    // on startup, don't worry about it
    // this is trying to update the prefs for who is open
    // before Preferences.init() has been called.
    if (preferencesFile == null) return;

    // Fix for 0163 to properly use Unicode when writing preferences.txt
    PrintWriter writer = PApplet.createWriter(preferencesFile);

    Enumeration e = table.keys(); //properties.propertyNames();
    while (e.hasMoreElements()) {
      String key = (String) e.nextElement();
      writer.println(key + "=" + ((String) table.get(key)));
    }

    writer.flush();
    writer.close();

//    } catch (Exception ex) {
//      Base.showWarning(null, "Error while saving the settings file", ex);
//    }
  }


  // .................................................................


  // all the information from preferences.txt

  //static public String get(String attribute) {
  //return get(attribute, null);
  //}

  static public String get(String attribute /*, String defaultValue */) {
    return (String) table.get(attribute);
    /*
    //String value = (properties != null) ?
    //properties.getProperty(attribute) : applet.getParameter(attribute);
    String value = properties.getProperty(attribute);

    return (value == null) ?
      defaultValue : value;
    */
  }


  static public String getDefault(String attribute) {
    return (String) defaults.get(attribute);
  }


  static public void set(String attribute, String value) {
    table.put(attribute, value);
  }


  static public boolean getBoolean(String attribute) {
    String value = get(attribute); //, null);
    return (new Boolean(value)).booleanValue();

    /*
      supposedly not needed, because anything besides 'true'
      (ignoring case) will just be false.. so if malformed -> false
    if (value == null) return defaultValue;

    try {
      return (new Boolean(value)).booleanValue();
    } catch (NumberFormatException e) {
      System.err.println("expecting an integer: " + attribute + " = " + value);
    }
    return defaultValue;
    */
  }


  static public void setBoolean(String attribute, boolean value) {
    set(attribute, value ? "true" : "false");
  }


  static public int getInteger(String attribute /*, int defaultValue*/) {
    return Integer.parseInt(get(attribute));

    /*
    String value = get(attribute, null);
    if (value == null) return defaultValue;

    try {
      return Integer.parseInt(value);
    } catch (NumberFormatException e) {
      // ignored will just fall through to returning the default
      System.err.println("expecting an integer: " + attribute + " = " + value);
    }
    return defaultValue;
    //if (value == null) return defaultValue;
    //return (value == null) ? defaultValue :
    //Integer.parseInt(value);
    */
  }


  static public void setInteger(String key, int value) {
    set(key, String.valueOf(value));
  }


  static public Color getColor(String name) {
    Color parsed = Color.GRAY;  // set a default  
    String s = get(name);
    if ((s != null) && (s.indexOf("#") == 0)) {
      try {
        parsed = new Color(Integer.parseInt(s.substring(1), 16));
      } catch (Exception e) { }
    }
    return parsed;
  }


  static public void setColor(String attr, Color what) {
//    String r = Integer.toHexString(what.getRed());
//    String g = Integer.toHexString(what.getGreen());
//    String b = Integer.toHexString(what.getBlue());
//    set(attr, "#" + r.substring(r.length() - 2) +
//        g.substring(g.length() - 2) + b.substring(b.length() - 2));
    set(attr, "#" + PApplet.hex(what.getRGB() & 0xffffff, 6));
  }


  static public Font getFont(String attr) {
    boolean replace = false;
    String value = get(attr);
    if (value == null) {
      //System.out.println("reset 1");
      value = getDefault(attr);
      replace = true;
    }

    String[] pieces = PApplet.split(value, ',');
    if (pieces.length != 3) {
      value = getDefault(attr);
      //System.out.println("reset 2 for " + attr);
      pieces = PApplet.split(value, ',');
      //PApplet.println(pieces);
      replace = true;
    }

    String name = pieces[0];
    int style = Font.PLAIN;  // equals zero
    if (pieces[1].indexOf("bold") != -1) {
      style |= Font.BOLD;
    }
    if (pieces[1].indexOf("italic") != -1) {
      style |= Font.ITALIC;
    }
    int size = PApplet.parseInt(pieces[2], 12);
    Font font = new Font(name, style, size);

    // replace bad font with the default
    if (replace) {
      set(attr, value);
    }

    return font;
  }


  static public SyntaxStyle getStyle(String what /*, String dflt*/) {
    String str = get("editor." + what + ".style"); //, dflt);

    StringTokenizer st = new StringTokenizer(str, ",");

    String s = st.nextToken();
    if (s.indexOf("#") == 0) s = s.substring(1);
    Color color = Color.DARK_GRAY; 
    try {
      color = new Color(Integer.parseInt(s, 16));
    } catch (Exception e) { }

    s = st.nextToken();
    boolean bold = (s.indexOf("bold") != -1);
    boolean italic = (s.indexOf("italic") != -1);
    //System.out.println(what + " = " + str + " " + bold + " " + italic);

    return new SyntaxStyle(color, italic, bold);
  }
}
