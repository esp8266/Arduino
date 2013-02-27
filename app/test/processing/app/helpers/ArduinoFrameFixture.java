package processing.app.helpers;

import org.fest.swing.core.Robot;
import org.fest.swing.fixture.FrameFixture;
import processing.app.syntax.JEditTextArea;

import java.awt.*;

public class ArduinoFrameFixture extends FrameFixture {

  public ArduinoFrameFixture(Frame target) {
    super(target);
  }

  public ArduinoFrameFixture(org.fest.swing.core.Robot robot, Frame target) {
    super(robot, target);
  }

  public ArduinoFrameFixture(Robot robot, String name) {
    super(robot, name);
  }

  public ArduinoFrameFixture(String name) {
    super(name);
  }

  public JEditTextAreaFixture jEditTextArea(String name) {
    return new JEditTextAreaFixture(robot, (JEditTextArea) this.robot.finder().find(new JEditTextAreaComponentMatcher(name)));
  }
}
