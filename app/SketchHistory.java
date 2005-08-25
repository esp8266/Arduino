/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  SketchHistory - handler for storing history information about a project
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
*/


/*

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.util.*;
import java.util.zip.*;

import javax.swing.*;


public class SketchHistory {
  Editor editor;

  // why things have been saved for history
  static final int RUN      = 5;
  static final int SAVE     = 6;
  static final int AUTOSAVE = 7;
  static final int BEAUTIFY = 8;

  static final String HISTORY_SEPARATOR =
    "#################################################";

  JMenu menu;

  // true if the sketch is read-only,
  // meaning that no history will be recorded
  boolean readOnlySketch;

  File historyFile;
  String lastRecorded;

  ActionListener menuListener;


  //public SketchHistory(Editor editor) {
  //this.editor = editor;
  //}

  public SketchHistory(Sketch sketch) {
    menu = new JMenu("History");

    menuListener = new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          retrieve(e.getActionCommand());
        }
      };
  }



   /// Set the path for the current sketch

  public void setPath(String path, boolean readOnlySketch) {
    this.readOnlySketch = true;

    if (readOnlySketch) return;
    historyFile = new File(path, "history.gz");
  }


  public void attachMenu(JMenu parent) {
    //if (Preferences.getBoolean("history.recording")) {
    parent.add(menu);

    // should leave enabled, since can still get old history
    // even if the new stuff isn't being recorded
    //menu.setEnabled(Preferences.getBoolean("history.recording"));
    //}
  }


  /// Check to see if history should be recorded.
  /// mode is RUN, SAVE, AUTOSAVE, or BEAUTIFY
  public void record(String program, int mode) {
    if (readOnlySketch) return;

    if (!Preferences.getBoolean("history.recording")) return;

    if ((lastRecorded != null) &&
        (lastRecorded.equals(program))) return;

    String modeStr = null;
    switch (mode) {
    case RUN: modeStr = "run"; break;
    case SAVE: modeStr = "save"; break;
    case AUTOSAVE: modeStr = "autosave"; break;
    case BEAUTIFY: modeStr = "beautify"; break;
    }

    try {
      boolean noPreviousHistory = false;

      ByteArrayOutputStream old = null;
      if (historyFile.exists()) {
        InputStream oldStream = new GZIPInputStream(new BufferedInputStream(new FileInputStream(historyFile)));
        old = new ByteArrayOutputStream();

        int c = oldStream.read();
        while (c != -1) {
          old.write(c);
          c = oldStream.read();
        }
        //return out.toByteArray();
        oldStream.close();

      } else {
        noPreviousHistory = true;  // rebuild menu
      }

      OutputStream historyStream =
        new GZIPOutputStream(new FileOutputStream(historyFile));

      if (old != null) {
        historyStream.write(old.toByteArray());
      }
      PrintWriter historyWriter =
        new PrintWriter(new OutputStreamWriter(historyStream));

      historyWriter.println();
      historyWriter.println(HISTORY_SEPARATOR);

      Calendar now = Calendar.getInstance();
      // 2002 06 18  11 43 29
      // when listing, study for descrepancies.. if all are
      // 2002, then don't list the year and soforth.
      // for the other end, if all minutes are unique,
      // then don't show seconds
      int year = now.get(Calendar.YEAR);
      int month = now.get(Calendar.MONTH) + 1;
      int day = now.get(Calendar.DAY_OF_MONTH);
      int hour = now.get(Calendar.HOUR_OF_DAY);
      int minute = now.get(Calendar.MINUTE);
      int second = now.get(Calendar.SECOND);
      String parseDate = year + " " + month + " " + day + " " +
        hour + " " + minute + " " + second;

      String readableDate = now.getTime().toString();

      // increment this so sketchbook won't be mangled
      // each time this format has to change
      String historyVersion = "1";
      //Date date = new Date();
      //String datestamp = date.toString();

      historyWriter.println(historyVersion + " " + modeStr + " - " +
                            parseDate + " - " + readableDate);
      historyWriter.println();
      historyWriter.println(program);
      historyWriter.flush();  // ??
      lastRecorded = program;

      //JMenuItem menuItem = new JMenuItem(modeStr + " - " + readableDate);
      JMenuItem menuItem = new JMenuItem(modeStr + " - " + readableDate);
      menuItem.addActionListener(menuListener);
      menu.insert(menuItem, 2);

      historyWriter.flush();
      historyWriter.close();

      if (noPreviousHistory) {
        // to get add the actual menu, to get the 'clear' item in there
        //rebuildMenu(historyFile.getPath());
        rebuildMenu();
      }

    } catch (IOException e) {
      e.printStackTrace();
    }
  }


  public void retrieve(String selection) {
    //System.out.println("sel '" + selection + "'");
    String readableDate =
      selection.substring(selection.indexOf("-") + 2);

    // make history for the current guy
    record(editor.textarea.getText(), AUTOSAVE);
    // mark editor text as having been edited

    try {
      BufferedReader reader = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(historyFile))));
      String line = null;

      int historyCount = 0;
      String historyList[] = new String[100];

      try {
        boolean found = false;
        while ((line = reader.readLine()) != null) {
          //System.out.println("->" + line);
          if (line.equals(HISTORY_SEPARATOR)) {
            line = reader.readLine();
            if (line.indexOf(readableDate) != -1) {  // this is the one
              found = true;
              break;
            }
          }
        }
        if (found) {
          // read lines until the next separator
          line = reader.readLine(); // ignored
          //String sep = System.getProperty("line.separator");
          StringBuffer buffer = new StringBuffer();
          while ((line = reader.readLine()) != null) {
            if (line.equals(HISTORY_SEPARATOR)) break;
            //textarea.append(line + sep);
            //buffer.append(line + sep);  // JTextPane wants only \n going in
            buffer.append(line + "\n");
            //System.out.println("'" + line + "'");
          }
          //textarea.editorSetText(buffer.toString());
          editor.changeText(buffer.toString(), true);
          lastRecorded = editor.textarea.getText();
          editor.setSketchModified(false);

        } else {
          System.err.println("couldn't find history entry for " +
                             "'" + readableDate + "'");
        }
      } catch (IOException e) {
        e.printStackTrace();
      }
    } catch (IOException e) {
      e.printStackTrace();
    }
  }


//  class HistoryMenuListener implements ActionListener {
//    public void actionPerformed(ActionEvent e) {
//      editor.selectHistory(e.getActionCommand);
//    }
//  }


  //public void rebuildHistoryMenu(String path) {
  //rebuildHistoryMenu(historyMenu, path);
  //}


  //public void rebuildHistoryMenu(Menu menu, String path) {
  public void rebuildMenu() {  //String path) {
    //if (!recordingHistory) return;
    //if (!Preferences.getBoolean("history.recording")) return;

    menu.removeAll();

    //File hfile = new File(path);
    //if (!hfile.exists()) return;  // no history yet
    if (!historyFile.exists()) return;

    JMenuItem item = new JMenuItem("Clear History");
    item.addActionListener(new ActionListener() {
        public void actionPerformed(ActionEvent e) {
          if (!historyFile.delete()) {
            //System.err.println("couldn't erase history");
            Base.showWarning("History Problem",
                                "Could not erase history", null);
          }
          rebuildMenu();
          //SketchHistory.this.rebuildMenu(historyFile.getPath());
        }
      });
    menu.add(item);
    menu.addSeparator();

    try {
      BufferedReader reader = new BufferedReader(new InputStreamReader(new GZIPInputStream(new FileInputStream(historyFile))));
      String line = null;

      int historyCount = 0;
      String historyList[] = new String[100];

      try {
        while ((line = reader.readLine()) != null) {
        //while (line = reader.readLine()) {
        //while (true) { line = reader.readLine();
          //if (line == null) continue;
          //System.out.println("line: " + line);
          if (line.equals(HISTORY_SEPARATOR)) {
            // next line is the good stuff
            line = reader.readLine();
            int version =
              Integer.parseInt(line.substring(0, line.indexOf(' ')));
            if (version == 1) {
              String whysub = line.substring(2);  // after "1 "
              String why = whysub.substring(0, whysub.indexOf(" -"));
              //System.out.println("'" + why + "'");

              String readable = line.substring(line.lastIndexOf("-") + 2);
              if (historyList.length == historyCount) {
                String temp[] = new String[historyCount*2];
                System.arraycopy(historyList, 0, temp, 0, historyCount);
                historyList = temp;
              }
              historyList[historyCount++] = why + " - " + readable;

            } // otherwise don't know what to do
          }
        }
        //System.out.println(line);
      } catch (IOException e) {
        e.printStackTrace();
      }

      // add the items to the menu in reverse order

      //ActionListener historyMenuListener =
      //  new ActionListener() {
          //  public void actionPerformed(ActionEvent e) {
            //  editor.retrieveHistory(e.getActionCommand());
         //}
      //};

      for (int i = historyCount-1; i >= 0; --i) {
        JMenuItem mi = new JMenuItem(historyList[i]);
        mi.addActionListener(menuListener);
        menu.add(mi);
      }

      reader.close();

    } catch (IOException e) {
      e.printStackTrace();
    }
  }
}


*/
