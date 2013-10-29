/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-09 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License version 2
  as published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package processing.app;

import cc.arduino.packages.UploaderAndMonitorFactory;
import com.jcraft.jsch.JSchException;
import processing.app.debug.*;
import processing.app.forms.PasswordAuthorizationDialog;
import processing.app.helpers.PreferencesMapException;
import processing.app.syntax.*;
import processing.app.tools.*;
import processing.core.*;
import static processing.app.I18n._;

import java.awt.*;
import java.awt.datatransfer.*;
import java.awt.event.*;
import java.awt.print.*;
import java.io.*;
import java.net.*;
import java.util.*;
import java.util.List;
import java.util.zip.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.undo.*;

import cc.arduino.packages.BoardPort;
import cc.arduino.packages.Uploader;
import cc.arduino.packages.uploaders.SerialUploader;

/**
 * Main editor panel for the Processing Development Environment.
 */
@SuppressWarnings("serial")
public class Editor extends JFrame implements RunnerListener {

  Base base;

  // otherwise, if the window is resized with the message label
  // set to blank, it's preferredSize() will be fukered
  static protected final String EMPTY =
    "                                                                     " +
    "                                                                     " +
    "                                                                     ";

  /** Command on Mac OS X, Ctrl on Windows and Linux */
  static final int SHORTCUT_KEY_MASK =
    Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
  /** Command-W on Mac OS X, Ctrl-W on Windows and Linux */
  static final KeyStroke WINDOW_CLOSE_KEYSTROKE =
    KeyStroke.getKeyStroke('W', SHORTCUT_KEY_MASK);
  /** Command-Option on Mac OS X, Ctrl-Alt on Windows and Linux */
  static final int SHORTCUT_ALT_KEY_MASK = ActionEvent.ALT_MASK |
    Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();

  /**
   * true if this file has not yet been given a name by the user
   */
  boolean untitled;

  PageFormat pageFormat;
  PrinterJob printerJob;

  // file, sketch, and tools menus for re-inserting items
  JMenu fileMenu;
  JMenu sketchMenu;
  JMenu toolsMenu;

  int numTools = 0;

  EditorToolbar toolbar;
  // these menus are shared so that they needn't be rebuilt for all windows
  // each time a sketch is created, renamed, or moved.
  static JMenu toolbarMenu;
  static JMenu sketchbookMenu;
  static JMenu examplesMenu;
  static JMenu importMenu;

  // these menus are shared so that the board and serial port selections
  // are the same for all windows (since the board and serial port that are
  // actually used are determined by the preferences, which are shared)
  static List<JMenu> boardsMenus;
  static JMenu serialMenu;

  static AbstractMonitor serialMonitor;

  EditorHeader header;
  EditorStatus status;
  EditorConsole console;

  JSplitPane splitPane;
  JPanel consolePanel;

  JLabel lineNumberComponent;

  // currently opened program
  Sketch sketch;

  EditorLineStatus lineStatus;

  //JEditorPane editorPane;

  JEditTextArea textarea;
  EditorListener listener;

  // runtime information and window placement
  Point sketchWindowLocation;
  //Runner runtime;

  JMenuItem exportAppItem;
  JMenuItem saveMenuItem;
  JMenuItem saveAsMenuItem;

  boolean running;
  //boolean presenting;
  boolean uploading;

  // undo fellers
  JMenuItem undoItem, redoItem;
  protected UndoAction undoAction;
  protected RedoAction redoAction;
  LastUndoableEditAwareUndoManager undo;
  // used internally, and only briefly
  CompoundEdit compoundEdit;

  FindReplace find;

  Runnable runHandler;
  Runnable presentHandler;
  Runnable stopHandler;
  Runnable exportHandler;
  Runnable exportAppHandler;


  public Editor(Base ibase, String path, int[] location) throws Exception {
    super("Arduino");
    this.base = ibase;

    Base.setIcon(this);

    // Install default actions for Run, Present, etc.
    resetHandlers();

    // add listener to handle window close box hit event
    addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
          base.handleClose(Editor.this);
        }
      });
    // don't close the window when clicked, the app will take care
    // of that via the handleQuitInternal() methods
    // http://dev.processing.org/bugs/show_bug.cgi?id=440
    setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);

    // When bringing a window to front, let the Base know
    addWindowListener(new WindowAdapter() {
        public void windowActivated(WindowEvent e) {
//          System.err.println("activate");  // not coming through
          base.handleActivated(Editor.this);
          // re-add the sub-menus that are shared by all windows
          fileMenu.insert(sketchbookMenu, 2);
          fileMenu.insert(examplesMenu, 3);
          sketchMenu.insert(importMenu, 4);
          int offset = 0;
          for (JMenu menu : boardsMenus) {
            toolsMenu.insert(menu, numTools + offset);
            offset++;
          }
          toolsMenu.insert(serialMenu, numTools + offset);
        }

        // added for 1.0.5
        // http://dev.processing.org/bugs/show_bug.cgi?id=1260
        public void windowDeactivated(WindowEvent e) {
//          System.err.println("deactivate");  // not coming through
          fileMenu.remove(sketchbookMenu);
          fileMenu.remove(examplesMenu);
          sketchMenu.remove(importMenu);
          for (JMenu menu : boardsMenus) {
            toolsMenu.remove(menu);
          }
          toolsMenu.remove(serialMenu);
        }
      });

    //PdeKeywords keywords = new PdeKeywords();
    //sketchbook = new Sketchbook(this);

    buildMenuBar();

    // For rev 0120, placing things inside a JPanel
    Container contentPain = getContentPane();
    contentPain.setLayout(new BorderLayout());
    JPanel pain = new JPanel();
    pain.setLayout(new BorderLayout());
    contentPain.add(pain, BorderLayout.CENTER);

    Box box = Box.createVerticalBox();
    Box upper = Box.createVerticalBox();

    if (toolbarMenu == null) {
      toolbarMenu = new JMenu();
      base.rebuildToolbarMenu(toolbarMenu);
    }
    toolbar = new EditorToolbar(this, toolbarMenu);
    upper.add(toolbar);

    header = new EditorHeader(this);
    upper.add(header);

    textarea = new JEditTextArea(new PdeTextAreaDefaults());
    textarea.setName("editor");
    textarea.setRightClickPopup(new TextAreaPopup());
    textarea.setHorizontalOffset(6);

    // assemble console panel, consisting of status area and the console itself
    consolePanel = new JPanel();
    consolePanel.setLayout(new BorderLayout());

    status = new EditorStatus(this);
    consolePanel.add(status, BorderLayout.NORTH);

    console = new EditorConsole(this);
    console.setName("console");
    // windows puts an ugly border on this guy
    console.setBorder(null);
    consolePanel.add(console, BorderLayout.CENTER);

    lineStatus = new EditorLineStatus(textarea);
    consolePanel.add(lineStatus, BorderLayout.SOUTH);

    upper.add(textarea);
    splitPane = new JSplitPane(JSplitPane.VERTICAL_SPLIT,
                               upper, consolePanel);

    splitPane.setOneTouchExpandable(true);
    // repaint child panes while resizing
    splitPane.setContinuousLayout(true);
    // if window increases in size, give all of increase to
    // the textarea in the uppper pane
    splitPane.setResizeWeight(1D);

    // to fix ugliness.. normally macosx java 1.3 puts an
    // ugly white border around this object, so turn it off.
    splitPane.setBorder(null);

    // the default size on windows is too small and kinda ugly
    int dividerSize = Preferences.getInteger("editor.divider.size");
    if (dividerSize != 0) {
      splitPane.setDividerSize(dividerSize);
    }

    // the following changed from 600, 400 for netbooks
    // http://code.google.com/p/arduino/issues/detail?id=52
    splitPane.setMinimumSize(new Dimension(600, 100));
    box.add(splitPane);

    // hopefully these are no longer needed w/ swing
    // (har har har.. that was wishful thinking)
    listener = new EditorListener(this, textarea);
    pain.add(box);

    // get shift down/up events so we can show the alt version of toolbar buttons
    textarea.addKeyListener(toolbar);

    pain.setTransferHandler(new FileDropHandler());

//    System.out.println("t1");

    // Finish preparing Editor (formerly found in Base)
    pack();

//    System.out.println("t2");

    // Set the window bounds and the divider location before setting it visible
    setPlacement(location);


    // Set the minimum size for the editor window
    setMinimumSize(new Dimension(Preferences.getInteger("editor.window.width.min"),
                                 Preferences.getInteger("editor.window.height.min")));
//    System.out.println("t3");

    // Bring back the general options for the editor
    applyPreferences();

//    System.out.println("t4");

    // Open the document that was passed in
    boolean loaded = handleOpenInternal(path);
    if (!loaded) sketch = null;

