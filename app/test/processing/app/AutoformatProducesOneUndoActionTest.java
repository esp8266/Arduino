package processing.app;

import org.fest.swing.fixture.JMenuItemFixture;
import org.junit.Test;
import processing.app.helpers.RSyntaxTextAreaFixture;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotEquals;

public class AutoformatProducesOneUndoActionTest extends AbstractGUITest {

  public static final String SOURCE_BEFORE = "void setup() {\n" +
          "              // put your setup code here, to run once:\n" +
          "\n" +
          "}\n" +
          "\n" +
          "void loop() {\n" +
          "  // put your main code here, to run repeatedly:\n" +
          "\n" +
          "}";
  public static final String SOURCE_AFTER = "void setup() {\n" +
          "  // put your setup code here, to run once:\n" +
          "\n" +
          "}\n" +
          "\n" +
          "void loop() {\n" +
          "  // put your main code here, to run repeatedly:\n" +
          "\n" +
          "}";

  @Test
  public void shouldSaveCaretPositionAfterAutoformat() {
    JMenuItemFixture menuEditUndo = window.menuItem("menuEditUndo");
    menuEditUndo.requireDisabled();

    JMenuItemFixture menuToolsAutoFormat = window.menuItem("menuToolsAutoFormat");
    menuToolsAutoFormat.requireEnabled();

    RSyntaxTextAreaFixture editor = window.RSyntaxTextArea("editor");
    editor.setText(SOURCE_BEFORE);

    editor.setCaretPosition(29); // right before the first // (double slash)

    menuToolsAutoFormat.click();

    String formattedText = editor.getText();
    assertEquals(SOURCE_AFTER, formattedText);

    assertEquals(29, editor.getCaretPosition());

    menuEditUndo.requireEnabled();
    menuEditUndo.click();
    assertEquals(SOURCE_BEFORE, editor.getText());
  }

}
