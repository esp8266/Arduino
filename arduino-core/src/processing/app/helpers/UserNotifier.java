package processing.app.helpers;

public abstract class UserNotifier {

  public void showError(String title, String message, int exit_code) {
    showError(title, message, null, exit_code);
  }

  public void showError(String title, String message, Throwable e) {
    showError(title, message, e, 1);
  }

  public abstract void showError(String title, String message, Throwable e, int exit_code);

  public abstract void showMessage(String title, String message);

  public abstract void showWarning(String title, String message, Exception e);

}
