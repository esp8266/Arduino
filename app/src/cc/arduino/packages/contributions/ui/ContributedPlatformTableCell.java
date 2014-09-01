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

import static processing.app.I18n._;
import static processing.app.I18n.format;

import java.awt.Color;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Insets;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JPanel;
import javax.swing.JTable;
import javax.swing.JTextPane;
import javax.swing.border.EmptyBorder;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.text.Document;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.StyleSheet;

import processing.app.Base;
import cc.arduino.packages.contributions.ContributedBoard;
import cc.arduino.packages.contributions.ContributedPlatform;
import cc.arduino.packages.contributions.ui.ContributionIndexTableModel.ContributedPlatformReleases;
import cc.arduino.ui.InstallerTableCell;

@SuppressWarnings("serial")
public class ContributedPlatformTableCell extends InstallerTableCell {

  private JPanel panel;
  private JTextPane description;
  private JButton installButton;
  private JButton removeButton;
  private Component removeButtonPlaceholder;

  public ContributedPlatformTableCell() {
    description = new JTextPane();
    description.setInheritsPopupMenu(true);
    Insets margin = description.getMargin();
    margin.bottom = 0;
    description.setMargin(margin);
    description.setContentType("text/html");
    Document doc = description.getDocument();
    if (doc instanceof HTMLDocument) {
      HTMLDocument html = (HTMLDocument) doc;
      StyleSheet stylesheet = html.getStyleSheet();
      stylesheet.addRule("body { margin: 0; padding: 0;"
          + "font-family: Verdana, Geneva, Arial, Helvetica, sans-serif;"
          + "font-size: 100%;" + "font-size: 0.95em; }");
    }
    description.setOpaque(false);
    description.setBorder(new EmptyBorder(4, 7, 7, 7));
    description.setHighlighter(null);
    description.setEditable(false);
    description.addHyperlinkListener(new HyperlinkListener() {
      @Override
      public void hyperlinkUpdate(HyperlinkEvent e) {
        if (e.getEventType() == HyperlinkEvent.EventType.ACTIVATED) {
          Base.openURL(e.getDescription());
        }
      }
    });

    installButton = new JButton(_("Install"));
    installButton.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        onInstall(editorValue.getSelected());
      }
    });

    removeButton = new JButton(_("Remove"));
    removeButton.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        onRemove(editorValue.getInstalled());
      }
    });

    int width = removeButton.getPreferredSize().width;
    removeButtonPlaceholder = Box.createRigidArea(new Dimension(width, 1));

    panel = new JPanel();
    panel.setLayout(new BoxLayout(panel, BoxLayout.X_AXIS));

    panel.add(description);
    panel.add(Box.createHorizontalStrut(5));
    panel.add(installButton);
    panel.add(Box.createHorizontalStrut(5));
    panel.add(removeButton);
    panel.add(removeButtonPlaceholder);
    panel.add(Box.createHorizontalStrut(5));
  }

  protected void onRemove(ContributedPlatform contributedPlatform) {
    // Empty
  }

  protected void onInstall(ContributedPlatform contributedPlatform) {
    // Empty
  }

  public Component getTableCellRendererComponent(JTable table, Object value,
                                                 boolean isSelected,
                                                 boolean hasFocus, int row,
                                                 int column) {
    parentTable = table;
    Component panel = getUpdatedCellComponent(value, isSelected, row);
    return panel;
  }

  private ContributedPlatformReleases editorValue;
  private JTable parentTable;

  @Override
  public Object getCellEditorValue() {
    return editorValue;
  }

  @Override
  public Component getTableCellEditorComponent(JTable table, Object value,
                                               boolean isSelected, int row,
                                               int column) {
    parentTable = table;
    editorValue = (ContributedPlatformReleases) value;
    return getUpdatedCellComponent(value, true, row);
  }

  private Component getUpdatedCellComponent(Object value, boolean isSelected,
                                            int row) {
    ContributedPlatformReleases releases = (ContributedPlatformReleases) value;
    ContributedPlatform selectedPlatform = releases.getSelected();
    ContributedPlatform installedPlatform = releases.getInstalled();

    boolean removable, installable, upgradable;
    if (installedPlatform == null) {
      installable = true;
      removable = false;
      upgradable = false;
    } else {
      installable = false;
      removable = true;
      upgradable = (selectedPlatform != installedPlatform);
    }

    if (installable)
      installButton.setText(_("Install"));
    if (upgradable)
      installButton.setText(_("Upgrade"));
    installButton.setVisible(installable || upgradable);

    removeButton.setVisible(removable);
    removeButtonPlaceholder.setVisible(!removable);

    String desc = "<html><body>";
    desc += "<b>" + selectedPlatform.getName() + "</b>";
    String author = selectedPlatform.getParentPackage().getMaintainer();
    String url = selectedPlatform.getParentPackage().getWebsiteURL();
    if (author != null && !author.isEmpty()) {
      desc += format(" by <a href=\"{0}\">{1}</a>", url, author);
    }
    desc += "<br />";
    desc += "<br />";

    desc += _("Boards contributed in this package:") + "<br />";
    for (ContributedBoard board : selectedPlatform.getBoards())
      desc += format("- {0}<br />", board.getName());
    desc += "<br />";
    desc += format(_("Available version: <b>{0}</b>"),
                   selectedPlatform.getVersion());
    desc += "<br />";
    if (removable) {
      desc += format(_("Installed version: <b>{0}</b>"),
                     installedPlatform.getVersion());
    }
    desc += "<br />";
    desc += "</body></html>";
    description.setText(desc);
    description.setBackground(Color.WHITE);

    if (isSelected) {
      panel.setBackground(parentTable.getSelectionBackground());
      panel.setForeground(parentTable.getSelectionForeground());
    } else {
      panel.setBackground(parentTable.getBackground());
      panel.setForeground(parentTable.getForeground());
    }

    return panel;
  }

  @Override
  public void setEnabled(boolean enabled) {
    installButton.setEnabled(enabled);
    removeButton.setEnabled(enabled);
  }

  public void invalidate() {
    panel.invalidate();
  }

}
