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

package cc.arduino.contributions;

import cc.arduino.utils.Progress;
import org.apache.commons.compress.compressors.gzip.GzipCompressorInputStream;
import org.apache.commons.compress.compressors.gzip.GzipUtils;
import org.apache.commons.compress.utils.IOUtils;

import java.io.*;
import java.net.URL;

public class GZippedJsonDownloader {

  private final DownloadableContributionsDownloader downloader;
  private final URL url;
  private final URL gzippedUrl;

  public GZippedJsonDownloader(DownloadableContributionsDownloader downloader, URL url, URL gzippedUrl) {
    this.downloader = downloader;
    this.url = url;
    this.gzippedUrl = gzippedUrl;
  }

  public void download(File tmpFile, Progress progress, String statusText) throws Exception {
    try {
      new JsonDownloader(downloader, gzippedUrl).download(tmpFile, progress, statusText);
      File gzipTmpFile = new File(tmpFile.getParentFile(), GzipUtils.getCompressedFilename(tmpFile.getName()));
      tmpFile.renameTo(gzipTmpFile);
      decompress(gzipTmpFile, tmpFile);
    } catch (Exception e) {
      new JsonDownloader(downloader, url).download(tmpFile, progress, statusText);
    }
  }

  private void decompress(File gzipTmpFile, File tmpFile) throws IOException {
    OutputStream os = null;
    GzipCompressorInputStream gzipIs = null;
    try {
      os = new FileOutputStream(tmpFile);
      gzipIs = new GzipCompressorInputStream(new FileInputStream(gzipTmpFile));
      final byte[] buffer = new byte[4096];
      int n = 0;
      while (-1 != (n = gzipIs.read(buffer))) {
        os.write(buffer, 0, n);
      }
    } finally {
      IOUtils.closeQuietly(os);
      IOUtils.closeQuietly(gzipIs);
    }
  }
}
