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
package cc.arduino.utils.network;

import org.apache.commons.codec.binary.Base64;
import processing.app.PreferencesData;

import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.SocketTimeoutException;
import java.net.URL;
import java.util.Observable;

public class FileDownloader extends Observable {

  public enum Status {
    CONNECTING, //
    CONNECTION_TIMEOUT_ERROR, //
    DOWNLOADING, //
    COMPLETE, //
    CANCELLED, //
    ERROR, //
  }

  private Status status;
  private long initialSize;
  private Long downloadSize = null;
  private long downloaded;
  private final URL downloadUrl;

  private final File outputFile;
  private InputStream stream = null;
  private Exception error;

  public FileDownloader(URL url, File file) {
    downloadUrl = url;
    outputFile = file;
    downloaded = 0;
    initialSize = 0;
  }

  public long getInitialSize() {
    return initialSize;
  }

  public Long getDownloadSize() {
    return downloadSize;
  }

  public void setDownloadSize(Long downloadSize) {
    this.downloadSize = downloadSize;
    setChanged();
    notifyObservers();
  }

  public long getDownloaded() {
    return downloaded;
  }

  private void setDownloaded(long downloaded) {
    this.downloaded = downloaded;
    setChanged();
    notifyObservers();
  }

  public float getProgress() {
    if (downloadSize == null)
      return 0;
    if (downloadSize == 0)
      return 100;
    return ((float) downloaded / downloadSize) * 100;
  }

  public Status getStatus() {
    return status;
  }

  public void setStatus(Status status) {
    this.status = status;
    setChanged();
    notifyObservers();
  }

  public void download() throws InterruptedException {
    RandomAccessFile file = null;

    try {
      // Open file and seek to the end of it
      file = new RandomAccessFile(outputFile, "rw");
      initialSize = file.length();
      file.seek(initialSize);

      setStatus(Status.CONNECTING);

      System.getProperties().remove("http.proxyHost");
      System.getProperties().remove("http.proxyPort");
      System.getProperties().remove("https.proxyHost");
      System.getProperties().remove("https.proxyPort");
      System.getProperties().remove("http.proxyUser");
      System.getProperties().remove("http.proxyPassword");

      if (PreferencesData.has("proxy.http.server") && PreferencesData.get("proxy.http.server") != null && !PreferencesData.get("proxy.http.server").equals("")) {
        System.getProperties().put("http.proxyHost", PreferencesData.get("proxy.http.server"));
        System.getProperties().put("http.proxyPort", PreferencesData.get("proxy.http.port"));
      }
      if (PreferencesData.has("proxy.https.server") && PreferencesData.get("proxy.https.server") != null && !PreferencesData.get("proxy.https.server").equals("")) {
        System.getProperties().put("https.proxyHost", PreferencesData.get("proxy.https.server"));
        System.getProperties().put("https.proxyPort", PreferencesData.get("proxy.https.port"));
      }
      if (PreferencesData.has("proxy.user") && PreferencesData.get("proxy.user") != null && !PreferencesData.get("proxy.user").equals("")) {
        System.getProperties().put("http.proxyUser", PreferencesData.get("proxy.user"));
        System.getProperties().put("http.proxyPassword", PreferencesData.get("proxy.password"));
        System.getProperties().put("https.proxyUser", PreferencesData.get("proxy.user"));
        System.getProperties().put("https.proxyPassword", PreferencesData.get("proxy.password"));
      }

      HttpURLConnection connection = (HttpURLConnection) downloadUrl.openConnection();

      if (downloadUrl.getUserInfo() != null) {
        String auth = "Basic " + new String(new Base64().encode(downloadUrl.getUserInfo().getBytes()));
        connection.setRequestProperty("Authorization", auth);
      }

      connection.setRequestProperty("Range", "bytes=" + initialSize + "-");
      connection.setConnectTimeout(5000);
      setDownloaded(0);

      // Connect
      connection.connect();
      int resp = connection.getResponseCode();

      if (resp == HttpURLConnection.HTTP_MOVED_PERM || resp == HttpURLConnection.HTTP_MOVED_TEMP) {
        String newUrl = connection.getHeaderField("Location");

        // open the new connnection again
        connection = (HttpURLConnection) new URL(newUrl).openConnection();
        if (downloadUrl.getUserInfo() != null) {
          String auth = "Basic " + new String(new Base64().encode(downloadUrl.getUserInfo().getBytes()));
          connection.setRequestProperty("Authorization", auth);
        }

        connection.setRequestProperty("Range", "bytes=" + initialSize + "-");
        connection.setConnectTimeout(5000);

        connection.connect();
        resp = connection.getResponseCode();
      }

      if (resp < 200 || resp >= 300) {
        throw new IOException("Recevied invalid http status code from server: " + resp);
      }

      // Check for valid content length.
      long len = connection.getContentLength();
      if (len >= 0) {
        setDownloadSize(len);
      }
      setStatus(Status.DOWNLOADING);

      synchronized (this) {
        stream = connection.getInputStream();
      }
      byte buffer[] = new byte[10240];
      while (status == Status.DOWNLOADING) {
        int read = stream.read(buffer);
        if (read == -1)
          break;

        file.write(buffer, 0, read);
        setDownloaded(getDownloaded() + read);

        if (Thread.interrupted())
          throw new InterruptedException();
      }

      if (getDownloadSize() != null) {
        if (getDownloaded() < getDownloadSize())
          throw new Exception("Incomplete download");
      }
      setStatus(Status.COMPLETE);
    } catch (InterruptedException e) {
      setStatus(Status.CANCELLED);
      // lets InterruptedException go up to the caller
      throw e;

    } catch (SocketTimeoutException e) {
      setStatus(Status.CONNECTION_TIMEOUT_ERROR);
      setError(e);

    } catch (Exception e) {
      setStatus(Status.ERROR);
      setError(e);

    } finally {
      if (file != null) {
        try {
          file.close();
        } catch (Exception e) {
          //ignore
        }
      }

      synchronized (this) {
        if (stream != null) {
          try {
            stream.close();
          } catch (Exception e) {
            //ignore
          }
        }
      }
    }
  }

  private void setError(Exception e) {
    error = e;
  }

  public Exception getError() {
    return error;
  }

  public boolean isCompleted() {
    return status == Status.COMPLETE;
  }
}
