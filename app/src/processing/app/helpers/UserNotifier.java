package processing.app.helpers;

public interface UserNotifier {

  public void showError(String title, String message, Throwable e);

  public void showError(String title, String message, Throwable e, int exit_code);

  public void showMessage(String title, String message);

}
