/*
 * This file is part of Arduino.
 *
 * Copyright 2014 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */
package cc.arduino.packages.contributions.ui;

import static cc.arduino.packages.contributions.ui.ContributionIndexTableModel.DESCRIPTION_COL;
import static cc.arduino.packages.contributions.ui.ContributionIndexTableModel.INSTALLED_COL;
import static cc.arduino.packages.contributions.ui.ContributionIndexTableModel.VERSION_COL;
import static processing.app.I18n._;

import java.awt.BorderLayout;
import java.awt.Color;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dialog;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Frame;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.List;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.ScrollPaneConstants;
import javax.swing.UIManager;
import javax.swing.border.EmptyBorder;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.table.TableColumn;
import javax.swing.table.TableColumnModel;

import cc.arduino.packages.contributions.ContributedPlatform;
import cc.arduino.packages.contributions.ContributionsIndex;

@SuppressWarnings("serial")
public class JContributionManagerDialog extends JDialog {

  private FilterField filterField;
  private JScrollPane scrollPane;
  private StatusPanel status;

  private JContributionManagerDialogListener listener = null;

  private String category;
  private JLabel categoryLabel;
  private JComboBox categoryChooser;
  private Component categoryStrut1;
  private Component categoryStrut2;
  private Component categoryStrut3;

  private ContributionIndexTableModel contribModel = new ContributionIndexTableModel();
  private JTable contribTable;

