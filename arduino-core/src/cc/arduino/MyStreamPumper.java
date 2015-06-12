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
