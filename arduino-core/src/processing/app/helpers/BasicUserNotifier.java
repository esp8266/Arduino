package processing.app.helpers;

import static processing.app.I18n._;

public class BasicUserNotifier extends UserNotifier {

  /**
   * Show an error message that's actually fatal to the program.
   * This is an error that can't be recovered. Use showWarning()
   * for errors that allow P5 to continue running.
   */
  public void showError(String title, String message, Throwable e, int exit_code) {
    if (title == null) title = _("Error");

    System.err.println(title + ": " + message);

    if (e != null) e.printStackTrace();
    System.exit(exit_code);
  }

  public void showMessage(String title, String message) {
    if (title == null) title = _("Message");

    System.out.println(title + ": " + message);
  }

  /**
   * Non-fatal error message with optional stack trace side dish.
   */
  public void showWarning(String title, String message, Exception e) {
    if (title == null) title = _("Warning");

    System.out.println(title + ": " + message);

    if (e != null) e.printStackTrace();
  }

}
