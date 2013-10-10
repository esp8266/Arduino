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
import com.jcraft.jsch.Session;

import java.io.*;

public class SCP extends SSH {

  private Channel channel;
  private OutputStream out;
  private InputStream in;

  public SCP(Session session) {
    super(session);
  }

  public void open() throws IOException {
    try {
      channel = session.openChannel("exec");
      ((ChannelExec) channel).setCommand("scp -t -r -d /");

      out = channel.getOutputStream();
      in = channel.getInputStream();

      channel.connect();
      ensureAcknowledged();
    } catch (Exception e) {
      close();
    }
  }

  public void close() throws IOException {
    if (out != null) {
      out.close();
    }
    if (in != null) {
      in.close();
    }
    if (channel != null) {
      channel.disconnect();
    }
  }

  protected void ensureAcknowledged() throws IOException {
    out.flush();

    int b = in.read();

    if (b == 0) return;
    if (b == -1) return;

    if (b == 1 || b == 2) {
      StringBuilder sb = new StringBuilder();
      sb.append("SCP error: ");

      int c;
      do {
        c = in.read();
        sb.append((char) c);
      } while (c != '\n');

      throw new IOException(sb.toString());
    }

    throw new IOException("Uknown SCP error: " + b);
  }

  public void sendFile(File localFile) throws IOException {
    sendFile(localFile, localFile.getName());
  }

  public void sendFile(File localFile, String remoteFile) throws IOException {
    out.write(("C0644 " + localFile.length() + " " + remoteFile + "\n").getBytes());
    ensureAcknowledged();

    FileInputStream fis = null;
    try {
      fis = new FileInputStream(localFile);
      byte[] buf = new byte[4096];
      while (true) {
        int len = fis.read(buf, 0, buf.length);
        if (len <= 0) break;
        out.write(buf, 0, len);
      }

      // \0 terminates file
      buf[0] = 0;
      out.write(buf, 0, 1);
    } finally {
      if (fis != null) {
        fis.close();
      }
    }

    ensureAcknowledged();
  }

  public void startFolder(String folder) throws IOException {
    out.write(("D0755 0 " + folder + "\n").getBytes());
    ensureAcknowledged();
  }

  public void endFolder() throws IOException {
    out.write("E\n".getBytes());
    ensureAcknowledged();
  }

}