  public JContributionManagerDialog(Frame parent) {
    super(parent, "Boards Manager", Dialog.ModalityType.APPLICATION_MODAL);
    setResizable(true);

    Container pane = getContentPane();
    pane.setLayout(new BorderLayout());

    categoryStrut1 = Box.createHorizontalStrut(5);
    categoryStrut2 = Box.createHorizontalStrut(5);
    categoryStrut3 = Box.createHorizontalStrut(5);

    categoryLabel = new JLabel(_("Category:"));

    categoryChooser = new JComboBox();
    categoryChooser.setMaximumRowCount(20);
    categoryChooser.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent arg0) {
        notifyCategoryChange();
      }
    });

    setCategories(new ArrayList<String>());

    filterField = new FilterField();

    JPanel filterPanel = new JPanel();
    filterPanel.setLayout(new BoxLayout(filterPanel, BoxLayout.X_AXIS));
    pane.add(filterPanel, BorderLayout.NORTH);
    filterPanel.add(categoryStrut1);
    filterPanel.add(categoryLabel);
    filterPanel.add(categoryStrut2);
    filterPanel.add(categoryChooser);
    filterPanel.add(categoryStrut3);
    filterPanel.add(filterField);
    filterPanel.setBorder(new EmptyBorder(7, 7, 7, 7));

    contribTable = new JTable(contribModel);
    // contribTable.setTableHeader(null);
    // contribTable.setRowSelectionAllowed(false);
    contribTable.setSelectionMode(ListSelectionModel.SINGLE_SELECTION);
    contribTable.setColumnSelectionAllowed(false);
    contribTable.setDragEnabled(false);
    contribTable.setIntercellSpacing(new Dimension(0, 1));
    contribTable.setShowVerticalLines(false);
    contribTable.getTableHeader().setEnabled(false);
    // contribTable.addMouseListener(new MouseAdapter() {
    // @Override
    // public void mousePressed(MouseEvent e) {
    // if (listener == null)
    // return;
    // Point point = e.getPoint();
    // int row = contribTable.rowAtPoint(point);
    // int col = contribTable.columnAtPoint(point);
    // }
    // });
    TableColumnModel tcm = contribTable.getColumnModel();
    TableColumn descriptionCol = tcm.getColumn(DESCRIPTION_COL);
    TableColumn versionCol = tcm.getColumn(VERSION_COL);
    TableColumn installedCol = tcm.getColumn(INSTALLED_COL);

    descriptionCol.setCellRenderer(new ContributedPlatformTableCellRenderer());
    descriptionCol.setResizable(true);

    {
      versionCol.setCellRenderer(new VersionSelectorTableCellRenderer());
      VersionSelectorTableCellEditor editor = new VersionSelectorTableCellEditor();
      editor.setListener(new VersionSelectorTableCellEditor.Listener() {
        @Override
        public void onInstallEvent(int row) {
          if (listener == null)
            return;
          ContributedPlatform selected = contribModel.getSelectedRelease(row);
          listener.onInstall(selected);
        }
      });
      versionCol.setCellEditor(editor);
      versionCol.setResizable(false);
      versionCol.setWidth(140);
    }

    {
      installedCol.setCellRenderer(new VersionInstalledTableCellRenderer());
      VersionInstalledTableCellEditor editor = new VersionInstalledTableCellEditor();
      editor.setListener(new VersionInstalledTableCellEditor.Listener() {
        @Override
        public void onRemoveEvent(int row) {
          if (listener == null)
            return;
          ContributedPlatform installed = contribModel.getReleases(row)
              .getInstalled();
          listener.onRemove(installed);
        }
      });
      installedCol.setCellEditor(editor);
      installedCol.setResizable(false);
      installedCol.setMaxWidth(70);
    }

    scrollPane = new JScrollPane();
    scrollPane.setPreferredSize(new Dimension(300, 300));
    scrollPane.setViewportView(contribTable);
    scrollPane
        .setVerticalScrollBarPolicy(ScrollPaneConstants.VERTICAL_SCROLLBAR_ALWAYS);
    scrollPane
        .setHorizontalScrollBarPolicy(ScrollPaneConstants.HORIZONTAL_SCROLLBAR_NEVER);
    pane.add(scrollPane, BorderLayout.CENTER);

    pane.add(Box.createHorizontalStrut(10), BorderLayout.WEST);
    pane.add(Box.createHorizontalStrut(10), BorderLayout.EAST);

    status = new StatusPanel();
    status.setBorder(new EmptyBorder(7, 7, 7, 7));
    pane.add(status, BorderLayout.SOUTH);

    setMinimumSize(new Dimension(450, 400));
  }

  public void setListener(JContributionManagerDialogListener listener) {
    this.listener = listener;
  }

  public void setCategories(Collection<String> categories) {
    category = null;
    categoryChooser.removeAllItems();
    for (String s : categories)
      categoryChooser.addItem(s);

    // Disable if only one possible choice
    boolean single = categories.size() == 1;
    categoryChooser.setEnabled(!single);

    // Show if there is at lease one possible choice
    boolean show = !categories.isEmpty();
    categoryStrut1.setVisible(show);
    categoryLabel.setVisible(show);
    categoryStrut2.setVisible(show);
    categoryChooser.setVisible(show);
    categoryStrut3.setVisible(show);
  }

  private synchronized void notifyCategoryChange() {
    if (listener == null)
      return;
    String selected = (String) categoryChooser.getSelectedItem();
    if (category == null || !category.equals(selected)) {
      category = selected;
      listener.onCategoryChange(category);
    }
  }

  class FilterField extends JTextField {
    final static String filterHint = "Filter your search...";
    boolean showingHint;
    List<String> filters;

    public FilterField() {
      super(filterHint);

      showingHint = true;
      filters = new ArrayList<String>();
      updateStyle();

      addFocusListener(new FocusListener() {
        public void focusLost(FocusEvent focusEvent) {
          if (filterField.getText().isEmpty()) {
            showingHint = true;
          }
          updateStyle();
        }

        public void focusGained(FocusEvent focusEvent) {
          if (showingHint) {
            showingHint = false;
            filterField.setText("");
          }
          updateStyle();
        }
      });

      getDocument().addDocumentListener(new DocumentListener() {
        public void removeUpdate(DocumentEvent e) {
          applyFilter();
        }

        public void insertUpdate(DocumentEvent e) {
          applyFilter();
        }

        public void changedUpdate(DocumentEvent e) {
          applyFilter();
        }
      });
    }

    public void applyFilter() {
      String filter = filterField.getFilterText();
      filter = filter.toLowerCase();

      // Replace anything but 0-9, a-z, or : with a space
      filter = filter.replaceAll("[^\\x30-\\x39^\\x61-\\x7a^\\x3a]", " ");
      filters = Arrays.asList(filter.split(" "));
      // filterLibraries(category, filters);
    }

    public String getFilterText() {
      return showingHint ? "" : getText();
    }

    public void updateStyle() {
      if (showingHint) {
        setText(filterHint);
        setForeground(Color.gray);
        setFont(getFont().deriveFont(Font.ITALIC));
      } else {
        setForeground(UIManager.getColor("TextField.foreground"));
        setFont(getFont().deriveFont(Font.PLAIN));
      }
    }
  }

  public void addContributions(ContributionsIndex index) {
    contribModel.updateIndex(index);
  }

}
