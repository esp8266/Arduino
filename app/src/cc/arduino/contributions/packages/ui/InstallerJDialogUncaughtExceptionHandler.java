package cc.arduino.contributions.packages.ui;

import cc.arduino.contributions.ui.InstallerJDialog;

import javax.swing.*;

import static processing.app.I18n._;

public class InstallerJDialogUncaughtExceptionHandler implements Thread.UncaughtExceptionHandler {

  private final InstallerJDialog parent;
  private final String connectionErrorMessage;

  public InstallerJDialogUncaughtExceptionHandler(InstallerJDialog parent, String connectionErrorMessage) {
    this.parent = parent;
    this.connectionErrorMessage = connectionErrorMessage;
  }

  @Override
  public void uncaughtException(Thread t, final Throwable e) {
    String errorMessage = _(e.getMessage().substring(e.getMessage().indexOf(":") + 2));
    if (errorMessage.startsWith("Error downloading")) {
      errorMessage = connectionErrorMessage;
    }
    final String finalErrorMessage = errorMessage;
    SwingUtilities.invokeLater(new Runnable() {
      @Override
      public void run() {
        System.err.println(finalErrorMessage);
        e.printStackTrace();
      }
    });
    parent.setErrorMessage(finalErrorMessage);
  }

}
