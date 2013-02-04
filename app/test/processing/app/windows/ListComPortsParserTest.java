package processing.app.windows;

import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class ListComPortsParserTest {

  @Test
  public void shouldFindVIDPID() throws Exception {
    String listComPortsOutput = "COM26 - FTDI - FTDIBUS\\VID_0403+PID_6001+A6004CCFA\\0000\nCOM24 - PJRC.COM, LLC. - USB\\VID_16C0&PID_0483\\12345";

    ListComPortsParser parser = new ListComPortsParser();

    assertEquals("0403_6001", parser.extractVIDAndPID(listComPortsOutput, "COM26"));
    assertEquals("16C0_0483", parser.extractVIDAndPID(listComPortsOutput, "COM24"));
  }


}
