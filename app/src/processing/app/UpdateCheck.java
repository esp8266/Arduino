/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2005-06 Ben Fry and Casey Reas

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

package processing.app;

import java.io.BufferedReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.net.URL;
import java.net.URLEncoder;
import java.util.Random;

import javax.swing.JOptionPane;

import processing.core.PApplet;
import static processing.app.I18n._;


/**
 * Threaded class to check for updates in the background.
 * <P>
 * This is the class that handles the mind control and stuff for
 * spying on our users and stealing their personal information.
 * A random ID number is generated for each user, and hits the server
 * to check for updates. Also included is the operating system and
 * its version and the version of Java being used to run Processing.
 * <P>
 * The ID number also helps provide us a general idea of how many
 * people are using Processing, which helps us when writing grant
 * proposals and that kind of thing so that we can keep Processing free.
 */
public class UpdateCheck implements Runnable {
  Base base;
  String downloadURL = _("http://www.arduino.cc/latest.txt");

  static final long ONE_DAY = 24 * 60 * 60 * 1000;


  public UpdateCheck(Base base) {
    Thread thread = new Thread(this);
    this.base = base;
    thread.start();
  }


  public void run() {
    //System.out.println("checking for updates...");

    // generate a random id in case none exists yet
    Random r = new Random();
    long id = r.nextLong();

    String idString = Preferences.get("update.id");
    if (idString != null) {
      id = Long.parseLong(idString);
    } else {
      Preferences.set("update.id", String.valueOf(id));
    }

    try {
      String info;
      info = URLEncoder.encode(id + "\t" +
                        PApplet.nf(Base.REVISION, 4) + "\t" +
                        System.getProperty("java.version") + "\t" +
                        System.getProperty("java.vendor") + "\t" +
                        System.getProperty("os.name") + "\t" +
                        System.getProperty("os.version") + "\t" +
                        System.getProperty("os.arch"), "UTF-8");
      
      int latest = readInt(downloadURL + "?" + info);

      String lastString = Preferences.get("update.last");
      long now = System.currentTimeMillis();
      if (lastString != null) {
        long when = Long.parseLong(lastString);
        if (now - when < ONE_DAY) {
          // don't annoy the shit outta people
          return;
        }
      }
      Preferences.set("update.last", String.valueOf(now));

      String prompt =
        _("A new version of Arduino is available,\n" +
          "would you like to visit the Arduino download page?");
        
      if (base.activeEditor != null) {
        if (latest > Base.REVISION) {
          Object[] options = { _("Yes"), _("No") };
          int result = JOptionPane.showOptionDialog(base.activeEditor,
                                                    prompt,
                                                    _("Update"),
                                                    JOptionPane.YES_NO_OPTION,
                                                    JOptionPane.QUESTION_MESSAGE,
                                                    null,
                                                    options,
                                                    options[0]);
          if (result == JOptionPane.YES_OPTION) {
            Base.openURL(_("http://www.arduino.cc/en/Main/Software"));
          }
        }
      }
    } catch (Exception e) {
      //e.printStackTrace();
      //System.err.println("Error while trying to check for an update.");
    }
  }


  protected int readInt(String filename) throws Exception {
    URL url = new URL(filename);
    InputStream stream = url.openStream();
    InputStreamReader isr = new InputStreamReader(stream);
    BufferedReader reader = new BufferedReader(isr);
    return Integer.parseInt(reader.readLine());
  }
}
