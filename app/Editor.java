/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Editor - main editor panel for the processing development environment
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

import processing.app.syntax.*;
import processing.app.tools.*;

import java.awt.*;
import java.awt.datatransfer.*;
import java.awt.dnd.*;
import java.awt.event.*;
import java.io.*;
import java.lang.reflect.*;
import java.net.*;
import java.util.*;
import java.util.zip.*;

import javax.swing.*;
import javax.swing.border.*;
import javax.swing.event.*;
import javax.swing.text.*;
import javax.swing.undo.*;

import com.oroinc.text.regex.*;

import com.apple.mrj.*;
import gnu.io.*;

public class Editor extends JFrame
  implements MRJAboutHandler, MRJQuitHandler, MRJPrefsHandler,
             MRJOpenDocumentHandler //, MRJOpenApplicationHandler
{
  // yeah
  static final String WINDOW_TITLE = "Arduino" + " - " + Base.VERSION_NAME;

  // p5 icon for the window
  Image icon;

  // otherwise, if the window is resized with the message label
  // set to blank, it's preferredSize() will be fukered
  static public final String EMPTY =
    "                                                                     " +
    "                                                                     " +
    "                                                                     ";

  static public final KeyStroke WINDOW_CLOSE_KEYSTROKE =
    KeyStroke.getKeyStroke('W', Toolkit.getDefaultToolkit().getMenuShortcutKeyMask());

  static final int HANDLE_NEW  = 1;
  static final int HANDLE_OPEN = 2;
  static final int HANDLE_QUIT = 3;
  int checkModifiedMode;
  String handleOpenPath;
  boolean handleNewShift;
  boolean handleNewLibrary;

  EditorButtons buttons;
  EditorHeader header;
  EditorStatus status;
  EditorConsole console;
  Serial serialPort;
  JSplitPane splitPane;
  JPanel consolePanel;

  JLabel lineNumberComponent;

  // currently opened program
  public Sketch sketch;

  EditorLineStatus lineStatus;

  public JEditTextArea textarea;
  EditorListener listener;

  // runtime information and window placement
  Point appletLocation;
  //Point presentLocation;
  //Window presentationWindow;
  RunButtonWatcher watcher;
  //Runner runtime;


  JMenuItem burnBootloaderItem = null;
  JMenuItem burnBootloaderParallelItem = null;
  JMenuItem exportAppItem;
  JMenuItem saveMenuItem;
  JMenuItem saveAsMenuItem;
  JMenu serialMenu;
  JMenu serialRateMenu;
  JMenu mcuMenu;
  SerialMenuListener serialMenuListener;

  boolean running;
  boolean presenting;
  boolean debugging;

  // undo fellers
  JMenuItem undoItem, redoItem;
  protected UndoAction undoAction;
  protected RedoAction redoAction;
  UndoManager undo;
  // used internally, and only briefly
  CompoundEdit compoundEdit;

  //static public UndoManager undo = new UndoManager(); // editor needs this guy

  //

  //SketchHistory history;  // TODO re-enable history
  Sketchbook sketchbook;
  //Preferences preferences;
  //FindReplace find;

  //static Properties keywords; // keyword -> reference html lookup


  public Editor() {
    super(WINDOW_TITLE);

    // #@$*(@#$ apple.. always gotta think different
    MRJApplicationUtils.registerAboutHandler(this);
    MRJApplicationUtils.registerPrefsHandler(this);
    MRJApplicationUtils.registerQuitHandler(this);
    MRJApplicationUtils.registerOpenDocumentHandler(this);

    // run static initialization that grabs all the prefs
    Preferences.init();

    // set the window icon
    try {
      icon = Base.getImage("icon.gif", this);
      setIconImage(icon);
    } catch (Exception e) { } // fail silently, no big whup


    // add listener to handle window close box hit event
    addWindowListener(new WindowAdapter() {
        public void windowClosing(WindowEvent e) {
          handleQuit();
        }
      });

    PdeKeywords keywords = new PdeKeywords();
    sketchbook = new Sketchbook(this);

    JMenuBar menubar = new JMenuBar();
    menubar.add(buildFileMenu());
    menubar.add(buildEditMenu());
    menubar.add(buildSketchMenu());
    menubar.add(buildToolsMenu());
    // what platform has their help menu way on the right? motif?
    //menubar.add(Box.createHorizontalGlue());
    menubar.add(buildHelpMenu());

    setJMenuBar(menubar);

    // doesn't matter when this is created, just make it happen at some point
    //find = new FindReplace(Editor.this);

    Container pain = getContentPane();
    pain.setLayout(new BorderLayout());

    Box box = Box.createVerticalBox();
    Box upper = Box.createVerticalBox();

    buttons = new EditorButtons(this);
    upper.add(buttons);

    header = new EditorHeader(this);
    //header.setBorder(null);
    upper.add(header);

    textarea = new JEditTextArea(new PdeTextAreaDefaults());
    textarea.setRightClickPopup(new TextAreaPopup());
    //textarea.setTokenMarker(new PdeKeywords());
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
                               //textarea, consolePanel);

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

    splitPane.setMinimumSize(new Dimension(600, 600));
    box.add(splitPane);

    // hopefully these are no longer needed w/ swing
    // (har har har.. that was wishful thinking)
    listener = new EditorListener(this, textarea);
    pain.add(box);

    DropTarget dt = new DropTarget(this, new DropTargetListener() {

        public void dragEnter(DropTargetDragEvent event) {
          // debug messages for diagnostics
          //System.out.println("dragEnter " + event);
          event.acceptDrag(DnDConstants.ACTION_COPY);
        }

        public void dragExit(DropTargetEvent event) {
          //System.out.println("dragExit " + event);
        }

        public void dragOver(DropTargetDragEvent event) {
          //System.out.println("dragOver " + event);
          event.acceptDrag(DnDConstants.ACTION_COPY);
        }

        public void dropActionChanged(DropTargetDragEvent event) {
          //System.out.println("dropActionChanged " + event);
        }

        public void drop(DropTargetDropEvent event) {
          //System.out.println("drop " + event);
          event.acceptDrop(DnDConstants.ACTION_COPY);

          Transferable transferable = event.getTransferable();
          DataFlavor flavors[] = transferable.getTransferDataFlavors();
          int successful = 0;

          for (int i = 0; i < flavors.length; i++) {
            try {
              //System.out.println(flavors[i]);
              //System.out.println(transferable.getTransferData(flavors[i]));
              java.util.List list =
                (java.util.List) transferable.getTransferData(flavors[i]);
              for (int j = 0; j < list.size(); j++) {
                Object item = list.get(j);
                if (item instanceof File) {
                  File file = (File) item;

                  // see if this is a .pde file to be opened
                  String filename = file.getName();
                  if (filename.endsWith(".pde")) {
                    String name = filename.substring(0, filename.length() - 4);
                    File parent = file.getParentFile();
                    if (name.equals(parent.getName())) {
                      handleOpenFile(file);
                      return;
                    }
                  }

                  if (sketch.addFile(file)) {
                    successful++;
                  }
                }
              }

            } catch (Exception e) {
              e.printStackTrace();
            }
          }

          if (successful == 0) {
            error("No files were added to the sketch.");

          } else if (successful == 1) {
            message("One file added to the sketch.");

          } else {
            message(successful + " files added to the sketch.");
          }
        }
      });
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


  // ...................................................................

  /**
   * Builds any unbuilt buildable libraries
   * Adds syntax coloring from those libraries (if exists)
   * Rebuilds sketchbook menu with library examples (if they exist)
   */
  public void prepareLibraries() {
    // build any unbuilt libraries
    try {
      LibraryManager libraryManager = new LibraryManager();
      libraryManager.buildAllUnbuilt();
      // update syntax coloring table
      libraryManager.addSyntaxColoring(new PdeKeywords());
    } catch (RunnerException re) {
      message("Error compiling library ...");
      error(re);
    } catch (Exception ex) {
      ex.printStackTrace();
    }
    // update sketchbook menu, this adds examples of any built libs
    sketchbook.rebuildMenus();
  }

  // ...................................................................

  /**
   * Post-constructor setup for the editor area. Loads the last
   * sketch that was used (if any), and restores other Editor settings.
   * The complement to "storePreferences", this is called when the
   * application is first launched.
   */
  public void restorePreferences() {
    // figure out window placement

    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    boolean windowPositionValid = true;

    if (Preferences.get("last.screen.height") != null) {
      // if screen size has changed, the window coordinates no longer
      // make sense, so don't use them unless they're identical
      int screenW = Preferences.getInteger("last.screen.width");
      int screenH = Preferences.getInteger("last.screen.height");

      if ((screen.width != screenW) || (screen.height != screenH)) {
        windowPositionValid = false;
      }
      int windowX = Preferences.getInteger("last.window.x");
      int windowY = Preferences.getInteger("last.window.y");
      if ((windowX < 0) || (windowY < 0) ||
          (windowX > screenW) || (windowY > screenH)) {
        windowPositionValid = false;
      }

    } else {
      windowPositionValid = false;
    }

    if (!windowPositionValid) {
      //System.out.println("using default size");
      int windowH = Preferences.getInteger("default.window.height");
      int windowW = Preferences.getInteger("default.window.width");
      setBounds((screen.width - windowW) / 2,
                (screen.height - windowH) / 2,
                windowW, windowH);
      // this will be invalid as well, so grab the new value
      Preferences.setInteger("last.divider.location",
                             splitPane.getDividerLocation());
    } else {
      setBounds(Preferences.getInteger("last.window.x"),
                Preferences.getInteger("last.window.y"),
                Preferences.getInteger("last.window.width"),
                Preferences.getInteger("last.window.height"));
    }


    // last sketch that was in use, or used to launch the app

    if (Base.openedAtStartup != null) {
      handleOpen2(Base.openedAtStartup);

    } else {
      //String sketchName = Preferences.get("last.sketch.name");
      String sketchPath = Preferences.get("last.sketch.path");
      //Sketch sketchTemp = new Sketch(sketchPath);

      if ((sketchPath != null) && (new File(sketchPath)).exists()) {
        // don't check modified because nothing is open yet
        handleOpen2(sketchPath);

      } else {
        handleNew2(true);
      }
    }


    // location for the console/editor area divider

    int location = Preferences.getInteger("last.divider.location");
    splitPane.setDividerLocation(location);


    // read the preferences that are settable in the preferences window

    applyPreferences();
  }


  /**
   * Read and apply new values from the preferences, either because
   * the app is just starting up, or the user just finished messing
   * with things in the Preferences window.
   */
  public void applyPreferences() {

    // apply the setting for 'use external editor'
    boolean external = Preferences.getBoolean("editor.external");

    textarea.setEditable(!external);
    saveMenuItem.setEnabled(!external);
    saveAsMenuItem.setEnabled(!external);
    //beautifyMenuItem.setEnabled(!external);

    TextAreaPainter painter = textarea.getPainter();
    if (external) {
      // disable line highlight and turn off the caret when disabling
      Color color = Preferences.getColor("editor.external.bgcolor");
      painter.setBackground(color);
      painter.setLineHighlightEnabled(false);
      textarea.setCaretVisible(false);

    } else {
      Color color = Preferences.getColor("editor.bgcolor");
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
    sketchbook.rebuildMenus();
  }


  /**
   * Store preferences about the editor's current state.
   * Called when the application is quitting.
   */
  public void storePreferences() {
    //System.out.println("storing preferences");

    // window location information
    Rectangle bounds = getBounds();
    Preferences.setInteger("last.window.x", bounds.x);
    Preferences.setInteger("last.window.y", bounds.y);
    Preferences.setInteger("last.window.width", bounds.width);
    Preferences.setInteger("last.window.height", bounds.height);

    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    Preferences.setInteger("last.screen.width", screen.width);
    Preferences.setInteger("last.screen.height", screen.height);

    // last sketch that was in use
    //Preferences.set("last.sketch.name", sketchName);
    //Preferences.set("last.sketch.name", sketch.name);
    Preferences.set("last.sketch.path", sketch.getMainFilePath());

    // location for the console/editor area divider
    int location = splitPane.getDividerLocation();
    Preferences.setInteger("last.divider.location", location);
  }


  // ...................................................................


  protected JMenu buildFileMenu() {
    JMenuItem item;
    JMenu menu = new JMenu("File");

    item = newJMenuItem("New", 'N');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleNew(false);
        }
      });
    menu.add(item);
    menu.add(sketchbook.getOpenMenu());

    saveMenuItem = newJMenuItem("Save", 'S');
    saveMenuItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleSave(false);
        }
      });
    menu.add(saveMenuItem);

    saveAsMenuItem = newJMenuItem("Save As...", 'S', true);
    saveAsMenuItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleSaveAs();
        }
      });
    menu.add(saveAsMenuItem);

    item = newJMenuItem("Upload to I/O Board", 'U');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleExport();
        }
      });
    menu.add(item);

    /*exportAppItem = newJMenuItem("Export Application", 'E', true);
    exportAppItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //buttons.activate(EditorButtons.EXPORT);
          //SwingUtilities.invokeLater(new Runnable() {
          //public void run() {
          handleExportApplication();
          //}});
        }
      });
    menu.add(exportAppItem);
    */
    menu.addSeparator();

    item = newJMenuItem("Page Setup", 'P', true);
    item.setEnabled(false);
    menu.add(item);

    item = newJMenuItem("Print", 'P');
    item.setEnabled(false);
    menu.add(item);

    // macosx already has its own preferences and quit menu
    if (!Base.isMacOS()) {
      menu.addSeparator();

      item = newJMenuItem("Preferences", ',');
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handlePrefs();
          }
        });
      menu.add(item);

      menu.addSeparator();

      item = newJMenuItem("Quit", 'Q');
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleQuit();
          }
        });
      menu.add(item);
    }
    return menu;
  }


  protected JMenu buildSketchMenu() {
    JMenuItem item;
    JMenu menu = new JMenu("Sketch");

    item = newJMenuItem("Verify/Compile", 'R');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleRun(false);
        }
      });
    menu.add(item);

    /*item = newJMenuItem("Present", 'R', true);
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleRun(true);
        }
      });
    menu.add(item);
    */
    item = new JMenuItem("Stop");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleStop();
        }
      });
    menu.add(item);

    menu.addSeparator();

    item = new JMenuItem("Add File...");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          sketch.addFile();
        }
      });
    menu.add(item);

    menu.add(sketchbook.getImportMenu());

    if (Base.isWindows() || Base.isMacOS()) {
      // no way to do an 'open in file browser' on other platforms
      // since there isn't any sort of standard
      item = newJMenuItem("Show Sketch Folder", 'K', false);
      item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //Base.openFolder(sketchDir);
          Base.openFolder(sketch.folder);
        }
      });
      menu.add(item);
    }

    // TODO re-enable history
    //history.attachMenu(menu);
    return menu;
  }


  protected JMenu buildToolsMenu() {
    JMenuItem item;
    JMenuItem rbMenuItem;
    JMenuItem cbMenuItem;
    
    serialMenuListener  = new SerialMenuListener();

    JMenu menu = new JMenu("Tools");

    item = newJMenuItem("Auto Format", 'T', false);
    item.addActionListener(new ActionListener() {
        synchronized public void actionPerformed(ActionEvent e) {
          new AutoFormat(Editor.this).show();
          //handleBeautify();
        }
      });
    menu.add(item);

    /*item = new JMenuItem("Create Font...");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          //new CreateFont().show(sketch.dataFolder);
          new CreateFont(Editor.this).show();
        }
      });
    menu.add(item);
    */
    item = new JMenuItem("Archive Sketch");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          new Archiver(Editor.this).show();
          //Archiver archiver = new Archiver();
          //archiver.setup(Editor.this);
          //archiver.show();
        }
      });
    menu.add(item);

    item = new JMenuItem("Export Folder...");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          new ExportFolder(Editor.this).show();
        }
      });
    menu.add(item);
    menu.addSeparator();
    
    mcuMenu = new JMenu("Microcontroller (MCU)");
    String curr_mcu = Preferences.get("build.mcu");
    ButtonGroup mcuGroup = new ButtonGroup();
    McuMenuListener mml = new McuMenuListener();
    
    item = new JCheckBoxMenuItem("atmega8", "atmega8".equals(curr_mcu));
    item.addActionListener(mml);
    mcuGroup.add(item);
    mcuMenu.add(item);
	
    item = new JCheckBoxMenuItem("atmega168", "atmega168".equals(curr_mcu));
    item.addActionListener(mml);
    mcuGroup.add(item);
    mcuMenu.add(item);

    
    menu.add(mcuMenu);
	
    serialMenu = new JMenu("Serial Port");
    populateSerialMenu();
    menu.add(serialMenu);
	  
    menu.addSeparator();
    
    burnBootloaderItem = new JMenuItem("Burn Bootloader");
    burnBootloaderItem.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          handleBurnBootloader(false);
        }
      });
    menu.add(burnBootloaderItem);
    
    if (!Preferences.get("build.mcu").equals("atmega8"))
      burnBootloaderItem.setEnabled(false);
      
    if (!Base.isMacOS()) {
      burnBootloaderParallelItem =
        new JMenuItem("Burn Bootloader (parallel port)");
      burnBootloaderParallelItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleBurnBootloader(true);
          }
        });
      menu.add(burnBootloaderParallelItem);
    
      if (!Preferences.get("build.mcu").equals("atmega8"))
        burnBootloaderParallelItem.setEnabled(false);
    }
    
    menu.addMenuListener(new MenuListener() {
      public void menuCanceled(MenuEvent e) {}
      public void menuDeselected(MenuEvent e) {}
      public void menuSelected(MenuEvent e) {
        //System.out.println("Tools menu selected.");
        populateSerialMenu();
      }
    });


    return menu;
  }

  class SerialMenuListener implements ActionListener {
    //public SerialMenuListener() { }

    public void actionPerformed(ActionEvent e) {
      if(serialMenu == null) {
        System.out.println("serialMenu is null");
        return;
      }
      int count = serialMenu.getItemCount();
      for (int i = 0; i < count; i++) {
        ((JCheckBoxMenuItem)serialMenu.getItem(i)).setState(false);
      }
      JCheckBoxMenuItem item = (JCheckBoxMenuItem)e.getSource();
      item.setState(true);
      String name = item.getLabel();
      //System.out.println(item.getLabel());
      Preferences.set("serial.port", name);
      //System.out.println("set to " + get("serial.port"));
    }

    /*
    public void actionPerformed(ActionEvent e) {
      System.out.println(e.getSource());
      String name = e.getActionCommand();
      PdeBase.properties.put("serial.port", name);
      System.out.println("set to " + get("serial.port"));
      //editor.skOpen(path + File.separator + name, name);
      // need to push "serial.port" into PdeBase.properties
    }
    */
  }

  class McuMenuListener implements ActionListener {
    McuMenuListener() {}
  
    public void actionPerformed(ActionEvent actionevent) {
      for (int i = 0; i < mcuMenu.getItemCount(); i++)
        if (mcuMenu.getItem(i) instanceof JCheckBoxMenuItem)
          ((JCheckBoxMenuItem) mcuMenu.getItem(i)).setState(false);
      
      ((JCheckBoxMenuItem) actionevent.getSource()).setState(true);
      Preferences.set("build.mcu",
        ((JCheckBoxMenuItem) actionevent.getSource()).getLabel());

      boolean bootloadingEnabled =
        Preferences.get("build.mcu").equals("atmega8");
      burnBootloaderItem.setEnabled(bootloadingEnabled);
      if (burnBootloaderParallelItem != null)
        burnBootloaderParallelItem.setEnabled(bootloadingEnabled);

      try {
        LibraryManager libraryManager = new LibraryManager();
        libraryManager.rebuildAllBuilt();
      } catch (IOException e) {
        e.printStackTrace();
      } catch (RunnerException e) {
        message("Error rebuilding libraries...");
        error(e);
      }
    }
  }
  
  protected void populateSerialMenu() {
    // getting list of ports

    JMenuItem rbMenuItem;
    
    //System.out.println("Clearing serial port menu.");
	
    serialMenu.removeAll();

    try
    {
      for (Enumeration enumeration = CommPortIdentifier.getPortIdentifiers(); enumeration.hasMoreElements();)
      {
        CommPortIdentifier commportidentifier = (CommPortIdentifier)enumeration.nextElement();
        //System.out.println("Found communication port: " + commportidentifier);
        if (commportidentifier.getPortType() == CommPortIdentifier.PORT_SERIAL)
        {
          //System.out.println("Adding port to serial port menu: " + commportidentifier);
          String curr_port = commportidentifier.getName();
          rbMenuItem = new JCheckBoxMenuItem(curr_port, curr_port.equals(Preferences.get("serial.port")));
          rbMenuItem.addActionListener(serialMenuListener);
          //serialGroup.add(rbMenuItem);
          serialMenu.add(rbMenuItem);
        }
      }

    }

    catch (Exception exception)
    {
      System.out.println("error retrieving port list");
      exception.printStackTrace();
    }
	
    if (serialMenu.getItemCount() == 0) {
      serialMenu.setEnabled(false);
    }

    //serialMenu.addSeparator();
    //serialMenu.add(item);
  }

  protected JMenu buildHelpMenu() {
    JMenu menu = new JMenu("Help");
    JMenuItem item;

    if (!Base.isLinux()) {
      item = new JMenuItem("Getting Started");
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            if (Base.isWindows())
              Base.openURL(System.getProperty("user.dir") + File.separator +
                           "reference" + File.separator + "Guide_Windows.html");
            else
              Base.openURL(System.getProperty("user.dir") + File.separator +
                           "reference" + File.separator + "Guide_MacOSX.html");
          }
        });
      menu.add(item);
    }

    item = new JMenuItem("Troubleshooting");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL(System.getProperty("user.dir") + File.separator +
                       "reference" + File.separator +
                       "Guide_Troubleshooting.html");
        }
      });
    menu.add(item);

    item = new JMenuItem("Environment");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL(System.getProperty("user.dir") + File.separator +
                       "reference" + File.separator + "Guide_Environment.html");
        }
      });
    menu.add(item);

    item = new JMenuItem("Reference");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL(System.getProperty("user.dir") + File.separator +
                       "reference" + File.separator + "index.html");
        }
      });
    menu.add(item);

    item = new JMenuItem("Frequently Asked Questions");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL(System.getProperty("user.dir") + File.separator +
                       "reference" + File.separator + "FAQ.html");
        }
      });
    menu.add(item);