//    System.out.println("t5");

    // All set, now show the window
    //setVisible(true);
  }


  /**
   * Handles files dragged & dropped from the desktop and into the editor
   * window. Dragging files into the editor window is the same as using
   * "Sketch &rarr; Add File" for each file.
   */
  class FileDropHandler extends TransferHandler {
    public boolean canImport(JComponent dest, DataFlavor[] flavors) {
      return true;
    }

    @SuppressWarnings("unchecked")
    public boolean importData(JComponent src, Transferable transferable) {
      int successful = 0;

      try {
        DataFlavor uriListFlavor =
          new DataFlavor("text/uri-list;class=java.lang.String");

        if (transferable.isDataFlavorSupported(DataFlavor.javaFileListFlavor)) {
          java.util.List list = (java.util.List)
            transferable.getTransferData(DataFlavor.javaFileListFlavor);
          for (int i = 0; i < list.size(); i++) {
            File file = (File) list.get(i);
            if (sketch.addFile(file)) {
              successful++;
            }
          }
        } else if (transferable.isDataFlavorSupported(uriListFlavor)) {
          // Some platforms (Mac OS X and Linux, when this began) preferred
          // this method of moving files.
          String data = (String)transferable.getTransferData(uriListFlavor);
          String[] pieces = PApplet.splitTokens(data, "\r\n");
          for (int i = 0; i < pieces.length; i++) {
            if (pieces[i].startsWith("#")) continue;

            String path = null;
            if (pieces[i].startsWith("file:///")) {
              path = pieces[i].substring(7);
            } else if (pieces[i].startsWith("file:/")) {
              path = pieces[i].substring(5);
            }
            if (sketch.addFile(new File(path))) {
              successful++;
            }
          }
        }
      } catch (Exception e) {
        e.printStackTrace();
        return false;
      }

      if (successful == 0) {
        statusError(_("No files were added to the sketch."));

      } else if (successful == 1) {
        statusNotice(_("One file added to the sketch."));

      } else {
        statusNotice(
	    I18n.format(_("{0} files added to the sketch."), successful));
      }
      return true;
    }
  }


  protected void setPlacement(int[] location) {
    setBounds(location[0], location[1], location[2], location[3]);
    if (location[4] != 0) {
      splitPane.setDividerLocation(location[4]);
    }
  }


  protected int[] getPlacement() {
    int[] location = new int[5];

    // Get the dimensions of the Frame
    Rectangle bounds = getBounds();
    location[0] = bounds.x;
    location[1] = bounds.y;
    location[2] = bounds.width;
    location[3] = bounds.height;

    // Get the current placement of the divider
    location[4] = splitPane.getDividerLocation();

    return location;
  }


  /**
   * Hack for #@#)$(* Mac OS X 10.2.
   * <p/>
   * This appears to only be required on OS X 10.2, and is not
   * even being called on later versions of OS X or Windows.
   */
//  public Dimension getMinimumSize() {
//    //System.out.println("getting minimum size");
//    return new Dimension(500, 550);
//  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Read and apply new values from the preferences, either because
   * the app is just starting up, or the user just finished messing
   * with things in the Preferences window.
   */
  protected void applyPreferences() {

    // apply the setting for 'use external editor'
    boolean external = Preferences.getBoolean("editor.external");

    textarea.setEditable(!external);
    saveMenuItem.setEnabled(!external);
    saveAsMenuItem.setEnabled(!external);
    
    textarea.setDisplayLineNumbers(Preferences.getBoolean("editor.linenumbers"));

    TextAreaPainter painter = textarea.getPainter();
    if (external) {
      // disable line highlight and turn off the caret when disabling
      Color color = Theme.getColor("editor.external.bgcolor");
      painter.setBackground(color);
      painter.setLineHighlightEnabled(false);
      textarea.setCaretVisible(false);

    } else {
      Color color = Theme.getColor("editor.bgcolor");
      painter.setBackground(color);
      boolean highlight = Preferences.getBoolean("editor.linehighlight");
      painter.setLineHighlightEnabled(highlight);
      textarea.setCaretVisible(true);
    }

    // apply changes to the font size for the editor
    //TextAreaPainter painter = textarea.getPainter();
    painter.setFont(Preferences.getFont("editor.font"));
    //Font font = painter.getFont();
    //textarea.getPainter().setFont(new Font("Courier", Font.PLAIN, 36));

    // in case tab expansion stuff has changed
    listener.applyPreferences();

    // in case moved to a new location
    // For 0125, changing to async version (to be implemented later)
    //sketchbook.rebuildMenus();
    // For 0126, moved into Base, which will notify all editors.
    //base.rebuildMenusAsync();
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  protected void buildMenuBar() throws Exception {
    JMenuBar menubar = new JMenuBar();
    menubar.add(buildFileMenu());
    menubar.add(buildEditMenu());
    menubar.add(buildSketchMenu());
    menubar.add(buildToolsMenu());
    menubar.add(buildHelpMenu());
    setJMenuBar(menubar);
  }


  protected JMenu buildFileMenu() {
    JMenuItem item;
    fileMenu = new JMenu(_("File"));

    item = newJMenuItem(_("New"), 'N');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          try {
            base.handleNew();
          } catch (Exception e1) {
            e1.printStackTrace();
          }
        }
      });
    fileMenu.add(item);

    item = Editor.newJMenuItem(_("Open..."), 'O');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          try {
            base.handleOpenPrompt();
          } catch (Exception e1) {
            e1.printStackTrace();
          }
        }
      });
    fileMenu.add(item);

    if (sketchbookMenu == null) {
      sketchbookMenu = new JMenu(_("Sketchbook"));
      MenuScroller.setScrollerFor(sketchbookMenu);
      base.rebuildSketchbookMenu(sketchbookMenu);
    }
    fileMenu.add(sketchbookMenu);

    if (examplesMenu == null) {
      examplesMenu = new JMenu(_("Examples"));
      MenuScroller.setScrollerFor(examplesMenu);
      base.rebuildExamplesMenu(examplesMenu);
    }
    fileMenu.add(examplesMenu);

    item = Editor.newJMenuItem(_("Close"), 'W');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          base.handleClose(Editor.this);
        }
      });
    fileMenu.add(item);

    saveMenuItem = newJMenuItem(_("Save"), 'S');
    saveMenuItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleSave(false);
        }
      });
    fileMenu.add(saveMenuItem);

    saveAsMenuItem = newJMenuItemShift(_("Save As..."), 'S');
    saveAsMenuItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleSaveAs();
        }
      });
    fileMenu.add(saveAsMenuItem);

    item = newJMenuItem(_("Upload"), 'U');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleExport(false);
        }
      });
    fileMenu.add(item);

    item = newJMenuItemShift(_("Upload Using Programmer"), 'U');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleExport(true);
        }
      });
    fileMenu.add(item);

    fileMenu.addSeparator();

    item = newJMenuItemShift(_("Page Setup"), 'P');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handlePageSetup();
        }
      });
    fileMenu.add(item);

    item = newJMenuItem(_("Print"), 'P');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handlePrint();
        }
      });
    fileMenu.add(item);

    // macosx already has its own preferences and quit menu
    if (!Base.isMacOS()) {
      fileMenu.addSeparator();

      item = newJMenuItem(_("Preferences"), ',');
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            base.handlePrefs();
          }
        });
      fileMenu.add(item);

      fileMenu.addSeparator();

      item = newJMenuItem(_("Quit"), 'Q');
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            base.handleQuit();
          }
        });
      fileMenu.add(item);
    }
    return fileMenu;
  }


  protected JMenu buildSketchMenu() {
    JMenuItem item;
    sketchMenu = new JMenu(_("Sketch"));

    item = newJMenuItem(_("Verify / Compile"), 'R');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleRun(false);
        }
      });
    sketchMenu.add(item);

//    item = newJMenuItemShift("Verify / Compile (verbose)", 'R');
//    item.addActionListener(new ActionListener() {
//        public void actionPerformed(ActionEvent e) {
//          handleRun(true);
//        }
//      });
//    sketchMenu.add(item);

