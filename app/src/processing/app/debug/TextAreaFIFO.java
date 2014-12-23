/*
  Copyright (c) 2014 Paul Stoffregen <paul@pjrc.com>

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
  
  $Id$
*/

// adapted from https://community.oracle.com/thread/1479784

package processing.app.debug;

import javax.swing.JTextArea;
import javax.swing.SwingUtilities;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.BadLocationException;

public class TextAreaFIFO extends JTextArea implements DocumentListener {
  private int maxChars;
  private int trimMaxChars;

  private int updateCount; // limit how often we trim the document

  private boolean doTrim;

  public TextAreaFIFO(int max) {
    maxChars = max;
    trimMaxChars = max / 2;
    updateCount = 0;
    doTrim = true;
    getDocument().addDocumentListener(this);
  }

  public void insertUpdate(DocumentEvent e) {
    if (++updateCount > 150 && doTrim) {
      updateCount = 0;
      SwingUtilities.invokeLater(new Runnable() {
        public void run() {
          trimDocument();
        }
      });
    }
  }

  public void removeUpdate(DocumentEvent e) {
  }

  public void changedUpdate(DocumentEvent e) {
  }

  public void trimDocument() {
    int len = 0;
    len = getDocument().getLength();
    if (len > trimMaxChars) {
      int n = len - trimMaxChars;
      //System.out.println("trimDocument: remove " + n + " chars");
      try {
        getDocument().remove(0, n);
      } catch (BadLocationException ble) {
      }
    }
  }

  public void appendNoTrim(String s) {
    int free = maxChars - getDocument().getLength();
    if (free <= 0)
      return;
    if (s.length() > free)
      append(s.substring(0, free));
    else
      append(s);
    doTrim = false;
  }

  public void appendTrim(String str) {
    append(str);
    doTrim = true;
  }
}
