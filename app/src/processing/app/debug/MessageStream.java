/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2004-08 Ben Fry and Casey Reas
  Copyright (c) 2001-04 Massachusetts Institute of Technology

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software Foundation,
  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

package processing.app.debug;

import java.io.*;


/**
 * OutputStream to handle stdout/stderr messages.
 * <P>
 * This is used by Editor, System.err is set to
 * new PrintStream(new MessageStream()).
 * It's also used by Compiler.
 */
class MessageStream extends OutputStream {

  MessageConsumer messageConsumer;

  public MessageStream(MessageConsumer messageConsumer) {
    this.messageConsumer = messageConsumer;
  }

  public void close() { }

  public void flush() { }

  public void write(byte b[]) {
    // this never seems to get called
    System.out.println("leech1: " + new String(b));
  }

  public void write(byte b[], int offset, int length) {
    //System.out.println("leech2: " + new String(b));
    this.messageConsumer.message(new String(b, offset, length));
  }

  public void write(int b) {
    // this never seems to get called
    System.out.println("leech3: '" + ((char)b) + "'");
  }
}
