package processing.app.syntax;

import org.fife.ui.rsyntaxtextarea.RSyntaxTextAreaDefaultInputMap;

import javax.swing.*;
import java.awt.event.KeyEvent;

public class SketchTextAreaDefaultInputMap extends RSyntaxTextAreaDefaultInputMap {

  public SketchTextAreaDefaultInputMap() {
    int defaultMod = getDefaultModifier();

    remove(KeyStroke.getKeyStroke(KeyEvent.VK_K, defaultMod));
  }
}
