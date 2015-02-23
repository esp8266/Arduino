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
package cc.arduino.libraries.contributions.ui;

import java.awt.Dialog;
import java.awt.Frame;
import java.util.Collection;

import cc.arduino.libraries.contributions.ContributedLibrary;
import cc.arduino.libraries.contributions.LibrariesIndexer;
import cc.arduino.ui.FilteredAbstractTableModel;
import cc.arduino.ui.InstallerJDialog;
import cc.arduino.ui.InstallerTableCell;
import cc.arduino.utils.Progress;

@SuppressWarnings("serial")
public class LibraryManagerUI extends InstallerJDialog {

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
      protected void onInstall(ContributedLibrary selectedPlatform) {
        onInstallPressed(selectedPlatform);
      }

      @Override
      protected void onRemove(ContributedLibrary installedPlatform) {
        onRemovePressed(installedPlatform);
      }
    };
  }

  public LibraryManagerUI(Frame parent) {
    super(parent, "Library Manager", Dialog.ModalityType.APPLICATION_MODAL);
  }

  public void setIndexer(LibrariesIndexer indexer) {
    getContribModel().removeTableModelListener(tableModelListener);
    categoryChooser.removeActionListener(categoryChooserActionListener);

    // TODO: Remove setIndexer and make getContribModel 
    // return a FilteredAbstractTableModel
    getContribModel().setIndexer(indexer);

    category = null;
    categoryChooser.removeAllItems();

    getContribModel().addTableModelListener(tableModelListener);
    categoryChooser.addActionListener(categoryChooserActionListener);

    // Load categories
    Collection<String> categories = indexer.getIndex().getCategories();
    categoryChooser.addItem("");
    for (String s : categories)
      categoryChooser.addItem(s);

    // Enable categories combo only if there are two or more choices
    int count = categoryChooser.getItemCount();
    categoryChooser.setEnabled(count > 1);

    filterField.setEnabled(contribModel.getRowCount() > 0);

    // Create LibrariesInstaller tied with the provided index
    installer = new LibraryInstaller(indexer) {
      @Override
      public void onProgress(Progress progress) {
        setProgress(progress);
      }
    };
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
    if (installerThread != null)
      installerThread.interrupt();
  }

  @Override
  protected void onUpdatePressed() {
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true);
          installer.updateIndex();
          onIndexesUpdated();
        } catch (Exception e) {
          throw new RuntimeException(e);
        } finally {
          setProgressVisible(false);
        }
      }
    });
    installerThread.start();
  }

  public void onInstallPressed(final ContributedLibrary lib) {
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true);
          installer.install(lib);
          getContribModel().updateLibrary(lib);
        } catch (Exception e) {
          throw new RuntimeException(e);
        } finally {
          setProgressVisible(false);
        }
      }
    });
    installerThread.start();
  }

  public void onRemovePressed(final ContributedLibrary lib) {
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true);
          installer.remove(lib);
          getContribModel().updateLibrary(lib);
        } catch (Exception e) {
          throw new RuntimeException(e);
        } finally {
          setProgressVisible(false);
        }
      }
    });
    installerThread.start();
  }

  protected void onIndexesUpdated() throws Exception {
    // Empty
  }

}
