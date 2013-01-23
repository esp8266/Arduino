package processing.app;

import org.junit.Test;

import java.io.*;
import java.util.*;

import static org.junit.Assert.assertTrue;

public class I18NTest {

  private Set<String> loadAllI18NKeys() throws IOException {
    Properties properties = new Properties();
    for (File file : listPropertiesFiles()) {
      properties.putAll(loadProperties(file));
    }
    Set<String> keys = new HashSet<String>();
    for (Object key : properties.keySet()) {
      keys.add(key.toString());
    }
    return keys;
  }

  private File[] listPropertiesFiles() {
    return new File(I18NTest.class.getResource(".").getFile()).listFiles(new FileFilter() {
      @Override
      public boolean accept(File file) {
        return file.isFile() && file.getName().endsWith(".properties");
      }
    });
  }

  private Properties loadProperties(File file) throws IOException {
    Properties properties = new Properties();
    InputStream is = null;
    try {
      is = new FileInputStream(file);
      properties.load(is);
    } finally {
      if (is != null) {
        is.close();
      }
    }
    return properties;
  }

  @Test
  public void ensureEveryTranslationIsComplete() throws Exception {
    Set<String> keys = loadAllI18NKeys();

    Map<String, List<String>> missingTranslationsPerFile = new HashMap<String, List<String>>();

    for (File file : listPropertiesFiles()) {
      Properties properties = loadProperties(file);
      for (String key : keys) {
        if (!properties.containsKey(key) || properties.get(key).equals("")) {
          addMissingTranslation(missingTranslationsPerFile, file, key);
        }
      }
    }

    if (!missingTranslationsPerFile.isEmpty()) {
      for (Map.Entry<String, List<String>> entry : missingTranslationsPerFile.entrySet()) {
        System.out.println("Following translations in file " + entry.getKey() + " are missing:");
        for (String key : entry.getValue()) {
          System.out.println("==> '" + key + "'");
        }
        System.out.println();
      }
      assertTrue(false);
    }
  }

  private void addMissingTranslation(Map<String, List<String>> missingTranslationsPerFile, File file, String key) {
    if (!missingTranslationsPerFile.containsKey(file.getName())) {
      missingTranslationsPerFile.put(file.getName(), new LinkedList<String>());
    }

    missingTranslationsPerFile.get(file.getName()).add(key);
  }

}
