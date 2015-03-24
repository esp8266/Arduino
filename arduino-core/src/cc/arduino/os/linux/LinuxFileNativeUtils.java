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
package cc.arduino.os.linux;

import java.io.File;
import java.io.IOException;

public class LinuxFileNativeUtils {

  public static final LibCNative libc = LibCNative.libc;

  public static void chmod(File file, int mode) throws IOException {
    int res = libc.chmod(file.getAbsolutePath(), mode);
    if (res == -1) {
      throw new IOException("Could not change file permission: " + strerror());
    }
  }

  public static void link(File something, File somewhere) throws IOException {
    int res = libc.link(something.getAbsolutePath(), somewhere.getAbsolutePath());
    if (res == -1) {
      throw new IOException("Could not create hard link to " + somewhere + " from " + something + ": " + strerror());
    }
  }

  public static void symlink(File something, File somewhere) throws IOException {
    int res = libc.symlink(something.getPath(), somewhere.getAbsolutePath());
    if (res == -1) {
      throw new IOException("Could not create symlink to " + somewhere + " from " + something + ": " + strerror());
    }
  }

  private static String strerror() {
    return libc.strerror(LibCNative.errno.getInt(0));
  }

}