//    item = new JMenuItem("Stop");
//    item.addActionListener(new ActionListener() {
//        public void actionPerformed(ActionEvent e) {
//          handleStop();
//        }
//      });
//    sketchMenu.add(item);

    sketchMenu.addSeparator();

    if (importMenu == null) {
      importMenu = new JMenu(_("Import Library..."));
      MenuScroller.setScrollerFor(importMenu);
      base.rebuildImportMenu(importMenu);
    }
    sketchMenu.add(importMenu);

    item = newJMenuItem(_("Show Sketch Folder"), 'K');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openFolder(sketch.getFolder());
        }
      });
    sketchMenu.add(item);
    item.setEnabled(Base.openFolderAvailable());

    item = new JMenuItem(_("Add File..."));
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          sketch.handleAddFile();
        }
      });
    sketchMenu.add(item);

    return sketchMenu;
  }


  protected JMenu buildToolsMenu() throws Exception {
    toolsMenu = new JMenu(_("Tools"));
    JMenu menu = toolsMenu;
    JMenuItem item;

    addInternalTools(menu);

    item = newJMenuItemShift(_("Serial Monitor"), 'M');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleSerial();
        }
      });
    menu.add(item);

    addTools(menu, Base.getToolsFolder());
    File sketchbookTools = new File(Base.getSketchbookFolder(), "tools");
    addTools(menu, sketchbookTools);

    menu.addSeparator();

    numTools = menu.getItemCount();

    // XXX: DAM: these should probably be implemented using the Tools plugin
    // API, if possible (i.e. if it supports custom actions, etc.)

    if (boardsMenus == null) {
      boardsMenus = new LinkedList<JMenu>();

      JMenu boardsMenu = new JMenu(_("Board"));
      MenuScroller.setScrollerFor(boardsMenu);
      Editor.boardsMenus.add(boardsMenu);
      toolsMenu.add(boardsMenu);

      base.rebuildBoardsMenu(toolsMenu, this);
      //Debug: rebuild imports
      importMenu.removeAll();
      base.rebuildImportMenu(importMenu);
    }

    if (serialMenu == null)
      serialMenu = new JMenu(_("Port"));
    populatePortMenu();
    menu.add(serialMenu);
    menu.addSeparator();

    JMenu programmerMenu = new JMenu(_("Programmer"));
    base.rebuildProgrammerMenu(programmerMenu);
    menu.add(programmerMenu);

    item = new JMenuItem(_("Burn Bootloader"));
    item.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        handleBurnBootloader();
      }
    });
    menu.add(item);

    menu.addMenuListener(new MenuListener() {
      public void menuCanceled(MenuEvent e) {}
      public void menuDeselected(MenuEvent e) {}
      public void menuSelected(MenuEvent e) {
        //System.out.println("Tools menu selected.");
        populatePortMenu();
      }
    });

    return menu;
  }


  protected void addTools(JMenu menu, File sourceFolder) {
    if (sourceFolder == null)
      return;

    Map<String, JMenuItem> toolItems = new HashMap<String, JMenuItem>();

    File[] folders = sourceFolder.listFiles(new FileFilter() {
      public boolean accept(File folder) {
        if (folder.isDirectory()) {
          //System.out.println("checking " + folder);
          File subfolder = new File(folder, "tool");
          return subfolder.exists();
        }
        return false;
      }
    });

    if (folders == null || folders.length == 0) {
      return;
    }

    for (int i = 0; i < folders.length; i++) {
      File toolDirectory = new File(folders[i], "tool");

      try {
        // add dir to classpath for .classes
        //urlList.add(toolDirectory.toURL());

        // add .jar files to classpath
        File[] archives = toolDirectory.listFiles(new FilenameFilter() {
          public boolean accept(File dir, String name) {
            return (name.toLowerCase().endsWith(".jar") ||
                    name.toLowerCase().endsWith(".zip"));
          }
        });

        URL[] urlList = new URL[archives.length];
        for (int j = 0; j < urlList.length; j++) {
          urlList[j] = archives[j].toURI().toURL();
        }
        URLClassLoader loader = new URLClassLoader(urlList);

        String className = null;
        for (int j = 0; j < archives.length; j++) {
          className = findClassInZipFile(folders[i].getName(), archives[j]);
          if (className != null) break;
        }

        /*
        // Alternatively, could use manifest files with special attributes:
        // http://java.sun.com/j2se/1.3/docs/guide/jar/jar.html
        // Example code for loading from a manifest file:
        // http://forums.sun.com/thread.jspa?messageID=3791501
        File infoFile = new File(toolDirectory, "tool.txt");
        if (!infoFile.exists()) continue;

        String[] info = PApplet.loadStrings(infoFile);
        //Main-Class: org.poo.shoe.AwesomerTool
        //String className = folders[i].getName();
        String className = null;
        for (int k = 0; k < info.length; k++) {
          if (info[k].startsWith(";")) continue;

          String[] pieces = PApplet.splitTokens(info[k], ": ");
          if (pieces.length == 2) {
            if (pieces[0].equals("Main-Class")) {
              className = pieces[1];
            }
          }
        }
        */
        // If no class name found, just move on.
        if (className == null) continue;

        Class<?> toolClass = Class.forName(className, true, loader);
        final Tool tool = (Tool) toolClass.newInstance();

        tool.init(Editor.this);

        String title = tool.getMenuTitle();
        JMenuItem item = new JMenuItem(title);
        item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(tool);
            //new Thread(tool).start();
          }
        });
        //menu.add(item);
        toolItems.put(title, item);

      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    ArrayList<String> toolList = new ArrayList<String>(toolItems.keySet());
    if (toolList.size() == 0) return;

    menu.addSeparator();
    Collections.sort(toolList);
    for (String title : toolList) {
      menu.add((JMenuItem) toolItems.get(title));
    }
  }


  protected String findClassInZipFile(String base, File file) {
    // Class file to search for
    String classFileName = "/" + base + ".class";

    try {
      ZipFile zipFile = new ZipFile(file);
      Enumeration<?> entries = zipFile.entries();
      while (entries.hasMoreElements()) {
        ZipEntry entry = (ZipEntry) entries.nextElement();

        if (!entry.isDirectory()) {
          String name = entry.getName();
          //System.out.println("entry: " + name);

          if (name.endsWith(classFileName)) {
            //int slash = name.lastIndexOf('/');
            //String packageName = (slash == -1) ? "" : name.substring(0, slash);
            // Remove .class and convert slashes to periods.
            return name.substring(0, name.length() - 6).replace('/', '.');
          }
        }
      }
    } catch (IOException e) {
      //System.err.println("Ignoring " + filename + " (" + e.getMessage() + ")");
      e.printStackTrace();
    }
    return null;
  }


  protected JMenuItem createToolMenuItem(String className) {
    try {
      Class<?> toolClass = Class.forName(className);
      final Tool tool = (Tool) toolClass.newInstance();

      JMenuItem item = new JMenuItem(tool.getMenuTitle());

      tool.init(Editor.this);

      item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          SwingUtilities.invokeLater(tool);
        }
      });
      return item;

    } catch (Exception e) {
      e.printStackTrace();
      return null;
    }
  }


  protected JMenu addInternalTools(JMenu menu) {
    JMenuItem item;

    item = createToolMenuItem("cc.arduino.packages.formatter.AStyle");
    item.setName("menuToolsAutoFormat");
    int modifiers = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
    item.setAccelerator(KeyStroke.getKeyStroke('T', modifiers));
    menu.add(item);

    //menu.add(createToolMenuItem("processing.app.tools.CreateFont"));
    //menu.add(createToolMenuItem("processing.app.tools.ColorSelector"));
    menu.add(createToolMenuItem("processing.app.tools.Archiver"));
    menu.add(createToolMenuItem("processing.app.tools.FixEncoding"));

//    // These are temporary entries while Android mode is being worked out.
//    // The mode will not be in the tools menu, and won't involve a cmd-key
//    if (!Base.RELEASE) {
//      item = createToolMenuItem("processing.app.tools.android.AndroidTool");
//    item.setAccelerator(KeyStroke.getKeyStroke('D', modifiers));
//    menu.add(item);
//      menu.add(createToolMenuItem("processing.app.tools.android.Reset"));
//    }

    return menu;
  }


  class SerialMenuListener implements ActionListener {

    private final String serialPort;

    public SerialMenuListener(String serialPort) {
      this.serialPort = serialPort;
    }

    public void actionPerformed(ActionEvent e) {
      selectSerialPort(serialPort);
      base.onBoardOrPortChange();
    }

  }

  protected void selectSerialPort(String name) {
    if(serialMenu == null) {
      System.out.println(_("serialMenu is null"));
      return;
    }
    if (name == null) {
      System.out.println(_("name is null"));
      return;
    }
    JCheckBoxMenuItem selection = null;
    for (int i = 0; i < serialMenu.getItemCount(); i++) {
      JCheckBoxMenuItem item = ((JCheckBoxMenuItem)serialMenu.getItem(i));
      if (item == null) {
        System.out.println(_("name is null"));
        continue;
      }
      item.setState(false);
      if (name.equals(item.getText())) selection = item;
    }
    if (selection != null) selection.setState(true);
    //System.out.println(item.getLabel());
    Preferences.set("serial.port", name);
    if (name.startsWith("/dev/"))
      Preferences.set("serial.port.file", name.substring(5));
    else
      Preferences.set("serial.port.file", name);
    if (serialMonitor != null) {
      try {
        serialMonitor.close();
        serialMonitor.setVisible(false);
      } catch (Exception e) {
        // ignore
      }
    }

    onBoardOrPortChange();

    //System.out.println("set to " + get("serial.port"));
  }


  protected void populatePortMenu() {
    serialMenu.removeAll();

    String selectedPort = Preferences.get("serial.port");

    List<BoardPort> ports = Base.getDiscoveryManager().discovery();
    for (BoardPort port : ports) {
      String address = port.getAddress();
      String label = port.getLabel();

      JCheckBoxMenuItem item = new JCheckBoxMenuItem(label, address.equals(selectedPort));
      item.addActionListener(new SerialMenuListener(address));
      serialMenu.add(item);
    }

    serialMenu.setEnabled(serialMenu.getMenuComponentCount() > 0);
  }


  protected JMenu buildHelpMenu() {
    // To deal with a Mac OS X 10.5 bug, add an extra space after the name
    // so that the OS doesn't try to insert its slow help menu.
    JMenu menu = new JMenu(_("Help"));
    JMenuItem item;

    /*
    // testing internal web server to serve up docs from a zip file
    item = new JMenuItem("Web Server Test");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //WebServer ws = new WebServer();
          SwingUtilities.invokeLater(new Runnable() {
            public void run() {
              try {
                int port = WebServer.launch("/Users/fry/coconut/processing/build/shared/reference.zip");
                Base.openURL("http://127.0.0.1:" + port + "/reference/setup_.html");

              } catch (IOException e1) {
                e1.printStackTrace();
              }
            }
          });
        }
      });
    menu.add(item);
    */

    /*
    item = new JMenuItem("Browser Test");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //Base.openURL("http://processing.org/learning/gettingstarted/");
          //JFrame browserFrame = new JFrame("Browser");
          BrowserStartup bs = new BrowserStartup("jar:file:/Users/fry/coconut/processing/build/shared/reference.zip!/reference/setup_.html");
          bs.initUI();
          bs.launch();
        }
      });
    menu.add(item);
    */

    item = new JMenuItem(_("Getting Started"));
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.showGettingStarted();
        }
      });
    menu.add(item);

    item = new JMenuItem(_("Environment"));
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.showEnvironment();
        }
      });
    menu.add(item);

    item = new JMenuItem(_("Troubleshooting"));
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.showTroubleshooting();
        }
      });
    menu.add(item);

    item = new JMenuItem(_("Reference"));
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.showReference();
        }
      });
    menu.add(item);

    item = newJMenuItemShift(_("Find in Reference"), 'F');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
