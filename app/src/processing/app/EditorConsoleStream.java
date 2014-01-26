package processing.app;

import static processing.app.I18n._;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;

class EditorConsoleStream extends OutputStream {
  static File tempFolder;
  static File outFile;
  static File errFile;

  static EditorConsole currentConsole;

  static OutputStream stderrFile;
  static OutputStream stdoutFile;
  static PrintStream consoleErr;
  static PrintStream consoleOut;
  static public PrintStream systemErr;
  static public PrintStream systemOut;

  public static void init() {
    if (systemOut == null) {
      systemOut = System.out;
      systemErr = System.err;

      // Create a temporary folder which will have a randomized name. Has to
      // be randomized otherwise another instance of Processing (or one of its
      // sister IDEs) might collide with the file causing permissions problems.
      // The files and folders are not deleted on exit because they may be
      // needed for debugging or bug reporting.
      tempFolder = Base.createTempFolder("console");
      tempFolder.deleteOnExit();
      try {
        String outFileName = Preferences.get("console.output.file");
        if (outFileName != null) {
          outFile = new File(tempFolder, outFileName);
          outFile.deleteOnExit();
          stdoutFile = new FileOutputStream(outFile);
        }

        String errFileName = Preferences.get("console.error.file");
        if (errFileName != null) {
          errFile = new File(tempFolder, errFileName);
          errFile.deleteOnExit();
          stderrFile = new FileOutputStream(errFile);
        }
      } catch (IOException e) {
        Base.showWarning(_("Console Error"),
                         _("A problem occurred while trying to open the\nfiles used to store the console output."),
                         e);
      }
      consoleOut = new PrintStream(new EditorConsoleStream(false));
      consoleErr = new PrintStream(new EditorConsoleStream(true));

      if (Preferences.getBoolean("console")) {
        try {
          System.setOut(consoleOut);
          System.setErr(consoleErr);
        } catch (Exception e) {
          e.printStackTrace(systemOut);
        }
      }
    }
  }

  /**
   * Close the streams so that the temporary files can be deleted.
   * <p/>
   * File.deleteOnExit() cannot be used because the stdout and stderr files are
   * inside a folder, and have to be deleted before the folder itself is
   * deleted, which can't be guaranteed when using the deleteOnExit() method.
   */
  public static void quit() {
    // replace original streams to remove references to console's streams
    System.setOut(systemOut);
    System.setErr(systemErr);

    // close the PrintStream
    consoleOut.close();
    consoleErr.close();

    // also have to close the original FileOutputStream
    // otherwise it won't be shut down completely
    try {
      stdoutFile.close();
      stderrFile.close();
    } catch (IOException e) {
      e.printStackTrace();
    }

    outFile.delete();
    errFile.delete();
    tempFolder.delete();
  }

  final boolean err; // whether stderr or stdout
  PrintStream system;
  OutputStream file;

  public EditorConsoleStream(boolean _err) {
    err = _err;
    if (err) {
      system = systemErr;
      file = stderrFile;
    } else {
      system = systemOut;
      file = stdoutFile;
    }
  }

  public void close() {
  }

  public void flush() {
  }

  public void write(int b) {
    write(new byte[] { (byte) b });
  }

  public void write(byte b[]) { // appears never to be used
    write(b, 0, b.length);
  }

  public void write(byte b[], int offset, int length) {
    if (currentConsole != null)
      currentConsole.appendText(new String(b, offset, length), err);

    system.write(b, offset, length);

    if (file != null) {
      try {
        file.write(b, offset, length);
        file.flush();
      } catch (IOException e) {
        e.printStackTrace();
        file = null;
      }
    }
  }

  static public void setCurrent(EditorConsole console) {
    currentConsole = console;
  }

}