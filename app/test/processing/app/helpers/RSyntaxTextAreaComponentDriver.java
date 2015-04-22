package processing.app.helpers;

import org.fest.swing.core.Robot;
import org.fest.swing.driver.JComponentDriver;
import org.fest.swing.edt.GuiActionRunner;
import org.fest.swing.edt.GuiQuery;
import org.fife.ui.rsyntaxtextarea.RSyntaxTextArea;

public class RSyntaxTextAreaComponentDriver extends JComponentDriver {

  public RSyntaxTextAreaComponentDriver(Robot robot) {
    super(robot);
  }

  public void enterText(RSyntaxTextArea target, String text) {
    focusAndWaitForFocusGain(target);
    robot.enterText(text);
  }

  public void setText(final RSyntaxTextArea target, final String text) {
    focusAndWaitForFocusGain(target);
    GuiActionRunner.execute(new GuiQuery<RSyntaxTextArea>() {

      protected RSyntaxTextArea executeInEDT() {
        target.setText(text);
        return target;
      }

    });
    robot.waitForIdle();
  }

  public String getText(final RSyntaxTextArea target) {
    focusAndWaitForFocusGain(target);
    return GuiActionRunner.execute(new GuiQuery<String>() {

      protected String executeInEDT() {
        return target.getText();
      }

    });
  }

  public RSyntaxTextArea selectAll(final RSyntaxTextArea target) {
    return GuiActionRunner.execute(new GuiQuery<RSyntaxTextArea>() {

      protected RSyntaxTextArea executeInEDT() {
        target.selectAll();
        return target;
      }

    });
  }

  public Integer getCaretPosition(final RSyntaxTextArea target) {
    focusAndWaitForFocusGain(target);
    return GuiActionRunner.execute(new GuiQuery<Integer>() {

      protected Integer executeInEDT() {
        return target.getCaretPosition();
      }

    });
  }

  public void setCaretPosition(final RSyntaxTextArea target, final int caretPosition) {
    focusAndWaitForFocusGain(target);
    GuiActionRunner.execute(new GuiQuery<RSyntaxTextArea>() {

      protected RSyntaxTextArea executeInEDT() {
        target.setCaretPosition(caretPosition);
        return target;
      }

    });
    robot.waitForIdle();
  }

}
