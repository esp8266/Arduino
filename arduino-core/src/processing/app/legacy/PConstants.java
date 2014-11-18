package processing.app.legacy;

public class PConstants {

  // platform IDs for PApplet.platform

  public static final int OTHER   = 0;
  public static final int WINDOWS = 1;
  public static final int MACOSX  = 2;
  public static final int LINUX   = 3;

  public static final String[] platformNames = {
    "other", "windows", "macosx", "linux"
  };


  // used by split, all the standard whitespace chars
  // (also includes unicode nbsp, that little bostage)

  static final String WHITESPACE = " \t\n\r\f\u00A0";

}
