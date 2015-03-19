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
package cc.arduino.os;

import java.io.File;
import java.io.IOException;

import cc.arduino.os.linux.LinuxFileNativeUtils;
import cc.arduino.os.macos.MacOSFileNativeUtils;
import cc.arduino.os.windows.WindowsFileNativeUtils;
import processing.app.helpers.OSUtils;

public class FileNativeUtils {

  /**
   * Change file access permissions (UNIX). If the underlying filesystem doesn't
   * support UNIX permission then the command is ignored.
   * 
   * @param file
   * @param mode
   * @throws IOException
   */
  public static void chmod(File file, int mode) throws IOException {
    if (OSUtils.isLinux())
      LinuxFileNativeUtils.chmod(file, mode);
    if (OSUtils.isMacOS())
      MacOSFileNativeUtils.chmod(file, mode);
    if (OSUtils.isWindows())
      WindowsFileNativeUtils.chmod(file, mode);
  }

  /**
   * Create a hard link from <b>oldFile</b> to <b>newFile</b>. If the underlying
   * filesystem doesn't support hard links then the command is ignored.
   * 
   * @param something
   * @param somewhere
   * @throws IOException
   */
  public static void link(File something, File somewhere) throws IOException {
    if (OSUtils.isLinux())
      LinuxFileNativeUtils.link(something, somewhere);
    if (OSUtils.isMacOS())
      MacOSFileNativeUtils.link(something, somewhere);
    if (OSUtils.isWindows())
      WindowsFileNativeUtils.link(something, somewhere);
  }

  /**
   * Create a symlink link from <b>oldFile</b> to <b>newFile</b>. If the
   * underlying filesystem doesn't support symlinks then the command is ignored.
   * 
   * @param something
   * @param somewhere
   * @throws IOException
   */
  public static void symlink(File something, File somewhere) throws IOException {
    if (OSUtils.isLinux())
      LinuxFileNativeUtils.symlink(something, somewhere);
    if (OSUtils.isMacOS())
      MacOSFileNativeUtils.symlink(something, somewhere);
    if (OSUtils.isWindows())
      WindowsFileNativeUtils.symlink(something, somewhere);
  }
}
