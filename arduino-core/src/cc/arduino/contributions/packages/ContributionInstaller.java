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

import cc.arduino.utils.ArchiveExtractor;
import cc.arduino.utils.MultiStepProgress;
import cc.arduino.utils.Progress;
import com.google.common.base.Predicate;
import com.google.common.collect.Collections2;
import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.Executor;
import processing.app.BaseNoGui;
import processing.app.helpers.FileUtils;
import processing.app.helpers.filefilters.OnlyDirs;
import processing.app.tools.CollectStdOutStdErrExecutor;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;

import static processing.app.I18n._;
import static processing.app.I18n.format;

public class ContributionInstaller {

  private static final String PACKAGE_INDEX_URL;

  static {
    String extenalPackageIndexUrl = System.getProperty("PACKAGE_INDEX_URL");
    if (extenalPackageIndexUrl != null && !"".equals(extenalPackageIndexUrl)) {
      PACKAGE_INDEX_URL = extenalPackageIndexUrl;
    } else {
      PACKAGE_INDEX_URL = "http://arduino.cc/download.php?f=/packages/package_index.json";
    }
  }

  private final ContributionsIndexer indexer;
  private final DownloadableContributionsDownloader downloader;

  public ContributionInstaller(ContributionsIndexer contributionsIndexer) {
    File stagingFolder = contributionsIndexer.getStagingFolder();
    indexer = contributionsIndexer;
    downloader = new DownloadableContributionsDownloader(stagingFolder) {
      @Override
      protected void onProgress(Progress progress) {
        ContributionInstaller.this.onProgress(progress);
      }
    };
  }

  public List<String> install(ContributedPlatform platform) throws Exception {
    List<String> errors = new LinkedList<String>();
    if (platform.isInstalled()) {
      throw new Exception("Platform is already installed!");
    }

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
    MultiStepProgress progress = new MultiStepProgress((tools.size() + 1) * 2);

    // Download all
    try {
      // Download platform
      downloader.download(platform, progress, _("Downloading boards definitions."));
      progress.stepDone();

      // Download tools
      int i = 1;
      for (ContributedTool tool : tools) {
        String msg = format(_("Downloading tools ({0}/{1})."), i, tools.size());
        i++;
        downloader.download(tool.getDownloadableContribution(), progress, msg);
        progress.stepDone();
      }
    } catch (InterruptedException e) {
      // Download interrupted... just exit
      return errors;
    }

    ContributedPackage pack = platform.getParentPackage();
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
      DownloadableContribution toolContrib = tool.getDownloadableContribution();
      File destFolder = new File(toolsFolder, tool.getName() + File.separator + tool.getVersion());

      destFolder.mkdirs();
      assert toolContrib.getDownloadedFile() != null;
      new ArchiveExtractor(BaseNoGui.getPlatform()).extract(toolContrib.getDownloadedFile(), destFolder, 1);
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
    File platformFolder = new File(packageFolder, "hardware" + File.separator + platform.getArchitecture());
    File destFolder = new File(platformFolder, platform.getParsedVersion());
    destFolder.mkdirs();
    new ArchiveExtractor(BaseNoGui.getPlatform()).extract(platform.getDownloadedFile(), destFolder, 1);
    platform.setInstalled(true);
    platform.setInstalledFolder(destFolder);
    progress.stepDone();

    progress.setStatus(_("Installation completed!"));
    onProgress(progress);

    return errors;
  }

  private void executePostInstallScriptIfAny(File folder) throws IOException {
    Collection<File> postInstallScripts = Collections2.filter(BaseNoGui.getPlatform().postInstallScripts(folder), new Predicate<File>() {
      @Override
      public boolean apply(File file) {
        return file.isFile() && file.exists() && file.canRead() && file.canExecute();
      }
    });

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
    Executor executor = new CollectStdOutStdErrExecutor(stdout, stderr);
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

  public List<String> remove(ContributedPlatform platform) {
    if (platform == null || platform.isReadOnly()) {
      return new LinkedList<String>();
    }
    List<String> errors = new LinkedList<String>();
    FileUtils.recursiveDelete(platform.getInstalledFolder());
    platform.setInstalled(false);
    platform.setInstalledFolder(null);

    // Check if the tools are no longer needed
    for (ContributedTool tool : platform.getResolvedTools()) {
      if (indexer.isContributedToolUsed(tool)) {
        continue;
      }

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
        // ignore
      }
    }

    return errors;
  }

  public List<String> updateIndex() throws Exception {
    List<String> errors = new LinkedList<String>();
    MultiStepProgress progress = new MultiStepProgress(1);
    String statusText = _("Downloading platforms index...");

    URL url = new URL(PACKAGE_INDEX_URL);
    File outputFile = indexer.getIndexFile();
    File tmpFile = new File(outputFile.getAbsolutePath() + ".tmp");
    downloader.download(url, tmpFile, progress, statusText);
    progress.stepDone();

    // TODO: Check downloaded index

    // Replace old index with the updated one
    if (outputFile.exists()) {
      outputFile.delete();
    }
    if (!tmpFile.renameTo(outputFile)) {
      throw new Exception("An error occurred while updating platforms index!");
    }
    return errors;
  }

  protected void onProgress(Progress progress) {
    // Empty
  }
}
