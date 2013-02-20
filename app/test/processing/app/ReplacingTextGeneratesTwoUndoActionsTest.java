package processing.app;

import org.fest.swing.core.ComponentMatcher;
import org.fest.swing.fixture.FrameFixture;
import org.fest.swing.fixture.JMenuItemFixture;
import org.junit.Before;
import org.junit.Test;
import processing.app.syntax.JEditTextArea;

import javax.swing.*;
import java.awt.*;

import static org.junit.Assert.assertEquals;

public class ReplacingTextGeneratesTwoUndoActionsTest {

  private FrameFixture window;
  private Base base;

  @Before
  public void setUp() throws Exception {
    Base.initPlatform();
    Preferences.init(null);
    JPopupMenu.setDefaultLightWeightPopupEnabled(false);
    Theme.init();
    Base.platform.setLookAndFeel();
    Base.untitledFolder = Base.createTempFolder("untitled");
    Base.untitledFolder.deleteOnExit();

    base = new Base(new String[0]);
    window = new FrameFixture(base.editors.get(0));
  }

  @Test
  public void testName() throws Exception {
    JMenuItemFixture menuEditUndo = window.menuItem("menuEditUndo");
    menuEditUndo.requireDisabled();
    JMenuItemFixture menuEditRedo = window.menuItem("menuEditRedo");
    menuEditRedo.requireDisabled();

    JEditTextArea jEditTextArea = (JEditTextArea) window.robot.finder().find(new ComponentMatcher() {
      @Override
      public boolean matches(Component component) {
        return component instanceof JEditTextArea && "editor".equals(component.getName());
      }
    });

    jEditTextArea.setText("fake text");

    menuEditUndo.requireEnabled();
    menuEditUndo.click();

    assertEquals("", jEditTextArea.getText());

    menuEditRedo.requireEnabled();
    menuEditRedo.click();

    assertEquals("fake text", jEditTextArea.getText());

    menuEditUndo.requireEnabled();
    menuEditUndo.click();
    menuEditUndo.click();
    menuEditUndo.requireDisabled();
    menuEditRedo.requireEnabled();
  }
}
