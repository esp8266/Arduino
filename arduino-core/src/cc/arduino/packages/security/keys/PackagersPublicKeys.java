/*
 * This file is part of Arduino.
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
 *
 * Copyright 2013 Arduino LLC (http://www.arduino.cc/)
 */

package cc.arduino.packages.security.keys;

import java.io.ByteArrayInputStream;
import java.io.IOException;

import org.bouncycastle.openpgp.PGPException;
import org.bouncycastle.openpgp.PGPPublicKeyRingCollection;
import org.bouncycastle.openpgp.PGPUtil;

public class PackagersPublicKeys extends PGPPublicKeyRingCollection {

  public static final String ARDUINO_PK = "" //
      + "-----BEGIN PGP PUBLIC KEY BLOCK-----\n"
      + "Version: GnuPG v1.4.11 (GNU/Linux)\n"
      + "\n"
      + "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\n"
      + "KCcKZZVUQLJhOIwgHQMy7ck=\n" //
      + "=u0/X\n" //
      + "-----END PGP PUBLIC KEY BLOCK-----\n";

  public PackagersPublicKeys() throws IOException, PGPException {
    super(PGPUtil.getDecoderStream(new ByteArrayInputStream(ARDUINO_PK.getBytes())));
  }
  
}
