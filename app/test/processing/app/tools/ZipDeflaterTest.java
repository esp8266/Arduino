package processing.app.tools;

import static org.junit.Assert.assertEquals;

import java.io.File;
import java.util.Arrays;
import java.util.Random;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import processing.app.tools.ZipDeflater;

public class ZipDeflaterTest {

  private File destFolder;

  @Before
  public void makeTempFolder() {
    destFolder = new File(System.getProperty("java.io.tmpdir") + File.separator + "arduino_zip_test_" + new Random().nextInt(100000));
    destFolder.mkdir();
  }

  @Test
  public void shouldDeflateZip() throws Exception {
    File file = new File(ZipDeflater.class.getResource("/Test2.zip").getFile());
    new ZipDeflater(file, destFolder).deflate();

    String[] files = destFolder.list();
    assertEquals(1, files.length);
    assertEquals("Test2", files[0]);

    file = destFolder.listFiles()[0];
    files = file.list();
    assertEquals(5, files.length);
    Arrays.sort(files);
    assertEquals("Test.cpp", files[0]);
    assertEquals("Test.h", files[1]);
    assertEquals("examples", files[2]);
    assertEquals("keywords.txt", files[3]);
    assertEquals("readme.txt", files[4]);
  }

  @Test
  public void shouldDeflateZipAndMoveContentsToParentFolder() throws Exception {
    File file = new File(ZipDeflater.class.getResource("/Test.zip").getFile());
    new ZipDeflater(file, destFolder).deflate();

    String[] files = destFolder.list();
    assertEquals(1, files.length);
    assertEquals("Test", files[0]);

    file = destFolder.listFiles()[0];
    files = file.list();
    assertEquals(5, files.length);
    Arrays.sort(files);
    assertEquals("Test.cpp", files[0]);
    assertEquals("Test.h", files[1]);
    assertEquals("examples", files[2]);
    assertEquals("keywords.txt", files[3]);
    assertEquals("readme.txt", files[4]);
  }

  @After
  public void deleteTempFolder() {
    recursiveDelete(destFolder);
  }

  private void recursiveDelete(File folder) {
    for (File file : folder.listFiles()) {
      if (file.isDirectory()) {
        recursiveDelete(file);
      } else {
        file.delete();
      }
    }
    folder.delete();
  }

}
