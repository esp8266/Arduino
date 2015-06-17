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

import cc.arduino.contributions.DownloadableContribution;
import cc.arduino.contributions.libraries.ContributedLibrary;
import cc.arduino.contributions.libraries.LibrariesIndexer;
import cc.arduino.contributions.libraries.LibraryInstaller;
import cc.arduino.contributions.libraries.LibraryTypeComparator;
import cc.arduino.contributions.ui.*;
import cc.arduino.utils.Progress;
import com.google.common.base.Predicate;
import processing.app.Platform;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Collection;
import java.util.Collections;
import java.util.LinkedList;

import static processing.app.I18n._;

@SuppressWarnings("serial")
public class LibraryManagerUI extends InstallerJDialog<ContributedLibrary> {

  private final JComboBox typeChooser;
  private final Platform platform;
  private LibrariesIndexer indexer;
  private Predicate<ContributedLibrary> typeFilter;

  @Override
  protected FilteredAbstractTableModel createContribModel() {
    return new LibrariesIndexTableModel();
  }

  private LibrariesIndexTableModel getContribModel() {
    return (LibrariesIndexTableModel) contribModel;
  }

  @Override
  protected InstallerTableCell createCellRenderer() {
    return new ContributedLibraryTableCell();
  }

  @Override
  protected InstallerTableCell createCellEditor() {
    return new ContributedLibraryTableCell() {
      @Override
      protected void onInstall(ContributedLibrary selectedLibrary, ContributedLibrary installedLibrary) {
        if (selectedLibrary.isReadOnly()) {
          onRemovePressed(installedLibrary);
        } else {
          onInstallPressed(selectedLibrary, installedLibrary);
        }
      }

      @Override
      protected void onRemove(ContributedLibrary library) {
        onRemovePressed(library);
      }
    };
  }

  public LibraryManagerUI(Frame parent, Platform platform) {
    super(parent, "Library Manager", Dialog.ModalityType.APPLICATION_MODAL, _("Unable to reach Arduino.cc due to possible network issues."));
    this.platform = platform;

    filtersContainer.add(new JLabel(_("Topic")), 1);
    filtersContainer.remove(2);

    typeChooser = new JComboBox();
    typeChooser.setMaximumRowCount(20);
    typeChooser.setEnabled(false);

    filtersContainer.add(Box.createHorizontalStrut(5), 0);
    filtersContainer.add(new JLabel(_("Type")), 1);
    filtersContainer.add(Box.createHorizontalStrut(5), 2);
    filtersContainer.add(typeChooser, 3);
  }

  protected final ActionListener typeChooserActionListener = new ActionListener() {

    @Override
    public void actionPerformed(ActionEvent event) {
      DropdownItem<ContributedLibrary> selected = (DropdownItem<ContributedLibrary>) typeChooser.getSelectedItem();
      if (typeFilter == null || !typeFilter.equals(selected)) {
        typeFilter = selected.getFilterPredicate();
        if (contribTable.getCellEditor() != null) {
          contribTable.getCellEditor().stopCellEditing();
        }
        updateIndexFilter(filters, categoryFilter, typeFilter);
      }
    }
  };

  @Override
  public void updateIndexFilter(String[] filters, Predicate<ContributedLibrary>... additionalFilters) {
    if (additionalFilters.length == 1) {
      additionalFilters = new Predicate[] { additionalFilters[0], typeFilter };
    }
    super.updateIndexFilter(filters, additionalFilters);
  }

