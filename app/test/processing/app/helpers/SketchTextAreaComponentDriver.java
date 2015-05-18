/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Ricardo JL Rufino (ricardo@criativasoft.com.br)
 * Copyright 2015 Arduino LLC
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

package processing.app.helpers;

import org.fest.swing.core.Robot;
import org.fest.swing.driver.JComponentDriver;
import org.fest.swing.edt.GuiActionRunner;
import org.fest.swing.edt.GuiQuery;
import processing.app.syntax.SketchTextArea;

public class SketchTextAreaComponentDriver extends JComponentDriver {

  public SketchTextAreaComponentDriver(Robot robot) {
    super(robot);
  }

  public void enterText(SketchTextArea target, String text) {
    focusAndWaitForFocusGain(target);
    robot.enterText(text);
  }

  public void setText(final SketchTextArea target, final String text) {
    focusAndWaitForFocusGain(target);
    GuiActionRunner.execute(new GuiQuery<SketchTextArea>() {

      protected SketchTextArea executeInEDT() {
        target.setText(text);
        return target;
      }

    });
    robot.waitForIdle();
  }

  public String getText(final SketchTextArea target) {
    focusAndWaitForFocusGain(target);
    return GuiActionRunner.execute(new GuiQuery<String>() {

      protected String executeInEDT() {
        return target.getText();
      }

    });
  }

  public SketchTextArea selectAll(final SketchTextArea target) {
    return GuiActionRunner.execute(new GuiQuery<SketchTextArea>() {

      protected SketchTextArea executeInEDT() {
        target.selectAll();
        return target;
      }

    });
  }

  public Integer getCaretPosition(final SketchTextArea target) {
    focusAndWaitForFocusGain(target);
    return GuiActionRunner.execute(new GuiQuery<Integer>() {

      protected Integer executeInEDT() {
        return target.getCaretPosition();
      }

    });
  }

  public void setCaretPosition(final SketchTextArea target, final int caretPosition) {
    focusAndWaitForFocusGain(target);
    GuiActionRunner.execute(new GuiQuery<SketchTextArea>() {

      protected SketchTextArea executeInEDT() {
        target.setCaretPosition(caretPosition);
        return target;
      }

    });
    robot.waitForIdle();
  }

}
