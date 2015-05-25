package cc.arduino;

import processing.app.debug.MessageConsumer;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;

/**
 * A version of StreamPumper from commons-exec that write to a MessageConsumer
 */
public class MyStreamPumper implements Runnable {

  /**
   * the input stream to pump from
   */
  private final BufferedReader reader;

  /**
   * the output stream to pmp into
   */
  private final MessageConsumer messageConsumer;

  /**
   * was the end of the stream reached
   */
  private boolean finished;

  public MyStreamPumper(final InputStream is, final MessageConsumer messageConsumer) {
    this.reader = new BufferedReader(new InputStreamReader(is));
    this.messageConsumer = messageConsumer;
  }

  /**
   * Copies data from the input stream to the output stream. Terminates as
   * soon as the input stream is closed or an error occurs.
   */
  public void run() {
    synchronized (this) {
      // Just in case this object is reused in the future
      finished = false;
    }

    try {
      String line;
      while ((line = reader.readLine()) != null) {
        messageConsumer.message(line + "\n");
      }
    } catch (Exception e) {
      // nothing to do - happens quite often with watchdog
    } finally {
      synchronized (this) {
        finished = true;
        notifyAll();
      }
    }
  }

  /**
   * Tells whether the end of the stream has been reached.
   *
   * @return true is the stream has been exhausted.
   */
  public synchronized boolean isFinished() {
    return finished;
  }

  /**
   * This method blocks until the stream pumper finishes.
   *
   * @see #isFinished()
   */
  public synchronized void waitFor() throws InterruptedException {
    while (!isFinished()) {
      wait();
    }
  }
}
