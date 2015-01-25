package processing.app;

import static processing.app.I18n._;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.util.Arrays;
import java.util.MissingResourceException;

import processing.app.helpers.PreferencesMap;
import processing.app.legacy.PApplet;
import processing.app.legacy.PConstants;


public class PreferencesData {

  static final String PREFS_FILE = "preferences.txt";

  // data model

  static PreferencesMap defaults;
  static PreferencesMap prefs = new PreferencesMap();
  static File preferencesFile;
  static boolean doSave = true;


  static public void init(File file) {
    if (file != null)
      preferencesFile = file;
    else
      preferencesFile = BaseNoGui.getSettingsFile(PREFS_FILE);

    // start by loading the defaults, in case something
    // important was deleted from the user prefs
    try {
      prefs.load(BaseNoGui.getLibStream("preferences.txt"));
    } catch (IOException e) {
      BaseNoGui.showError(null, _("Could not read default settings.\n" +
                                  "You'll need to reinstall Arduino."), e);
    }

    // set some runtime constants (not saved on preferences file)
    File hardwareFolder = BaseNoGui.getHardwareFolder();
    prefs.put("runtime.ide.path", hardwareFolder.getParentFile().getAbsolutePath());
    prefs.put("runtime.ide.version", "" + BaseNoGui.REVISION);
    
    // clone the hash table
    defaults = new PreferencesMap(prefs);

    if (preferencesFile.exists()) {
      // load the previous preferences file
      try {
        prefs.load(preferencesFile);
      } catch (IOException ex) {
        BaseNoGui.showError(_("Error reading preferences"),
                            I18n.format(_("Error reading the preferences file. "
                                            + "Please delete (or move)\n"
                                            + "{0} and restart Arduino."),
                                        preferencesFile.getAbsolutePath()), ex);
      }
    }

    // load the I18n module for internationalization
    try {
      I18n.init(get("editor.languages.current"));
    } catch (MissingResourceException e) {
      I18n.init("en");
      set("editor.languages.current", "en");
    }

    // set some other runtime constants (not saved on preferences file)
    set("runtime.os", PConstants.platformNames[PApplet.platform]);

    fixPreferences();
  }

  private static void fixPreferences() {
    String baud = get("serial.debug_rate");
    if ("14400".equals(baud) || "28800".equals(baud)) {
      set("serial.debug_rate", "9600");
    }
  }


  static public String[] loadStrings(InputStream input) {
    try {
      BufferedReader reader =
              new BufferedReader(new InputStreamReader(input, "UTF-8"));

      String lines[] = new String[100];
      int lineCount = 0;
      String line = null;
      while ((line = reader.readLine()) != null) {
        if (lineCount == lines.length) {
          String temp[] = new String[lineCount << 1];
          System.arraycopy(lines, 0, temp, 0, lineCount);
          lines = temp;
        }
        lines[lineCount++] = line;
      }
      reader.close();

      if (lineCount == lines.length) {
        return lines;
      }

      // resize array to appropriate amount for these lines
      String output[] = new String[lineCount];
      System.arraycopy(lines, 0, output, 0, lineCount);
      return output;

    } catch (IOException e) {
      e.printStackTrace();
      //throw new RuntimeException("Error inside loadStrings()");
    }
    return null;
  }


  static protected void save() {
    if (!doSave)
      return;

    // on startup, don't worry about it
    // this is trying to update the prefs for who is open
    // before Preferences.init() has been called.
    if (preferencesFile == null) return;

    // Fix for 0163 to properly use Unicode when writing preferences.txt
    PrintWriter writer = PApplet.createWriter(preferencesFile);

    String[] keys = prefs.keySet().toArray(new String[0]);
    Arrays.sort(keys);
    for (String key: keys) {
      if (key.startsWith("runtime."))
        continue;
      writer.println(key + "=" + prefs.get(key));
    }

    writer.flush();
    writer.close();
  }


  // .................................................................

  static public String get(String attribute) {
    return prefs.get(attribute);
  }

  static public String get(String attribute, String defaultValue) {
    String value = get(attribute);
    return (value == null) ? defaultValue : value;
  }

  public static boolean has(String key) {
    return prefs.containsKey(key);
  }

  public static void remove(String key) {
    prefs.remove(key);
  }

  static public String getDefault(String attribute) {
    return defaults.get(attribute);
  }


  static public void set(String attribute, String value) {
    prefs.put(attribute, value);
  }


  static public void unset(String attribute) {
    prefs.remove(attribute);
  }


  static public boolean getBoolean(String attribute) {
    return prefs.getBoolean(attribute);
  }


  static public void setBoolean(String attribute, boolean value) {
    prefs.putBoolean(attribute, value);
  }


  static public int getInteger(String attribute) {
    return Integer.parseInt(get(attribute));
  }


  static public void setInteger(String key, int value) {
    set(key, String.valueOf(value));
  }

  // get a copy of the Preferences
  static public PreferencesMap getMap() 
  {
    return new PreferencesMap(prefs);
  }

  // Decide wether changed preferences will be saved. When value is
  // false, Preferences.save becomes a no-op.
  static public void setDoSave(boolean value)
  {
    doSave = value;
  }
}
