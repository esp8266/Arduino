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

package cc.arduino.contributions;

import cc.arduino.utils.FileHash;
import cc.arduino.utils.Progress;
import cc.arduino.utils.network.FileDownloader;

import java.io.File;
import java.net.URL;
import java.util.Observable;
import java.util.Observer;

import static processing.app.I18n._;
import static processing.app.I18n.format;

public class DownloadableContributionsDownloader {

  private final File stagingFolder;

  public DownloadableContributionsDownloader(File _stagingFolder) {
    stagingFolder = _stagingFolder;
  }

  public File download(DownloadableContribution contribution,
                       final Progress progress, final String statusText)
          throws Exception {
    URL url = new URL(contribution.getUrl());
    final File outputFile = new File(stagingFolder, contribution.getArchiveFileName());

    // Ensure the existence of staging folder
    stagingFolder.mkdirs();

    // Need to download or resume downloading?
    if (!outputFile.isFile() || (outputFile.length() < contribution.getSize())) {
      download(url, outputFile, progress, statusText);
    }

    // Test checksum
    progress.setStatus(_("Verifying archive integrity..."));
    onProgress(progress);
    String checksum = contribution.getChecksum();
    String algo = checksum.split(":")[0];
    if (!FileHash.hash(outputFile, algo).equalsIgnoreCase(checksum)) {
      throw new Exception(_("CRC doesn't match. File is corrupted."));
    }

    contribution.setDownloaded(true);
    contribution.setDownloadedFile(outputFile);
    return outputFile;
  }

  public void download(URL url, File tmpFile, final Progress progress,
                       final String statusText) throws Exception {
    FileDownloader downloader = new FileDownloader(url, tmpFile);
    downloader.addObserver(new Observer() {
      @Override
      public void update(Observable o, Object arg) {
        FileDownloader me = (FileDownloader) o;
        String msg = "";
        if (me.getDownloadSize() != null) {
          long downloaded = (me.getInitialSize() + me.getDownloaded()) / 1000;
          long total = (me.getInitialSize() + me.getDownloadSize()) / 1000;
          msg = format(_("Downloaded {0}kb of {1}kb."), downloaded, total);
        }
        progress.setStatus(statusText + " " + msg);
        progress.setProgress(me.getProgress());
        onProgress(progress);
      }
    });
    downloader.download();
    if (!downloader.isCompleted()) {
      throw new Exception(format(_("Error downloading {0}"), url), downloader.getError());
    }
  }

  protected void onProgress(Progress progress) {
    // Empty
  }

}
