package cc.arduino.contributions;

import cc.arduino.contributions.libraries.LibrariesIndex;
import cc.arduino.utils.MultiStepProgress;
import com.fasterxml.jackson.databind.DeserializationFeature;
import com.fasterxml.jackson.databind.ObjectMapper;
import com.fasterxml.jackson.module.mrbean.MrBeanModule;
import org.junit.After;
import org.junit.Before;
import org.junit.Test;
import processing.app.helpers.FileUtils;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.net.URL;

import static org.junit.Assert.assertTrue;

public class JsonDownloaderTest {

  private File tempFolder;
  private File tempFile;
  private DownloadableContributionsDownloader downloader;

  @Before
  public void setUp() throws Exception {
    tempFolder = FileUtils.createTempFolder();
    tempFile = File.createTempFile("test", ".json");
    downloader = new DownloadableContributionsDownloader(tempFolder);
  }

  @After
  public void tearDown() throws Exception {
    FileUtils.recursiveDelete(tempFolder);
    FileUtils.recursiveDelete(tempFile);
  }

  @Test
  public void testJsonDownload() throws Exception {
    new JsonDownloader(downloader, new URL("http://downloads.arduino.cc/libraries/library_index.json")).download(tempFile, new MultiStepProgress(1), "");

    InputStream indexIn = new FileInputStream(tempFile);
    ObjectMapper mapper = new ObjectMapper();
    mapper.registerModule(new MrBeanModule());
    mapper.configure(DeserializationFeature.ACCEPT_SINGLE_VALUE_AS_ARRAY, true);
    mapper.configure(DeserializationFeature.EAGER_DESERIALIZER_FETCH, true);
    mapper.configure(DeserializationFeature.FAIL_ON_UNKNOWN_PROPERTIES, false);
    LibrariesIndex librariesIndex = mapper.readValue(indexIn, LibrariesIndex.class);

    assertTrue(librariesIndex != null);
  }
}
