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

import java.io.File;
import java.net.URL;

import processing.app.helpers.FileUtils;

import cc.arduino.utils.ArchiveExtractor;
import cc.arduino.utils.FileHash;

public class ContributionInstaller {

  private File preferencesFolder;
  private File stagingFolder;
  private ContributionsIndexer indexer;

  public ContributionInstaller(File _preferencesFolder,
                               ContributionsIndexer contributionsIndexer) {
    preferencesFolder = _preferencesFolder;
    stagingFolder = new File(preferencesFolder, "staging");
    indexer = contributionsIndexer;
  }

  public void install(ContributedPlatform platform) throws Exception {
    // Download all files and dependencies
    download(platform);
    for (ContributedTool tool : platform.getResolvedTools()) {
      download(tool.getDownloadableContribution());
    }

    ContributedPackage pack = platform.getParentPackage();
    File packageFolder = new File(indexer.getPackagesFolder(), pack.getName());

    // Unzip tools on the correct location
    File toolsFolder = new File(packageFolder, "tools");
    for (ContributedTool tool : platform.getResolvedTools()) {
      DownloadableContribution toolContrib = tool.getDownloadableContribution();
      File destFolder = new File(toolsFolder, tool.getName() + File.separator +
          tool.getVersion());

      destFolder.mkdirs();
      ArchiveExtractor.extract(toolContrib.getDownloadedFile(), destFolder, 1);
      toolContrib.setInstalled(true);
      toolContrib.setInstalledFolder(destFolder);
    }

    // Unzip platform on the correct location
    File platformFolder = new File(packageFolder, "hardware" + File.separator +
        platform.getArchitecture());
    File destFolder = new File(platformFolder, platform.getVersion());
    destFolder.mkdirs();
    ArchiveExtractor.extract(platform.getDownloadedFile(), destFolder, 1);
    platform.setInstalled(true);
    platform.setInstalledFolder(destFolder);

    // TODO: Update index
  }

  public File download(DownloadableContribution contribution) throws Exception {
    contribution.setDownloaded(false);

    System.out.println("Downloading " + contribution.getUrl());

    URL url = new URL(contribution.getUrl());
    String path = url.getPath();
    String fileName = path.substring(path.lastIndexOf('/') + 1);
    File outputFile = new File(stagingFolder, fileName);

    if (outputFile.isFile()) {
      if (outputFile.length() != contribution.getSize()) {
        // TODO: RESUME DOWNLOAD
      }
    } else {
      // TODO: DOWNLOAD
    }

    // Test checksum
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
