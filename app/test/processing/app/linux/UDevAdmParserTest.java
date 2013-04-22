package processing.app.linux;

import org.junit.Test;
import processing.app.TestHelper;

import static org.junit.Assert.assertEquals;

public class UDevAdmParserTest {

  @Test
  public void shouldCorrectlyParse() throws Exception {
    String output = TestHelper.inputStreamToString(UDevAdmParserTest.class.getResourceAsStream("udev_output.txt"));

    assertEquals("0X2341_0X0036", new UDevAdmParser().extractVIDAndPID(output));
  }
}
