package processing.app.macosx;

import org.junit.Test;
import processing.app.TestHelper;

import static org.junit.Assert.assertEquals;

public class SystemProfilerParserTest {

  @Test
  public void shouldCorrectlyParse() throws Exception {
    String output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output.txt"));

    assertEquals("0X2341_0X0044", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbmodemfa121"));
    assertEquals("0X2341_0X0044", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodemfa121"));

    output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output2.txt"));

    assertEquals("0X2341_0X8036", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbmodemfd131"));
    assertEquals("0X2341_0X8036", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodemfd131"));

    output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output3.txt"));

    assertEquals("0X2341_0X8041", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbmodemfd121"));
    assertEquals("0X2341_0X8041", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodemfd121"));

    output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output4.txt"));

    assertEquals("0X2341_0X0041", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbmodem411"));
    assertEquals("0X2341_0X0041", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodem411"));

    output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output5.txt"));

    assertEquals("0X2341_0X8041", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbmodem621"));
    assertEquals("0X2341_0X8041", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodem621"));

    output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output6.txt"));

    assertEquals("0X2341_0X8041", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbmodem1421"));
    assertEquals("0X2341_0X8041", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodem1421"));

    output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output7.txt"));

    assertEquals("0X2341_0X8036", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbmodem24131"));
    assertEquals("0X2341_0X8036", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodem24131"));
    assertEquals("0X0403_0X6015", new SystemProfilerParser().extractVIDAndPID(output, "/dev/cu.usbserial-DN0031EV"));
    assertEquals("0X0403_0X6015", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbserial-DN0031EV"));
  }
}
