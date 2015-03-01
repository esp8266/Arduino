package processing.app.helpers;

import org.fest.swing.fixture.FrameFixture;
import processing.app.Editor;
import processing.app.syntax.JEditTextArea;

public class ArduinoFrameFixture extends FrameFixture {

  private final Editor editor;

  public ArduinoFrameFixture(Editor editor) {
    super(editor);
    this.editor = editor;
  }

  public JEditTextAreaFixture jEditTextArea(String name) {
    return new JEditTextAreaFixture(robot, (JEditTextArea) this.robot.finder().find(new JEditTextAreaComponentMatcher(name)));
  }

  public Editor getEditor() {
    return editor;
  }
}
