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