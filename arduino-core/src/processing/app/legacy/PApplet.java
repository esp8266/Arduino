package processing.app.legacy;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.PrintWriter;
import java.io.UnsupportedEncodingException;
import java.text.NumberFormat;
import java.util.ArrayList;
import java.util.StringTokenizer;
import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.util.zip.GZIPInputStream;
import java.util.zip.GZIPOutputStream;

public class PApplet {

  /** Path to sketch folder */
  public String sketchPath; //folder;

  /**
   * Full name of the Java version (i.e. 1.5.0_11).
   * Prior to 0125, this was only the first three digits.
   */
  public static final String javaVersionName =
    System.getProperty("java.version");

  /**
   * Version of Java that's in use, whether 1.1 or 1.3 or whatever,
   * stored as a float.
   * <P>
   * Note that because this is stored as a float, the values may
   * not be <EM>exactly</EM> 1.3 or 1.4. Instead, make sure you're
   * comparing against 1.3f or 1.4f, which will have the same amount
   * of error (i.e. 1.40000001). This could just be a double, but
   * since Processing only uses floats, it's safer for this to be a float
   * because there's no good way to specify a double with the preproc.
   */
  public static final float javaVersion =
    new Float(javaVersionName.substring(0, 3)).floatValue();

  /**
   * Current platform in use, one of the
   * PConstants WINDOWS, MACOSX, MACOS9, LINUX or OTHER.
   */
  static public int platform;

  /**
   * Name associated with the current 'platform' (see PConstants.platformNames)
   */
  //static public String platformName;

  static {
    String osname = System.getProperty("os.name");

    if (osname.indexOf("Mac") != -1) {
      platform = PConstants.MACOSX;

    } else if (osname.indexOf("Windows") != -1) {
      platform = PConstants.WINDOWS;

    } else if (osname.equals("Linux")) {  // true for the ibm vm
      platform = PConstants.LINUX;

    } else {
      platform = PConstants.OTHER;
    }
  }

  /**
   * GIF image of the Processing logo.
   */
  static public final byte[] ICON_IMAGE = {
    71, 73, 70, 56, 57, 97, 16, 0, 16, 0, -60, 0, 0, 0, 0, 0, 
    0, 0, -127, 0, -127, 0, 0, -127, -127, -127, 0, 0, -127, 0, -127, -127, 
    -127, 0, -127, -127, -127, -63, -63, -63, 0, 0, -1, 0, -1, 0, 0, -1, 
    -1, -1, 0, 0, -1, 0, -1, -1, -1, 0, -1, -1, -1, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 33, -7, 4, 
    9, 0, 0, 16, 0, 44, 0, 0, 0, 0, 16, 0, 16, 0, 0, 5, 
    75, 32, 36, -118, -57, 96, 14, -57, -88, 66, -27, -23, -90, -86, 43, -97, 
    99, 59, -65, -30, 125, -77, 3, -14, -4, 8, -109, 15, -120, -22, 61, 78, 
    15, -124, 15, 25, 28, 28, 93, 63, -45, 115, -22, -116, 90, -83, 82, 89, 
    -44, -103, 61, 44, -91, -54, -89, 19, -111, 50, 18, -51, -55, 1, 73, -121, 
    -53, -79, 77, 43, -101, 12, -74, -30, -99, -24, -94, 16, 0, 59,
  };

  /**
   * Split the provided String at wherever whitespace occurs. Multiple
   * whitespace (extra spaces or tabs or whatever) between items will count as a
   * single break.
   * <P>
   * The whitespace characters are "\t\n\r\f", which are the defaults for
   * java.util.StringTokenizer, plus the unicode non-breaking space character,
   * which is found commonly on files created by or used in conjunction with Mac
   * OS X (character 160, or 0x00A0 in hex).
   * 
   * <PRE>
   * i.e. splitTokens("a b") -> { "a", "b" }
   *      splitTokens("a    b") -> { "a", "b" }
   *      splitTokens("a\tb") -> { "a", "b" }
   *      splitTokens("a \t  b  ") -> { "a", "b" }
   * </PRE>
   */
  static public String[] splitTokens(String what) {
    return splitTokens(what, PConstants.WHITESPACE);
  }

