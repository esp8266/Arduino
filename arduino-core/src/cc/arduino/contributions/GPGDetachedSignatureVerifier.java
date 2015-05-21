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
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 */

package cc.arduino.contributions;

import org.apache.commons.compress.utils.IOUtils;
import org.bouncycastle.openpgp.*;
import org.bouncycastle.openpgp.operator.bc.BcKeyFingerprintCalculator;
import org.bouncycastle.openpgp.operator.bc.BcPGPContentVerifierBuilderProvider;

import java.io.*;
import java.util.Iterator;

public class GPGDetachedSignatureVerifier {

  private String keyId;

  public GPGDetachedSignatureVerifier() {
    this("7F294291");
  }

  public GPGDetachedSignatureVerifier(String keyId) {
    this.keyId = keyId;
  }

  public boolean verify(File signedFile, File signature, File publicKey) throws IOException, PGPException {
    PGPPublicKey pgpPublicKey = readPublicKey(publicKey, keyId);

    FileInputStream signatureInputStream = null;
    FileInputStream signedFileInputStream = null;
    try {
      signatureInputStream = new FileInputStream(signature);
      PGPObjectFactory pgpObjectFactory = new PGPObjectFactory(signatureInputStream, new BcKeyFingerprintCalculator());

      Object nextObject;
      try {
        nextObject = pgpObjectFactory.nextObject();
        if (!(nextObject instanceof PGPSignatureList)) {
          return false;
        }
      } catch (IOException e) {
        return false;
      }
      PGPSignatureList pgpSignatureList = (PGPSignatureList) nextObject;
      assert pgpSignatureList.size() == 1;
      PGPSignature pgpSignature = pgpSignatureList.get(0);

      pgpSignature.init(new BcPGPContentVerifierBuilderProvider(), pgpPublicKey);
      signedFileInputStream = new FileInputStream(signedFile);
      pgpSignature.update(IOUtils.toByteArray(signedFileInputStream));

      return pgpSignature.verify();
    } finally {
      IOUtils.closeQuietly(signatureInputStream);
      IOUtils.closeQuietly(signedFileInputStream);
    }
  }

  private PGPPublicKey readPublicKey(File file, String keyId) throws IOException, PGPException {
    InputStream keyIn = null;
    try {
      keyIn = new BufferedInputStream(new FileInputStream(file));
      return readPublicKey(keyIn, keyId);
    } finally {
      IOUtils.closeQuietly(keyIn);
    }
  }

  private PGPPublicKey readPublicKey(InputStream input, String keyId) throws IOException, PGPException {
    PGPPublicKeyRingCollection pgpPub = new PGPPublicKeyRingCollection(PGPUtil.getDecoderStream(input), new BcKeyFingerprintCalculator());

    Iterator keyRingIter = pgpPub.getKeyRings();
    while (keyRingIter.hasNext()) {
      PGPPublicKeyRing keyRing = (PGPPublicKeyRing) keyRingIter.next();

      Iterator keyIter = keyRing.getPublicKeys();
      while (keyIter.hasNext()) {
        PGPPublicKey key = (PGPPublicKey) keyIter.next();

        if (Long.toHexString(key.getKeyID()).toUpperCase().endsWith(keyId)) {
          return key;
        }
      }
    }

    throw new IllegalArgumentException("Can't find encryption key in key ring.");
  }

}
