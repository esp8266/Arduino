package processing.app.windows;

import org.junit.Test;

import static org.junit.Assert.assertEquals;

public class RegQueryParserTest {

  @Test
  public void testRegQueryParser() throws Exception {
    String output = "! REG.EXE VERSION 3.0\n" +
      "\n" +
      "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders\n" +
      "\n" +
      "    Local AppData    REG_SZ    C:\\Documents and Settings\\username\\My Documents";

    String folderPath = new RegQueryParser(output).getValueOfKey();
    assertEquals("C:\\Documents and Settings\\username\\My Documents", folderPath);
  }

  @Test
  public void testRegQueryParser2() throws Exception {
    String output = "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders\n" +
      "    Local AppData    REG_SZ    C:\\Users\\username\\AppData\\Local";

    String folderPath = new RegQueryParser(output).getValueOfKey();
    assertEquals("C:\\Users\\username\\AppData\\Local", folderPath);
  }

  @Test
  public void testRegQueryParserXP() throws Exception {
    String output = "! REG.EXE VERSION 3.0\n" +
      "\n" +
      "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders\n" +
      "\n" +
      "\tLocal AppData    REG_SZ    C:\\Documents and Settings\\username\\My Documents";

    String folderPath = new RegQueryParser(output).getValueOfKey();
    assertEquals("C:\\Documents and Settings\\username\\My Documents", folderPath);
  }

}
