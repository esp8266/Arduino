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

package processing.app;

import org.apache.commons.compress.utils.IOUtils;
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
      IOUtils.closeQuietly(is);
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
