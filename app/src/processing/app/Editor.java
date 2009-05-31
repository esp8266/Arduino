/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-08 Ben Fry and Casey Reas
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

import processing.app.debug.*;
import processing.app.syntax.*;
import processing.app.tools.*;
import processing.core.*;

import java.awt.*;
import java.awt.datatransfer.*;
import java.awt.event.*;
import java.awt.print.*;
import java.io.*;
import java.net.*;
import java.util.*;
import java.util.zip.*;

import javax.swing.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.undo.*;


/**
 * Main editor panel for the Processing Development Environment.
 */
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

  // file and sketch menus for re-inserting items
  JMenu fileMenu;
  JMenu sketchMenu;

  EditorToolbar toolbar;
  // these menus are shared so that they needn't be rebuilt for all windows
  // each time a sketch is created, renamed, or moved.
  static JMenu toolbarMenu;
  static JMenu sketchbookMenu;
  static JMenu examplesMenu;
  static JMenu importMenu;

  EditorHeader header;
  EditorStatus status;
  EditorConsole console;

  JSplitPane splitPane;
  JPanel consolePanel;

  JLabel lineNumberComponent;

  // currently opened program
  Sketch sketch;

  EditorLineStatus lineStatus;

  JEditTextArea textarea;
  EditorListener listener;

  // runtime information and window placement
  Point sketchWindowLocation;
  Runner runtime;

  JMenuItem exportAppItem;
  JMenuItem saveMenuItem;
  JMenuItem saveAsMenuItem;

  boolean running;
  boolean presenting;

  // undo fellers
  JMenuItem undoItem, redoItem;
  protected UndoAction undoAction;
  protected RedoAction redoAction;
  UndoManager undo;
  // used internally, and only briefly
  CompoundEdit compoundEdit;

  FindReplace find;


  public Editor(Base ibase, String path, int[] location) {
    super("Processing");
    this.base = ibase;

    Base.setIcon(this);

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
          base.handleActivated(Editor.this);

          // re-add the sub-menus that are shared by all windows
          fileMenu.insert(sketchbookMenu, 2);
          fileMenu.insert(examplesMenu, 3);
          sketchMenu.insert(importMenu, 4);
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
    textarea.setRightClickPopup(new TextAreaPopup());
    textarea.setHorizontalOffset(6);

    // assemble console panel, consisting of status area and the console itself
    consolePanel = new JPanel();
    consolePanel.setLayout(new BorderLayout());

    status = new EditorStatus(this);
    consolePanel.add(status, BorderLayout.NORTH);

    console = new EditorConsole(this);
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

    splitPane.setMinimumSize(new Dimension(600, 400));
    box.add(splitPane);

    // hopefully these are no longer needed w/ swing
    // (har har har.. that was wishful thinking)
    listener = new EditorListener(this, textarea);
    pain.add(box);

    pain.setTransferHandler(new TransferHandler() {

        public boolean canImport(JComponent dest, DataFlavor[] flavors) {
          return true;
        }

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
              //System.out.println("uri list");
              String data = (String)transferable.getTransferData(uriListFlavor);
              String[] pieces = PApplet.splitTokens(data, "\r\n");
              //PApplet.println(pieces);
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
            statusError("No files were added to the sketch.");

          } else if (successful == 1) {
            statusNotice("One file added to the sketch.");

          } else {
            statusNotice(successful + " files added to the sketch.");
          }
          return true;
        }
      });

//    System.out.println("t1");

    // Finish preparing Editor (formerly found in Base)
    pack();

