package cc.arduino.packages.contributions.ui;

import cc.arduino.ui.InstallerJDialog;

import javax.swing.*;

import static processing.app.I18n._;

public class InstallerJDialogUncaughtExceptionHandler implements Thread.UncaughtExceptionHandler {

  private final InstallerJDialog parent;

  public InstallerJDialogUncaughtExceptionHandler(InstallerJDialog parent) {
    this.parent = parent;
  }

  @Override
  public void uncaughtException(Thread t, final Throwable e) {
    SwingUtilities.invokeLater(new Runnable() {
      @Override
      public void run() {
        e.printStackTrace();
      }
    });
    parent.setErrorMessage(_(e.getMessage().substring(e.getMessage().indexOf(":") + 2)));
  }

}
