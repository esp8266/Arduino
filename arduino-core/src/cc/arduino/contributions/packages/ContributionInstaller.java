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

package cc.arduino.contributions.packages;

import cc.arduino.contributions.DownloadableContribution;
import cc.arduino.contributions.DownloadableContributionsDownloader;
import cc.arduino.contributions.GPGDetachedSignatureVerifier;
import cc.arduino.filters.FileExecutablePredicate;
import cc.arduino.utils.ArchiveExtractor;
import cc.arduino.utils.MultiStepProgress;
import cc.arduino.utils.Progress;
import com.google.common.collect.Collections2;
import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.Executor;
import org.apache.commons.exec.PumpStreamHandler;
import processing.app.BaseNoGui;
import processing.app.I18n;
import processing.app.Platform;
import processing.app.PreferencesData;
import processing.app.helpers.FileUtils;
import processing.app.helpers.filefilters.OnlyDirs;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.*;

import static processing.app.I18n._;
import static processing.app.I18n.format;

public class ContributionInstaller {

  private final ContributionsIndexer indexer;
  private final DownloadableContributionsDownloader downloader;
  private final Platform platform;

  public ContributionInstaller(ContributionsIndexer contributionsIndexer, Platform platform) {
    this.platform = platform;
    File stagingFolder = contributionsIndexer.getStagingFolder();
    indexer = contributionsIndexer;
    downloader = new DownloadableContributionsDownloader(stagingFolder) {
      @Override
      protected void onProgress(Progress progress) {
        ContributionInstaller.this.onProgress(progress);
      }
    };
  }

  public List<String> install(ContributedPlatform contributedPlatform) throws Exception {
    List<String> errors = new LinkedList<String>();
    if (contributedPlatform.isInstalled()) {
      throw new Exception("Platform is already installed!");
    }

    // Do not download already installed tools
    List<ContributedTool> tools = new LinkedList<ContributedTool>(contributedPlatform.getResolvedTools());
    Iterator<ContributedTool> toolsIterator = tools.iterator();
    while (toolsIterator.hasNext()) {
      ContributedTool tool = toolsIterator.next();
      DownloadableContribution downloadable = tool.getDownloadableContribution(platform);
      if (downloadable == null) {
        throw new Exception(format(_("Tool {0} is not available for your operating system."), tool.getName()));
      }
      if (downloadable.isInstalled()) {
        toolsIterator.remove();
      }
    }

    // Calculate progress increases
    MultiStepProgress progress = new MultiStepProgress((tools.size() + 1) * 2);

    // Download all
    try {
      // Download platform
      downloader.download(contributedPlatform, progress, _("Downloading boards definitions."));
      progress.stepDone();

      // Download tools
      int i = 1;
      for (ContributedTool tool : tools) {
        String msg = format(_("Downloading tools ({0}/{1})."), i, tools.size());
        i++;
        downloader.download(tool.getDownloadableContribution(platform), progress, msg);
        progress.stepDone();
      }
    } catch (InterruptedException e) {
      // Download interrupted... just exit
      return errors;
    }

    ContributedPackage pack = contributedPlatform.getParentPackage();
    File packageFolder = new File(indexer.getPackagesFolder(), pack.getName());

    // TODO: Extract to temporary folders and move to the final destination only
    // once everything is successfully unpacked. If the operation fails remove
    // all the temporary folders and abort installation.

    // Unzip tools on the correct location
    File toolsFolder = new File(packageFolder, "tools");
    int i = 1;
    for (ContributedTool tool : tools) {
      progress.setStatus(format(_("Installing tools ({0}/{1})..."), i, tools.size()));
      onProgress(progress);
      i++;
      DownloadableContribution toolContrib = tool.getDownloadableContribution(platform);
      File destFolder = new File(toolsFolder, tool.getName() + File.separator + tool.getVersion());

      destFolder.mkdirs();
      assert toolContrib.getDownloadedFile() != null;
      new ArchiveExtractor(platform).extract(toolContrib.getDownloadedFile(), destFolder, 1);
      try {
        executePostInstallScriptIfAny(destFolder);
      } catch (IOException e) {
        errors.add(_("Error running post install script"));
      }
      toolContrib.setInstalled(true);
      toolContrib.setInstalledFolder(destFolder);
      progress.stepDone();
    }

    // Unpack platform on the correct location
    progress.setStatus(_("Installing boards..."));
    onProgress(progress);
    File platformFolder = new File(packageFolder, "hardware" + File.separator + contributedPlatform.getArchitecture());
    File destFolder = new File(platformFolder, contributedPlatform.getParsedVersion());
    destFolder.mkdirs();
    new ArchiveExtractor(platform).extract(contributedPlatform.getDownloadedFile(), destFolder, 1);
    contributedPlatform.setInstalled(true);
    contributedPlatform.setInstalledFolder(destFolder);
    progress.stepDone();

    progress.setStatus(_("Installation completed!"));
    onProgress(progress);

    return errors;
  }

