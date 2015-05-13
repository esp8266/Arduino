package processing.app.syntax;

import org.fife.ui.rsyntaxtextarea.RSyntaxTextAreaDefaultInputMap;
import org.fife.ui.rtextarea.RTextArea;
import org.fife.ui.rtextarea.RTextAreaEditorKit;
import processing.app.PreferencesData;

import javax.swing.*;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;

public class SketchTextAreaDefaultInputMap extends RSyntaxTextAreaDefaultInputMap {

  public SketchTextAreaDefaultInputMap() {
    int defaultMod = getDefaultModifier();
    int alt = InputEvent.ALT_MASK;

    remove(KeyStroke.getKeyStroke(KeyEvent.VK_K, defaultMod));

    if (PreferencesData.getBoolean("editor.advanced")) {
      put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, alt), RTextAreaEditorKit.rtaLineDownAction);
      put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, alt), RTextAreaEditorKit.rtaLineUpAction);
    } else {
      remove(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, alt));
      remove(KeyStroke.getKeyStroke(KeyEvent.VK_UP, alt));
    }

    boolean isOSX = RTextArea.isOSX();
    if (isOSX) {
      put(KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, alt), SketchTextAreaEditorKit.rtaDeleteNextWordAction);
    }
  }
}
