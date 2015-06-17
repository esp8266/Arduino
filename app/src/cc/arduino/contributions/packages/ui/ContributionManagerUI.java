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

package cc.arduino.contributions.packages.ui;

import cc.arduino.contributions.DownloadableContribution;
import cc.arduino.contributions.packages.ContributedPlatform;
import cc.arduino.contributions.packages.ContributionInstaller;
import cc.arduino.contributions.packages.ContributionsIndexer;
import cc.arduino.contributions.ui.*;
import cc.arduino.utils.Progress;
import processing.app.I18n;
import processing.app.Platform;

import javax.swing.*;
import java.awt.*;
import java.util.Collection;
import java.util.LinkedList;
import java.util.List;

import static processing.app.I18n._;

@SuppressWarnings("serial")
public class ContributionManagerUI extends InstallerJDialog {

  private final Platform platform;

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
      protected void onInstall(ContributedPlatform selected, ContributedPlatform installed) {
        if (selected.isReadOnly()) {
          onRemovePressed(installed, false);
        } else {
          onInstallPressed(selected, installed);
        }
      }

      @Override
      protected void onRemove(ContributedPlatform installedPlatform) {
        onRemovePressed(installedPlatform, true);
      }
    };
  }

  public ContributionManagerUI(Frame parent, Platform platform) {
    super(parent, _("Boards Manager"), Dialog.ModalityType.APPLICATION_MODAL, _("Unable to reach Arduino.cc due to possible network issues."));
    this.platform = platform;
  }

  public void setIndexer(ContributionsIndexer indexer) {
    DropdownItem<DownloadableContribution> previouslySelectedCategory = (DropdownItem<DownloadableContribution>) categoryChooser.getSelectedItem();

    categoryChooser.removeActionListener(categoryChooserActionListener);

    getContribModel().setIndexer(indexer);

    categoryFilter = null;
    categoryChooser.removeAllItems();

    filterField.setEnabled(getContribModel().getRowCount() > 0);

    categoryChooser.addActionListener(categoryChooserActionListener);

    // Enable categories combo only if there are two or more choices
    categoryChooser.addItem(new DropdownAllCoresItem());
    Collection<String> categories = indexer.getCategories();
    for (String s : categories) {
      categoryChooser.addItem(new DropdownCoreOfCategoryItem(s));
    }
    if (previouslySelectedCategory != null) {
      categoryChooser.setSelectedItem(previouslySelectedCategory);
    } else {
      categoryChooser.setSelectedIndex(0);
    }

    // Create ConstributionInstaller tied with the provided index
    installer = new ContributionInstaller(indexer, platform) {
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
    super.onCancelPressed();
    if (installerThread != null) {
      installerThread.interrupt();
    }
  }

  @Override
  public void onUpdatePressed() {
    super.onUpdatePressed();
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true, "");
          List<String> downloadedPackageIndexFiles = installer.updateIndex();
          installer.deleteUnknownFiles(downloadedPackageIndexFiles);
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

  public void onInstallPressed(final ContributedPlatform platformToInstall, final ContributedPlatform platformToRemove) {
    clearErrorMessage();
    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        List<String> errors = new LinkedList<String>();
        try {
          setProgressVisible(true, _("Installing..."));
          errors.addAll(installer.install(platformToInstall));
          if (platformToRemove != null && !platformToRemove.isReadOnly()) {
            errors.addAll(installer.remove(platformToRemove));
          }
          onIndexesUpdated();
        } catch (Exception e) {
          throw new RuntimeException(e);
        } finally {
          setProgressVisible(false, "");
          if (!errors.isEmpty()) {
            setErrorMessage(errors.get(0));
          }
        }
      }
    });
    installerThread.setUncaughtExceptionHandler(new InstallerJDialogUncaughtExceptionHandler(this, noConnectionErrorMessage));
    installerThread.start();
  }

  public void onRemovePressed(final ContributedPlatform platform, boolean showWarning) {
    clearErrorMessage();

    if (showWarning) {
      int chosenOption = JOptionPane.showConfirmDialog(this, I18n.format(_("Do you want to remove {0}?\nIf you do so you won't be able to use {0} any more."), platform.getName()), _("Please confirm boards deletion"), JOptionPane.YES_NO_OPTION, JOptionPane.QUESTION_MESSAGE);
      if (chosenOption != JOptionPane.YES_OPTION) {
        return;
      }
    }

    installerThread = new Thread(new Runnable() {
      @Override
      public void run() {
        try {
          setProgressVisible(true, _("Removing..."));
          installer.remove(platform);
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

  /**
   * Callback invoked when indexes are updated
   *
   * @throws Exception
   */
  protected void onIndexesUpdated() throws Exception {
    // Empty
  }

}
