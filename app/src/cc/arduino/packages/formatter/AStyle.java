/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

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
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 */

package cc.arduino.packages.formatter;

import processing.app.Base;
import processing.app.BaseNoGui;
import processing.app.Editor;
import processing.app.helpers.FileUtils;
import processing.app.syntax.SketchTextArea;
import processing.app.tools.Tool;

import javax.swing.text.BadLocationException;
import java.io.File;
import java.io.IOException;

import static processing.app.I18n._;

public class AStyle implements Tool {

  private static final String FORMATTER_CONF = "formatter.conf";

  private final AStyleInterface aStyleInterface;
  private final String formatterConfiguration;
  private Editor editor;

  public AStyle() {
    this.aStyleInterface = new AStyleInterface();
    File customFormatterConf = BaseNoGui.getSettingsFile(FORMATTER_CONF);
    File defaultFormatterConf = new File(Base.getContentFile("lib"), FORMATTER_CONF);

    File formatterConf;
    if (customFormatterConf.exists()) {
      formatterConf = customFormatterConf;
    } else {
      formatterConf = defaultFormatterConf;
    }
    String formatterConfiguration = "";

    try {
      formatterConfiguration = FileUtils.readFileToString(formatterConf);
    } catch (IOException e) {
      // ignored
    }
    this.formatterConfiguration = formatterConfiguration;
  }

  @Override
  public void init(Editor editor) {
    this.editor = editor;
  }

  @Override
  public void run() {
    String originalText = editor.getText();
    String formattedText = aStyleInterface.AStyleMain(originalText, formatterConfiguration);

    if (formattedText.equals(originalText)) {
      editor.statusNotice(_("No changes necessary for Auto Format."));
      return;
    }

    SketchTextArea textArea = editor.getTextArea();

    int line = getLineOfOffset(textArea);
    int lineOffset = getLineOffset(textArea, line);

    editor.getTextArea().getUndoManager().beginInternalAtomicEdit();
    editor.setText(formattedText);
    editor.getSketch().setModified(true);
    editor.getTextArea().getUndoManager().endInternalAtomicEdit();

    if (line != -1 && lineOffset != -1) {
      setCaretPosition(textArea, line, lineOffset);
    }

    // mark as finished
    editor.statusNotice(_("Auto Format finished."));
  }

  private void setCaretPosition(SketchTextArea textArea, int line, int lineOffset) {
    try {
      textArea.setCaretPosition(Math.min(textArea.getLineStartOffset(line) + lineOffset, textArea.getLineEndOffset(line) - 1));
    } catch (BadLocationException e) {
      e.printStackTrace();
    }
  }

  private int getLineOffset(SketchTextArea textArea, int line) {
    try {
      return textArea.getCaretPosition() - textArea.getLineStartOffset(line);
    } catch (BadLocationException e) {
      e.printStackTrace();
    }
    return -1;
  }

  private int getLineOfOffset(SketchTextArea textArea) {
    try {
      return textArea.getLineOfOffset(textArea.getCaretPosition());
    } catch (BadLocationException e) {
      e.printStackTrace();
    }
    return -1;
  }

  @Override
  public String getMenuTitle() {
    return _("Auto Format");
  }

}
