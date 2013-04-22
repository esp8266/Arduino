package processing.app.macosx;

import org.junit.Test;
import processing.app.TestHelper;

import static org.junit.Assert.assertEquals;

public class SystemProfilerParserTest {

  @Test
  public void shouldCorrectlyParse() throws Exception {
    String output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output.txt"));

    assertEquals("2341_0044", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbmodemfa121"));
    assertEquals("2341_0044", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodemfa121"));

       output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output2.txt"));

      assertEquals("2341_8036", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbmodemfd131"));
      assertEquals("2341_8036", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodemfd131"));

  }
}