  /**
   * Splits a string into pieces, using any of the chars in the String 'delim'
   * as separator characters. For instance, in addition to white space, you
   * might want to treat commas as a separator. The delimeter characters won't
   * appear in the returned String array.
   * 
   * <PRE>
   * i.e. splitTokens("a, b", " ,") -> { "a", "b" }
   * </PRE>
   * 
   * To include all the whitespace possibilities, use the variable WHITESPACE,
   * found in PConstants:
   * 
   * <PRE>
   * i.e. splitTokens("a   | b", WHITESPACE + "|");  ->  { "a", "b" }
   * </PRE>
   */
  static public String[] splitTokens(String what, String delim) {
    StringTokenizer toker = new StringTokenizer(what, delim);
    String pieces[] = new String[toker.countTokens()];

    int index = 0;
    while (toker.hasMoreTokens()) {
      pieces[index++] = toker.nextToken();
    }
    return pieces;
  }

  /**
   * Split a string into pieces along a specific character. Most commonly used
   * to break up a String along a space or a tab character.
   * <P>
   * This operates differently than the others, where the single delimeter is
   * the only breaking point, and consecutive delimeters will produce an empty
   * string (""). This way, one can split on tab characters, but maintain the
   * column alignments (of say an excel file) where there are empty columns.
   */
  static public String[] split(String what, char delim) {
    // do this so that the exception occurs inside the user's
    // program, rather than appearing to be a bug inside split()
    if (what == null)
      return null;

    char chars[] = what.toCharArray();
    int splitCount = 0; // 1;
    for (int i = 0; i < chars.length; i++) {
      if (chars[i] == delim)
        splitCount++;
    }
    if (splitCount == 0) {
      String splits[] = new String[1];
      splits[0] = new String(what);
      return splits;
    }
    String splits[] = new String[splitCount + 1];
    int splitIndex = 0;
    int startIndex = 0;
    for (int i = 0; i < chars.length; i++) {
      if (chars[i] == delim) {
        splits[splitIndex++] = new String(chars, startIndex, i - startIndex);
        startIndex = i + 1;
      }
    }
    splits[splitIndex] = new String(chars, startIndex, chars.length
                                                       - startIndex);
    return splits;
  }

  static public String[] subset(String list[], int start, int count) {
    String output[] = new String[count];
    System.arraycopy(list, start, output, 0, count);
    return output;
  }


  /**
   * Join an array of Strings together as a single String,
   * separated by the whatever's passed in for the separator.
   */
  static public String join(String str[], char separator) {
    return join(str, String.valueOf(separator));
  }


  /**
   * Join an array of Strings together as a single String,
   * separated by the whatever's passed in for the separator.
   * <P>
   * To use this on numbers, first pass the array to nf() or nfs()
   * to get a list of String objects, then use join on that.
   * <PRE>
   * e.g. String stuff[] = { "apple", "bear", "cat" };
   *      String list = join(stuff, ", ");
   *      // list is now "apple, bear, cat"</PRE>
   */
  static public String join(String str[], String separator) {
    StringBuffer buffer = new StringBuffer();
    for (int i = 0; i < str.length; i++) {
      if (i != 0) buffer.append(separator);
      buffer.append(str[i]);
    }
    return buffer.toString();
  }

  /**
   * Parse a String into an int value. Returns 0 if the value is bad.
   */
  static final public int parseInt(String what) {
    return parseInt(what, 0);
  }

  /**
   * Parse a String to an int, and provide an alternate value that
   * should be used when the number is invalid.
   */
  static final public int parseInt(String what, int otherwise) {
    try {
      int offset = what.indexOf('.');
      if (offset == -1) {
        return Integer.parseInt(what);
      } else {
        return Integer.parseInt(what.substring(0, offset));
      }
    } catch (NumberFormatException e) { }
    return otherwise;
  }

  /**
   * Make an array of int elements from an array of String objects.
   * If the String can't be parsed as a number, it will be set to zero.
   *
   * String s[] = { "1", "300", "44" };
   * int numbers[] = parseInt(s);
   *
   * numbers will contain { 1, 300, 44 }
   */
  static public int[] parseInt(String what[]) {
    return parseInt(what, 0);
  }

  /**
   * Make an array of int elements from an array of String objects.
   * If the String can't be parsed as a number, its entry in the
   * array will be set to the value of the "missing" parameter.
   *
   * String s[] = { "1", "300", "apple", "44" };
   * int numbers[] = parseInt(s, 9999);
   *
   * numbers will contain { 1, 300, 9999, 44 }
   */
  static public int[] parseInt(String what[], int missing) {
    int output[] = new int[what.length];
    for (int i = 0; i < what.length; i++) {
      try {
        output[i] = Integer.parseInt(what[i]);
      } catch (NumberFormatException e) {
        output[i] = missing;
      }
    }
    return output;
  }

