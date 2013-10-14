package cc.arduino.packages.formatter;

import processing.app.Base;

import java.io.File;

public class AStyleInterface {

  static {
    File astyleLib = new File(Base.getContentFile("lib"), System.mapLibraryName("astylej"));
    String astylePath = astyleLib.getAbsolutePath();
    try {
      System.load(astylePath);
    } catch (UnsatisfiedLinkError e) {
      e.printStackTrace();
      System.out.println(e.getMessage());
      System.out.println("Cannot load native library " + astylePath);
      System.out.println("The program has terminated!");
      System.exit(1);
    }
  }

  /**
   * Calls the AStyleMain function in Artistic Style.
   *
   * @param textIn  A string containing the source code to be formatted.
   * @param options A string of options to Artistic Style.
   * @return A String containing the formatted source from Artistic Style.
   */
  public native String AStyleMain(String textIn, String options);

  /**
   * Calls the AStyleGetVersion function in Artistic Style.
   *
   * @return A String containing the version number of Artistic Style.
   */
  public native String AStyleGetVersion();

  /**
   * Error handler for messages from Artistic Style.
   * This method is called only if there are errors when AStyleMain is called.
   * This is for debugging and there should be no errors when the calling
   * parameters are correct.
   * Changing the method name requires changing Artistic Style.
   * Signature: (ILjava/lang/String;)V.
   *
   * @param errorNumber  The error number from Artistic Style.
   * @param errorMessage The error message from Artistic Style.
   */
  private void ErrorHandler(int errorNumber, String errorMessage) {
    System.out.println("AStyle error " + String.valueOf(errorNumber) + " - " + errorMessage);
  }

}
