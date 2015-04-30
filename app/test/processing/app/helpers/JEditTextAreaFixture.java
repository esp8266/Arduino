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

package processing.app.helpers;

import org.fest.swing.core.Robot;
import org.fest.swing.fixture.ComponentFixture;
import processing.app.syntax.JEditTextArea;

public class JEditTextAreaFixture extends ComponentFixture {

  private final JEditTextAreaComponentDriver driver;

  public JEditTextAreaFixture(Robot robot, Class type) {
    super(robot, type);
    this.driver = new JEditTextAreaComponentDriver(robot);
  }

  public JEditTextAreaFixture(Robot robot, String name, Class type) {
    super(robot, name, type);
    this.driver = new JEditTextAreaComponentDriver(robot);
  }

  public JEditTextAreaFixture(Robot robot, JEditTextArea target) {
    super(robot, target);
    this.driver = new JEditTextAreaComponentDriver(robot);
  }

  public JEditTextAreaFixture enterText(String text) {
    driver.enterText((JEditTextArea) target, text);
    return this;
  }

  public JEditTextAreaFixture setText(String text) {
    driver.setText((JEditTextArea) target, text);
    return this;
  }

  public String getText() {
    return driver.getText((JEditTextArea) target);
  }

  public JEditTextAreaFixture selectAll() {
    driver.selectAll((JEditTextArea) target);
    return this;
  }

  public int getCaretPosition() {
    return driver.getCaretPosition((JEditTextArea) target);
  }

  public void setCaretPosition(int caretPosition) {
    driver.setCaretPosition((JEditTextArea) target, caretPosition);
  }
}
