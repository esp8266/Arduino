package cc.arduino.packages.contributions.ui;

import javax.swing.*;
import java.awt.*;

import static processing.app.I18n._;

public class ContributionUncaughtExceptionHandler implements Thread.UncaughtExceptionHandler {

  private final Component parent;

  public ContributionUncaughtExceptionHandler(Component parent) {
    this.parent = parent;
  }

  @Override
  public void uncaughtException(Thread t, Throwable e) {
    e.printStackTrace();
    JOptionPane.showMessageDialog(parent, _(e.getMessage()), "Error", JOptionPane.ERROR_MESSAGE);
  }

}
