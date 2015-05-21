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
