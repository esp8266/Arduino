package processing.app;

import org.fest.swing.edt.GuiActionRunner;
import org.fest.swing.edt.GuiQuery;
import org.fest.swing.fixture.JMenuItemFixture;
import org.junit.Test;
import processing.app.helpers.JEditTextAreaFixture;

import java.awt.*;

import static org.junit.Assert.assertEquals;

public class BlockCommentGeneratesOneUndoActionTest extends AbstractGUITest {

  @Test
  public void shouldUndoAndRedo() throws Exception {
    JMenuItemFixture menuEditUndo = window.menuItem("menuEditUndo");
    menuEditUndo.requireDisabled();

    JEditTextAreaFixture jEditTextArea = window.jEditTextArea("editor");
    String previousText = jEditTextArea.getText();

    jEditTextArea.selectAll();

    GuiActionRunner.execute(new GuiQuery<Frame>() {

      protected Frame executeInEDT() {
        window.getEditor().handleCommentUncomment();
        return window.getEditor();
      }

    });

    menuEditUndo.requireEnabled();
    menuEditUndo.click();

    assertEquals(previousText, jEditTextArea.getText());

    menuEditUndo.requireDisabled();
  }
}
