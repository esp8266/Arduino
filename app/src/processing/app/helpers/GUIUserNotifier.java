package processing.app.helpers;

import static processing.app.I18n._;

import java.awt.Frame;

import javax.swing.JOptionPane;

public class GUIUserNotifier extends UserNotifier {

  /**
   * Show an error message that's actually fatal to the program.
   * This is an error that can't be recovered. Use showWarning()
   * for errors that allow P5 to continue running.
   */
  public void showError(String title, String message, Throwable e, int exit_code) {
    if (title == null) title = _("Error");

    JOptionPane.showMessageDialog(new Frame(), message, title,
                                  JOptionPane.ERROR_MESSAGE);

    if (e != null) e.printStackTrace();
    System.exit(exit_code);
  }

  /**
   * "No cookie for you" type messages. Nothing fatal or all that
   * much of a bummer, but something to notify the user about.
   */
  public void showMessage(String title, String message) {
    if (title == null) title = _("Message");

    JOptionPane.showMessageDialog(new Frame(), message, title,
                                  JOptionPane.INFORMATION_MESSAGE);
  }

  /**
   * Non-fatal error message with optional stack trace side dish.
   */
  public void showWarning(String title, String message, Exception e) {
    if (title == null) title = _("Warning");

    JOptionPane.showMessageDialog(new Frame(), message, title,
                                  JOptionPane.WARNING_MESSAGE);

    if (e != null) e.printStackTrace();
  }

}
