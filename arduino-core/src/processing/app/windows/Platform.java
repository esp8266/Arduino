/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2008-2009 Ben Fry and Casey Reas

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package processing.app.windows;

import org.apache.commons.exec.CommandLine;
import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.Executor;
import org.apache.commons.exec.PumpStreamHandler;
import processing.app.debug.TargetPackage;
import processing.app.legacy.PApplet;
import processing.app.legacy.PConstants;

import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.IOException;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;


public class Platform extends processing.app.Platform {

  private File settingsFolder;
  private File defaultSketchbookFolder;

  public void init() throws IOException {
    super.init();

    checkPath();
    recoverSettingsFolderPath();
    recoverDefaultSketchbookFolder();
  }

  private void recoverSettingsFolderPath() throws IOException {
    String path = getFolderPathFromRegistry("AppData");
    this.settingsFolder = new File(path, "Arduino15");
  }

  private void recoverDefaultSketchbookFolder() throws IOException {
    String path = getFolderPathFromRegistry("Personal");
    this.defaultSketchbookFolder = new File(path, "Arduino");
  }

  private String getFolderPathFromRegistry(String folderType) throws IOException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    Executor executor = new DefaultExecutor();
    executor.setStreamHandler(new PumpStreamHandler(baos, null));

    CommandLine toDevicePath = CommandLine.parse("reg query \"HKCU\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders\" /v \"" + folderType + "\"");
    executor.execute(toDevicePath);
    return new RegQueryParser(new String(baos.toByteArray())).getValueOfKey();
  }

  /**
   * Remove extra quotes, slashes, and garbage from the Windows PATH.
   */
  protected void checkPath() {
    String path = System.getProperty("java.library.path");
    String[] pieces = PApplet.split(path, File.pathSeparatorChar);
    String[] legit = new String[pieces.length];
    int legitCount = 0;
    for (String item : pieces) {
      if (item.startsWith("\"")) {
        item = item.substring(1);
      }
      if (item.endsWith("\"")) {
        item = item.substring(0, item.length() - 1);
      }
      if (item.endsWith(File.separator)) {
        item = item.substring(0, item.length() - File.separator.length());
      }
      File directory = new File(item);
      if (!directory.exists()) {
        continue;
      }
      if (item.trim().length() == 0) {
        continue;
      }
      legit[legitCount++] = item;
    }
    legit = PApplet.subset(legit, 0, legitCount);
    String newPath = PApplet.join(legit, File.pathSeparator);
    if (!newPath.equals(path)) {
      System.setProperty("java.library.path", newPath);
    }
  }

  public File getSettingsFolder() {
    return settingsFolder;
  }


  public File getDefaultSketchbookFolder() throws Exception {
    return defaultSketchbookFolder;
  }

  public void openURL(String url) throws Exception {
    // this is not guaranteed to work, because who knows if the
    // path will always be c:\progra~1 et al. also if the user has
    // a different browser set as their default (which would
    // include me) it'd be annoying to be dropped into ie.
    //Runtime.getRuntime().exec("c:\\progra~1\\intern~1\\iexplore "
    // + currentDir

    // the following uses a shell execute to launch the .html file
    // note that under cygwin, the .html files have to be chmodded +x
    // after they're unpacked from the zip file. i don't know why,
    // and don't understand what this does in terms of windows
    // permissions. without the chmod, the command prompt says
    // "Access is denied" in both cygwin and the "dos" prompt.
    //Runtime.getRuntime().exec("cmd /c " + currentDir + "\\reference\\" +
    //                    referenceFile + ".html");
    if (url.startsWith("http") || url.startsWith("file:")) {
      // open dos prompt, give it 'start' command, which will
      // open the url properly. start by itself won't work since
      // it appears to need cmd
      Runtime.getRuntime().exec("cmd /c start " + url);
    } else {
      // just launching the .html file via the shell works
      // but make sure to chmod +x the .html files first
      // also place quotes around it in case there's a space
      // in the user.dir part of the url
      Runtime.getRuntime().exec("cmd /c \"" + url + "\"");
    }
  }


  public boolean openFolderAvailable() {
    return true;
  }


  public void openFolder(File file) throws Exception {
    String folder = file.getAbsolutePath();

    // doesn't work
    //Runtime.getRuntime().exec("cmd /c \"" + folder + "\"");

    // works fine on winxp, prolly win2k as well
    Runtime.getRuntime().exec("explorer \"" + folder + "\"");

    // not tested
    //Runtime.getRuntime().exec("start explorer \"" + folder + "\"");
  }


  // . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .


  @Override
  public String getName() {
    return PConstants.platformNames[PConstants.WINDOWS];
  }

  @Override
  public Map<String, Object> resolveDeviceAttachedTo(String serial, Map<String, TargetPackage> packages, String devicesListOutput) {
    assert packages != null;
    if (devicesListOutput == null) {
      return super.resolveDeviceAttachedTo(serial, packages, devicesListOutput);
    }

    try {
      String vidPid = new ListComPortsParser().extractVIDAndPID(devicesListOutput, serial);

      if (vidPid == null) {
        return super.resolveDeviceAttachedTo(serial, packages, devicesListOutput);
      }

      return super.resolveDeviceByVendorIdProductId(packages, vidPid);
    } catch (IOException e) {
      return super.resolveDeviceAttachedTo(serial, packages, devicesListOutput);
    }
  }

  @Override
  public String preListAllCandidateDevices() {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    Executor executor = new DefaultExecutor();
    executor.setStreamHandler(new PumpStreamHandler(baos, null));

    try {
      String listComPorts = new File(System.getProperty("user.dir"), "hardware/tools/listComPorts.exe").getCanonicalPath();

      CommandLine toDevicePath = CommandLine.parse(listComPorts);
      executor.execute(toDevicePath);
      return new String(baos.toByteArray());
    } catch (Throwable e) {
      return super.preListAllCandidateDevices();
    }
  }

  @Override
  public void fixPrefsFilePermissions(File prefsFile) throws IOException {
    //noop
  }

  public List<File> postInstallScripts(File folder) {
    List<File> scripts = new LinkedList<File>();
    scripts.add(new File(folder, "post_install.bat"));
    return scripts;
  }

  public void symlink(File something, File somewhere) throws IOException, InterruptedException {
  }

  public void link(File something, File somewhere) throws IOException, InterruptedException {
  }

  public void chmod(File file, int mode) throws IOException, InterruptedException {
  }
}
