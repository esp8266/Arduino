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
package cc.arduino.packages.contributions;

import static processing.app.I18n._;
import static processing.app.I18n.format;

import java.io.File;
import java.net.URL;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Observable;
import java.util.Observer;

import processing.app.helpers.FileUtils;
import cc.arduino.utils.ArchiveExtractor;
import cc.arduino.utils.FileHash;
import cc.arduino.utils.network.FileDownloader;

public class ContributionInstaller {

  /**
   * Listener for installation progress.
   */
  public static interface Listener {
    /**
     * Receive the latest progress update.
     * 
     * @param progress
     *          Actual progress in the range 0...100
     * @param message
     *          A verbose description message of the actual operation
     */
    void onProgress(double progress, String message);
  }

  private Listener listener = null;

  private File stagingFolder;
  private ContributionsIndexer indexer;

  private double progress;
  private double progressStepsDelta;

  public void setListener(Listener listener) {
    this.listener = listener;
  }

  private void updateProgress(double progress, String message) {
    if (listener != null)
      listener.onProgress(progress, message);
  }

  public ContributionInstaller(ContributionsIndexer contributionsIndexer) {
    stagingFolder = contributionsIndexer.getStagingFolder();
    indexer = contributionsIndexer;
  }

  public void install(ContributedPlatform platform) throws Exception {
    if (platform.isInstalled())
      throw new Exception("Platform is already installed!");

    // Do not download already installed tools
    List<ContributedTool> tools = new LinkedList<ContributedTool>(platform.getResolvedTools());
    Iterator<ContributedTool> toolsIterator = tools.iterator();
    while (toolsIterator.hasNext()) {
      ContributedTool tool = toolsIterator.next();
      DownloadableContribution downloadable = tool.getDownloadableContribution();
      if (downloadable == null) {
        throw new Exception(format(_("Tool {0} is not available for your operating system."), tool.getName()));
      }
      if (downloadable.isInstalled()) {
        toolsIterator.remove();
      }
    }

    // Calculate progress increases
    progress = 0.0;
    progressStepsDelta = 100.0 / (tools.size() + 1) / 2.0;

    // Download all
    try {
      // Download platform
      download(platform, _("Downloading boards definitions."));

      // Download tools
      int i = 1;
      for (ContributedTool tool : tools) {
        String msg = format(_("Downloading tools ({0}/{1})."), i, tools.size());
        download(tool.getDownloadableContribution(), msg);
        i++;
      }
    } catch (InterruptedException e) {
      // Download interrupted... just exit
      return;
    }

    ContributedPackage pack = platform.getParentPackage();
    File packageFolder = new File(indexer.getPackagesFolder(), pack.getName());

    // TODO: Extract to temporary folders and move to the final destination only
    // once everything is successfully unpacked. If the operation fails remove
    // all the temporary folders and abort installation.

    // Unzip tools on the correct location
    File toolsFolder = new File(packageFolder, "tools");
    int i = 1;
    for (ContributedTool tool : platform.getResolvedTools()) {
      String msg = format(_("Installing tools ({0}/{1})..."), i, tools.size());
      updateProgress(progress, msg);
      i++;
      DownloadableContribution toolContrib = tool.getDownloadableContribution();
      File destFolder = new File(toolsFolder, tool.getName() + File.separator +
          tool.getVersion());

      destFolder.mkdirs();
      ArchiveExtractor.extract(toolContrib.getDownloadedFile(), destFolder, 1);
      toolContrib.setInstalled(true);
      toolContrib.setInstalledFolder(destFolder);
      progress += progressStepsDelta;
    }

    // Unpack platform on the correct location
    updateProgress(progress, _("Installing boards..."));
    File platformFolder = new File(packageFolder, "hardware" + File.separator +
        platform.getArchitecture());
    File destFolder = new File(platformFolder, platform.getVersion());
    destFolder.mkdirs();
    ArchiveExtractor.extract(platform.getDownloadedFile(), destFolder, 1);
    platform.setInstalled(true);
    platform.setInstalledFolder(destFolder);
    progress += progressStepsDelta;

    updateProgress(100.0, _("Installation completed!"));
  }

  public File download(DownloadableContribution contribution,
                       final String statusText) throws Exception {
    URL url = new URL(contribution.getUrl());
    String path = url.getPath();
    String fileName = path.substring(path.lastIndexOf('/') + 1);
    final File outputFile = new File(stagingFolder, fileName);

    // Ensure the existence of staging folder
    stagingFolder.mkdirs();

    // Need to download or resume downloading?
    if (!outputFile.isFile() || (outputFile.length() < contribution.getSize())) {

      // Use FileDownloader to retrieve the file
      FileDownloader downloader = new FileDownloader(url, outputFile);
      downloader.addObserver(new Observer() {
        @Override
        public void update(Observable o, Object arg) {
          FileDownloader me = (FileDownloader) o;
          String msg = "";
          if (me.getDownloadSize() != null) {
            long downloaded = me.getInitialSize() + me.getDownloaded() / 1000;
            long total = me.getInitialSize() + me.getDownloadSize() / 1000;
            msg = format(_("Downloaded {0}kb of {1}kb."), downloaded, total);
          }
          updateProgress((int) progress + progressStepsDelta *
              me.getProgress() / 100.0, statusText + " " + msg);
        }
      });
      downloader.download();
      if (!downloader.isCompleted())
        throw new Exception("Error dowloading " + url, downloader.getError());
    }
    progress += progressStepsDelta;

    // Test checksum
    updateProgress(progress, _("Verifying archive integrity..."));
    String checksum = contribution.getChecksum();
    String algo = checksum.split(":")[0];
    if (!FileHash.hash(outputFile, algo).equals(checksum))
      throw new Exception("CRC doesn't match. File is corrupted.");

    contribution.setDownloaded(true);
    contribution.setDownloadedFile(outputFile);
    return outputFile;
  }

  public void remove(ContributedPlatform platform) {
    FileUtils.recursiveDelete(platform.getInstalledFolder());
    platform.setInstalled(false);
    platform.setInstalledFolder(null);

    // Check if the tools are no longer needed
    for (ContributedTool tool : platform.getResolvedTools()) {
      if (indexer.isContributedToolUsed(tool))
        continue;

      DownloadableContribution toolContrib = tool.getDownloadableContribution();
      File destFolder = toolContrib.getInstalledFolder();
      FileUtils.recursiveDelete(destFolder);
      toolContrib.setInstalled(false);
      toolContrib.setInstalledFolder(null);

      // We removed the version folder (.../tools/TOOL_NAME/VERSION)
      // now try to remove the containing TOOL_NAME folder
      // (and silently fail if another version of the tool is installed)
      try {
        destFolder.getParentFile().delete();
      } catch (SecurityException e) {
        // Do nothing
      }
    }
  }
}
