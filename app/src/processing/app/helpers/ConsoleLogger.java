package processing.app.helpers;

import java.io.FileDescriptor;
import java.io.FileOutputStream;
import java.io.PrintStream;
import java.util.logging.LogRecord;
import java.util.logging.StreamHandler;

public class ConsoleLogger extends StreamHandler {

  public ConsoleLogger() {
    setOutputStream(new PrintStream(new FileOutputStream(FileDescriptor.out)));
  }


  public void publish(LogRecord record) {
    super.publish(record);
    flush();
  }

  /**
   * Override <tt>StreamHandler.close</tt> to do a flush but not
   * to close the output stream. That is, we do <b>not</b>
   * close <tt>FileDescriptor.out</tt>.
   */
  public void close() {
    flush();
  }

}
