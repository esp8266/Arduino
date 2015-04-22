package processing.app.helpers;

import java.io.PrintWriter;
import java.io.StringWriter;
import java.util.Date;
import java.util.logging.Formatter;
import java.util.logging.LogRecord;

public class LogFormatter extends Formatter  {
  
  public String format;
  private final Date dat = new Date();
  
  public LogFormatter(String logformat) {
    format = logformat;
  }

  @Override
  public String format(LogRecord record) {
    dat.setTime(record.getMillis());
    String source;
    if (record.getSourceClassName() != null) {
        source = record.getSourceClassName().substring(record.getSourceClassName().lastIndexOf('.') + 1);
        if (record.getSourceMethodName() != null) {
           source += "." + record.getSourceMethodName();
        }
    } else {
        source = record.getLoggerName();
    }
    String message = formatMessage(record);
    String throwable = "";
    if (record.getThrown() != null) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);
        pw.println();
        record.getThrown().printStackTrace(pw);
        pw.close();
        throwable = sw.toString();
    }
    return String.format(format,
                         dat,
                         source,
                         record.getLoggerName(),
                         record.getLevel(),
                         message,
                         throwable);
  }

}
