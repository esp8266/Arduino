package processing.app.helpers;

import org.fest.swing.core.Robot;
import org.fest.swing.driver.JComponentDriver;
import org.fest.swing.edt.GuiActionRunner;
import org.fest.swing.edt.GuiQuery;
import processing.app.syntax.JEditTextArea;

public class JEditTextAreaComponentDriver extends JComponentDriver {

  public JEditTextAreaComponentDriver(Robot robot) {
    super(robot);
  }

  public void enterText(JEditTextArea target, String text) {
    focusAndWaitForFocusGain(target);
    robot.enterText(text);
  }

  public void setText(final JEditTextArea target, final String text) {
    focusAndWaitForFocusGain(target);
    GuiActionRunner.execute(new GuiQuery<JEditTextArea>() {

      protected JEditTextArea executeInEDT() {
        target.setText(text);
        return target;
      }

    });
    robot.waitForIdle();
  }

  public String getText(final JEditTextArea target) {
    focusAndWaitForFocusGain(target);
    return GuiActionRunner.execute(new GuiQuery<String>() {

      protected String executeInEDT() {
        return target.getText();
      }

    });
  }

  public JEditTextArea selectAll(final JEditTextArea target) {
    return GuiActionRunner.execute(new GuiQuery<JEditTextArea>() {

      protected JEditTextArea executeInEDT() {
        target.selectAll();
        return target;
      }

    });
  }

  public Integer getCaretPosition(final JEditTextArea target) {
    focusAndWaitForFocusGain(target);
    return GuiActionRunner.execute(new GuiQuery<Integer>() {

      protected Integer executeInEDT() {
        return target.getCaretPosition();
      }

    });
  }

  public void setCaretPosition(final JEditTextArea target, final int caretPosition) {
    focusAndWaitForFocusGain(target);
    GuiActionRunner.execute(new GuiQuery<JEditTextArea>() {

      protected JEditTextArea executeInEDT() {
        target.setCaretPosition(caretPosition);
        return target;
      }

    });
    robot.waitForIdle();
  }

}
