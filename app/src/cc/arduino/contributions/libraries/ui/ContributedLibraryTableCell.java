/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
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
package cc.arduino.contributions.libraries.ui;

import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.libraries.ContributedLibraryComparator;
import cc.arduino.contributions.libraries.filters.BuiltInPredicate;
import cc.arduino.contributions.libraries.filters.InstalledPredicate;
import cc.arduino.contributions.libraries.filters.OnlyUpstreamReleasePredicate;
import cc.arduino.contributions.VersionComparator;
import cc.arduino.contributions.ui.InstallerTableCell;
import cc.arduino.utils.ReverseComparator;
import com.google.common.base.Function;
import com.google.common.base.Predicates;
import com.google.common.collect.Collections2;
import com.google.common.collect.Lists;
import processing.app.Base;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.event.HyperlinkEvent;
import javax.swing.event.HyperlinkListener;
import javax.swing.text.Document;
import javax.swing.text.html.HTMLDocument;
import javax.swing.text.html.StyleSheet;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.ItemEvent;
import java.awt.event.ItemListener;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

import static processing.app.I18n._;
import static processing.app.I18n.format;

@SuppressWarnings("serial")
public class ContributedLibraryTableCell extends InstallerTableCell {

  private JPanel panel;
  private JTextPane description;
  private JButton installButton;
  private JButton removeButton;
  private Component removeButtonPlaceholder;
  private Component installButtonPlaceholder;
  private JComboBox downgradeChooser;
  private JComboBox versionToInstallChooser;
  private JButton downgradeButton;
  private JPanel buttonsPanel;
  private Component removeButtonStrut;
  private JPanel inactiveButtonsPanel;
  private JLabel statusLabel;

