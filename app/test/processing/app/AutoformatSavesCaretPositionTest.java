package processing.app;

import org.fest.swing.fixture.JMenuItemFixture;
import org.junit.Test;
import processing.app.helpers.JEditTextAreaFixture;

import static org.junit.Assert.assertEquals;

public class AutoformatSavesCaretPositionTest extends AbstractGUITest {

  @Test
  public void shouldSaveCaretPositionAfterAutoformat() {
    JMenuItemFixture menuToolsAutoFormat = window.menuItem("menuToolsAutoFormat");
    menuToolsAutoFormat.requireEnabled();

    JEditTextAreaFixture editor = window.jEditTextArea("editor");
    editor.setText("void setup() {\n" +
            "              // put your setup code here, to run once:\n" +
            "\n" +
            "}\n" +
            "\n" +
            "void loop() {\n" +
            "  // put your main code here, to run repeatedly:\n" +
            "\n" +
            "}");

    editor.setCaretPosition(29); // right before the first // (double slash)

    menuToolsAutoFormat.click();

    String formattedText = editor.getText();
    assertEquals("void setup() {\n" +
            "  // put your setup code here, to run once:\n" +
            "\n" +
            "}\n" +
            "\n" +
            "void loop() {\n" +
            "  // put your main code here, to run repeatedly:\n" +
            "\n" +
            "}", formattedText);

    assertEquals(29, editor.getCaretPosition());

  }

}