//    System.out.println("t2");

    // Set the window bounds and the divider location before setting it visible
    setPlacement(location);

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
  public Dimension getMinimumSize() {
    //System.out.println("getting minimum size");
    return new Dimension(500, 550);
  }


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


  protected void buildMenuBar() {
    JMenuBar menubar = new JMenuBar();
    menubar = new JMenuBar();
    menubar.add(buildFileMenu());
    menubar.add(buildEditMenu());
    menubar.add(buildSketchMenu());
    menubar.add(buildToolsMenu());
    menubar.add(buildHelpMenu());
    setJMenuBar(menubar);
  }


  protected JMenu buildFileMenu() {
    JMenuItem item;
    fileMenu = new JMenu("File");

    item = newJMenuItem("New", 'N');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          base.handleNew();
        }
      });
    fileMenu.add(item);

    item = Editor.newJMenuItem("Open...", 'O');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          base.handleOpenPrompt();
        }
      });
    fileMenu.add(item);

    if (sketchbookMenu == null) {
      sketchbookMenu = new JMenu("Sketchbook");
      base.rebuildSketchbookMenu(sketchbookMenu);
    }
    fileMenu.add(sketchbookMenu);

    if (examplesMenu == null) {
      examplesMenu = new JMenu("Examples");
      base.rebuildExamplesMenu(examplesMenu);
    }
    fileMenu.add(examplesMenu);

    item = Editor.newJMenuItem("Close", 'W');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          base.handleClose(Editor.this);
        }
      });
    fileMenu.add(item);

    saveMenuItem = newJMenuItem("Save", 'S');
    saveMenuItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleSave(false);
        }
      });
    fileMenu.add(saveMenuItem);

    saveAsMenuItem = newJMenuItemShift("Save As...", 'S');
    saveAsMenuItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleSaveAs();
        }
      });
    fileMenu.add(saveAsMenuItem);

    item = newJMenuItem("Export", 'E');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleExport();
        }
      });
    fileMenu.add(item);

    exportAppItem = newJMenuItemShift("Export Application", 'E');
    exportAppItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //buttons.activate(EditorButtons.EXPORT);
          //SwingUtilities.invokeLater(new Runnable() {
          //public void run() {
          handleExportApplication();
          //}});
        }
      });
    fileMenu.add(exportAppItem);

    fileMenu.addSeparator();

    item = newJMenuItemShift("Page Setup", 'P');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handlePageSetup();
        }
      });
    fileMenu.add(item);

    item = newJMenuItem("Print", 'P');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handlePrint();
        }
      });
    fileMenu.add(item);

    // macosx already has its own preferences and quit menu
    if (!Base.isMacOS()) {
      fileMenu.addSeparator();

      item = newJMenuItem("Preferences", ',');
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            base.handlePrefs();
          }
        });
      fileMenu.add(item);

      fileMenu.addSeparator();

      item = newJMenuItem("Quit", 'Q');
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
    sketchMenu = new JMenu("Sketch");

    item = newJMenuItem("Run", 'R');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleRun(false);
        }
      });
    sketchMenu.add(item);

    item = newJMenuItemShift("Present", 'R');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleRun(true);
        }
      });
    sketchMenu.add(item);

    item = new JMenuItem("Stop");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleStop();
        }
      });
    sketchMenu.add(item);

    sketchMenu.addSeparator();

    if (importMenu == null) {
      importMenu = new JMenu("Import Library...");
      base.rebuildImportMenu(importMenu);
    }
    sketchMenu.add(importMenu);

    item = newJMenuItem("Show Sketch Folder", 'K');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openFolder(sketch.getFolder());
        }
      });
    sketchMenu.add(item);
    item.setEnabled(Base.openFolderAvailable());

    item = new JMenuItem("Add File...");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          sketch.handleAddFile();
        }
      });
    sketchMenu.add(item);

    return sketchMenu;
  }


  protected JMenu buildToolsMenu() {
    JMenu menu = new JMenu("Tools");

    addInternalTools(menu);
    addTools(menu, Base.getToolsFolder());
    File sketchbookTools = new File(Base.getSketchbookFolder(), "tools");
    addTools(menu, sketchbookTools);

    return menu;
  }


  protected void addTools(JMenu menu, File sourceFolder) {
    HashMap toolItems = new HashMap();

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
          urlList[j] = archives[j].toURL();
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

        Class toolClass = Class.forName(className, true, loader);
        final Tool tool = (Tool) toolClass.newInstance();

        tool.init(Editor.this);

        String title = tool.getMenuTitle();
        JMenuItem item = new JMenuItem(title);
        item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            SwingUtilities.invokeLater(tool);
          }
        });
        //menu.add(item);
        toolItems.put(title, item);

      } catch (Exception e) {
        e.printStackTrace();
      }
    }
    ArrayList<String> toolList = new ArrayList(toolItems.keySet());
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
      Enumeration entries = zipFile.entries();
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
      Class toolClass = Class.forName(className);
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

    item = createToolMenuItem("processing.app.tools.AutoFormat");
    int modifiers = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
    item.setAccelerator(KeyStroke.getKeyStroke('T', modifiers));
    menu.add(item);

    menu.add(createToolMenuItem("processing.app.tools.CreateFont"));
    menu.add(createToolMenuItem("processing.app.tools.ColorSelector"));
    menu.add(createToolMenuItem("processing.app.tools.Archiver"));
    menu.add(createToolMenuItem("processing.app.tools.FixEncoding"));

    return menu;
  }


  protected JMenu buildHelpMenu() {
    JMenu menu = new JMenu("Help");
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

    item = new JMenuItem("Getting Started");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL("http://processing.org/learning/gettingstarted/");
        }
      });
    menu.add(item);

    item = new JMenuItem("Environment");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.showEnvironment();
        }
      });
    menu.add(item);

    item = new JMenuItem("Troubleshooting");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.showTroubleshooting();
        }
      });
    menu.add(item);

    item = new JMenuItem("Reference");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.showReference();
        }
      });
    menu.add(item);

    item = newJMenuItemShift("Find in Reference", 'F');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (textarea.isSelectionActive()) {
            handleFindReference();
          }
        }
      });
    menu.add(item);

    item = new JMenuItem("Frequently Asked Questions");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL("http://processing.org/faq.html");
        }
      });
    menu.add(item);

    item = newJMenuItem("Visit Processing.org", '5');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL("http://processing.org/");
        }
      });
    menu.add(item);

    // macosx already has its own about menu
    if (!Base.isMacOS()) {
      menu.addSeparator();
      item = new JMenuItem("About Processing");
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
    JMenu menu = new JMenu("Edit");
    JMenuItem item;

    undoItem = newJMenuItem("Undo", 'Z');
    undoItem.addActionListener(undoAction = new UndoAction());
    menu.add(undoItem);

    redoItem = newJMenuItem("Redo", 'Y');
    redoItem.addActionListener(redoAction = new RedoAction());
    menu.add(redoItem);

    menu.addSeparator();

    // TODO "cut" and "copy" should really only be enabled
    // if some text is currently selected
    item = newJMenuItem("Cut", 'X');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleCut();
        }
      });
    menu.add(item);

    item = newJMenuItem("Copy", 'C');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          textarea.copy();
        }
      });
    menu.add(item);

    item = newJMenuItemShift("Copy for Discourse", 'C');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