//          if (textarea.isSelectionActive()) {
//            handleFindReference();
//          }
        	handleFindReference();
        }
      });
    menu.add(item);

    item = new JMenuItem(_("Frequently Asked Questions"));
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.showFAQ();
        }
      });
    menu.add(item);

    item = new JMenuItem(_("Visit Arduino.cc"));
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL(_("http://arduino.cc/"));
        }
      });
    menu.add(item);

    // macosx already has its own about menu
    if (!Base.isMacOS()) {
      menu.addSeparator();
      item = new JMenuItem(_("About Arduino"));
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            base.handleAbout();
          }
        });
      menu.add(item);
    }

    return menu;
  }


  protected JMenu buildEditMenu() {
    JMenu menu = new JMenu(_("Edit"));
    menu.setName("menuEdit");
    JMenuItem item;

    undoItem = newJMenuItem(_("Undo"), 'Z');
    undoItem.setName("menuEditUndo");
    undoItem.addActionListener(undoAction = new UndoAction());
    menu.add(undoItem);

    if (!Base.isMacOS()) {
        redoItem = newJMenuItem(_("Redo"), 'Y');
    } else {
        redoItem = newJMenuItemShift(_("Redo"), 'Z');
    }
    redoItem.setName("menuEditRedo");
    redoItem.addActionListener(redoAction = new RedoAction());
    menu.add(redoItem);

    menu.addSeparator();

    // TODO "cut" and "copy" should really only be enabled
    // if some text is currently selected
    item = newJMenuItem(_("Cut"), 'X');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleCut();
        }
      });
    menu.add(item);

    item = newJMenuItem(_("Copy"), 'C');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          textarea.copy();
        }
      });
    menu.add(item);

    item = newJMenuItemShift(_("Copy for Forum"), 'C');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
//          SwingUtilities.invokeLater(new Runnable() {
//              public void run() {
          new DiscourseFormat(Editor.this, false).show();
//              }
//            });
        }
      });
    menu.add(item);

    item = newJMenuItemAlt(_("Copy as HTML"), 'C');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
//          SwingUtilities.invokeLater(new Runnable() {
//              public void run() {
          new DiscourseFormat(Editor.this, true).show();
