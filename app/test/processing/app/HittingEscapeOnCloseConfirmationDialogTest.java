package processing.app;

import org.fest.swing.core.KeyPressInfo;
import org.fest.swing.finder.WindowFinder;
import org.fest.swing.fixture.DialogFixture;
import org.fest.swing.fixture.JScrollPaneFixture;
import org.junit.Test;
import processing.app.helpers.JEditTextAreaFixture;

import javax.swing.*;
import java.awt.event.KeyEvent;
import java.io.ByteArrayOutputStream;
import java.io.PrintStream;

import static org.junit.Assert.assertEquals;

public class HittingEscapeOnCloseConfirmationDialogTest extends AbstractGUITest {

  @Test
  public void shouldJustCloseTheDialog() throws Exception {
    JEditTextAreaFixture editor = window.jEditTextArea("editor");
    editor.setText("test");

    window.close();

    DialogFixture dialog = WindowFinder.findDialog(JDialog.class).using(window.robot);
    dialog.pressAndReleaseKey(KeyPressInfo.keyCode(KeyEvent.VK_ESCAPE));

    EditorConsole console = (EditorConsole) window.scrollPane("console").component();

    assertEquals("", console.consoleDoc.getText(0, console.consoleDoc.getLength()));
  }
}
