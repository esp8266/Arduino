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

    Base.initPlatform();
    Preferences.init(null);
    JPopupMenu.setDefaultLightWeightPopupEnabled(false);
    Theme.init();
    Base.getPlatform().setLookAndFeel();
    Base.untitledFolder = Base.createTempFolder("untitled");
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
