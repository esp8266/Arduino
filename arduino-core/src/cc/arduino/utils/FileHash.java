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

package cc.arduino.utils;

import org.apache.commons.compress.utils.IOUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

public class FileHash {

  /**
   * Calculate a message digest of a file using the algorithm specified. The
   * result is a string containing the algorithm name followed by ":" and by the
   * resulting hash in hex.
   *
   * @param file
   * @param algorithm For example "SHA-256"
   * @return The algorithm followed by ":" and the hash, for example:<br />
   * "SHA-256:ee6796513086080cca078cbb383f543c5e508b647a71c9d6f39b7bca41071883"
   * @throws IOException
   * @throws NoSuchAlgorithmException
   */
  public static String hash(File file, String algorithm) throws IOException, NoSuchAlgorithmException {
    FileInputStream in = null;
    try {
      in = new FileInputStream(file);
      byte buff[] = new byte[10240];
      MessageDigest digest = MessageDigest.getInstance(algorithm);
      while (in.available() > 0) {
        int read = in.read(buff);
        digest.update(buff, 0, read);
      }
      byte[] hash = digest.digest();
      String res = "";
      for (byte b : hash) {
        int c = b & 0xFF;
        if (c < 0x10)
          res += "0";
        res += Integer.toHexString(c);
      }
      return algorithm + ":" + res;
    } finally {
      IOUtils.closeQuietly(in);
    }
  }
}
