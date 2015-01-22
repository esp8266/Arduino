package processing.app.helpers;

import org.fest.swing.core.Robot;
import org.fest.swing.fixture.ComponentFixture;
import processing.app.syntax.JEditTextArea;

public class JEditTextAreaFixture extends ComponentFixture {

  private final JEditTextAreaComponentDriver driver;

  public JEditTextAreaFixture(Robot robot, Class type) {
    super(robot, type);
    this.driver = new JEditTextAreaComponentDriver(robot);
  }

  public JEditTextAreaFixture(Robot robot, String name, Class type) {
    super(robot, name, type);
    this.driver = new JEditTextAreaComponentDriver(robot);
  }

  public JEditTextAreaFixture(Robot robot, JEditTextArea target) {
    super(robot, target);
    this.driver = new JEditTextAreaComponentDriver(robot);
  }

  public JEditTextAreaFixture enterText(String text) {
    driver.enterText((JEditTextArea) target, text);
    return this;
  }

  public JEditTextAreaFixture setText(String text) {
    driver.setText((JEditTextArea) target, text);
    return this;
  }

  public String getText() {
    return driver.getText((JEditTextArea) target);
  }

  public JEditTextAreaFixture selectAll() {
    driver.selectAll((JEditTextArea) target);
    return this;
  }

  public int getCaretPosition() {
    return driver.getCaretPosition((JEditTextArea) target);
  }

  public void setCaretPosition(int caretPosition) {
    driver.setCaretPosition((JEditTextArea) target, caretPosition);
  }
}