//              }
//            });
        }
      });
    menu.add(item);

    item = newJMenuItem(_("Paste"), 'V');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          textarea.paste();
          sketch.setModified(true);
        }
      });
    menu.add(item);

    item = newJMenuItem(_("Select All"), 'A');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          textarea.selectAll();
        }
      });
    menu.add(item);

    menu.addSeparator();

    item = newJMenuItem(_("Comment/Uncomment"), '/');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleCommentUncomment();
        }
    });
    menu.add(item);

    item = newJMenuItem(_("Increase Indent"), ']');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleIndentOutdent(true);
        }
    });
    menu.add(item);

    item = newJMenuItem(_("Decrease Indent"), '[');
    item.setName("menuDecreaseIndent");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleIndentOutdent(false);
        }
    });
    menu.add(item);

    menu.addSeparator();

    item = newJMenuItem(_("Find..."), 'F');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (find == null) {
            find = new FindReplace(Editor.this);
          }
          //new FindReplace(Editor.this).show();
          find.setVisible(true);
          //find.setVisible(true);
        }
      });
    menu.add(item);

    // TODO find next should only be enabled after a
    // search has actually taken place
    item = newJMenuItem(_("Find Next"), 'G');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (find != null) {
            find.findNext();
          }
        }
      });
    menu.add(item);

    item = newJMenuItemShift(_("Find Previous"), 'G');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (find != null) {
            find.findPrevious();
          }
        }
      });
    menu.add(item);

    item = newJMenuItem(_("Use Selection For Find"), 'E');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (find == null) {
            find = new FindReplace(Editor.this);
          }
          find.setFindText( getSelectedText() );
        }
      });
    menu.add(item);

    return menu;
  }


  /**
   * A software engineer, somewhere, needs to have his abstraction
   * taken away. In some countries they jail or beat people for writing
   * the sort of API that would require a five line helper function
   * just to set the command key for a menu item.
   */
  static public JMenuItem newJMenuItem(String title, int what) {
    JMenuItem menuItem = new JMenuItem(title);
    menuItem.setAccelerator(KeyStroke.getKeyStroke(what, SHORTCUT_KEY_MASK));
    return menuItem;
  }


  /**
   * Like newJMenuItem() but adds shift as a modifier for the key command.
   */
  static public JMenuItem newJMenuItemShift(String title, int what) {
    JMenuItem menuItem = new JMenuItem(title);
    menuItem.setAccelerator(KeyStroke.getKeyStroke(what, SHORTCUT_KEY_MASK | ActionEvent.SHIFT_MASK));
    return menuItem;
  }


  /**
   * Same as newJMenuItem(), but adds the ALT (on Linux and Windows)
   * or OPTION (on Mac OS X) key as a modifier.
   */
  static public JMenuItem newJMenuItemAlt(String title, int what) {
    JMenuItem menuItem = new JMenuItem(title);
    menuItem.setAccelerator(KeyStroke.getKeyStroke(what, SHORTCUT_ALT_KEY_MASK));
    return menuItem;
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  class UndoAction extends AbstractAction {
    public UndoAction() {
      super("Undo");
      this.setEnabled(false);
    }

    public void actionPerformed(ActionEvent e) {
      try {
        undo.undo();
      } catch (CannotUndoException ex) {
        //System.out.println("Unable to undo: " + ex);
        //ex.printStackTrace();
      }
      if (undo.getLastUndoableEdit() != null && undo.getLastUndoableEdit() instanceof CaretAwareUndoableEdit) {
        CaretAwareUndoableEdit undoableEdit = (CaretAwareUndoableEdit) undo.getLastUndoableEdit();
        int nextCaretPosition = undoableEdit.getCaretPosition() - 1;
        if (nextCaretPosition >= 0 && textarea.getDocumentLength() > nextCaretPosition) {
          textarea.setCaretPosition(nextCaretPosition);
        }
      }
      updateUndoState();
      redoAction.updateRedoState();
    }

    protected void updateUndoState() {
      if (undo.canUndo()) {
        this.setEnabled(true);
        undoItem.setEnabled(true);
        undoItem.setText(undo.getUndoPresentationName());
        putValue(Action.NAME, undo.getUndoPresentationName());
        if (sketch != null) {
          sketch.setModified(true);  // 0107
        }
      } else {
        this.setEnabled(false);
        undoItem.setEnabled(false);
        undoItem.setText(_("Undo"));
        putValue(Action.NAME, "Undo");
        if (sketch != null) {
          sketch.setModified(false);  // 0107
        }
      }
    }
  }


  class RedoAction extends AbstractAction {
    public RedoAction() {
      super("Redo");
      this.setEnabled(false);
    }

    public void actionPerformed(ActionEvent e) {
      try {
        undo.redo();
      } catch (CannotRedoException ex) {
        //System.out.println("Unable to redo: " + ex);
        //ex.printStackTrace();
      }
      if (undo.getLastUndoableEdit() != null && undo.getLastUndoableEdit() instanceof CaretAwareUndoableEdit) {
        CaretAwareUndoableEdit undoableEdit = (CaretAwareUndoableEdit) undo.getLastUndoableEdit();
        textarea.setCaretPosition(undoableEdit.getCaretPosition());
      }
      updateRedoState();
      undoAction.updateUndoState();
    }

    protected void updateRedoState() {
      if (undo.canRedo()) {
        redoItem.setEnabled(true);
        redoItem.setText(undo.getRedoPresentationName());
        putValue(Action.NAME, undo.getRedoPresentationName());
      } else {
        this.setEnabled(false);
        redoItem.setEnabled(false);
        redoItem.setText(_("Redo"));
        putValue(Action.NAME, "Redo");
      }
    }
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  // these will be done in a more generic way soon, more like:
  // setHandler("action name", Runnable);
  // but for the time being, working out the kinks of how many things to
  // abstract from the editor in this fashion.


  public void setHandlers(Runnable runHandler, Runnable presentHandler,
                          Runnable stopHandler,
                          Runnable exportHandler, Runnable exportAppHandler) {
    this.runHandler = runHandler;
    this.presentHandler = presentHandler;
    this.stopHandler = stopHandler;
    this.exportHandler = exportHandler;
    this.exportAppHandler = exportAppHandler;
  }


  public void resetHandlers() {
    runHandler = new DefaultRunHandler();
    presentHandler = new DefaultPresentHandler();
    stopHandler = new DefaultStopHandler();
    exportHandler = new DefaultExportHandler();
    exportAppHandler = new DefaultExportAppHandler();
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Gets the current sketch object.
   */
  public Sketch getSketch() {
    return sketch;
  }


  /**
   * Get the JEditTextArea object for use (not recommended). This should only
   * be used in obscure cases that really need to hack the internals of the
   * JEditTextArea. Most tools should only interface via the get/set functions
   * found in this class. This will maintain compatibility with future releases,
   * which will not use JEditTextArea.
   */
  public JEditTextArea getTextArea() {
    return textarea;
  }


  /**
   * Get the contents of the current buffer. Used by the Sketch class.
   */
  public String getText() {
    return textarea.getText();
  }


  /**
   * Get a range of text from the current buffer.
   */
  public String getText(int start, int stop) {
    return textarea.getText(start, stop - start);
  }


  /**
   * Replace the entire contents of the front-most tab.
   */
  public void setText(String what) {
    startCompoundEdit();
    textarea.setText(what);
    stopCompoundEdit();
  }


  public void insertText(String what) {
    startCompoundEdit();
    int caret = getCaretOffset();
    setSelection(caret, caret);
    textarea.setSelectedText(what);
    stopCompoundEdit();
  }


  /**
   * Called to update the text but not switch to a different set of code
   * (which would affect the undo manager).
   */
//  public void setText2(String what, int start, int stop) {
//    beginCompoundEdit();
//    textarea.setText(what);
//    endCompoundEdit();
//
//    // make sure that a tool isn't asking for a bad location
//    start = Math.max(0, Math.min(start, textarea.getDocumentLength()));
//    stop = Math.max(0, Math.min(start, textarea.getDocumentLength()));
//    textarea.select(start, stop);
//
//    textarea.requestFocus();  // get the caret blinking
//  }


  public String getSelectedText() {
    return textarea.getSelectedText();
  }


  public void setSelectedText(String what) {
    textarea.setSelectedText(what);
  }


  public void setSelection(int start, int stop) {
    // make sure that a tool isn't asking for a bad location
    start = PApplet.constrain(start, 0, textarea.getDocumentLength());
    stop = PApplet.constrain(stop, 0, textarea.getDocumentLength());

    textarea.select(start, stop);
  }


  /**
   * Get the position (character offset) of the caret. With text selected,
   * this will be the last character actually selected, no matter the direction
   * of the selection. That is, if the user clicks and drags to select lines
   * 7 up to 4, then the caret position will be somewhere on line four.
   */
  public int getCaretOffset() {
    return textarea.getCaretPosition();
  }


  /**
   * True if some text is currently selected.
   */
  public boolean isSelectionActive() {
    return textarea.isSelectionActive();
  }


  /**
   * Get the beginning point of the current selection.
   */
  public int getSelectionStart() {
    return textarea.getSelectionStart();
  }


  /**
   * Get the end point of the current selection.
   */
  public int getSelectionStop() {
    return textarea.getSelectionStop();
  }


  /**
   * Get text for a specified line.
   */
  public String getLineText(int line) {
    return textarea.getLineText(line);
  }


  /**
   * Replace the text on a specified line.
   */
  public void setLineText(int line, String what) {
    startCompoundEdit();
    textarea.select(getLineStartOffset(line), getLineStopOffset(line));
    textarea.setSelectedText(what);
    stopCompoundEdit();
  }


  /**
   * Get character offset for the start of a given line of text.
   */
  public int getLineStartOffset(int line) {
    return textarea.getLineStartOffset(line);
  }


  /**
   * Get character offset for end of a given line of text.
   */
  public int getLineStopOffset(int line) {
    return textarea.getLineStopOffset(line);
  }


  /**
   * Get the number of lines in the currently displayed buffer.
   */
  public int getLineCount() {
    return textarea.getLineCount();
  }


  /**
   * Use before a manipulating text to group editing operations together as a
   * single undo. Use stopCompoundEdit() once finished.
   */
  public void startCompoundEdit() {
    compoundEdit = new CompoundEdit();
  }


  /**
   * Use with startCompoundEdit() to group edit operations in a single undo.
   */
  public void stopCompoundEdit() {
    compoundEdit.end();
    undo.addEdit(compoundEdit);
    undoAction.updateUndoState();
    redoAction.updateRedoState();
    compoundEdit = null;
  }


  public int getScrollPosition() {
    return textarea.getScrollPosition();
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Switch between tabs, this swaps out the Document object
   * that's currently being manipulated.
   */
  protected void setCode(SketchCode code) {
    SyntaxDocument document = (SyntaxDocument) code.getDocument();

    if (document == null) {  // this document not yet inited
      document = new SyntaxDocument();
      code.setDocument(document);

      // turn on syntax highlighting
      document.setTokenMarker(new PdeKeywords());

      // insert the program text into the document object
      try {
        document.insertString(0, code.getProgram(), null);
      } catch (BadLocationException bl) {
        bl.printStackTrace();
      }

      // set up this guy's own undo manager
//      code.undo = new UndoManager();

      // connect the undo listener to the editor
      document.addUndoableEditListener(new UndoableEditListener() {
          public void undoableEditHappened(UndoableEditEvent e) {
            if (compoundEdit != null) {
              compoundEdit.addEdit(e.getEdit());

            } else if (undo != null) {
              undo.addEdit(new CaretAwareUndoableEdit(e.getEdit(), textarea));
              undoAction.updateUndoState();
              redoAction.updateRedoState();
            }
          }
        });
    }

    // update the document object that's in use
    textarea.setDocument(document,
                         code.getSelectionStart(), code.getSelectionStop(),
                         code.getScrollPosition());

    textarea.requestFocus();  // get the caret blinking

    this.undo = code.getUndo();
    undoAction.updateUndoState();
    redoAction.updateRedoState();
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Implements Edit &rarr; Cut.
   */
  public void handleCut() {
    textarea.cut();
    sketch.setModified(true);
  }


  /**
   * Implements Edit &rarr; Copy.
   */
  public void handleCopy() {
    textarea.copy();
  }


  protected void handleDiscourseCopy() {
    new DiscourseFormat(Editor.this, false).show();
  }


  protected void handleHTMLCopy() {
    new DiscourseFormat(Editor.this, true).show();
  }


  /**
   * Implements Edit &rarr; Paste.
   */
  public void handlePaste() {
    textarea.paste();
    sketch.setModified(true);
  }


  /**
   * Implements Edit &rarr; Select All.
   */
  public void handleSelectAll() {
    textarea.selectAll();
  }


  protected void handleCommentUncomment() {
    startCompoundEdit();

    int startLine = textarea.getSelectionStartLine();
    int stopLine = textarea.getSelectionStopLine();

    int lastLineStart = textarea.getLineStartOffset(stopLine);
    int selectionStop = textarea.getSelectionStop();
    // If the selection ends at the beginning of the last line,
    // then don't (un)comment that line.
    if (selectionStop == lastLineStart) {
      // Though if there's no selection, don't do that
      if (textarea.isSelectionActive()) {
        stopLine--;
      }
    }

    // If the text is empty, ignore the user.
    // Also ensure that all lines are commented (not just the first)
    // when determining whether to comment or uncomment.
    int length = textarea.getDocumentLength();
    boolean commented = true;
    for (int i = startLine; commented && (i <= stopLine); i++) {
      int pos = textarea.getLineStartOffset(i);
      if (pos + 2 > length) {
        commented = false;
      } else {
        // Check the first two characters to see if it's already a comment.
        String begin = textarea.getText(pos, 2);
        //System.out.println("begin is '" + begin + "'");
        commented = begin.equals("//");
      }
    }

    for (int line = startLine; line <= stopLine; line++) {
      int location = textarea.getLineStartOffset(line);
      if (commented) {
        // remove a comment
        textarea.select(location, location+2);
        if (textarea.getSelectedText().equals("//")) {
          textarea.setSelectedText("");
        }
      } else {
        // add a comment
        textarea.select(location, location);
        textarea.setSelectedText("//");
      }
    }
    // Subtract one from the end, otherwise selects past the current line.
    // (Which causes subsequent calls to keep expanding the selection)
    textarea.select(textarea.getLineStartOffset(startLine),
                    textarea.getLineStopOffset(stopLine) - 1);
    stopCompoundEdit();
  }


  protected void handleIndentOutdent(boolean indent) {
    int tabSize = Preferences.getInteger("editor.tabs.size");
    String tabString = Editor.EMPTY.substring(0, tabSize);

    startCompoundEdit();

    int startLine = textarea.getSelectionStartLine();
    int stopLine = textarea.getSelectionStopLine();

    // If the selection ends at the beginning of the last line,
    // then don't (un)comment that line.
    int lastLineStart = textarea.getLineStartOffset(stopLine);
    int selectionStop = textarea.getSelectionStop();
    if (selectionStop == lastLineStart) {
      // Though if there's no selection, don't do that
      if (textarea.isSelectionActive()) {
        stopLine--;
      }
    }

    for (int line = startLine; line <= stopLine; line++) {
      int location = textarea.getLineStartOffset(line);

      if (indent) {
        textarea.select(location, location);
        textarea.setSelectedText(tabString);

      } else {  // outdent
        textarea.select(location, location + tabSize);
        // Don't eat code if it's not indented
        if (textarea.getSelectedText().equals(tabString)) {
          textarea.setSelectedText("");
        }
      }
    }
    // Subtract one from the end, otherwise selects past the current line.
    // (Which causes subsequent calls to keep expanding the selection)
    textarea.select(textarea.getLineStartOffset(startLine),
                    textarea.getLineStopOffset(stopLine) - 1);
    stopCompoundEdit();
  }

	protected String getCurrentKeyword() {
		String text = "";
		if (textarea.getSelectedText() != null)
			text = textarea.getSelectedText().trim();

		try {
			int current = textarea.getCaretPosition();
			int startOffset = 0;
			int endIndex = current;
			String tmp = textarea.getDocument().getText(current, 1);
			// TODO probably a regexp that matches Arduino lang special chars
			// already exists.
			String regexp = "[\\s\\n();\\\\.!='\\[\\]{}]";

			while (!tmp.matches(regexp)) {
				endIndex++;
				tmp = textarea.getDocument().getText(endIndex, 1);
			}
			// For some reason document index start at 2.
			// if( current - start < 2 ) return;

			tmp = "";
			while (!tmp.matches(regexp)) {
				startOffset++;
				if (current - startOffset < 0) {
					tmp = textarea.getDocument().getText(0, 1);
					break;
				} else
					tmp = textarea.getDocument().getText(current - startOffset, 1);
			}
			startOffset--;

			int length = endIndex - current + startOffset;
			text = textarea.getDocument().getText(current - startOffset, length);

		} catch (BadLocationException bl) {
			bl.printStackTrace();
		} finally {
			return text;
		}
	}

	protected void handleFindReference() {
		String text = getCurrentKeyword();

		String referenceFile = PdeKeywords.getReference(text);
		if (referenceFile == null) {
			statusNotice(I18n.format(_("No reference available for \"{0}\""), text));
		} else {
			Base.showReference(I18n.format(_("{0}.html"), referenceFile));
		}
	}


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Implements Sketch &rarr; Run.
   * @param verbose Set true to run with verbose output.
   */
  public void handleRun(final boolean verbose) {
    internalCloseRunner();
    running = true;
    toolbar.activate(EditorToolbar.RUN);
    status.progress(_("Compiling sketch..."));

    // do this to advance/clear the terminal window / dos prompt / etc
    for (int i = 0; i < 10; i++) System.out.println();

    // clear the console on each run, unless the user doesn't want to
    if (Preferences.getBoolean("console.auto_clear")) {
      console.clear();
    }

    // Cannot use invokeLater() here, otherwise it gets
    // placed on the event thread and causes a hang--bad idea all around.
    new Thread(verbose ? presentHandler : runHandler).start();
  }

  // DAM: in Arduino, this is compile
  class DefaultRunHandler implements Runnable {
    public void run() {
      try {
        sketch.prepare();
        sketch.build(false);
        statusNotice(_("Done compiling."));
      } catch (Exception e) {
        status.unprogress();
        statusError(e);
      }

      status.unprogress();
      toolbar.deactivate(EditorToolbar.RUN);
    }
  }

  // DAM: in Arduino, this is compile (with verbose output)
  class DefaultPresentHandler implements Runnable {
    public void run() {
      try {
        sketch.prepare();
        sketch.build(true);
        statusNotice(_("Done compiling."));
      } catch (Exception e) {
        status.unprogress();
        statusError(e);
      }

      status.unprogress();
      toolbar.deactivate(EditorToolbar.RUN);
    }
  }

  class DefaultStopHandler implements Runnable {
    public void run() {
      try {
        // DAM: we should try to kill the compilation or upload process here.
      } catch (Exception e) {
        statusError(e);
      }
    }
  }

  /**
   * Set the location of the sketch run window. Used by Runner to update the
   * Editor about window drag events while the sketch is running.
   */
  public void setSketchLocation(Point p) {
    sketchWindowLocation = p;
  }


  /**
   * Get the last location of the sketch's run window. Used by Runner to make
   * the window show up in the same location as when it was last closed.
   */
  public Point getSketchLocation() {
    return sketchWindowLocation;
  }


  /**
   * Implements Sketch &rarr; Stop, or pressing Stop on the toolbar.
   */
  public void handleStop() {  // called by menu or buttons
//    toolbar.activate(EditorToolbar.STOP);

    internalCloseRunner();

    toolbar.deactivate(EditorToolbar.RUN);
//    toolbar.deactivate(EditorToolbar.STOP);

    // focus the PDE again after quitting presentation mode [toxi 030903]
    toFront();
  }


  /**
   * Deactivate the Run button. This is called by Runner to notify that the
   * sketch has stopped running, usually in response to an error (or maybe
   * the sketch completing and exiting?) Tools should not call this function.
   * To initiate a "stop" action, call handleStop() instead.
   */
  public void internalRunnerClosed() {
    running = false;
    toolbar.deactivate(EditorToolbar.RUN);
  }


  /**
   * Handle internal shutdown of the runner.
   */
  public void internalCloseRunner() {
    running = false;

    if (stopHandler != null)
    try {
      stopHandler.run();
    } catch (Exception e) { }
  }


  /**
   * Check if the sketch is modified and ask user to save changes.
   * @return false if canceling the close/quit operation
   */
  protected boolean checkModified() {
    if (!sketch.isModified()) return true;

    // As of Processing 1.0.10, this always happens immediately.
    // http://dev.processing.org/bugs/show_bug.cgi?id=1456

    String prompt = I18n.format(_("Save changes to \"{0}\"?  "), sketch.getName());

    if (!Base.isMacOS()) {
      int result =
        JOptionPane.showConfirmDialog(this, prompt, _("Close"),
                                      JOptionPane.YES_NO_CANCEL_OPTION,
                                      JOptionPane.QUESTION_MESSAGE);

      switch (result) {
        case JOptionPane.YES_OPTION:
          return handleSave(true);
        case JOptionPane.NO_OPTION:
          return true;  // ok to continue
        case JOptionPane.CANCEL_OPTION:
        case JOptionPane.CLOSED_OPTION:  // Escape key pressed
          return false;
        default:
          throw new IllegalStateException();
      }

    } else {
      // This code is disabled unless Java 1.5 is being used on Mac OS X
      // because of a Java bug that prevents the initial value of the
      // dialog from being set properly (at least on my MacBook Pro).
      // The bug causes the "Don't Save" option to be the highlighted,
      // blinking, default. This sucks. But I'll tell you what doesn't
      // suck--workarounds for the Mac and Apple's snobby attitude about it!
      // I think it's nifty that they treat their developers like dirt.

      // Pane formatting adapted from the quaqua guide
      // http://www.randelshofer.ch/quaqua/guide/joptionpane.html
      JOptionPane pane =
        new JOptionPane(_("<html> " +
                          "<head> <style type=\"text/css\">"+
                          "b { font: 13pt \"Lucida Grande\" }"+
                          "p { font: 11pt \"Lucida Grande\"; margin-top: 8px }"+
                          "</style> </head>" +
                          "<b>Do you want to save changes to this sketch<BR>" +
                          " before closing?</b>" +
                          "<p>If you don't save, your changes will be lost."),
                        JOptionPane.QUESTION_MESSAGE);

      String[] options = new String[] {
        _("Save"), _("Cancel"), _("Don't Save")
      };
      pane.setOptions(options);

      // highlight the safest option ala apple hig
      pane.setInitialValue(options[0]);

      // on macosx, setting the destructive property places this option
      // away from the others at the lefthand side
      pane.putClientProperty("Quaqua.OptionPane.destructiveOption",
                             new Integer(2));

      JDialog dialog = pane.createDialog(this, null);
      dialog.setVisible(true);

      Object result = pane.getValue();
      if (result == options[0]) {  // save (and close/quit)
        return handleSave(true);

      } else if (result == options[2]) {  // don't save (still close/quit)
        return true;

      } else {  // cancel?
        return false;
      }
    }
  }


  /**
   * Open a sketch from a particular path, but don't check to save changes.
   * Used by Sketch.saveAs() to re-open a sketch after the "Save As"
   */
  protected void handleOpenUnchecked(String path, int codeIndex,
                                     int selStart, int selStop, int scrollPos) {
    internalCloseRunner();
    handleOpenInternal(path);
    // Replacing a document that may be untitled. If this is an actual
    // untitled document, then editor.untitled will be set by Base.
    untitled = false;

    sketch.setCurrentCode(codeIndex);
    textarea.select(selStart, selStop);
    textarea.setScrollPosition(scrollPos);
  }


  /**
   * Second stage of open, occurs after having checked to see if the
   * modifications (if any) to the previous sketch need to be saved.
   */
  protected boolean handleOpenInternal(String path) {
    // check to make sure that this .pde file is
    // in a folder of the same name
    File file = new File(path);
    String fileName = file.getName();
    File parent = file.getParentFile();
    String parentName = parent.getName();
    String pdeName = parentName + ".pde";
    File altPdeFile = new File(parent, pdeName);
    String inoName = parentName + ".ino";
    File altInoFile = new File(parent, pdeName);

    if (pdeName.equals(fileName) || inoName.equals(fileName)) {
      // no beef with this guy

    } else if (altPdeFile.exists()) {
      // user selected a .java from the same sketch, but open the .pde instead
      path = altPdeFile.getAbsolutePath();
    } else if (altInoFile.exists()) {
      path = altInoFile.getAbsolutePath();
    } else if (!path.endsWith(".ino") && !path.endsWith(".pde")) {
      Base.showWarning(_("Bad file selected"),
                       _("Processing can only open its own sketches\n" +
                         "and other files ending in .ino or .pde"), null);
      return false;

    } else {
      String properParent =
        fileName.substring(0, fileName.length() - 4);

      Object[] options = { _("OK"), _("Cancel") };
      String prompt = I18n.format(
	_("The file \"{0}\" needs to be inside\n" +
	  "a sketch folder named \"{1}\".\n" +
	  "Create this folder, move the file, and continue?"),
	fileName,
	properParent
      );

      int result = JOptionPane.showOptionDialog(this,
                                                prompt,
                                                _("Moving"),
                                                JOptionPane.YES_NO_OPTION,
                                                JOptionPane.QUESTION_MESSAGE,
                                                null,
                                                options,
                                                options[0]);

      if (result == JOptionPane.YES_OPTION) {
        // create properly named folder
        File properFolder = new File(file.getParent(), properParent);
        if (properFolder.exists()) {
          Base.showWarning(_("Error"),
                           I18n.format(
                             _("A folder named \"{0}\" already exists. " +
                               "Can't open sketch."),
                             properParent
                           ),
			   null);
          return false;
        }
        if (!properFolder.mkdirs()) {
          //throw new IOException("Couldn't create sketch folder");
          Base.showWarning(_("Error"),
                           _("Could not create the sketch folder."), null);
          return false;
        }
        // copy the sketch inside
        File properPdeFile = new File(properFolder, file.getName());
        File origPdeFile = new File(path);
        try {
          Base.copyFile(origPdeFile, properPdeFile);
        } catch (IOException e) {
          Base.showWarning(_("Error"), _("Could not copy to a proper location."), e);
          return false;
        }

        // remove the original file, so user doesn't get confused
        origPdeFile.delete();

        // update with the new path
        path = properPdeFile.getAbsolutePath();

      } else if (result == JOptionPane.NO_OPTION) {
        return false;
      }
    }

    try {
      sketch = new Sketch(this, path);
    } catch (IOException e) {
      Base.showWarning(_("Error"), _("Could not create the sketch."), e);
      return false;
    }
    header.rebuild();
    // Set the title of the window to "sketch_070752a - Processing 0126"
    setTitle(
      I18n.format(
	_("{0} | Arduino {1}"),
	sketch.getName(),
	Base.VERSION_NAME
      )
    );
    // Disable untitled setting from previous document, if any
    untitled = false;

    // Store information on who's open and running
    // (in case there's a crash or something that can't be recovered)
    base.storeSketches();
    Preferences.save();

    // opening was successful
    return true;

//    } catch (Exception e) {
//      e.printStackTrace();
//      statusError(e);
//      return false;
//    }
  }


  /**
   * Actually handle the save command. If 'immediately' is set to false,
   * this will happen in another thread so that the message area
   * will update and the save button will stay highlighted while the
   * save is happening. If 'immediately' is true, then it will happen
   * immediately. This is used during a quit, because invokeLater()
   * won't run properly while a quit is happening. This fixes
   * <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=276">Bug 276</A>.
   */
  public boolean handleSave(boolean immediately) {
    //stopRunner();
    handleStop();  // 0136

    if (untitled) {
      return handleSaveAs();
      // need to get the name, user might also cancel here

    } else if (immediately) {
      return handleSave2();

    } else {
      SwingUtilities.invokeLater(new Runnable() {
          public void run() {
            handleSave2();
          }
        });
    }
    return true;
  }


  protected boolean handleSave2() {
    toolbar.activate(EditorToolbar.SAVE);
    statusNotice(_("Saving..."));
    boolean saved = false;
    try {
      saved = sketch.save();
      if (saved)
        statusNotice(_("Done Saving."));
      else
        statusEmpty();
      // rebuild sketch menu in case a save-as was forced
      // Disabling this for 0125, instead rebuild the menu inside
      // the Save As method of the Sketch object, since that's the
      // only one who knows whether something was renamed.
      //sketchbook.rebuildMenus();
      //sketchbook.rebuildMenusAsync();

    } catch (Exception e) {
      // show the error as a message in the window
      statusError(e);

      // zero out the current action,
      // so that checkModified2 will just do nothing
      //checkModifiedMode = 0;
      // this is used when another operation calls a save
    }
    //toolbar.clear();
    toolbar.deactivate(EditorToolbar.SAVE);
    return saved;
  }


  public boolean handleSaveAs() {
    //stopRunner();  // formerly from 0135
    handleStop();

    toolbar.activate(EditorToolbar.SAVE);

    //SwingUtilities.invokeLater(new Runnable() {
    //public void run() {
    statusNotice(_("Saving..."));
    try {
      if (sketch.saveAs()) {
        statusNotice(_("Done Saving."));
        // Disabling this for 0125, instead rebuild the menu inside
        // the Save As method of the Sketch object, since that's the
        // only one who knows whether something was renamed.
        //sketchbook.rebuildMenusAsync();
      } else {
        statusNotice(_("Save Canceled."));
        return false;
      }
    } catch (Exception e) {
      // show the error as a message in the window
      statusError(e);

    } finally {
      // make sure the toolbar button deactivates
      toolbar.deactivate(EditorToolbar.SAVE);
    }

    return true;
  }


  public boolean serialPrompt() {
    int count = serialMenu.getItemCount();
    Object[] names = new Object[count];
    for (int i = 0; i < count; i++) {
      names[i] = ((JCheckBoxMenuItem)serialMenu.getItem(i)).getText();
    }

    String result = (String)
      JOptionPane.showInputDialog(this,
	I18n.format(
	  _("Serial port {0} not found.\n" +
	    "Retry the upload with another serial port?"),
	  Preferences.get("serial.port")
	),
				  "Serial port not found",
                                  JOptionPane.PLAIN_MESSAGE,
                                  null,
                                  names,
                                  0);
    if (result == null) return false;
    selectSerialPort(result);
    base.onBoardOrPortChange();
    return true;
  }


  /**
   * Called by Sketch &rarr; Export.
   * Handles calling the export() function on sketch, and
   * queues all the gui status stuff that comes along with it.
   * <p/>
   * Made synchronized to (hopefully) avoid problems of people
   * hitting export twice, quickly, and horking things up.
   */
  /**
   * Handles calling the export() function on sketch, and
   * queues all the gui status stuff that comes along with it.
   *
   * Made synchronized to (hopefully) avoid problems of people
   * hitting export twice, quickly, and horking things up.
   */
  synchronized public void handleExport(final boolean usingProgrammer) {
    //if (!handleExportCheckModified()) return;
    toolbar.activate(EditorToolbar.EXPORT);
    console.clear();
    status.progress(_("Uploading to I/O Board..."));

    new Thread(usingProgrammer ? exportAppHandler : exportHandler).start();
  }

  // DAM: in Arduino, this is upload
  class DefaultExportHandler implements Runnable {
    public void run() {

      try {
        if (serialMonitor != null) {
          serialMonitor.close();
          serialMonitor.setVisible(false);
        }

        uploading = true;

        boolean success = sketch.exportApplet(false);
        if (success) {
          statusNotice(_("Done uploading."));
        } else {
          // error message will already be visible
        }
      } catch (SerialNotFoundException e) {
        populatePortMenu();
        if (serialMenu.getItemCount() == 0) statusError(e);
        else if (serialPrompt()) run();
        else statusNotice(_("Upload canceled."));
      } catch (PreferencesMapException e) {
        statusError(I18n.format(
                    _("Error while uploading: missing '{0}' configuration parameter"),
                    e.getMessage()));
      } catch (RunnerException e) {
        //statusError("Error during upload.");
        //e.printStackTrace();
        status.unprogress();
        statusError(e);
      } catch (Exception e) {
        e.printStackTrace();
      }
      status.unprogress();
      uploading = false;
      //toolbar.clear();
      toolbar.deactivate(EditorToolbar.EXPORT);
    }
  }

  // DAM: in Arduino, this is upload (with verbose output)
  class DefaultExportAppHandler implements Runnable {
    public void run() {

      try {
        if (serialMonitor != null) {
          serialMonitor.close();
          serialMonitor.setVisible(false);
        }

        uploading = true;

        boolean success = sketch.exportApplet(true);
        if (success) {
          statusNotice(_("Done uploading."));
        } else {
          // error message will already be visible
        }
      } catch (SerialNotFoundException e) {
        populatePortMenu();
        if (serialMenu.getItemCount() == 0) statusError(e);
        else if (serialPrompt()) run();
        else statusNotice(_("Upload canceled."));
      } catch (PreferencesMapException e) {
        statusError(I18n.format(
                    _("Error while uploading: missing '{0}' configuration parameter"),
                    e.getMessage()));
      } catch (RunnerException e) {
        //statusError("Error during upload.");
        //e.printStackTrace();
        status.unprogress();
        statusError(e);
      } catch (Exception e) {
        e.printStackTrace();
      }
      status.unprogress();
      uploading = false;
      //toolbar.clear();
      toolbar.deactivate(EditorToolbar.EXPORT);
    }
  }

  /**
   * Checks to see if the sketch has been modified, and if so,
   * asks the user to save the sketch or cancel the export.
   * This prevents issues where an incomplete version of the sketch
   * would be exported, and is a fix for
   * <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=157">Bug 157</A>
   */
  protected boolean handleExportCheckModified() {
    if (!sketch.isModified()) return true;

    Object[] options = { _("OK"), _("Cancel") };
    int result = JOptionPane.showOptionDialog(this,
                                              _("Save changes before export?"),
                                              _("Save"),
                                              JOptionPane.OK_CANCEL_OPTION,
                                              JOptionPane.QUESTION_MESSAGE,
                                              null,
                                              options,
                                              options[0]);

    if (result == JOptionPane.OK_OPTION) {
      handleSave(true);

    } else {
      // why it's not CANCEL_OPTION is beyond me (at least on the mac)
      // but f-- it.. let's get this shite done..
      //} else if (result == JOptionPane.CANCEL_OPTION) {
      statusNotice(_("Export canceled, changes must first be saved."));
      //toolbar.clear();
      return false;
    }
    return true;
  }


  public void handleSerial() {
    if (uploading) return;

    if (serialMonitor != null) {
      try {
        serialMonitor.close();
        serialMonitor.setVisible(false);
      } catch (Exception e) {
        // noop
      }
    }

    BoardPort port = Base.getDiscoveryManager().find(Preferences.get("serial.port"));

    if (port == null) {
      statusError(I18n.format("Board at {0} is not available", Preferences.get("serial.port")));
      return;
    }

    serialMonitor = new UploaderAndMonitorFactory().newMonitor(port, base);
    serialMonitor.setIconImage(getIconImage());

    boolean success = false;
    do {
      if (serialMonitor.requiresAuthorization() && !Preferences.has(serialMonitor.getAuthorizationKey())) {
        PasswordAuthorizationDialog dialog = new PasswordAuthorizationDialog(this, _("Type board password to access its console"));
        dialog.setLocationRelativeTo(this);
        dialog.setVisible(true);

        if (dialog.isCancelled()) {
          statusNotice(_("Unable to open serial monitor"));
          return;
        }

        Preferences.set(serialMonitor.getAuthorizationKey(), dialog.getPassword());
      }

      try {
        serialMonitor.open();
        serialMonitor.setVisible(true);
        success = true;
      } catch (ConnectException e) {
        statusError(_("Unable to connect: is the sketch using the bridge?"));
      } catch (JSchException e) {
        statusError(_("Unable to connect: wrong password?"));
      } catch (Exception e) {
        statusError(e);
      } finally {
        if (serialMonitor.requiresAuthorization() && !success) {
          Preferences.remove(serialMonitor.getAuthorizationKey());
        }
      }

    } while (serialMonitor.requiresAuthorization() && !success);

  }


  protected void handleBurnBootloader() {
    console.clear();
    statusNotice(_("Burning bootloader to I/O Board (this may take a minute)..."));
    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        try {
          Uploader uploader = new SerialUploader();
          if (uploader.burnBootloader()) {
            statusNotice(_("Done burning bootloader."));
          } else {
            statusError(_("Error while burning bootloader."));
            // error message will already be visible
          }
        } catch (PreferencesMapException e) {
          statusError(I18n.format(
                      _("Error while burning bootloader: missing '{0}' configuration parameter"),
                      e.getMessage()));
          //statusError(e);
        } catch (Exception e) {
          statusError(_("Error while burning bootloader."));
          e.printStackTrace();
        }
      }
    });
  }


  /**
   * Handler for File &rarr; Page Setup.
   */
  public void handlePageSetup() {
    //printerJob = null;
    if (printerJob == null) {
      printerJob = PrinterJob.getPrinterJob();
    }
    if (pageFormat == null) {
      pageFormat = printerJob.defaultPage();
    }
    pageFormat = printerJob.pageDialog(pageFormat);
    //System.out.println("page format is " + pageFormat);
  }


  /**
   * Handler for File &rarr; Print.
   */
  public void handlePrint() {
    statusNotice(_("Printing..."));
    //printerJob = null;
    if (printerJob == null) {
      printerJob = PrinterJob.getPrinterJob();
    }
    if (pageFormat != null) {
      //System.out.println("setting page format " + pageFormat);
      printerJob.setPrintable(textarea.getPainter(), pageFormat);
    } else {
      printerJob.setPrintable(textarea.getPainter());
    }
    // set the name of the job to the code name
    printerJob.setJobName(sketch.getCurrentCode().getPrettyName());

    if (printerJob.printDialog()) {
      try {
        printerJob.print();
        statusNotice(_("Done printing."));

      } catch (PrinterException pe) {
        statusError(_("Error while printing."));
        pe.printStackTrace();
      }
    } else {
      statusNotice(_("Printing canceled."));
    }
    //printerJob = null;  // clear this out?
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Show an error int the status bar.
   */
  public void statusError(String what) {
    status.error(what);
    //new Exception("deactivating RUN").printStackTrace();
    toolbar.deactivate(EditorToolbar.RUN);
  }


  /**
   * Show an exception in the editor status bar.
   */
  public void statusError(Exception e) {
    e.printStackTrace();
//    if (e == null) {
//      System.err.println("Editor.statusError() was passed a null exception.");
//      return;
//    }

    if (e instanceof RunnerException) {
      RunnerException re = (RunnerException) e;
      if (re.hasCodeIndex()) {
        sketch.setCurrentCode(re.getCodeIndex());
      }
      if (re.hasCodeLine()) {
        int line = re.getCodeLine();
        // subtract one from the end so that the \n ain't included
        if (line >= textarea.getLineCount()) {
          // The error is at the end of this current chunk of code,
          // so the last line needs to be selected.
          line = textarea.getLineCount() - 1;
          if (textarea.getLineText(line).length() == 0) {
            // The last line may be zero length, meaning nothing to select.
            // If so, back up one more line.
            line--;
          }
        }
        if (line < 0 || line >= textarea.getLineCount()) {
          System.err.println(I18n.format(_("Bad error line: {0}"), line));
        } else {
          textarea.select(textarea.getLineStartOffset(line),
                          textarea.getLineStopOffset(line) - 1);
        }
      }
    }

    // Since this will catch all Exception types, spend some time figuring
    // out which kind and try to give a better error message to the user.
    String mess = e.getMessage();
    if (mess != null) {
      String javaLang = "java.lang.";
      if (mess.indexOf(javaLang) == 0) {
        mess = mess.substring(javaLang.length());
      }
      String rxString = "RuntimeException: ";
      if (mess.indexOf(rxString) == 0) {
        mess = mess.substring(rxString.length());
      }
      statusError(mess);
    }
//    e.printStackTrace();
  }


  /**
   * Show a notice message in the editor status bar.
   */
  public void statusNotice(String msg) {
    status.notice(msg);
  }


  /**
   * Clear the status area.
   */
  public void statusEmpty() {
    statusNotice(EMPTY);
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .

  protected void onBoardOrPortChange() {
    Map<String, String> boardPreferences = Base.getBoardPreferences();
    lineStatus.setBoardName(boardPreferences.get("name"));
    lineStatus.setSerialPort(Preferences.get("serial.port"));
    lineStatus.repaint();
  }


  /**
   * Returns the edit popup menu.
   */
  class TextAreaPopup extends JPopupMenu {
    //private String currentDir = System.getProperty("user.dir");
    private String referenceFile = null;

    private JMenuItem cutItem;
    private JMenuItem copyItem;
    private JMenuItem discourseItem;
    private JMenuItem referenceItem;
    private JMenuItem openURLItem;
    private JSeparator openURLItemSeparator;

    private String clickedURL;

    public TextAreaPopup() {
      openURLItem = new JMenuItem(_("Open URL"));
      openURLItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL(clickedURL);
        }
      });
      add(openURLItem);

      openURLItemSeparator = new JSeparator();
      add(openURLItemSeparator);

      cutItem = new JMenuItem(_("Cut"));
      cutItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleCut();
          }
      });
      add(cutItem);

      copyItem = new JMenuItem(_("Copy"));
      copyItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleCopy();
          }
        });
      add(copyItem);

      discourseItem = new JMenuItem(_("Copy for Forum"));
      discourseItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleDiscourseCopy();
          }
        });
      add(discourseItem);

      discourseItem = new JMenuItem(_("Copy as HTML"));
      discourseItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleHTMLCopy();
          }
        });
      add(discourseItem);

      JMenuItem item = new JMenuItem(_("Paste"));
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handlePaste();
          }
        });
      add(item);

      item = new JMenuItem(_("Select All"));
      item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleSelectAll();
        }
      });
      add(item);

      addSeparator();

      item = new JMenuItem(_("Comment/Uncomment"));
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleCommentUncomment();
          }
      });
      add(item);

      item = new JMenuItem(_("Increase Indent"));
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleIndentOutdent(true);
          }
      });
      add(item);

      item = new JMenuItem(_("Decrease Indent"));
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleIndentOutdent(false);
          }
      });
      add(item);

      addSeparator();

      referenceItem = new JMenuItem(_("Find in Reference"));
      referenceItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleFindReference();
          }
        });
      add(referenceItem);
    }

    // if no text is selected, disable copy and cut menu items
    public void show(Component component, int x, int y) {
      int lineNo = textarea.getLineOfOffset(textarea.xyToOffset(x, y));
      int offset = textarea.xToOffset(lineNo, x);
      String line = textarea.getLineText(lineNo);
      clickedURL = textarea.checkClickedURL(line, offset);
      if (clickedURL != null) {
        openURLItem.setVisible(true);
        openURLItemSeparator.setVisible(true);
      } else {
        openURLItem.setVisible(false);
        openURLItemSeparator.setVisible(false);
      }

      if (textarea.isSelectionActive()) {
        cutItem.setEnabled(true);
        copyItem.setEnabled(true);
        discourseItem.setEnabled(true);

      } else {
        cutItem.setEnabled(false);
        copyItem.setEnabled(false);
        discourseItem.setEnabled(false);
      }

      referenceFile = PdeKeywords.getReference(getCurrentKeyword());
      referenceItem.setEnabled(referenceFile != null);

      super.show(component, x, y);
    }
  }
}
