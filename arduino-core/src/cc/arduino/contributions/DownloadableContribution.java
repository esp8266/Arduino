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

import com.github.zafarkhaja.semver.Version;

import java.io.File;

public abstract class DownloadableContribution {

  private boolean installed;
  private File installedFolder;

  private boolean downloaded;
  private File downloadedFile;

  public abstract String getUrl();

  public abstract String getVersion();

  public abstract String getChecksum();

  public abstract long getSize();

  public abstract String getArchiveFileName();

  public boolean isDownloaded() {
    return downloaded;
  }

  public void setDownloaded(boolean downloaded) {
    this.downloaded = downloaded;
  }

  public File getDownloadedFile() {
    return downloadedFile;
  }

  public void setDownloadedFile(File downloadedFile) {
    this.downloadedFile = downloadedFile;
  }

  public boolean isInstalled() {
    return installed;
  }

  public void setInstalled(boolean installed) {
    this.installed = installed;
  }

  public File getInstalledFolder() {
    return installedFolder;
  }

  public void setInstalledFolder(File installedFolder) {
    this.installedFolder = installedFolder;
  }

  private boolean readOnly;

  public boolean isReadOnly() {
    return readOnly;
  }

  public void setReadOnly(boolean readOnly) {
    this.readOnly = readOnly;
  }

  public String getParsedVersion() {
    Version version = VersionHelper.valueOf(getVersion());
    if (version == null) {
      return null;
    }
    return version.toString();
  }

  @Override
  public String toString() {
    String res = "";
    if (installed) {
      res += "installed on " + installedFolder.getAbsolutePath() + " (" + getSize() + " bytes)";
    }
    return res;
  }
}