  public void setIndexer(LibrariesIndexer indexer) {
    this.indexer = indexer;

    DropdownItem<DownloadableContribution> previouslySelectedCategory = (DropdownItem<DownloadableContribution>) categoryChooser.getSelectedItem();
    DropdownItem<DownloadableContribution> previouslySelectedType = (DropdownItem<DownloadableContribution>) typeChooser.getSelectedItem();

    categoryChooser.removeActionListener(categoryChooserActionListener);
    typeChooser.removeActionListener(typeChooserActionListener);

    // TODO: Remove setIndexer and make getContribModel 
    // return a FilteredAbstractTableModel
    getContribModel().setIndexer(indexer);

    categoryFilter = null;
    categoryChooser.removeAllItems();

    // Load categories
    categoryChooser.addItem(new DropdownAllItem());
    Collection<String> categories = indexer.getIndex().getCategories();
    for (String category : categories) {
      categoryChooser.addItem(new DropdownLibraryOfCategoryItem(category));
    }

    categoryChooser.setEnabled(categoryChooser.getItemCount() > 1);

    categoryChooser.addActionListener(categoryChooserActionListener);
    if (previouslySelectedCategory != null) {
      categoryChooser.setSelectedItem(previouslySelectedCategory);
    } else {
      categoryChooser.setSelectedIndex(0);
    }

    typeFilter = null;
    typeChooser.removeAllItems();
    typeChooser.addItem(new DropdownAllItem());
    typeChooser.addItem(new DropdownInstalledLibraryItem(indexer.getIndex()));
    java.util.List<String> types = new LinkedList<String>(indexer.getIndex().getTypes());
    Collections.sort(types, new LibraryTypeComparator());
    for (String type : types) {
      typeChooser.addItem(new DropdownLibraryOfTypeItem(type));
    }
    typeChooser.setEnabled(typeChooser.getItemCount() > 1);
    typeChooser.addActionListener(typeChooserActionListener);
    if (previouslySelectedType != null) {
      typeChooser.setSelectedItem(previouslySelectedType);
    } else {
      typeChooser.setSelectedIndex(0);
    }

    filterField.setEnabled(contribModel.getRowCount() > 0);

    // Create LibrariesInstaller tied with the provided index
    installer = new LibraryInstaller(indexer, platform) {
      @Override
      public void onProgress(Progress progress) {
        setProgress(progress);
      }
    };
  }

  public LibrariesIndexer getIndexer() {
    return indexer;
  }

  public void setProgress(Progress progress) {
    progressBar.setValue(progress);
  }

  /*
   * Installer methods follows
   */

  private LibraryInstaller installer;
  private Thread installerThread = null;

  @Override
  protected void onCancelPressed() {
    super.onUpdatePressed();
    if (installerThread != null) {
      installerThread.interrupt();
    }
  }

  @Override
  protected void onUpdatePressed() {
    super.onUpdatePressed();
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true, "");
          installer.updateIndex();
          onIndexesUpdated();
        } catch (Exception e) {
          throw new RuntimeException(e);
        } finally {
          setProgressVisible(false, "");
        }
      }
    });
    installerThread.setUncaughtExceptionHandler(new InstallerJDialogUncaughtExceptionHandler(this, noConnectionErrorMessage));
    installerThread.start();
  }

  public void onInstallPressed(final ContributedLibrary lib, final ContributedLibrary replaced) {
    clearErrorMessage();
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true, _("Installing..."));
          installer.install(lib, replaced);
          onIndexesUpdated(); // TODO: Do a better job in refreshing only the needed element
          //getContribModel().updateLibrary(lib);
        } catch (Exception e) {
          throw new RuntimeException(e);
        } finally {
          setProgressVisible(false, "");
        }
      }
    });
    installerThread.setUncaughtExceptionHandler(new InstallerJDialogUncaughtExceptionHandler(this, noConnectionErrorMessage));
    installerThread.start();
  }

  public void onRemovePressed(final ContributedLibrary lib) {
    boolean managedByIndex = indexer.getIndex().getLibraries().contains(lib);

    if (!managedByIndex) {
      int chosenOption = JOptionPane.showConfirmDialog(this, _("This library is not listed on Library Manager. You won't be able to resinstall it from here.\nAre you sure you want to delete it?"), _("Please confirm library deletion"), JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
      if (chosenOption != JOptionPane.YES_OPTION) {
        return;
      }
    }

    clearErrorMessage();
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true, _("Removing..."));
          installer.remove(lib);
          onIndexesUpdated(); // TODO: Do a better job in refreshing only the needed element
          //getContribModel().updateLibrary(lib);
        } catch (Exception e) {
          throw new RuntimeException(e);
        } finally {
          setProgressVisible(false, "");
        }
      }
    });
    installerThread.setUncaughtExceptionHandler(new InstallerJDialogUncaughtExceptionHandler(this, noConnectionErrorMessage));
    installerThread.start();
  }

  protected void onIndexesUpdated() throws Exception {
    // Empty
  }

}
