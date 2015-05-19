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

package processing.app.preproc;

import org.junit.Test;
import processing.app.helpers.FileUtils;

import java.io.File;

import static org.junit.Assert.assertEquals;

public class PdePreprocessorTest {

  @Test
  public void testSourceWithQuoteAndDoubleQuotesEscapedAndFinalQuoteShouldNotRaiseException() throws Exception {
    String s = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("RemoteCallLogger_v1e0.ino").getFile()));

    PdePreprocessor pdePreprocessor = new PdePreprocessor();
    String strippedOutput = pdePreprocessor.strip(s);
    String expectedStrippedOutput = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("RemoteCallLogger_v1e0.stripped.ino").getFile()));

    assertEquals(expectedStrippedOutput, strippedOutput);

    pdePreprocessor.writePrefix(s);

    String actualCodeWithoutComments = pdePreprocessor.program;
    String expectedCodeWithoutComments = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("RemoteCallLogger_v1e0.nocomments.ino").getFile()));

    assertEquals(expectedCodeWithoutComments, actualCodeWithoutComments);

    assertEquals(2, pdePreprocessor.getExtraImports().size());
    assertEquals("SoftwareSerial.h", pdePreprocessor.getExtraImports().get(0));
    assertEquals("Wire.h", pdePreprocessor.getExtraImports().get(1));
  }

  @Test
  public void testIncludeInsideMultilineComment() throws Exception {
    String s = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("IncludeBetweenMultilineComment.ino").getFile()));

    PdePreprocessor pdePreprocessor = new PdePreprocessor();
    String strippedOutput = pdePreprocessor.strip(s);
    String expectedStrippedOutput = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("IncludeBetweenMultilineComment.stripped.ino").getFile()));

    assertEquals(expectedStrippedOutput, strippedOutput);

    pdePreprocessor.writePrefix(s);

    String actualCodeWithoutComments = pdePreprocessor.program;
    String expectedCodeWithoutComments = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("IncludeBetweenMultilineComment.nocomments.ino").getFile()));

    assertEquals(expectedCodeWithoutComments, actualCodeWithoutComments);

    assertEquals(1, pdePreprocessor.getExtraImports().size());
    assertEquals("CapacitiveSensorDue.h", pdePreprocessor.getExtraImports().get(0));
  }

  @Test
  public void testPdePreprocessorRegressionBaladuino() throws Exception {
    String s = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("Baladuino.ino").getFile()));

    PdePreprocessor pdePreprocessor = new PdePreprocessor();
    String strippedOutput = pdePreprocessor.strip(s);
    String expectedStrippedOutput = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("Baladuino.stripped.ino").getFile()));

    assertEquals(expectedStrippedOutput, strippedOutput);

    pdePreprocessor.writePrefix(s);

    String actualCodeWithoutComments = pdePreprocessor.program;
    String expectedCodeWithoutComments = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("Baladuino.nocomments.ino").getFile()));

    assertEquals(expectedCodeWithoutComments, actualCodeWithoutComments);

    assertEquals(9, pdePreprocessor.getExtraImports().size());
    assertEquals("Balanduino.h", pdePreprocessor.getExtraImports().get(0));
    assertEquals("Wire.h", pdePreprocessor.getExtraImports().get(1));
    assertEquals("usbhub.h", pdePreprocessor.getExtraImports().get(2));
    assertEquals("adk.h", pdePreprocessor.getExtraImports().get(3));
    assertEquals("Kalman.h", pdePreprocessor.getExtraImports().get(4));
    assertEquals("XBOXRECV.h", pdePreprocessor.getExtraImports().get(5));
    assertEquals("SPP.h", pdePreprocessor.getExtraImports().get(6));
    assertEquals("PS3BT.h", pdePreprocessor.getExtraImports().get(7));
    assertEquals("Wii.h", pdePreprocessor.getExtraImports().get(8));
  }

  @Test
  public void testStringWithCcomment() throws Exception {
    String s = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("StringWithCcomment.ino").getFile()));

    PdePreprocessor pdePreprocessor = new PdePreprocessor();
    String strippedOutput = pdePreprocessor.strip(s);
    String expectedStrippedOutput = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("StringWithCcomment.stripped.ino").getFile()));

    assertEquals(expectedStrippedOutput, strippedOutput);

    pdePreprocessor.writePrefix(s);

    String actualCodeWithoutComments = pdePreprocessor.program;
    String expectedCodeWithoutComments = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("StringWithCcomment.nocomments.ino").getFile()));

    assertEquals(expectedCodeWithoutComments, actualCodeWithoutComments);

    assertEquals(0, pdePreprocessor.getExtraImports().size());
  }

  @Test
  public void testCharWithEscapedDoubleQuote() throws Exception {
    String s = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("CharWithEscapedDoubleQuote.ino").getFile()));

    PdePreprocessor pdePreprocessor = new PdePreprocessor();
    String strippedOutput = pdePreprocessor.strip(s);
    String expectedStrippedOutput = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("CharWithEscapedDoubleQuote.stripped.ino").getFile()));

    assertEquals(expectedStrippedOutput, strippedOutput);

    pdePreprocessor.writePrefix(s);

    String actualCodeWithoutComments = pdePreprocessor.program;
    String expectedCodeWithoutComments = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("CharWithEscapedDoubleQuote.nocomments.ino").getFile()));

    assertEquals(expectedCodeWithoutComments, actualCodeWithoutComments);

    assertEquals(2, pdePreprocessor.getExtraImports().size());
    assertEquals("SoftwareSerial.h", pdePreprocessor.getExtraImports().get(0));
    assertEquals("Wire.h", pdePreprocessor.getExtraImports().get(1));
  }

  @Test
  public void testLineContinuations() throws Exception {
    String s = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("LineContinuations.ino").getFile()));

    PdePreprocessor pdePreprocessor = new PdePreprocessor();
    String strippedOutput = pdePreprocessor.strip(s);
    String expectedStrippedOutput = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("LineContinuations.stripped.ino").getFile()));

    assertEquals(expectedStrippedOutput, strippedOutput);

    pdePreprocessor.writePrefix(s);

    String actualCodeWithoutComments = pdePreprocessor.program;
    String expectedCodeWithoutComments = FileUtils.readFileToString(new File(PdePreprocessorTest.class.getResource("LineContinuations.nocomments.ino").getFile()));

    assertEquals(expectedCodeWithoutComments, actualCodeWithoutComments);

    assertEquals(0, pdePreprocessor.getExtraImports().size());
  }

}