  public ContributedLibraryTableCell() {
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

    {
      installButton = new JButton(_("Install"));
      installButton.addActionListener(new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
          onInstall(editorValue.getSelected(), editorValue.getInstalled());
        }
      });
      int width = installButton.getPreferredSize().width;
      installButtonPlaceholder = Box.createRigidArea(new Dimension(width, 1));
    }

    {
      removeButton = new JButton(_("Remove"));
      removeButton.addActionListener(new ActionListener() {
        @Override
        public void actionPerformed(ActionEvent e) {
          onRemove(editorValue.getInstalled());
        }
      });
      int width = removeButton.getPreferredSize().width;
      removeButtonPlaceholder = Box.createRigidArea(new Dimension(width, 1));
    }

    downgradeButton = new JButton(_("Install"));
    downgradeButton.addActionListener(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        ContributedLibrary selected = (ContributedLibrary) downgradeChooser.getSelectedItem();
        onInstall(selected, editorValue.getInstalled());
      }
    });

    downgradeChooser = new JComboBox();
    downgradeChooser.addItem("-");
    downgradeChooser.setMaximumSize(downgradeChooser.getPreferredSize());
    downgradeChooser.addItemListener(new ItemListener() {
      @Override
      public void itemStateChanged(ItemEvent e) {
        Object selectVersionItem = downgradeChooser.getItemAt(0);
        boolean disableDowngrade = (e.getItem() == selectVersionItem);
        downgradeButton.setEnabled(!disableDowngrade);
      }
    });

    versionToInstallChooser = new JComboBox();
    versionToInstallChooser.addItem("-");
    versionToInstallChooser.setMaximumSize(versionToInstallChooser.getPreferredSize());
    versionToInstallChooser.addItemListener(new ItemListener() {
      @Override
      public void itemStateChanged(ItemEvent e) {
        editorValue.select((ContributedLibrary) versionToInstallChooser.getSelectedItem());
      }
    });

    panel = new JPanel();
    panel.setLayout(new BoxLayout(panel, BoxLayout.Y_AXIS));

    panel.add(description);

    {
      buttonsPanel = new JPanel();
      buttonsPanel.setLayout(new BoxLayout(buttonsPanel, BoxLayout.X_AXIS));
      buttonsPanel.setOpaque(false);

      buttonsPanel.add(Box.createHorizontalStrut(7));
      buttonsPanel.add(downgradeChooser);
      buttonsPanel.add(Box.createHorizontalStrut(5));
      buttonsPanel.add(downgradeButton);

      buttonsPanel.add(Box.createHorizontalGlue());

      buttonsPanel.add(versionToInstallChooser);
      buttonsPanel.add(Box.createHorizontalStrut(5));
      buttonsPanel.add(installButton);
      buttonsPanel.add(Box.createHorizontalStrut(5));
      buttonsPanel.add(removeButton);
      removeButtonStrut = Box.createHorizontalStrut(5);
      buttonsPanel.add(removeButtonStrut);
      buttonsPanel.add(Box.createHorizontalStrut(15));

      panel.add(buttonsPanel);
    }

    {
      inactiveButtonsPanel = new JPanel();
      inactiveButtonsPanel.setLayout(new BoxLayout(inactiveButtonsPanel, BoxLayout.X_AXIS));
      inactiveButtonsPanel.setOpaque(false);

      int height = installButton.getMinimumSize().height;
      inactiveButtonsPanel.add(Box.createVerticalStrut(height));
      inactiveButtonsPanel.add(Box.createGlue());

      statusLabel = new JLabel(" ");
      inactiveButtonsPanel.add(statusLabel);

      inactiveButtonsPanel.add(Box.createGlue());
      inactiveButtonsPanel.add(Box.createVerticalStrut(height));

      panel.add(inactiveButtonsPanel);
    }

    panel.add(Box.createVerticalStrut(15));
  }

  protected void onRemove(ContributedLibrary selectedLib) {
    // Empty
  }

  protected void onInstall(ContributedLibrary selectedLib, ContributedLibrary installedLib) {
    // Empty
  }

  public Component getTableCellRendererComponent(JTable table, Object value,
                                                 boolean isSelected,
                                                 boolean hasFocus, int row,
                                                 int column) {
    parentTable = table;
    setEnabled(false);
    Component component = getUpdatedCellComponent(value, isSelected, row, false);
    if (row % 2 == 0) {
      component.setBackground(new Color(236, 241, 241)); //#ecf1f1
    } else {
      component.setBackground(new Color(255, 255, 255));
    }

    return component;
  }

  private LibrariesIndexTableModel.ContributedLibraryReleases editorValue;
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
    editorValue = (LibrariesIndexTableModel.ContributedLibraryReleases) value;
    setEnabled(true);

    final ContributedLibrary installedLibrary = editorValue.getInstalled();

    List<ContributedLibrary> libraries = new LinkedList<ContributedLibrary>(Collections2.filter(editorValue.releases, new OnlyUpstreamReleasePredicate()));
    List<ContributedLibrary> uninstalledLibraries = new LinkedList<ContributedLibrary>(Collections2.filter(libraries, Predicates.not(new InstalledPredicate())));

    List<ContributedLibrary> installedBuiltIn = new LinkedList<ContributedLibrary>(Collections2.filter(libraries, Predicates.and(new InstalledPredicate(), new BuiltInPredicate())));

    if (installedLibrary != null && !installedBuiltIn.contains(installedLibrary)) {
      uninstalledLibraries.addAll(installedBuiltIn);
    }

    Collections.sort(uninstalledLibraries, new ReverseComparator<ContributedLibrary>(new ContributedLibraryComparator()));

    downgradeChooser.removeAllItems();
    downgradeChooser.addItem(_("Select version"));

    final List<ContributedLibrary> uninstalledPreviousLibraries = Lists.newLinkedList();
    final List<ContributedLibrary> uninstalledNewerLibraries = Lists.newLinkedList();

    Lists.newLinkedList(Lists.transform(uninstalledLibraries, new Function<ContributedLibrary, ContributedLibrary>() {
      @Override
      public ContributedLibrary apply(ContributedLibrary input) {
        if (installedLibrary == null || VersionComparator.VERSION_COMPARATOR.greaterThan(installedLibrary.getVersion(), input.getVersion())) {
          uninstalledPreviousLibraries.add(input);
        } else {
          uninstalledNewerLibraries.add(input);
        }

        return input;
      }
    }));
    for (ContributedLibrary release : uninstalledNewerLibraries) {
      downgradeChooser.addItem(release);
    }
    for (ContributedLibrary release : uninstalledPreviousLibraries) {
      downgradeChooser.addItem(release);
    }

    downgradeChooser.setVisible(installedLibrary != null && (!uninstalledPreviousLibraries.isEmpty() || uninstalledNewerLibraries.size() > 1));
    downgradeButton.setVisible(installedLibrary != null && (!uninstalledPreviousLibraries.isEmpty() || uninstalledNewerLibraries.size() > 1));

    versionToInstallChooser.removeAllItems();
    for (ContributedLibrary release : uninstalledLibraries) {
      versionToInstallChooser.addItem(release);
    }
    versionToInstallChooser.setVisible(installedLibrary == null && uninstalledLibraries.size() > 1);

    Component component = getUpdatedCellComponent(value, true, row, !installedBuiltIn.isEmpty());
    component.setBackground(new Color(218, 227, 227)); //#dae3e3
    return component;
  }

  private Component getUpdatedCellComponent(Object value, boolean isSelected, int row, boolean hasBuiltInRelease) {
    LibrariesIndexTableModel.ContributedLibraryReleases releases = (LibrariesIndexTableModel.ContributedLibraryReleases) value;

    //FIXME: happens on macosx, don't know why
    if (releases == null) {
      return panel;
    }

    ContributedLibrary selectedLib = releases.getSelected();
    ContributedLibrary installedLib = releases.getInstalled();

    boolean removable, installable, upgradable;
    if (installedLib == null) {
      installable = true;
      removable = false;
      upgradable = false;
    } else {
      installable = false;
      removable = !installedLib.isReadOnly() && !hasBuiltInRelease;
      upgradable = new ContributedLibraryComparator().compare(selectedLib, installedLib) > 0;
    }
    if (installable) {
      installButton.setText(_("Install"));
    }
    if (upgradable) {
      installButton.setText(_("Update"));
    }
    installButton.setVisible(installable || upgradable);
    installButtonPlaceholder.setVisible(!(installable || upgradable));
    removeButton.setVisible(removable);
    removeButtonPlaceholder.setVisible(!removable);

    String name = selectedLib.getName();
    String author = selectedLib.getAuthor();
    // String maintainer = selectedLib.getMaintainer();
    String website = selectedLib.getWebsite();
    String sentence = selectedLib.getSentence();
    String paragraph = selectedLib.getParagraph();
    // String availableVer = selectedLib.getVersion();
    String url = selectedLib.getUrl();

    String midcolor = isSelected ? "#000000" : "#888888";

    String desc = "<html><body>";

    // Library name...
    desc += format("<b>{0}</b> ", name);
    if (installedLib != null && installedLib.isReadOnly()) {
      desc += "Built-In ";
    }

    // ...author...
    desc += format("<font color=\"{0}\">", midcolor);
    if (author != null && !author.isEmpty()) {
      desc += format(" by <b>{0}</b>", author);
    }

    // ...version.
    if (installedLib != null) {
      String installedVer = installedLib.getVersion();
      if (installedVer == null) {
        desc += " " + _("Version unknown");
      } else {
        desc += " " + format(_("Version <b>{0}</b>"), installedVer);
      }
    } else {
      // not installed...
    }
    desc += "</font>";

    if (installedLib != null) {
      desc += " <strong><font color=\"#00979D\">INSTALLED</font></strong>";
    }

    desc += "<br/>";

    // Description
    if (sentence != null) {
      desc += format("<b>{0}</b> ", sentence);
      if (paragraph != null && !paragraph.isEmpty())
        desc += format("{0}", paragraph);
      desc += "<br />";
    }
    if (author != null && !author.isEmpty()) {
      desc += format("<a href=\"{0}\">More info</a>", website);
    }

    desc += "</body></html>";
    description.setText(desc);
    description.setBackground(Color.WHITE);

    // for modelToView to work, the text area has to be sized. It doesn't
    // matter if it's visible or not.

    // See:
    // http://stackoverflow.com/questions/3081210/how-to-set-jtextarea-to-have-height-that-matches-the-size-of-a-text-it-contains
    int width = parentTable.getBounds().width;
    setJTextPaneDimensionToFitContainedText(description, width);

    if (isSelected) {
      panel.setBackground(parentTable.getSelectionBackground());
      panel.setForeground(parentTable.getSelectionForeground());
    } else {
      panel.setBackground(parentTable.getBackground());
      panel.setForeground(parentTable.getForeground());
    }

    return panel;
  }

  private Timer enabler = new Timer(100, new ActionListener() {
    @Override
    public void actionPerformed(ActionEvent e) {
      enable(true);
      enabler.stop();
    }
  });

  @Override
  public void setEnabled(boolean enabled) {
    enable(false);
    if (enabled) {
      enabler.start();
    } else {
      enabler.stop();
    }
    buttonsPanel.setVisible(enabled);
    inactiveButtonsPanel.setVisible(!enabled);
  }

  public void enable(boolean enabled) {
    installButton.setEnabled(enabled);
    removeButton.setEnabled(enabled);
  }

  public void invalidate() {
    panel.invalidate();
  }

}
