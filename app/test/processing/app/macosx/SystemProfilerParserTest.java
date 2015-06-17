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

    output = TestHelper.inputStreamToString(SystemProfilerParserTest.class.getResourceAsStream("system_profiler_output8.txt"));

    assertEquals("0X03EB_0X2157", new SystemProfilerParser().extractVIDAndPID(output, "/dev/tty.usbmodemfd132"));
  }
}
