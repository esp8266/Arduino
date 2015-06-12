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

package cc.arduino.files;

import processing.app.PreferencesData;
import processing.app.helpers.FileUtils;

import java.io.File;
import java.util.Collections;
import java.util.LinkedList;
import java.util.List;

public class DeleteFilesOnShutdown implements Runnable {

  public static final DeleteFilesOnShutdown INSTANCE = new DeleteFilesOnShutdown();

  public static void add(File file) {
    INSTANCE.addFile(file);
  }

  private final List<File> files;

  public DeleteFilesOnShutdown() {
    this.files = new LinkedList<File>();
  }

  public synchronized void addFile(File file) {
    this.files.add(file);
  }

  @Override
  public void run() {
    boolean preserveTempFiles = PreferencesData.getBoolean("runtime.preserve.temp.files");
    if (preserveTempFiles) {
      return;
    }
    List<File> copyOfFiles;
    synchronized (this) {
      copyOfFiles = new LinkedList<File>(files);
    }
    Collections.reverse(copyOfFiles);
    for (File file : copyOfFiles) {
      if (file.exists() && file.canWrite()) {
        FileUtils.recursiveDelete(file);
      }
    }
  }

}
