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

package cc.arduino.packages.security;

import cc.arduino.packages.security.keys.PackagersPublicKeys;
import org.bouncycastle.bcpg.ArmoredInputStream;
import org.bouncycastle.jce.provider.BouncyCastleProvider;
import org.bouncycastle.openpgp.*;
import org.bouncycastle.openpgp.operator.jcajce.JcaPGPContentVerifierBuilderProvider;
import processing.app.helpers.StringUtils;

import java.io.*;
import java.security.Security;

public class ClearSignedVerifier {

  public static class VerifyResult {
    public byte clearText[];
    public boolean verified;
    public Exception error;
  }

  /**
   * Verify a PGP clearText-signature.
   *
   * @param signedTextFile A File containing the clearText signature
   * @param pubKeyRing     A public key-ring containing the public key needed for the
   *                       signature verification
   * @return A VerifyResult class with the clearText and the signature
   * verification status
   * @throws FileNotFoundException
   */
  public static VerifyResult verify(File signedTextFile,
                                    PGPPublicKeyRingCollection pubKeyRing) {
    // Create the result object
    VerifyResult result = new VerifyResult();
    result.clearText = null;
    result.verified = false;
    result.error = null;

    ArmoredInputStream in = null;
    try {
      // Extract clear text.
      // Dash-encoding is removed by ArmoredInputStream.
      in = new ArmoredInputStream(new FileInputStream(signedTextFile));
      ByteArrayOutputStream temp = new ByteArrayOutputStream(in.available());
      while (true) {
        int c = in.read();
        if (c == -1)
          throw new IOException("Unexpected end of file");
        if (!in.isClearText())
          break;
        temp.write(c);
      }
      byte clearText[] = temp.toByteArray();
      result.clearText = clearText;

      // Extract signature from clear-signed text
      PGPObjectFactory pgpFact = new PGPObjectFactory(in);
      PGPSignatureList p3 = (PGPSignatureList) pgpFact.nextObject();
      PGPSignature sig = p3.get(0);

      // Decode public key
      PGPPublicKey publicKey = pubKeyRing.getPublicKey(sig.getKeyID());

      // Verify signature
      Security.addProvider(new BouncyCastleProvider());
      sig.init(new JcaPGPContentVerifierBuilderProvider().setProvider("BC"),
              publicKey);
      // RFC 4880, section 7: http://tools.ietf.org/html/rfc4880#section-7
      // The signature must be validated using clear text:
      // - without trailing white spaces on every line
      // - using CR LF line endings, no matter what the original line ending is
      // - without the latest line ending
      BufferedReader textIn = new BufferedReader(new InputStreamReader(
              new ByteArrayInputStream(clearText)));
      while (true) {
        // remove trailing whitespace and line endings
        String line = StringUtils.rtrim(textIn.readLine());
        sig.update(line.getBytes());
        if (!textIn.ready()) // skip latest line ending
          break;
        // always use CR LF
        sig.update((byte) '\r');
        sig.update((byte) '\n');
      }

      // Prepare the result
      result.verified = sig.verify();
    } catch (Exception e) {
      result.error = e;
    } finally {
      if (in != null)
        try {
          in.close();
        } catch (IOException e) {
          // ignored
        }
    }
    return result;
  }

  public static void main(String[] args) throws Exception {
    VerifyResult verify = verify(new File(
            "/home/megabug/git/arduino/test.txt.asc"), new PackagersPublicKeys());
    System.out.println(verify.verified);
  }
}
