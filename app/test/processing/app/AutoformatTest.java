package processing.app;

import org.fest.swing.fixture.JMenuItemFixture;
import org.junit.Test;
import processing.app.helpers.JEditTextAreaFixture;

import static org.junit.Assert.assertEquals;

public class AutoformatTest extends AbstractGUITest {

  @Test
  public void shouldProduceNicelyFormattedCode() throws Exception {
    JMenuItemFixture menuToolsAutoFormat = window.menuItem("menuToolsAutoFormat");
    menuToolsAutoFormat.requireEnabled();

    JEditTextAreaFixture editor = window.jEditTextArea("editor");
    editor.setText("void setup() {\n" +
            "// put your setup code here, to run once:\n" +
            "int foo[] = { 1, 2, 3, 4, 5};\n" +
            "int foo[2][5] = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}};\n" +
            "}\n" +
            "\n" +
            "void loop() {\n" +
            "// put your main code here, to run repeatedly:\n" +
            "}");

    menuToolsAutoFormat.click();

    String formattedText = editor.getText();
    assertEquals("void setup() {\n" +
            "  // put your setup code here, to run once:\n" +
            "  int foo[] = { 1, 2, 3, 4, 5};\n" +
            "  int foo[2][5] = {{1, 2, 3, 4, 5}, {6, 7, 8, 9, 10}};\n" +
            "}\n" +
            "\n" +
            "void loop() {\n" +
            "  // put your main code here, to run repeatedly:\n" +
            "}", formattedText);

  }
}
