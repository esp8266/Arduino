/*
 * This file is part of Arduino.
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
 *
 * Copyright 2013 Arduino LLC (http://www.arduino.cc/)
 */

package cc.arduino.packages.ssh;

import com.jcraft.jsch.Channel;
import com.jcraft.jsch.ChannelExec;
import com.jcraft.jsch.JSchException;
import com.jcraft.jsch.Session;

import java.io.IOException;
import java.io.InputStream;
import java.io.PrintStream;

public class SSH {

  protected final Session session;

  public SSH(Session session) {
    this.session = session;
  }

  public boolean execSyncCommand(String command) throws JSchException, IOException {
    return execSyncCommand(command, null, null);
  }

  public boolean execSyncCommand(String command, PrintStream stdoutConsumer, PrintStream stderrConsumer) throws JSchException, IOException {
    InputStream stdout = null;
    InputStream stderr = null;
    Channel channel = null;
    try {
      channel = session.openChannel("exec");
      ((ChannelExec) channel).setCommand(command);

      channel.setInputStream(null);

      stdout = channel.getInputStream();
      stderr = ((ChannelExec) channel).getErrStream();

      channel.connect();

      int exitCode = consumeOutputSyncAndReturnExitCode(channel, stdout, stdoutConsumer, stderr, stderrConsumer);

      return exitCode == 0;

    } finally {
      if (stdout != null) {
        stdout.close();
      }
      if (stderr != null) {
        stderr.close();
      }
      if (channel != null) {
        channel.disconnect();
      }
    }
  }

  protected int consumeOutputSyncAndReturnExitCode(Channel channel, InputStream stdout, PrintStream stdoutConsumer, InputStream stderr, PrintStream stderrConsumer) throws IOException {
    byte[] tmp = new byte[102400];
    while (true) {
      consumeStream(tmp, stdout, stdoutConsumer);
      consumeStream(tmp, stderr, stderrConsumer);

      if (channel.isClosed()) {
        return channel.getExitStatus();
      }
      try {
        Thread.sleep(100);
      } catch (Exception ee) {
        // noop
      }
    }
  }

  protected void consumeStream(byte[] buffer, InputStream in, PrintStream out) throws IOException {
    while (in.available() > 0) {
      int length = in.read(buffer, 0, buffer.length);
      if (length < 0) {
        break;
      }
      if (out != null) {
        out.print(new String(buffer, 0, length));
      }
    }
  }

}
