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
import static processing.app.I18n._;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;
import javax.swing.border.Border;

import processing.app.helpers.OSUtils;


/**
 * Find & Replace window for the Processing editor.
 * <p/>
 * One major annoyance in this is that the window is re-created each time
 * that "Find" is called. This is because Mac OS X has a strange focus
 * issue with windows that are re-shown with setVisible() or show().
 * requestFocusInWindow() properly sets the focus to the find field,
 * however, just a short moment later, the focus is set to null. Even
 * trying to catch this scenario and request it again doesn't seem to work.
 * Most likely this is some annoyance buried deep in one of Apple's docs,
 * or in the doc for the focus stuff (I tend to think the former because
 * Windows doesn't seem to be quite so beligerent). Filed as
 * <A HREF="http://dev.processing.org/bugs/show_bug.cgi?id=244"> Bug 244</A>
 * should anyone have clues about how to fix.
 */
@SuppressWarnings("serial")
public class FindReplace extends JFrame implements ActionListener {

  private Editor editor;

  private JTextField findField;
  private JTextField replaceField;
  private static String findString;
  private static String replaceString;

  private JButton replaceButton;
  private JButton replaceAllButton;
  private JButton replaceFindButton;
  private JButton previousButton;
  private JButton findButton;

  private JCheckBox ignoreCaseBox;
  private static boolean ignoreCase = true;

  private JCheckBox wrapAroundBox;
  private static boolean wrapAround = true;

  private JCheckBox searchAllFilesBox;
  private static boolean searchAllFiles = false;

