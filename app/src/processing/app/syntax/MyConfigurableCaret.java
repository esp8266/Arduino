package processing.app.syntax;

import org.fife.ui.rtextarea.ConfigurableCaret;
import processing.app.helpers.OSUtils;

import javax.swing.*;
import java.awt.event.MouseEvent;

public class MyConfigurableCaret extends ConfigurableCaret {

  @Override
  public void mouseClicked(MouseEvent e) {
    if (e.isConsumed()) {
      return;
    }

    if (!OSUtils.isLinux() && SwingUtilities.isMiddleMouseButton(e)) {
      return;
    }

    super.mouseClicked(e);
  }
}
