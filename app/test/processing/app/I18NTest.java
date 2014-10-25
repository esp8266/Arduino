package processing.app;

import org.junit.Ignore;
import org.junit.Test;

import java.io.*;
import java.util.*;

public class I18NTest {

  private Set<String> loadReferenceI18NKeys() throws IOException {
    Properties properties = new Properties();
    properties.putAll(loadProperties(new File(I18NTest.class.getResource("./i18n/Resources_en.properties").getFile())));
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

  // XXX: I18NTest.class.getResource(".").getFile() no longer works, because
  // the class is now into the arudino-core package. This test should be refactored
  // in order to use ResourceBundles to load translations to be checked.

  @Test
  @Ignore
  public void ensureEveryTranslationIsComplete() throws Exception {
    Set<String> keys = loadReferenceI18NKeys();

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
      List<String> filesWithIncompleteTranslations = new LinkedList<String>(missingTranslationsPerFile.keySet());
      Collections.sort(filesWithIncompleteTranslations);
      System.out.println("Following files have missing translations:" + filesWithIncompleteTranslations);
      for (String file : filesWithIncompleteTranslations) {

        System.out.println("Following translations in file " + file + " are missing:");
        for (String key : missingTranslationsPerFile.get(file)) {
          System.out.println("==> '" + key.replaceAll("\n", "\\\\n").replaceAll(" ", "\\\\ ") + "'");
        }
        System.out.println();
      }
    }
  }

  private void addMissingTranslation(Map<String, List<String>> missingTranslationsPerFile, File file, String key) {
    if (!missingTranslationsPerFile.containsKey(file.getName())) {
      missingTranslationsPerFile.put(file.getName(), new LinkedList<String>());
    }

    missingTranslationsPerFile.get(file.getName()).add(key);
  }

}
