/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Part of the Processing project - http://processing.org

  Copyright (c) 2008 Ben Fry and Casey Reas

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

package processing.app.tools;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.io.IOException;

import javax.swing.JOptionPane;

import processing.app.*;
import static processing.app.I18n._;


public class FixEncoding implements Tool {
  Editor editor;


  public String getMenuTitle() {
    return _("Fix Encoding & Reload");
  }
  
  
  public void init(Editor editor) {
    this.editor = editor;
  }
  

  public void run() {
    Sketch sketch = editor.getSketch();
    //SketchCode code = sketch.current;
    
    if (sketch.isModified()) {
      int result = 
        JOptionPane.showConfirmDialog(editor, 
                                      _("Discard all changes and reload sketch?"), 
                                      _("Fix Encoding & Reload"),
                                      JOptionPane.YES_NO_OPTION,
                                      JOptionPane.QUESTION_MESSAGE);

      if (result == JOptionPane.NO_OPTION) {
        return;
      }
    }
    try {
      for (int i = 0; i < sketch.getCodeCount(); i++) {
        SketchCode code = sketch.getCode(i);
        code.setProgram(loadWithLocalEncoding(code.getFile()));
        code.setModified(true);  // yes, because we want them to save this
      }
      // Update the currently visible program with its code
      editor.setText(sketch.getCurrentCode().getProgram());
      
    } catch (IOException e) {
      String msg = 
        _("An error occurred while trying to fix the file encoding.\nDo not attempt to save this sketch as it may overwrite\nthe old version. Use Open to re-open the sketch and try again.\n") +
        e.getMessage();
      Base.showWarning(_("Fix Encoding & Reload"), msg, e); 
    }
  }


  protected String loadWithLocalEncoding(File file) throws IOException {
    // FileReader uses the default encoding, which is what we want.
    FileReader fr = new FileReader(file);
    BufferedReader reader = new BufferedReader(fr);

    StringBuffer buffer = new StringBuffer();
    String line = null;
    while ((line = reader.readLine()) != null) {
      buffer.append(line);
      buffer.append('\n');
    }
    reader.close();
    return buffer.toString();
  }
}
