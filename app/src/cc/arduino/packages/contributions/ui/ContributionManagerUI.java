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

import java.awt.Dialog;
import java.awt.Frame;
import java.util.Collection;

import cc.arduino.packages.contributions.ContributedPlatform;
import cc.arduino.packages.contributions.ContributionInstaller;
import cc.arduino.packages.contributions.ContributionsIndexer;
import cc.arduino.ui.FilteredAbstractTableModel;
import cc.arduino.ui.InstallerJDialog;
import cc.arduino.ui.InstallerTableCell;
import cc.arduino.utils.Progress;

@SuppressWarnings("serial")
public class ContributionManagerUI extends InstallerJDialog {

  // private ContributedPlatformTableCell cellEditor;

  @Override
  protected FilteredAbstractTableModel createContribModel() {
    return new ContributionIndexTableModel();
  }

  private ContributionIndexTableModel getContribModel() {
    return (ContributionIndexTableModel) contribModel;
  }

  @Override
  protected InstallerTableCell createCellRenderer() {
    return new ContributedPlatformTableCell();
  }

  @Override
  protected InstallerTableCell createCellEditor() {
    return new ContributedPlatformTableCell() {
      @Override
      protected void onInstall(ContributedPlatform selectedPlatform) {
        onInstallPressed(selectedPlatform);
      }

      @Override
      protected void onRemove(ContributedPlatform installedPlatform) {
        onRemovePressed(installedPlatform);
      }
    };
  }

  public ContributionManagerUI(Frame parent) {
    super(parent, "Boards Manager", Dialog.ModalityType.APPLICATION_MODAL);
  }

  public void setIndexer(ContributionsIndexer indexer) {
    getContribModel().removeTableModelListener(tableModelListener);
    categoryChooser.removeActionListener(categoryChooserActionListener);

    getContribModel().setIndex(indexer.getIndex());

    category = null;
    categoryChooser.removeAllItems();

    filterField.setEnabled(getContribModel().getRowCount() > 0);

    getContribModel().addTableModelListener(tableModelListener);
    categoryChooser.addActionListener(categoryChooserActionListener);

    // Enable categories combo only if there are two or more choices
    Collection<String> categories = indexer.getIndex().getCategories();
    int count = categories.size();
    categoryChooser.setEnabled(count > 1);

    for (String s : categories)
      categoryChooser.addItem(s);

    // Create ConstributionInstaller tied with the provided index
    installer = new ContributionInstaller(indexer) {
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

  private ContributionInstaller installer;
  private Thread installerThread = null;

  @Override
  public void onCancelPressed() {
    if (installerThread != null)
      installerThread.interrupt();
  }

  @Override
  public void onUpdatePressed() {
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true);
          installer.updateIndex();
          onIndexesUpdated();
        } catch (Exception e) {
          // TODO Show ERROR
          e.printStackTrace();
        } finally {
          setProgressVisible(false);
        }
      }
    });
    installerThread.start();
  }

  public void onInstallPressed(final ContributedPlatform platform) {
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true);
          installer.install(platform);
        } catch (Exception e) {
          // TODO Show ERROR
          e.printStackTrace();
        } finally {
          setProgressVisible(false);
        }
      }
    });
    installerThread.start();
  }

  public void onRemovePressed(final ContributedPlatform platform) {
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true);
          installer.remove(platform);
        } catch (Exception e) {
          // TODO Show ERROR
          e.printStackTrace();
        } finally {
          setProgressVisible(false);
        }
      }
    });
    installerThread.start();
  }

  /**
   * Callback invoked when indexes are updated
   * 
   * @throws Exception
   */
  protected void onIndexesUpdated() throws Exception {
    // Empty
  }

}
