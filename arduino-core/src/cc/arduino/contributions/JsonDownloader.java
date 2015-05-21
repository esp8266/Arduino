package cc.arduino.contributions;

import cc.arduino.utils.Progress;

import java.io.File;
import java.net.URL;

public class JsonDownloader {

  private final DownloadableContributionsDownloader downloader;
  private final URL url;

  public JsonDownloader(DownloadableContributionsDownloader downloader, URL url) {
    this.downloader = downloader;
    this.url = url;
  }

  public void download(File tmpFile, Progress progress, String statusText) throws Exception {
    try {
      downloader.download(url, tmpFile, progress, statusText);
    } catch (InterruptedException e) {
      // Download interrupted... just exit
      return;
    }
  }
}