  static public String[] loadStrings(File file) {
    InputStream is = createInput(file);
    if (is != null) return loadStrings(is);
    return null;
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

  public void saveStrings(String filename, String strings[]) {
    saveStrings(saveFile(filename), strings);
  }


  static public void saveStrings(File file, String strings[]) {
    saveStrings(createOutput(file), strings);
  }


  static public void saveStrings(OutputStream output, String strings[]) {
    PrintWriter writer = createWriter(output);
    for (int i = 0; i < strings.length; i++) {
      writer.println(strings[i]);
    }
    writer.flush();
    writer.close();
  }


  static public int[] expand(int list[]) {
    return expand(list, list.length << 1);
  }

  static public int[] expand(int list[], int newSize) {
    int temp[] = new int[newSize];
    System.arraycopy(list, 0, temp, 0, Math.min(newSize, list.length));
    return temp;
  }

  static final public String hex(int what, int digits) {
    String stuff = Integer.toHexString(what).toUpperCase();

    int length = stuff.length();
    if (length > digits) {
      return stuff.substring(length - digits);

    } else if (length < digits) {
      return "00000000".substring(8 - (digits-length)) + stuff;
    }
    return stuff;
  }

  static public final int constrain(int amt, int low, int high) {
    return (amt < low) ? low : ((amt > high) ? high : amt);
  }

  static public final float constrain(float amt, float low, float high) {
    return (amt < low) ? low : ((amt > high) ? high : amt);
  }

  /**
   * Attempts to open an application or file using your platform's launcher. The <b>file</b> parameter is a String specifying the file name and location. The location parameter must be a full path name, or the name of an executable in the system's PATH. In most cases, using a full path is the best option, rather than relying on the system PATH. Be sure to make the file executable before attempting to open it (chmod +x).
   * <br><br>
   * The <b>args</b> parameter is a String or String array which is passed to the command line. If you have multiple parameters, e.g. an application and a document, or a command with multiple switches, use the version that takes a String array, and place each individual item in a separate element.
   * <br><br>
   * If args is a String (not an array), then it can only be a single file or application with no parameters. It's not the same as executing that String using a shell. For instance, open("jikes -help") will not work properly.
   * <br><br>
   * This function behaves differently on each platform. On Windows, the parameters are sent to the Windows shell via "cmd /c". On Mac OS X, the "open" command is used (type "man open" in Terminal.app for documentation). On Linux, it first tries gnome-open, then kde-open, but if neither are available, it sends the command to the shell without any alterations.
   * <br><br>
   * For users familiar with Java, this is not quite the same as Runtime.exec(), because the launcher command is prepended. Instead, the <b>exec(String[])</b> function is a shortcut for Runtime.getRuntime.exec(String[]).
   *
   * @webref input:files
   * @param filename name of the file
   * @usage Application
   */
  static public void open(String filename) {
    open(new String[] { filename });
  }

  static String openLauncher;

  /**
   * Launch a process using a platforms shell. This version uses an array
   * to make it easier to deal with spaces in the individual elements.
   * (This avoids the situation of trying to put single or double quotes
   * around different bits).
   *
   * @param list of commands passed to the command line
   */
  static public Process open(String argv[]) {
    String[] params = null;

    if (platform == PConstants.WINDOWS) {
      // just launching the .html file via the shell works
      // but make sure to chmod +x the .html files first
      // also place quotes around it in case there's a space
      // in the user.dir part of the url
      params = new String[] { "cmd", "/c" };

    } else if (platform == PConstants.MACOSX) {
      params = new String[] { "open" };

    } else if (platform == PConstants.LINUX) {
      if (openLauncher == null) {
        // Attempt to use gnome-open
        try {
          Process p = Runtime.getRuntime().exec(new String[] { "gnome-open" });
          /*int result =*/ p.waitFor();
          // Not installed will throw an IOException (JDK 1.4.2, Ubuntu 7.04)
          openLauncher = "gnome-open";
        } catch (Exception e) { }
      }
      if (openLauncher == null) {
        // Attempt with kde-open
        try {
          Process p = Runtime.getRuntime().exec(new String[] { "kde-open" });
          /*int result =*/ p.waitFor();
          openLauncher = "kde-open";
        } catch (Exception e) { }
      }
      if (openLauncher == null) {
        System.err.println("Could not find gnome-open or kde-open, " +
                           "the open() command may not work.");
      }
      if (openLauncher != null) {
        params = new String[] { openLauncher };
      }
    //} else {  // give up and just pass it to Runtime.exec()
      //open(new String[] { filename });
      //params = new String[] { filename };
    }
    if (params != null) {
      // If the 'open', 'gnome-open' or 'cmd' are already included
      if (params[0].equals(argv[0])) {
        // then don't prepend those params again
        return exec(argv);
      } else {
        params = concat(params, argv);
        return exec(params);
      }
    } else {
      return exec(argv);
    }
  }

  static public Process exec(String[] argv) {
    try {
      return Runtime.getRuntime().exec(argv);
    } catch (Exception e) {
      e.printStackTrace();
      throw new RuntimeException("Could not open " + join(argv, ' '));
    }
  }

  static public String[] concat(String a[], String b[]) {
    String c[] = new String[a.length + b.length];
    System.arraycopy(a, 0, c, 0, a.length);
    System.arraycopy(b, 0, c, a.length, b.length);
    return c;
  }

  /**
   * Identical to match(), except that it returns an array of all matches in
   * the specified String, rather than just the first.
   */
  static public String[][] matchAll(String what, String regexp) {
    Pattern p = Pattern.compile(regexp, Pattern.MULTILINE | Pattern.DOTALL);
    Matcher m = p.matcher(what);
    ArrayList<String[]> results = new ArrayList<String[]>();
    int count = m.groupCount() + 1;
    while (m.find()) {
      String[] groups = new String[count];
      for (int i = 0; i < count; i++) {
        groups[i] = m.group(i);
      }
      results.add(groups);
    }
    if (results.isEmpty()) {
      return null;
    }
    String[][] matches = new String[results.size()][count];
    for (int i = 0; i < matches.length; i++) {
      matches[i] = (String[]) results.get(i);
    }
    return matches;
  }
  
  /**
   * Match a string with a regular expression, and returns the match as an
   * array. The first index is the matching expression, and array elements
   * [1] and higher represent each of the groups (sequences found in parens).
   *
   * This uses multiline matching (Pattern.MULTILINE) and dotall mode
   * (Pattern.DOTALL) by default, so that ^ and $ match the beginning and
   * end of any lines found in the source, and the . operator will also
   * pick up newline characters.
   */
  static public String[] match(String what, String regexp) {
    Pattern p = Pattern.compile(regexp, Pattern.MULTILINE | Pattern.DOTALL);
    Matcher m = p.matcher(what);
    if (m.find()) {
      int count = m.groupCount() + 1;
      String[] groups = new String[count];
      for (int i = 0; i < count; i++) {
        groups[i] = m.group(i);
      }
      return groups;
    }
    return null;
  }

  /**
   * Integer number formatter.
   */
  static private NumberFormat int_nf;
  static private int int_nf_digits;
  static private boolean int_nf_commas;

  static public String[] nf(int num[], int digits) {
    String formatted[] = new String[num.length];
    for (int i = 0; i < formatted.length; i++) {
      formatted[i] = nf(num[i], digits);
    }
    return formatted;
  }

  static public String nf(int num, int digits) {
    if ((int_nf != null) &&
        (int_nf_digits == digits) &&
        !int_nf_commas) {
      return int_nf.format(num);
    }

    int_nf = NumberFormat.getInstance();
    int_nf.setGroupingUsed(false); // no commas
    int_nf_commas = false;
    int_nf.setMinimumIntegerDigits(digits);
    int_nf_digits = digits;
    return int_nf.format(num);
  }

  static final public String[] str(int x[]) {
    String s[] = new String[x.length];
    for (int i = 0; i < x.length; i++) s[i] = String.valueOf(x[i]);
    return s;
  }

  /**
   * I want to print lines to a file. I have RSI from typing these
   * eight lines of code so many times.
   */
  static public PrintWriter createWriter(File file) {
    try {
      createPath(file);  // make sure in-between folders exist
      OutputStream output = new FileOutputStream(file);
      if (file.getName().toLowerCase().endsWith(".gz")) {
        output = new GZIPOutputStream(output);
      }
      return createWriter(output);

    } catch (Exception e) {
      if (file == null) {
        throw new RuntimeException("File passed to createWriter() was null");
      } else {
        e.printStackTrace();
        throw new RuntimeException("Couldn't create a writer for " +
                                   file.getAbsolutePath());
      }
    }
    //return null;
  }


  /**
   * I want to print lines to a file. Why am I always explaining myself?
   * It's the JavaSoft API engineers who need to explain themselves.
   */
  static public PrintWriter createWriter(OutputStream output) {
    try {
      OutputStreamWriter osw = new OutputStreamWriter(output, "UTF-8");
      return new PrintWriter(osw);
    } catch (UnsupportedEncodingException e) { }  // not gonna happen
    return null;
  }
  
  static public InputStream createInput(File file) {
    if (file == null) {
      throw new IllegalArgumentException("File passed to createInput() was null");
    }
    try {
      InputStream input = new FileInputStream(file);
      if (file.getName().toLowerCase().endsWith(".gz")) {
        return new GZIPInputStream(input);
      }
      return input;

    } catch (IOException e) {
      System.err.println("Could not createInput() for " + file);
      e.printStackTrace();
      return null;
    }
  }

  /**
   * Returns a path inside the applet folder to save to. Like sketchPath(),
   * but creates any in-between folders so that things save properly.
   * <p/>
   * All saveXxxx() functions use the path to the sketch folder, rather than
   * its data folder. Once exported, the data folder will be found inside the
   * jar file of the exported application or applet. In this case, it's not
   * possible to save data into the jar file, because it will often be running
   * from a server, or marked in-use if running from a local file system.
   * With this in mind, saving to the data path doesn't make sense anyway.
   * If you know you're running locally, and want to save to the data folder,
   * use <TT>saveXxxx("data/blah.dat")</TT>.
   */
  public String savePath(String where) {
    if (where == null) return null;
    String filename = sketchPath(where);
    createPath(filename);
    return filename;
  }


  /**
   * Identical to savePath(), but returns a File object.
   */
  public File saveFile(String where) {
    return new File(savePath(where));
  }

  /**
   * Similar to createInput() (formerly openStream), this creates a Java
   * OutputStream for a given filename or path. The file will be created in
   * the sketch folder, or in the same folder as an exported application.
   * <p/>
   * If the path does not exist, intermediate folders will be created. If an
   * exception occurs, it will be printed to the console, and null will be
   * returned.
   * <p/>
   * Future releases may also add support for handling HTTP POST via this
   * method (for better symmetry with createInput), however that's maybe a
   * little too clever (and then we'd have to add the same features to the
   * other file functions like createWriter). Who you callin' bloated?
   */
  public OutputStream createOutput(String filename) {
    return createOutput(saveFile(filename));
  }


  static public OutputStream createOutput(File file) {
    try {
      createPath(file);  // make sure the path exists
      FileOutputStream fos = new FileOutputStream(file);
      if (file.getName().toLowerCase().endsWith(".gz")) {
        return new GZIPOutputStream(fos);
      }
      return fos;

    } catch (IOException e) {
      e.printStackTrace();
    }
    return null;
  }

  /**
   * Prepend the sketch folder path to the filename (or path) that is
   * passed in. External libraries should use this function to save to
   * the sketch folder.
   * <p/>
   * Note that when running as an applet inside a web browser,
   * the sketchPath will be set to null, because security restrictions
   * prevent applets from accessing that information.
   * <p/>
   * This will also cause an error if the sketch is not inited properly,
   * meaning that init() was never called on the PApplet when hosted
   * my some other main() or by other code. For proper use of init(),
   * see the examples in the main description text for PApplet.
   */
  public String sketchPath(String where) {
    if (sketchPath == null) {
      return where;
//      throw new RuntimeException("The applet was not inited properly, " +
//                                 "or security restrictions prevented " +
//                                 "it from determining its path.");
    }
    // isAbsolute() could throw an access exception, but so will writing
    // to the local disk using the sketch path, so this is safe here.
    // for 0120, added a try/catch anyways.
    try {
      if (new File(where).isAbsolute()) return where;
    } catch (Exception e) { }

    return sketchPath + File.separator + where;
  }

  /**
   * Takes a path and creates any in-between folders if they don't
   * already exist. Useful when trying to save to a subfolder that
   * may not actually exist.
   */
  static public void createPath(String path) {
    createPath(new File(path));
  }


  static public void createPath(File file) {
    try {
      String parent = file.getParent();
      if (parent != null) {
        File unit = new File(parent);
        if (!unit.exists()) unit.mkdirs();
      }
    } catch (SecurityException se) {
      System.err.println("You don't have permissions to create " +
                         file.getAbsolutePath());
    }
  }


}
