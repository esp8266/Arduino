package processing.app.helpers;

import org.fest.swing.core.Robot;
import org.fest.swing.fixture.ComponentFixture;
import org.fife.ui.rsyntaxtextarea.RSyntaxTextArea;

public class RSyntaxTextAreaFixture extends ComponentFixture {

  private final RSyntaxTextAreaComponentDriver driver;

  public RSyntaxTextAreaFixture(Robot robot, Class type) {
    super(robot, type);
    this.driver = new RSyntaxTextAreaComponentDriver(robot);
  }

  public RSyntaxTextAreaFixture(Robot robot, String name, Class type) {
    super(robot, name, type);
    this.driver = new RSyntaxTextAreaComponentDriver(robot);
  }

  public RSyntaxTextAreaFixture(Robot robot, RSyntaxTextArea target) {
    super(robot, target);
    this.driver = new RSyntaxTextAreaComponentDriver(robot);
  }

  public RSyntaxTextAreaFixture enterText(String text) {
    driver.enterText((RSyntaxTextArea) target, text);
    return this;
  }

  public RSyntaxTextAreaFixture setText(String text) {
    driver.setText((RSyntaxTextArea) target, text);
    return this;
  }

  public String getText() {
    return driver.getText((RSyntaxTextArea) target);
  }

  public RSyntaxTextAreaFixture selectAll() {
    driver.selectAll((RSyntaxTextArea) target);
    return this;
  }

  public int getCaretPosition() {
    return driver.getCaretPosition((RSyntaxTextArea) target);
  }

  public void setCaretPosition(int caretPosition) {
    driver.setCaretPosition((RSyntaxTextArea) target, caretPosition);
  }
}
