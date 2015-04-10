package processing.app.windows;

import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class ListComPortsParserTest {

  @Test
  public void shouldFindVIDPID() throws Exception {
    String listComPortsOutput = "COM26 - FTDI - FTDIBUS\\VID_0403+PID_6001+A6004CCFA\\0000\nCOM24 - PJRC.COM, LLC. - USB\\VID_16C0&PID_0483\\12345";

    assertEquals("0X0403_0X6001", new ListComPortsParser().extractVIDAndPID(listComPortsOutput, "COM26"));
    assertEquals("0X16C0_0X0483", new ListComPortsParser().extractVIDAndPID(listComPortsOutput, "COM24"));
  }

  @Test
  public void shouldFindVIDPID2() throws Exception {
    String listComPortsOutput = "COM1 - (Standard port types) - ACPI\\PNP0501\\1\n" +
            "COM3 - IVT Corporation - {F12D3CF8-B11D-457E-8641-BE2AF2D6D204}\\IVTCOMM\\1&27902E60&2&0001\n" +
            "COM4 - IVT Corporation - {F12D3CF8-B11D-457E-8641-BE2AF2D6D204}\\IVTCOMM\\1&27902E60&2&0002\n" +
            "COM18 - FTDI - FTDIBUS\\VID_0403+PID_0000+A9EPHBR7A\\0000";

    assertEquals("0X0403_0X0000", new ListComPortsParser().extractVIDAndPID(listComPortsOutput, "COM18"));
  }


}
