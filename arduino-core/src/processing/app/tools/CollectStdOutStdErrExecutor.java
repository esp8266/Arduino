package processing.app.tools;

import org.apache.commons.exec.DefaultExecutor;
import org.apache.commons.exec.ExecuteStreamHandler;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/**
 * Handy process executor, collecting stdout and stderr into given OutputStreams
 */
public class CollectStdOutStdErrExecutor extends DefaultExecutor {

  public CollectStdOutStdErrExecutor(final OutputStream stdout, final OutputStream stderr) {
    this.setStreamHandler(new ExecuteStreamHandler() {
      @Override
      public void setProcessInputStream(OutputStream outputStream) throws IOException {
      }

      @Override
      public void setProcessErrorStream(InputStream inputStream) throws IOException {
        byte[] buf = new byte[4096];
        int bytes = -1;
        while ((bytes = inputStream.read(buf)) != -1) {
          stderr.write(buf, 0, bytes);
        }
      }

      @Override
      public void setProcessOutputStream(InputStream inputStream) throws IOException {
        byte[] buf = new byte[4096];
        int bytes = -1;
        while ((bytes = inputStream.read(buf)) != -1) {
          stdout.write(buf, 0, bytes);
        }
      }

      @Override
      public void start() throws IOException {
      }

      @Override
      public void stop() {
      }
    });

  }
}