//    item = newJMenuItem("Find in Reference", 'F', true);
//    item.addActionListener(new ActionListener() {
//        public void actionPerformed(ActionEvent e) {
//          if (textarea.isSelectionActive()) {
//            String text = textarea.getSelectedText();
//            if (text.length() == 0) {
//              message("First select a word to find in the reference.");
//
//            } else {
//              String referenceFile = PdeKeywords.getReference(text);
//              if (referenceFile == null) {
//                message("No reference available for \"" + text + "\"");
//              } else {
//                Base.showReference(referenceFile);
//              }
//            }
//          }
//        }
//      });
//    menu.add(item);

    item = newJMenuItem("Visit www.arduino.cc", '5');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          Base.openURL("http://www.arduino.cc/");
        }
      });
    menu.add(item);

    // macosx already has its own about menu
    if (!Base.isMacOS()) {
      menu.addSeparator();
      item = new JMenuItem("About Arduino");
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            handleAbout();
          }
        });
      menu.add(item);
    }

    return menu;
  }


  public JMenu buildEditMenu() {
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
          textarea.cut();
          sketch.setModified(true);
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

    item = newJMenuItem("Find...", 'F');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          new FindReplace(Editor.this).show();
          //find.show();
          //find.setVisible(true);
        }
      });
    menu.add(item);

    item = newJMenuItem("Find Next", 'G');
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          // TODO find next should only be enabled after a
          // search has actually taken place
          //find.find(true);
          FindReplace find = new FindReplace(Editor.this); //.show();
          find.find(true);
        }
      });
    menu.add(item);

    return menu;
  }


  /**
   * Convenience method, see below.
   */
  static public JMenuItem newJMenuItem(String title, int what) {
    return newJMenuItem(title, what, false);
  }


  /**
   * A software engineer, somewhere, needs to have his abstraction
   * taken away. In some countries they jail people for writing the
   * sort of crappy api that would require a four line helper function
   * to set the command key for a menu item.
   */
  static public JMenuItem newJMenuItem(String title,
                                       int what, boolean shift) {
    JMenuItem menuItem = new JMenuItem(title);
    int modifiers = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
    if (shift) modifiers |= ActionEvent.SHIFT_MASK;
    menuItem.setAccelerator(KeyStroke.getKeyStroke(what, modifiers));
    return menuItem;
  }


  // ...................................................................


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
        //this.setEnabled(true);
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


  // ...................................................................


  // interfaces for MRJ Handlers, but naming is fine
  // so used internally for everything else

  public void handleAbout() {
    final Image image = Base.getImage("about.jpg", this);
    int w = image.getWidth(this);
    int h = image.getHeight(this);
    final Window window = new Window(this) {
        public void paint(Graphics g) {
          g.drawImage(image, 0, 0, null);

          Graphics2D g2 = (Graphics2D) g;
          g2.setRenderingHint(RenderingHints.KEY_TEXT_ANTIALIASING,
                              RenderingHints.VALUE_TEXT_ANTIALIAS_OFF);

          g.setFont(new Font("SansSerif", Font.PLAIN, 11));
          g.setColor(Color.white);
          g.drawString(Base.VERSION_NAME, 50, 30);
        }
      };
    window.addMouseListener(new MouseAdapter() {
        public void mousePressed(MouseEvent e) {
          window.dispose();
        }
      });
    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    window.setBounds((screen.width-w)/2, (screen.height-h)/2, w, h);
    window.show();
  }


  /**
   * Show the preferences window.
   */
  public void handlePrefs() {
    Preferences preferences = new Preferences();
    preferences.showFrame(this);

    // since this can't actually block, it'll hide
    // the editor window while the prefs are open
    //preferences.showFrame(this);
    // and then call applyPreferences if 'ok' is hit
    // and then unhide

    // may need to rebuild sketch and other menus
    //applyPreferences();

    // next have editor do its thing
    //editor.appyPreferences();
  }


  // ...................................................................


  /**
   * Get the contents of the current buffer. Used by the Sketch class.
   */
  public String getText() {
    return textarea.getText();
  }


  /**
   * Called to update the text but not switch to a different
   * set of code (which would affect the undo manager).
   */
  public void setText(String what, int selectionStart, int selectionEnd) {
    beginCompoundEdit();
    textarea.setText(what);
    endCompoundEdit();

    // make sure that a tool isn't asking for a bad location
    selectionStart =
      Math.max(0, Math.min(selectionStart, textarea.getDocumentLength()));
    selectionEnd =
      Math.max(0, Math.min(selectionStart, textarea.getDocumentLength()));
    textarea.select(selectionStart, selectionEnd);

    textarea.requestFocus();  // get the caret blinking
  }


  /**
   * Switch between tabs, this swaps out the Document object
   * that's currently being manipulated.
   */
  public void setCode(SketchCode code) {
    if (code.document == null) {  // this document not yet inited
      code.document = new SyntaxDocument();

      // turn on syntax highlighting
      code.document.setTokenMarker(new PdeKeywords());

      // insert the program text into the document object
      try {
        code.document.insertString(0, code.program, null);
      } catch (BadLocationException bl) {
        bl.printStackTrace();
      }

      // set up this guy's own undo manager
      code.undo = new UndoManager();

      // connect the undo listener to the editor
      code.document.addUndoableEditListener(new UndoableEditListener() {
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
    textarea.setDocument(code.document,
                         code.selectionStart, code.selectionStop,
                         code.scrollPosition);

    textarea.requestFocus();  // get the caret blinking

    this.undo = code.undo;
    undoAction.updateUndoState();
    redoAction.updateRedoState();
  }

  public void beginCompoundEdit() {
    compoundEdit = new CompoundEdit();
  }

  public void endCompoundEdit() {
    compoundEdit.end();
    undo.addEdit(compoundEdit);
    undoAction.updateUndoState();
    redoAction.updateRedoState();
    compoundEdit = null;
  }



  public void handleRun(final boolean present) {
    doClose();
    running = true;
    buttons.activate(EditorButtons.RUN);
    message("Compiling...");
    // do this for the terminal window / dos prompt / etc
    for (int i = 0; i < 10; i++) System.out.println();

    // clear the console on each run, unless the user doesn't want to
    //if (Base.getBoolean("console.auto_clear", true)) {
    //if (Preferences.getBoolean("console.auto_clear", true)) {
    if (Preferences.getBoolean("console.auto_clear")) {
      console.clear();
    }

    presenting = present;
    if (presenting && Base.isMacOS()) {
      // check to see if osx 10.2, if so, show a warning
      String osver = System.getProperty("os.version").substring(0, 4);
      if (osver.equals("10.2")) {
        Base.showWarning("Time for an OS Upgrade",
                         "The \"Present\" feature may not be available on\n" +
                         "Mac OS X 10.2, because of what appears to be\n" +
                         "a bug in the Java 1.4 implementation on 10.2.\n" +
                         "In case it works on your machine, present mode\n" +
                         "will start, but if you get a flickering white\n" +
                         "window, using Command-Q to quit the sketch", null);
      }
    }

    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        try {
          if (!sketch.handleRun(new Target(
              System.getProperty("user.dir") + File.separator + "lib" +
              File.separator + "targets", Preferences.get("build.target"))))
            return;
    
          //runtime = new Runner(sketch, Editor.this);
          //runtime.start(appletLocation);
          watcher = new RunButtonWatcher();
          message("Done compiling.");
          if(watcher != null) watcher.stop();
    
        } catch (RunnerException e) {
          message("Error compiling...");
          error(e);

        } catch (Exception e) {
          e.printStackTrace();
        }
      }});
    
    // this doesn't seem to help much or at all
    /*
    final SwingWorker worker = new SwingWorker() {
        public Object construct() {
          try {
            if (!sketch.handleRun()) return null;

            runtime = new Runner(sketch, Editor.this);
            runtime.start(presenting ? presentLocation : appletLocation);
            watcher = new RunButtonWatcher();
            message("Done compiling.");

          } catch (RunnerException e) {
            message("Error compiling...");
            error(e);
          
          } catch (Exception e) {
            e.printStackTrace();
          }
          return null;  // needn't return anything
        }
      };
    worker.start();
    */
    //sketch.cleanup();  // where does this go?
    buttons.clear();
  }


  class RunButtonWatcher implements Runnable {
    Thread thread;

    public RunButtonWatcher() {
      thread = new Thread(this, "run button watcher");
      thread.setPriority(Thread.MIN_PRIORITY);
      thread.start();
    }

    public void run() {
      while (Thread.currentThread() == thread) {
        /*if (runtime == null) {
          stop();

        } else {
          if (runtime.applet != null) {
            if (runtime.applet.finished) {
              stop();
            }
            //buttons.running(!runtime.applet.finished);

          } else if (runtime.process != null) {
            //buttons.running(true);  // ??

          } else {
            stop();
          }
        }*/
        try {
          Thread.sleep(250);
        } catch (InterruptedException e) { }
        //System.out.println("still inside runner thread");
      }
    }

    public void stop() {
      buttons.running(false);
      thread = null;
    }
  }


  public void handleSerial() {
    if (!debugging) {
      console.clear();
      buttons.activate(EditorButtons.SERIAL);
      serialPort = new Serial(true);
      debugging = true;
      status.serial();
    } else {
      doStop();
    }
  }


  public void handleStop() {  // called by menu or buttons
    if (presenting) {
      doClose();
    } else {
      doStop();
    }
    buttons.clear();
  }


  /**
   * Stop the applet but don't kill its window.
   */
  public void doStop() {
    //if (runtime != null) runtime.stop();
    if (debugging)  {
      status.unserial();
      serialPort.dispose();
      debugging = false;
    }
    if (watcher != null) watcher.stop();
    message(EMPTY);

    // the buttons are sometimes still null during the constructor
    // is this still true? are people still hitting this error?
    /*if (buttons != null)*/ buttons.clear();

    running = false;
  }


  /**
   * Stop the applet and kill its window. When running in presentation
   * mode, this will always be called instead of doStop().
   */
  public void doClose() {
    //if (presenting) {
    //presentationWindow.hide();
    //} else {
    //try {
      // the window will also be null the process was running
      // externally. so don't even try setting if window is null
      // since Runner will set the appletLocation when an
      // external process is in use.
//      if (runtime.window != null) {
//        appletLocation = runtime.window.getLocation();
//      }
    //} catch (NullPointerException e) { }
    //}

    //if (running) doStop();
    doStop();  // need to stop if runtime error

    //try {
      /*if (runtime != null) {
        runtime.close();  // kills the window
        runtime = null; // will this help?
      }*/
    //} catch (Exception e) { }
    //buttons.clear();  // done by doStop

    sketch.cleanup();

    // [toxi 030903]
    // focus the PDE again after quitting presentation mode
    toFront();
  }


  /**
   * Check to see if there have been changes. If so, prompt user
   * whether or not to save first. If the user cancels, just ignore.
   * Otherwise, one of the other methods will handle calling
   * checkModified2() which will get on with business.
   */
  protected void checkModified(int checkModifiedMode) {
    this.checkModifiedMode = checkModifiedMode;

    if (!sketch.modified) {
      checkModified2();
      return;
    }

    String prompt = "Save changes to " + sketch.name + "?  ";

    if (checkModifiedMode != HANDLE_QUIT) {
      // if the user is not quitting, then use the nicer
      // dialog that's actually inside the p5 window.
      status.prompt(prompt);

    } else {
      // if the user selected quit, then this has to be done with
      // a JOptionPane instead of internally in the editor.
      // TODO this is actually just a bug to be fixed.

      // macosx java kills the app even though cancel might get hit
      // so the cancel button is (temporarily) left off
      // this may be treated differently in macosx java 1.4,
      // but 1.4 isn't currently stable enough to use.

      // turns out windows has the same problem (sometimes)
      // disable cancel for now until a fix can be found.

      Object[] options = { "Yes", "No" };
      int result = JOptionPane.showOptionDialog(this,
                                                prompt,
                                                "Quit",
                                                JOptionPane.YES_NO_OPTION,
                                                JOptionPane.QUESTION_MESSAGE,
                                                null,
                                                options,
                                                options[0]);

      if (result == JOptionPane.YES_OPTION) {
        handleSave(true);
        checkModified2();

      } else if (result == JOptionPane.NO_OPTION) {
        checkModified2();  // though this may just quit

      } else if (result == JOptionPane.CANCEL_OPTION) {
        // ignored
      }
    }
  }


  /**
   * Called by EditorStatus to complete the job and re-dispatch
   * to handleNew, handleOpen, handleQuit.
   */
  public void checkModified2() {
    switch (checkModifiedMode) {
      case HANDLE_NEW:  handleNew2(false); break;
      case HANDLE_OPEN: handleOpen2(handleOpenPath); break;
      case HANDLE_QUIT: handleQuit2(); break;
    }
    checkModifiedMode = 0;
  }


  /**
   * New was called (by buttons or by menu), first check modified
   * and if things work out ok, handleNew2() will be called.
   * <p/>
   * If shift is pressed when clicking the toolbar button, then
   * force the opposite behavior from sketchbook.prompt's setting
   */
  public void handleNew(final boolean shift) {
    buttons.activate(EditorButtons.NEW);

    SwingUtilities.invokeLater(new Runnable() {
        public void run() {
          doStop();
          handleNewShift = shift;
          handleNewLibrary = false;
          checkModified(HANDLE_NEW);
        }});
  }


  /**
   * Extra public method so that Sketch can call this when a sketch
   * is selected to be deleted, and it won't call checkModified()
   * to prompt for save as.
   */
  public void handleNewUnchecked() {
    doStop();
    handleNewShift = false;
    handleNewLibrary = false;
    handleNew2(true);
  }


  /**
   * User selected "New Library", this will act just like handleNew
   * but internally set a flag that the new guy is a library,
   * meaning that a "library" subfolder will be added.
   */
  public void handleNewLibrary() {
    doStop();
    handleNewShift = false;
    handleNewLibrary = true;
    checkModified(HANDLE_NEW);
  }


  /**
   * Does all the plumbing to create a new project
   * then calls handleOpen to load it up.
   *
   * @param noPrompt true to disable prompting for the sketch
   * name, used when the app is starting (auto-create a sketch)
   */
  protected void handleNew2(boolean noPrompt) {
    try {
      String pdePath =
        sketchbook.handleNew(noPrompt, handleNewShift, handleNewLibrary);
      if (pdePath != null) handleOpen2(pdePath);

    } catch (IOException e) {
      // not sure why this would happen, but since there's no way to
      // recover (outside of creating another new setkch, which might
      // just cause more trouble), then they've gotta quit.
      Base.showError("Problem creating a new sketch",
                     "An error occurred while creating\n" +
                     "a new sketch. Arduino must now quit.", e);
    }
    buttons.clear();
  }


  /**
   * This is the implementation of the MRJ open document event,
   * and the Windows XP open document will be routed through this too.
   */
  public void handleOpenFile(File file) {
    //System.out.println("handling open file: " + file);
    handleOpen(file.getAbsolutePath());
  }


  /**
   * Open a sketch given the full path to the .pde file.
   * Pass in 'null' to prompt the user for the name of the sketch.
   */
  public void handleOpen(final String ipath) {
    // haven't run across a case where i can verify that this works
    // because open is usually very fast.
    buttons.activate(EditorButtons.OPEN);

    SwingUtilities.invokeLater(new Runnable() {
        public void run() {
          String path = ipath;
          if (path == null) {  // "open..." selected from the menu
            path = sketchbook.handleOpen();
            if (path == null) return;
          }
          doClose();
          handleOpenPath = path;
          checkModified(HANDLE_OPEN);
        }});
  }


  /**
   * Open a sketch from a particular path, but don't check to save changes.
   * Used by Sketch.saveAs() to re-open a sketch after the "Save As"
   */
  public void handleOpenUnchecked(String path) {
    doClose();
    handleOpen2(path);
  }


  /**
   * Second stage of open, occurs after having checked to
   * see if the modifications (if any) to the previous sketch
   * need to be saved.
   */
  protected void handleOpen2(String path) {
    if (sketch != null) {
      // if leaving an empty sketch (i.e. the default) do an
      // auto-clean right away
      try {
        // don't clean if we're re-opening the same file
        String oldPath = sketch.code[0].file.getCanonicalPath();
        String newPath = new File(path).getCanonicalPath();
        if (!oldPath.equals(newPath)) {
          if (Base.calcFolderSize(sketch.folder) == 0) {
            Base.removeDir(sketch.folder);
            sketchbook.rebuildMenus();
          }
        }
      } catch (Exception e) { }   // oh well
    }

    try {
      // check to make sure that this .pde file is
      // in a folder of the same name
      File file = new File(path);
      File parentFile = new File(file.getParent());
      String parentName = parentFile.getName();
      String pdeName = parentName + ".pde";
      File altFile = new File(file.getParent(), pdeName);

      //System.out.println("path = " + file.getParent());
      //System.out.println("name = " + file.getName());
      //System.out.println("pname = " + parentName);

      if (pdeName.equals(file.getName())) {
        // no beef with this guy

      } else if (altFile.exists()) {
        // user selected a .java from the same sketch,
        // but open the .pde instead
        path = altFile.getAbsolutePath();
        //System.out.println("found alt file in same folder");

      } else if (!path.endsWith(".pde")) {
        Base.showWarning("Bad file selected",
                            "Arduino can only open its own sketches\n" +
                            "and other files ending in .pde", null);
        return;

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
            return;
          }
          if (!properFolder.mkdirs()) {
            throw new IOException("Couldn't create sketch folder");
          }
          // copy the sketch inside
          File properPdeFile = new File(properFolder, file.getName());
          File origPdeFile = new File(path);
          Base.copyFile(origPdeFile, properPdeFile);

          // remove the original file, so user doesn't get confused
          origPdeFile.delete();

          // update with the new path
          path = properPdeFile.getAbsolutePath();

        } else if (result == JOptionPane.NO_OPTION) {
          return;
        }
      }

      sketch = new Sketch(this, path);
      // TODO re-enable this once export application works
      //exportAppItem.setEnabled(false);
      //exportAppItem.setEnabled(false && !sketch.isLibrary());
      //buttons.disableRun(sketch.isLibrary());
      header.rebuild();
      if (Preferences.getBoolean("console.auto_clear")) {
        console.clear();
      }

    } catch (Exception e) {
      error(e);
    }
  }


  // there is no handleSave1 since there's never a need to prompt
  /**
   * Actually handle the save command. If 'force' is set to false,
   * this will happen in another thread so that the message area
   * will update and the save button will stay highlighted while the
   * save is happening. If 'force' is true, then it will happen
   * immediately. This is used during a quit, because invokeLater()
   * won't run properly while a quit is happening. This fixes
   * <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=276">Bug 276</A>.
   */
  public void handleSave(boolean force) {
    doStop();
    buttons.activate(EditorButtons.SAVE);

    if (force) {
      handleSave2();
    } else {
      SwingUtilities.invokeLater(new Runnable() {
          public void run() {
            handleSave2();
          }
        });
    }
  }


  public void handleSave2() {
    message("Saving...");
    try {
      if (sketch.save()) {
        message("Done Saving.");
      } else {
        message(EMPTY);
      }
      // rebuild sketch menu in case a save-as was forced
      sketchbook.rebuildMenus();

    } catch (Exception e) {
      // show the error as a message in the window
      error(e);

      // zero out the current action,
      // so that checkModified2 will just do nothing
      checkModifiedMode = 0;
      // this is used when another operation calls a save
    }
    buttons.clear();
  }


  public void handleSaveAs() {
    doStop();
    buttons.activate(EditorButtons.SAVE);

    SwingUtilities.invokeLater(new Runnable() {
        public void run() {
          message("Saving...");
          try {
            if (sketch.saveAs()) {
              message("Done Saving.");
              sketchbook.rebuildMenus();
            } else {
              message("Save Cancelled.");
            }
          } catch (Exception e) {
            // show the error as a message in the window
            error(e);
          }
          buttons.clear();
        }});
  }


  /**
   * Handles calling the export() function on sketch, and
   * queues all the gui status stuff that comes along with it.
   *
   * Made synchronized to (hopefully) avoid problems of people
   * hitting export twice, quickly, and horking things up.
   */
  synchronized public void handleExport() {
    if(debugging)
      doStop();
    buttons.activate(EditorButtons.EXPORT);
    console.clear();
    //String what = sketch.isLibrary() ? "Applet" : "Library";
    //message("Exporting " + what + "...");
    message("Uploading to I/O Board...");

    SwingUtilities.invokeLater(new Runnable() {
        public void run() {
          try {
            //boolean success = sketch.isLibrary() ?
            //sketch.exportLibrary() : sketch.exportApplet();
            boolean success = sketch.exportApplet(new Target(
                System.getProperty("user.dir") + File.separator + "lib" +
                File.separator + "targets", Preferences.get("build.target")));
            if (success) {
              message("Done uploading.");
            } else {
              // error message will already be visible
            }
          } catch (RunnerException e) {
            message("Error during upload.");
            //e.printStackTrace();
            error(e);
          } catch (Exception e) {
            e.printStackTrace();
          }
          buttons.clear();
        }});
  }


  synchronized public void handleExportApp() {
    message("Exporting application...");
    try {
      if (sketch.exportApplication()) {
        message("Done exporting.");
      } else {
        // error message will already be visible
      }
    } catch (Exception e) {
      message("Error during export.");
      e.printStackTrace();
    }
    buttons.clear();
  }


  /**
   * Checks to see if the sketch has been modified, and if so,
   * asks the user to save the sketch or cancel the export.
   * This prevents issues where an incomplete version of the sketch
   * would be exported, and is a fix for
   * <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=157">Bug 157</A>
   */
  public boolean handleExportCheckModified() {
    if (!sketch.modified) return true;

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
      message("Export canceled, changes must first be saved.");
      buttons.clear();
      return false;
    }
    return true;
  }

  
  /**
   * Quit, but first ask user if it's ok. Also store preferences
   * to disk just in case they want to quit. Final exit() happens
   * in Editor since it has the callback from EditorStatus.
   */
  public void handleQuit() {
    // doStop() isn't sufficient with external vm & quit
    // instead use doClose() which will kill the external vm
    doClose();

    checkModified(HANDLE_QUIT);
  }


  /**
   * Actually do the quit action.
   */
  protected void handleQuit2() {
    storePreferences();
    Preferences.save();

    sketchbook.clean();
    console.handleQuit();

    //System.out.println("exiting here");
    System.exit(0);
  }

  protected void handleBurnBootloader(final boolean parallel) {
    if(debugging)
      doStop();
    console.clear();
    if (!Preferences.get("build.mcu").equals("atmega8")) {
      error("Burn bootloader only works on ATmega8s.  See the Arduino FAQ for more info.");
      return;
    }
    //String what = sketch.isLibrary() ? "Applet" : "Library";
    //message("Exporting " + what + "...");
    message("Burning bootloader to I/O Board (this may take a minute)...");
    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        try {
          //boolean success = sketch.isLibrary() ?
          //sketch.exportLibrary() : sketch.exportApplet();
          Uploader uploader = new UispUploader();
          boolean success = parallel ? 
            uploader.burnBootloaderParallel() :
            uploader.burnBootloaderAVRISP();
          
          if (success) {
            message("Done burning bootloader.");
          } else {
            // error message will already be visible
          }
        } catch (RunnerException e) {
          message("Error while burning bootloader.");
          //e.printStackTrace();
          error(e);
        } catch (Exception e) {
          e.printStackTrace();
        }
        buttons.clear();
      }});
  }

  public void highlightLine(int lnum) {
    if (lnum < 0) {
      textarea.select(0, 0);
      return;
    }
    //System.out.println(lnum);
    String s = textarea.getText();
    int len = s.length();
    int st = -1;
    int ii = 0;
    int end = -1;
    int lc = 0;
    if (lnum == 0) st = 0;
    for (int i = 0; i < len; i++) {
      ii++;
      //if ((s.charAt(i) == '\n') || (s.charAt(i) == '\r')) {
      boolean newline = false;
      if (s.charAt(i) == '\r') {
        if ((i != len-1) && (s.charAt(i+1) == '\n')) {
          i++; //ii--;
        }
        lc++;
        newline = true;
      } else if (s.charAt(i) == '\n') {
        lc++;
        newline = true;
      }
      if (newline) {
        if (lc == lnum)
          st = ii;
        else if (lc == lnum+1) {
          //end = ii;
          // to avoid selecting entire, because doing so puts the
          // cursor on the next line [0090]
          end = ii - 1;
          break;
        }
      }
    }
    if (end == -1) end = len;

    // sometimes KJC claims that the line it found an error in is
    // the last line in the file + 1.  Just highlight the last line
    // in this case. [dmose]
    if (st == -1) st = len;

    textarea.select(st, end);
  }


  // ...................................................................


  /**
   * Show an error int the status bar.
   */
  public void error(String what) {
    status.error(what);
  }


  public void error(Exception e) {
    if (e == null) {
      System.err.println("Editor.error() was passed a null exception.");
      return;
    }

    // not sure if any RuntimeExceptions will actually arrive
    // through here, but gonna check for em just in case.
    String mess = e.getMessage();
    if (mess != null) {
      String rxString = "RuntimeException: ";
      if (mess.indexOf(rxString) == 0) {
        mess = mess.substring(rxString.length());
      }
      String javaLang = "java.lang.";
      if (mess.indexOf(javaLang) == 0) {
        mess = mess.substring(javaLang.length());
      }
      error(mess);
    }
    e.printStackTrace();
  }


  public void error(RunnerException e) {
    //System.out.println("ERORROOROROR 2");
    if (e.file >= 0) sketch.setCurrent(e.file);
    if (e.line >= 0) highlightLine(e.line);

    // remove the RuntimeException: message since it's not
    // really all that useful to the user
    //status.error(e.getMessage());
    String mess = e.getMessage();
    String rxString = "RuntimeException: ";
    if (mess.indexOf(rxString) == 0) {
      mess = mess.substring(rxString.length());
      //System.out.println("MESS3: " + mess);
    }
    String javaLang = "java.lang.";
    if (mess.indexOf(javaLang) == 0) {
      mess = mess.substring(javaLang.length());
    }
    error(mess);
    buttons.clear();
  }



  public void message(String msg) {
    status.notice(msg);
  }



  // ...................................................................


  /**
   * Returns the edit popup menu.
   */
  class TextAreaPopup extends JPopupMenu {
    String currentDir = System.getProperty("user.dir");
    String referenceFile = null;

    JMenuItem cutItem, copyItem;
    JMenuItem referenceItem;


    public TextAreaPopup() {
      JMenuItem item;

      cutItem = new JMenuItem("Cut");
      cutItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            textarea.cut();
            sketch.setModified(true);
          }
      });
      this.add(cutItem);

      copyItem = new JMenuItem("Copy");
      copyItem.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            textarea.copy();
          }
        });
      this.add(copyItem);

      item = new JMenuItem("Paste");
      item.addActionListener(new ActionListener() {
          public void actionPerformed(ActionEvent e) {
            textarea.paste();
            sketch.setModified(true);
          }
        });
      this.add(item);

      item = new JMenuItem("Select All");
      item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          textarea.selectAll();
        }
      });
      this.add(item);

      this.addSeparator();

//      referenceItem = new JMenuItem("Find in Reference");
//      referenceItem.addActionListener(new ActionListener() {
//          public void actionPerformed(ActionEvent e) {
//            Base.showReference(referenceFile);
//          }
//        });
//      this.add(referenceItem);
    }

    // if no text is selected, disable copy and cut menu items
    public void show(Component component, int x, int y) {
      if (textarea.isSelectionActive()) {
        cutItem.setEnabled(true);
        copyItem.setEnabled(true);

        //referenceFile = PdeKeywords.getReference(textarea.getSelectedText());
        //if (referenceFile != null) {
          //referenceItem.setEnabled(true);
        //}
      } else {
        cutItem.setEnabled(false);
        copyItem.setEnabled(false);
        //referenceItem.setEnabled(false);
      }
      super.show(component, x, y);
    }
  }
}