  public FindReplace(Editor editor) {
    super(_("Find"));
    this.editor = editor;

    JPanel contentPanel = new JPanel();
    Border padding = BorderFactory.createEmptyBorder(10, 10, 10, 10);
    contentPanel.setBorder(padding);
    setContentPane(contentPanel);
    
    JLabel findLabel = new JLabel(_("Find:"));
    findField = new JTextField(20);
    JLabel replaceLabel = new JLabel(_("Replace with:"));
    replaceField = new JTextField(20);

    // Fill the findString with selected text if no previous value
    if (editor.getSelectedText() != null
        && editor.getSelectedText().length() > 0)
      findString = editor.getSelectedText();
    
    if (findString != null)
      findField.setText(findString);
    if (replaceString != null)
      replaceField.setText(replaceString);

    ignoreCaseBox = new JCheckBox(_("Ignore Case"));
    ignoreCaseBox.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        ignoreCase = ignoreCaseBox.isSelected();
      }
    });
    ignoreCaseBox.setSelected(ignoreCase);

    wrapAroundBox = new JCheckBox(_("Wrap Around"));
    wrapAroundBox.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        wrapAround = wrapAroundBox.isSelected();
      }
    });
    wrapAroundBox.setSelected(wrapAround);

    searchAllFilesBox = new JCheckBox(_("Search all Sketch Tabs"));
    searchAllFilesBox.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent e) {
        searchAllFiles = searchAllFilesBox.isSelected();
      }
    });
    searchAllFilesBox.setSelected(searchAllFiles);

    JPanel checkboxPanel = new JPanel();
    checkboxPanel.setLayout(new BoxLayout(checkboxPanel, BoxLayout.PAGE_AXIS));
    checkboxPanel.add(ignoreCaseBox);
    checkboxPanel.add(Box.createRigidArea(new Dimension(8, 0)));
    checkboxPanel.add(wrapAroundBox);
    checkboxPanel.add(Box.createRigidArea(new Dimension(8, 0)));
    checkboxPanel.add(searchAllFilesBox);

    replaceAllButton = new JButton(_("Replace All"));
    replaceAllButton.addActionListener(this);
    replaceButton = new JButton(_("Replace"));
    replaceButton.addActionListener(this);
    replaceFindButton = new JButton(_("Replace & Find"));
    replaceFindButton.addActionListener(this);
    previousButton = new JButton(_("Previous"));
    previousButton.addActionListener(this);
    findButton = new JButton(_("Find"));
    findButton.addActionListener(this);

    JPanel buttonPanel = new JPanel();
    buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.LINE_AXIS));

    // ordering of buttons is different on mac versus pc
    if (OSUtils.isMacOS()) {
      buttonPanel.add(replaceAllButton);
      buttonPanel.add(Box.createRigidArea(new Dimension(8, 0)));
      buttonPanel.add(replaceButton);
      buttonPanel.add(Box.createRigidArea(new Dimension(8, 0)));
      buttonPanel.add(replaceFindButton);
      buttonPanel.add(Box.createRigidArea(new Dimension(8, 0)));
      buttonPanel.add(previousButton);
      buttonPanel.add(Box.createRigidArea(new Dimension(8, 0)));
      buttonPanel.add(findButton);

    } else {
      buttonPanel.add(findButton);
      buttonPanel.add(Box.createRigidArea(new Dimension(8, 0)));
      buttonPanel.add(previousButton); // is this the right position for
                                       // non-Mac?
      buttonPanel.add(Box.createRigidArea(new Dimension(8, 0)));
      buttonPanel.add(replaceFindButton);
      buttonPanel.add(Box.createRigidArea(new Dimension(8, 0)));
      buttonPanel.add(replaceButton);
      buttonPanel.add(Box.createRigidArea(new Dimension(8, 0)));
      buttonPanel.add(replaceAllButton);
    }

    // to fix ugliness.. normally macosx java 1.3 puts an
    // ugly white border around this object, so turn it off.
    if (OSUtils.isMacOS()) {
      buttonPanel.setBorder(null);
    }

    // Put all components onto the dialog window
    GridBagLayout searchLayout = new GridBagLayout();
    GridBagConstraints gbc = new GridBagConstraints();
    Container pane = getContentPane();
    pane.setLayout(searchLayout);
    
    gbc.insets = new Insets(4, 4, 4, 4);
    gbc.gridx = 0;
    gbc.weightx = 0.0;
    gbc.weighty = 0.0;
    gbc.fill = GridBagConstraints.NONE;
    gbc.anchor = GridBagConstraints.LINE_END;
    pane.add(findLabel, gbc);
    gbc.gridx = 1;
    gbc.weightx = 1.0;
    gbc.fill = GridBagConstraints.HORIZONTAL;
    gbc.anchor = GridBagConstraints.LINE_START;
    pane.add(findField, gbc);
    gbc.gridx = 0;
    gbc.gridy = 1;
    gbc.weightx = 0.0;
    gbc.fill = GridBagConstraints.NONE;
    gbc.anchor = GridBagConstraints.LINE_END;
    pane.add(replaceLabel, gbc);
    gbc.gridx = 1;
    gbc.weightx = 1.0;
    gbc.fill = GridBagConstraints.HORIZONTAL;
    gbc.anchor = GridBagConstraints.LINE_START;
    pane.add(replaceField, gbc);
    gbc.gridx = 1;
    gbc.gridy = 2;
    gbc.weighty = 0.0;
    gbc.fill = GridBagConstraints.NONE;
    pane.add(checkboxPanel, gbc);
    gbc.anchor = GridBagConstraints.CENTER;
    gbc.gridwidth = 2;
    gbc.gridx = 0;
    gbc.gridy = 3;
    gbc.insets = new Insets(12, 4, 4, 4);
    pane.add(buttonPanel, gbc);

    pack();
    setResizable(false);
    // centers the dialog on thew screen
    setLocationRelativeTo(null);

    // make the find button the blinky default
    getRootPane().setDefaultButton(findButton);

    setDefaultCloseOperation(WindowConstants.DO_NOTHING_ON_CLOSE);
    addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent e) {
        handleClose();
      }
    });
    Base.registerWindowCloseKeys(getRootPane(), new ActionListener() {
      public void actionPerformed(ActionEvent actionEvent) {
        // hide();
        handleClose();
      }
    });
    Base.setIcon(this);

    // hack to to get first field to focus properly on osx
    addWindowListener(new WindowAdapter() {
      public void windowActivated(WindowEvent e) {
        // System.out.println("activating");
        /* boolean ok = */findField.requestFocusInWindow();
        // System.out.println("got " + ok);
        findField.selectAll();
      }
    });
  }

  public void handleClose() {
    // System.out.println("handling close now");
    findString = findField.getText();
    replaceString = replaceField.getText();

    // this object should eventually become dereferenced
    setVisible(false);
  }

  /*
  public void show() {
    findField.requestFocusInWindow();
    super.show();
    //findField.selectAll();
    //findField.requestFocus();
  }
  */


  public void actionPerformed(ActionEvent e) {
    Object source = e.getSource();

    if (source == findButton) {
      findNext();

    } else if (source == previousButton) {
      findPrevious();

    } else if (source == replaceFindButton) {
      replaceAndFindNext();

    } else if (source == replaceButton) {
      replace();

    } else if (source == replaceAllButton) {
      replaceAll();
    }
  }

  // look for the next instance of the find string to be found
  // once found, select it (and go to that line)

  private boolean find(boolean wrap, boolean backwards, boolean searchTabs,
                       int originTab) {
    // System.out.println("Find: " + originTab);
    boolean wrapNeeded = false;
    String search = findField.getText();
    // System.out.println("finding for " + search + " " + findString);
    // this will catch "find next" being called when no search yet
    if (search.length() == 0)
      return false;

    String text = editor.getText();

    if (ignoreCase) {
      search = search.toLowerCase();
      text = text.toLowerCase();
    }

    int nextIndex;
    if (!backwards) {
      // int selectionStart = editor.textarea.getSelectionStart();
      int selectionEnd = editor.getSelectionStop();

      nextIndex = text.indexOf(search, selectionEnd);
      if (wrap && nextIndex == -1) {
        // if wrapping, a second chance is ok, start from beginning
        wrapNeeded = true;
      }
    } else {
      // int selectionStart = editor.textarea.getSelectionStart();
      int selectionStart = editor.getSelectionStart() - 1;

      if (selectionStart >= 0) {
        nextIndex = text.lastIndexOf(search, selectionStart);
      } else {
        nextIndex = -1;
      }
      if (wrap && nextIndex == -1) {
        // if wrapping, a second chance is ok, start from the end
        wrapNeeded = true;
      }
    }

    if (nextIndex == -1) {
      // Nothing found on this tab: Search other tabs if required
      if (searchTabs) {
        // editor.
        Sketch sketch = editor.getSketch();
        if (sketch.getCodeCount() > 1) {
          int realCurrentTab = sketch.getCodeIndex(sketch.getCurrentCode());

          if (originTab != realCurrentTab) {
            if (originTab < 0)
              originTab = realCurrentTab;

            if (!wrap)
              if ((!backwards && realCurrentTab + 1 >= sketch.getCodeCount())
                  || (backwards && realCurrentTab - 1 < 0))
                return false; // Can't continue without wrap

            if (backwards) {
              sketch.handlePrevCode();
              this.setVisible(true);
              int l = editor.getText().length() - 1;
              editor.setSelection(l, l);
            } else {
              sketch.handleNextCode();
              this.setVisible(true);
              editor.setSelection(0, 0);
            }

            return find(wrap, backwards, searchTabs, originTab);
          }
        }
      }

      if (wrapNeeded)
        nextIndex = backwards ? text.lastIndexOf(search) : text.indexOf(search,
                                                                        0);
    }

    if (nextIndex != -1) {
      editor.setSelection(nextIndex, nextIndex + search.length());
      return true;
    }

    return false;
  }

  /**
   * Replace the current selection with whatever's in the replacement text
   * field.
   */
  public void replace() {
    if (findField.getText().length() == 0)
      return;

    int newpos = editor.getSelectionStart() - findField.getText().length();
    if (newpos < 0)
      newpos = 0;
    editor.setSelection(newpos, newpos);

    boolean foundAtLeastOne = false;

    if (find(false, false, searchAllFiles, -1)) {
      foundAtLeastOne = true;
      editor.setSelectedText(replaceField.getText());
      editor.getSketch().setModified(true); // TODO is this necessary?
    }

    if (!foundAtLeastOne) {
      Toolkit.getDefaultToolkit().beep();
    }

  }

  /**
   * Replace the current selection with whatever's in the replacement text
   * field, and then find the next match
   */
  public void replaceAndFindNext() {
    replace();
    findNext();
  }

  /**
   * Replace everything that matches by doing find and replace alternately until
   * nothing more found.
   */
  public void replaceAll() {
    if (findField.getText().length() == 0)
      return;
    // move to the beginning
    editor.setSelection(0, 0);

    boolean foundAtLeastOne = false;
    while (true) {
      if (find(false, false, searchAllFiles, -1)) {
        foundAtLeastOne = true;
        editor.setSelectedText(replaceField.getText());
        editor.getSketch().setModified(true); // TODO is this necessary?
      } else {
        break;
      }
    }
    if (!foundAtLeastOne) {
      Toolkit.getDefaultToolkit().beep();
    }
  }

  public void setFindText(String t) {
    findField.setText(t);
    findString = t;
  }

  public void findNext() {
    if (!find(wrapAround, false, searchAllFiles, -1)) {
      Toolkit.getDefaultToolkit().beep();
    }
  }

  public void findPrevious() {
    if (!find(wrapAround, true, searchAllFiles, -1)) {
      Toolkit.getDefaultToolkit().beep();
    }
  }

}