//          SwingUtilities.invokeLater(new Runnable() {
//              public void run() {
          new DiscourseFormat(Editor.this).show();
//              }
//            });
        }
      });
    menu.add(item);

    item = newJMenuItem("Paste", 'V');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          textarea.paste();
          sketch.setModified(true);
        }
      });
    menu.add(item);

    item = newJMenuItem("Select All", 'A');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          textarea.selectAll();
        }
      });
    menu.add(item);

    menu.addSeparator();

    item = newJMenuItem("Comment/Uncomment", '/');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleCommentUncomment();
        }
    });
    menu.add(item);

    item = newJMenuItem("Increase Indent", ']');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleIndentOutdent(true);
        }
    });
    menu.add(item);

    item = newJMenuItem("Decrease Indent", '[');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleIndentOutdent(false);
        }
    });
    menu.add(item);

    menu.addSeparator();

    item = newJMenuItem("Find...", 'F');
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
    item = newJMenuItem("Find Next", 'G');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (find != null) {
            //find.find(true);
            //FindReplace find = new FindReplace(Editor.this); //.show();
            find.find(true);
          }
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
    int modifiers = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
    menuItem.setAccelerator(KeyStroke.getKeyStroke(what, modifiers));
    return menuItem;
  }


  /**
   * Like newJMenuItem() but adds shift as a modifier for the key command.
   */
  static public JMenuItem newJMenuItemShift(String title, int what) {
    JMenuItem menuItem = new JMenuItem(title);
    int modifiers = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
    modifiers |= ActionEvent.SHIFT_MASK;
    menuItem.setAccelerator(KeyStroke.getKeyStroke(what, modifiers));
    return menuItem;
  }


  /**
   * Same as newJMenuItem(), but adds the ALT (on Linux and Windows)
   * or OPTION (on Mac OS X) key as a modifier.
   */
  static public JMenuItem newJMenuItemAlt(String title, int what) {
    JMenuItem menuItem = new JMenuItem(title);
    //int modifiers = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
    //menuItem.setAccelerator(KeyStroke.getKeyStroke(what, modifiers));
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
        undoItem.setText("Undo");
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
        redoItem.setText("Redo");
        putValue(Action.NAME, "Redo");
      }
    }
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
              undo.addEdit(e.getEdit());
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
    new DiscourseFormat(Editor.this).show();
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


  protected void handleFindReference() {
    String text = textarea.getSelectedText().trim();

    if (text.length() == 0) {
      statusNotice("First select a word to find in the reference.");

    } else {
      String referenceFile = PdeKeywords.getReference(text);
      //System.out.println("reference file is " + referenceFile);
      if (referenceFile == null) {
        statusNotice("No reference available for \"" + text + "\"");
      } else {
        Base.showReference(referenceFile + ".html");
      }
    }
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  /**
   * Implements Sketch &rarr; Run.
   * @param present Set true to run in full screen (present mode).
   */
  public void handleRun(boolean present) {
    internalCloseRunner();
    running = true;
    toolbar.activate(EditorToolbar.RUN);
    statusEmpty();

    // do this to advance/clear the terminal window / dos prompt / etc
    for (int i = 0; i < 10; i++) System.out.println();

    // clear the console on each run, unless the user doesn't want to
    if (Preferences.getBoolean("console.auto_clear")) {
      console.clear();
    }

    presenting = present;

    try {
      String appletClassName = sketch.compile();
      if (appletClassName != null) {
        runtime = new Runner(sketch, appletClassName, presenting, Editor.this);

        // Cannot use invokeLater() here, otherwise it gets
        // placed on the event thread and causes a hang--bad idea all around.
        Thread t = new Thread(new Runnable() {
          public void run() {
            runtime.launch();
          }
        });
        t.start();
        //runtime.start(appletLocation);
      }

    } catch (Exception e) {
      //System.err.println("exception reached editor");
      //e.printStackTrace();
      statusError(e);
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
    toolbar.activate(EditorToolbar.STOP);

    internalCloseRunner();

    toolbar.deactivate(EditorToolbar.RUN);
    toolbar.deactivate(EditorToolbar.STOP);

    // focus the PDE again after quitting presentation mode [toxi 030903]
    toFront();
  }


  /**
   * Called by Runner to notify that the sketch has stopped running.
   * Tools should not call this function, use handleStop() instead.
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

    try {
      if (runtime != null) {
        runtime.close();  // kills the window
        runtime = null; // will this help?
      }
    } catch (Exception e) { }

    sketch.cleanup();
  }


  /**
   * Check if the sketch is modified and ask user to save changes.
   * Immediately should be set true when quitting, or when the save should
   * not happen asynchronously. Come to think of it, that's always now?
   * @return false if canceling the close/quit operation
   */
  protected boolean checkModified(boolean immediately) {
    if (!sketch.isModified()) return true;

    String prompt = "Save changes to " + sketch.getName() + "?  ";

    if (!Base.isMacOS()) {
      int result =
        JOptionPane.showConfirmDialog(this, prompt, "Close",
                                      JOptionPane.YES_NO_CANCEL_OPTION,
                                      JOptionPane.QUESTION_MESSAGE);

      if (result == JOptionPane.YES_OPTION) {
        return handleSave(immediately);

      } else if (result == JOptionPane.NO_OPTION) {
        return true;  // ok to continue

      } else if (result == JOptionPane.CANCEL_OPTION) {
        return false;
      } else {
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
        new JOptionPane("<html> " +
                        "<head> <style type=\"text/css\">"+
                        "b { font: 13pt \"Lucida Grande\" }"+
                        "p { font: 11pt \"Lucida Grande\"; margin-top: 8px }"+
                        "</style> </head>" +
                        "<b>Do you want to save changes to this sketch<BR>" +
                        " before closing?</b>" +
                        "<p>If you don't save, your changes will be lost.",
                        JOptionPane.QUESTION_MESSAGE);

      String[] options = new String[] {
        "Save", "Cancel", "Don't Save"
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
        return handleSave(immediately);

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
    File parentFile = new File(file.getParent());
    String parentName = parentFile.getName();
    String pdeName = parentName + ".pde";
    File altFile = new File(file.getParent(), pdeName);

    if (pdeName.equals(file.getName())) {
      // no beef with this guy

    } else if (altFile.exists()) {
      // user selected a .java from the same sketch,
      // but open the .pde instead
      path = altFile.getAbsolutePath();
      //System.out.println("found alt file in same folder");

    } else if (!path.endsWith(".pde")) {
      Base.showWarning("Bad file selected",
                       "Processing can only open its own sketches\n" +
                       "and other files ending in .pde", null);
      return false;

    } else {
      String properParent =
        file.getName().substring(0, file.getName().length() - 4);

      Object[] options = { "OK", "Cancel" };
      String prompt =
        "The file \"" + file.getName() + "\" needs to be inside\n" +
        "a sketch folder named \"" + properParent + "\".\n" +
        "Create this folder, move the file, and continue?";

      int result = JOptionPane.showOptionDialog(this,
                                                prompt,
                                                "Moving",
                                                JOptionPane.YES_NO_OPTION,
                                                JOptionPane.QUESTION_MESSAGE,
                                                null,
                                                options,
                                                options[0]);

      if (result == JOptionPane.YES_OPTION) {
        // create properly named folder
        File properFolder = new File(file.getParent(), properParent);
        if (properFolder.exists()) {
          Base.showWarning("Error",
                           "A folder named \"" + properParent + "\" " +
                           "already exists. Can't open sketch.", null);
          return false;
        }
        if (!properFolder.mkdirs()) {
          //throw new IOException("Couldn't create sketch folder");
          Base.showWarning("Error",
                           "Could not create the sketch folder.", null);
          return false;
        }
        // copy the sketch inside
        File properPdeFile = new File(properFolder, file.getName());
        File origPdeFile = new File(path);
        try {
          Base.copyFile(origPdeFile, properPdeFile);
        } catch (IOException e) {
          Base.showWarning("Error", "Could not copy to a proper location.", e);
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
      Base.showWarning("Error", "Could not create the sketch.", e);
      return false;
    }
    header.rebuild();
    // Set the title of the window to "sketch_070752a - Processing 0126"
    setTitle(sketch.getName() + " | Processing " + Base.VERSION_NAME);
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
      handleSave2();

    } else {
      SwingUtilities.invokeLater(new Runnable() {
          public void run() {
            handleSave2();
          }
        });
    }
    return true;
  }


  protected void handleSave2() {
    toolbar.activate(EditorToolbar.SAVE);
    statusNotice("Saving...");
    try {
      if (sketch.save()) {
        statusNotice("Done Saving.");
      } else {
        statusEmpty();
      }
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
  }


  public boolean handleSaveAs() {
    //stopRunner();  // formerly from 0135
    handleStop();

    toolbar.activate(EditorToolbar.SAVE);

    //SwingUtilities.invokeLater(new Runnable() {
    //public void run() {
    statusNotice("Saving...");
    try {
      if (sketch.saveAs()) {
        statusNotice("Done Saving.");
        // Disabling this for 0125, instead rebuild the menu inside
        // the Save As method of the Sketch object, since that's the
        // only one who knows whether something was renamed.
        //sketchbook.rebuildMenusAsync();
      } else {
        statusNotice("Save Canceled.");
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


  /**
   * Called by Sketch &rarr; Export.
   * Handles calling the export() function on sketch, and
   * queues all the gui status stuff that comes along with it.
   * <p/>
   * Made synchronized to (hopefully) avoid problems of people
   * hitting export twice, quickly, and horking things up.
   */
  synchronized public void handleExport() {
    if (!handleExportCheckModified()) return;
    toolbar.activate(EditorToolbar.EXPORT);

    //SwingUtilities.invokeLater(new Runnable() {
    Thread t = new Thread(new Runnable() {
        public void run() {
          try {
            boolean success = sketch.exportApplet();
            if (success) {
              File appletFolder = new File(sketch.getFolder(), "applet");
              Base.openFolder(appletFolder);
              statusNotice("Done exporting.");
            } else {
              // error message will already be visible
            }
          } catch (Exception e) {
            statusError(e);
          }
          //toolbar.clear();
          toolbar.deactivate(EditorToolbar.EXPORT);
        }});
    t.start();
  }


  /**
   * Handler for Sketch &rarr; Export Application
   */
  synchronized public void handleExportApplication() {
    if (!handleExportCheckModified()) return;
    toolbar.activate(EditorToolbar.EXPORT);

    //SwingUtilities.invokeLater(new Runnable() {
    SwingUtilities.invokeLater(new Runnable() {
        public void run() {
          statusNotice("Exporting application...");
          try {
            if (sketch.exportApplicationPrompt()) {
              Base.openFolder(sketch.getFolder());
              statusNotice("Done exporting.");
            } else {
              // error message will already be visible
              // or there was no error, in which case it was canceled.
            }
          } catch (Exception e) {
            statusNotice("Error during export.");
            e.printStackTrace();
          }
          //toolbar.clear();
          toolbar.deactivate(EditorToolbar.EXPORT);
        }});
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

    Object[] options = { "OK", "Cancel" };
    int result = JOptionPane.showOptionDialog(this,
                                              "Save changes before export?",
                                              "Save",
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
      statusNotice("Export canceled, changes must first be saved.");
      //toolbar.clear();
      return false;
    }
    return true;
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
    statusNotice("Printing...");
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
        statusNotice("Done printing.");

      } catch (PrinterException pe) {
        statusError("Error while printing.");
        pe.printStackTrace();
      }
    } else {
      statusNotice("Printing canceled.");
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
          System.err.println("Bad error line: " + line);
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
    e.printStackTrace();
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


  /**
   * Returns the edit popup menu.
   */
  class TextAreaPopup extends JPopupMenu {
    //String currentDir = System.getProperty("user.dir");
    String referenceFile = null;

    JMenuItem cutItem;
    JMenuItem copyItem;
    JMenuItem discourseItem;
    JMenuItem referenceItem;


    public TextAreaPopup() {
      JMenuItem item;

      cutItem = new JMenuItem("Cut");
      cutItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleCut();
          }
      });
      this.add(cutItem);

      copyItem = new JMenuItem("Copy");
      copyItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleCopy();
          }
        });
      this.add(copyItem);

      discourseItem = new JMenuItem("Copy for Discourse");
      discourseItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleDiscourseCopy();
          }
        });
      this.add(discourseItem);

      item = new JMenuItem("Paste");
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handlePaste();
          }
        });
      this.add(item);

      item = new JMenuItem("Select All");
      item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleSelectAll();
        }
      });
      this.add(item);

      this.addSeparator();

      item = new JMenuItem("Comment/Uncomment");
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleCommentUncomment();
          }
      });
      this.add(item);

      item = new JMenuItem("Increase Indent");
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleIndentOutdent(true);
          }
      });
      this.add(item);

      item = new JMenuItem("Decrease Indent");
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleIndentOutdent(false);
          }
      });
      this.add(item);

      this.addSeparator();

      referenceItem = new JMenuItem("Find in Reference");
      referenceItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleFindReference();
          }
        });
      this.add(referenceItem);
    }

    // if no text is selected, disable copy and cut menu items
    public void show(Component component, int x, int y) {
      if (textarea.isSelectionActive()) {
        cutItem.setEnabled(true);
        copyItem.setEnabled(true);
        discourseItem.setEnabled(true);

        String sel = textarea.getSelectedText().trim();
        referenceFile = PdeKeywords.getReference(sel);
        referenceItem.setEnabled(referenceFile != null);

      } else {
        cutItem.setEnabled(false);
        copyItem.setEnabled(false);
        discourseItem.setEnabled(false);
        referenceItem.setEnabled(false);
      }
      super.show(component, x, y);
    }
  }
}

