package processing.app;

import static org.junit.Assert.assertEquals;

import org.fest.swing.fixture.JMenuItemFixture;
import org.junit.Test;

import processing.app.helpers.JEditTextAreaFixture;

public class ReduceIndentWith1CharOnLastLineTest extends AbstractGUITest {

  @Test
  public void shouldJustCloseTheDialog() throws Exception {
    JMenuItemFixture menuDecreaseIndent = window.menuItem("menuDecreaseIndent");

    JEditTextAreaFixture editor = window.jEditTextArea("editor");
    editor.setText("void loop()\n{\n  Serial.begin(9600)\n}");

    editor.selectAll();

    menuDecreaseIndent.requireEnabled();
    menuDecreaseIndent.click();

    EditorConsole console = (EditorConsole) window.scrollPane("console").component();

    assertEquals("", console.consoleDoc.getText(0, console.consoleDoc.getLength()));
  }
}