  private void executePostInstallScriptIfAny(File folder) throws IOException {
    Collection<File> postInstallScripts = Collections2.filter(platform.postInstallScripts(folder), new FileExecutablePredicate());

    if (postInstallScripts.isEmpty()) {
      String[] subfolders = folder.list(new OnlyDirs());
      if (subfolders.length != 1) {
        return;
      }

      executePostInstallScriptIfAny(new File(folder, subfolders[0]));
      return;
    }

    File postInstallScript = postInstallScripts.iterator().next();

    ByteArrayOutputStream stdout = new ByteArrayOutputStream();
    ByteArrayOutputStream stderr = new ByteArrayOutputStream();
    Executor executor = new DefaultExecutor();
    executor.setStreamHandler(new PumpStreamHandler(stdout, stderr));
    executor.setWorkingDirectory(folder);
    executor.setExitValues(null);
    int exitValue = executor.execute(new CommandLine(postInstallScript));
    executor.setExitValues(new int[0]);

    System.out.write(stdout.toByteArray());
    System.err.write(stderr.toByteArray());

    if (executor.isFailure(exitValue)) {
      throw new IOException();
    }
  }

  public List<String> remove(ContributedPlatform contributedPlatform) {
    if (contributedPlatform == null || contributedPlatform.isReadOnly()) {
      return new LinkedList<String>();
    }
    List<String> errors = new LinkedList<String>();
    FileUtils.recursiveDelete(contributedPlatform.getInstalledFolder());
    contributedPlatform.setInstalled(false);
    contributedPlatform.setInstalledFolder(null);

    // Check if the tools are no longer needed
    for (ContributedTool tool : contributedPlatform.getResolvedTools()) {
      if (indexer.isContributedToolUsed(tool)) {
        continue;
      }

      DownloadableContribution toolContrib = tool.getDownloadableContribution(platform);
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
        // ignore
      }
    }

    return errors;
  }

  public List<String> updateIndex() throws Exception {
    MultiStepProgress progress = new MultiStepProgress(1);

    List<String> downloadedPackageIndexFilesAccumulator = new LinkedList<String>();
    downloadIndexAndSignature(progress, downloadedPackageIndexFilesAccumulator, Constants.PACKAGE_INDEX_URL);

    Set<String> packageIndexURLs = new HashSet<String>();
    String additionalURLs = PreferencesData.get(Constants.PREFERENCES_BOARDS_MANAGER_ADDITIONAL_URLS, "");
    if (!"".equals(additionalURLs)) {
      packageIndexURLs.addAll(Arrays.asList(additionalURLs.split(",")));
    }

    for (String packageIndexURL : packageIndexURLs) {
      downloadIndexAndSignature(progress, downloadedPackageIndexFilesAccumulator, packageIndexURL);
    }

    progress.stepDone();

    return downloadedPackageIndexFilesAccumulator;
  }

  private void downloadIndexAndSignature(MultiStepProgress progress, List<String> downloadedPackagedIndexFilesAccumulator, String packageIndexUrl) throws Exception {
    File packageIndex = download(progress, packageIndexUrl);
    downloadedPackagedIndexFilesAccumulator.add(packageIndex.getName());
    try {
      File packageIndexSignature = download(progress, packageIndexUrl + ".sig");
      boolean signatureVerified = new GPGDetachedSignatureVerifier().verify(packageIndex, packageIndexSignature, new File(BaseNoGui.getContentFile("lib"), "public.gpg.key"));
      if (signatureVerified) {
        downloadedPackagedIndexFilesAccumulator.add(packageIndexSignature.getName());
      } else {
        downloadedPackagedIndexFilesAccumulator.remove(packageIndex.getName());
        packageIndex.delete();
        packageIndexSignature.delete();
        System.err.println(I18n.format(_("{0} file signature verification failed. File ignored."), packageIndexUrl));
      }
    } catch (Exception e) {
      //ignore errors
    }
  }

  private File download(MultiStepProgress progress, String packageIndexUrl) throws Exception {
    String statusText = _("Downloading platforms index...");
    URL url = new URL(packageIndexUrl);
    String[] urlPathParts = url.getFile().split("/");
    File outputFile = indexer.getIndexFile(urlPathParts[urlPathParts.length - 1]);
    File tmpFile = new File(outputFile.getAbsolutePath() + ".tmp");
    downloader.download(url, tmpFile, progress, statusText);

    // Replace old index with the updated one
    if (outputFile.exists()) {
      if (!outputFile.delete()) {
        throw new Exception("An error occurred while updating platforms index! I can't delete file " + outputFile);
      }
    }
    if (!tmpFile.renameTo(outputFile)) {
      throw new Exception("An error occurred while updating platforms index! I can't rename file " + tmpFile);
    }

    return outputFile;
  }

  protected void onProgress(Progress progress) {
    // Empty
  }

  public void deleteUnknownFiles(List<String> downloadedPackageIndexFiles) {
    File preferencesFolder = indexer.getIndexFile(".").getParentFile();
    File[] additionalPackageIndexFiles = preferencesFolder.listFiles(new PackageIndexFilenameFilter(Constants.DEFAULT_INDEX_FILE_NAME));
    if (additionalPackageIndexFiles == null) {
      return;
    }
    for (File additionalPackageIndexFile : additionalPackageIndexFiles) {
      if (!downloadedPackageIndexFiles.contains(additionalPackageIndexFile.getName())) {
        additionalPackageIndexFile.delete();
      }
    }
  }
}
