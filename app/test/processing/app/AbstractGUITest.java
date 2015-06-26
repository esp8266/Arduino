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

package processing.app;

import cc.arduino.files.DeleteFilesOnShutdown;
import org.fest.swing.edt.FailOnThreadViolationRepaintManager;
import org.fest.swing.edt.GuiActionRunner;
import org.fest.swing.edt.GuiQuery;
import org.junit.After;
import org.junit.Before;
import processing.app.helpers.ArduinoFrameFixture;

import javax.swing.*;

public abstract class AbstractGUITest {

  protected ArduinoFrameFixture window;

  @Before
  public void startUpTheIDE() throws Exception {
    System.setProperty("mrj.version", "whynot"); //makes sense only on osx. See https://github.com/alexruiz/fest-swing-1.x/issues/2#issuecomment-86532042
    Runtime.getRuntime().addShutdownHook(new Thread(DeleteFilesOnShutdown.INSTANCE));

    FailOnThreadViolationRepaintManager.install();

    BaseNoGui.initPlatform();
    BaseNoGui.getPlatform().init();
    PreferencesData.init(null);
    JPopupMenu.setDefaultLightWeightPopupEnabled(false);
    Theme.init();
    BaseNoGui.getPlatform().setLookAndFeel();
    Base.untitledFolder = BaseNoGui.createTempFolder("untitled");
    DeleteFilesOnShutdown.add(Base.untitledFolder);

    window = GuiActionRunner.execute(new GuiQuery<ArduinoFrameFixture>() {
      @Override
      protected ArduinoFrameFixture executeInEDT() throws Throwable {
        return new ArduinoFrameFixture(new Base(new String[0]).editors.get(0));
      }
    });
  }

  @After
  public void stopTheIDE() {
    window.cleanUp();
  }

}
