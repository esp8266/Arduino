package processing.app.syntax;

import org.fife.ui.rsyntaxtextarea.RSyntaxTextAreaDefaultInputMap;
import org.fife.ui.rtextarea.RTextArea;
import org.fife.ui.rtextarea.RTextAreaEditorKit;
import processing.app.PreferencesData;

import javax.swing.*;
import javax.swing.text.DefaultEditorKit;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;

public class SketchTextAreaDefaultInputMap extends RSyntaxTextAreaDefaultInputMap {

  public SketchTextAreaDefaultInputMap() {
    int defaultModifier = getDefaultModifier();
    int alt = InputEvent.ALT_MASK;
    boolean isOSX = RTextArea.isOSX();
    int moveByWordMod = isOSX ? alt : defaultModifier;

    remove(KeyStroke.getKeyStroke(KeyEvent.VK_K, defaultModifier));

    if (PreferencesData.getBoolean("editor.advanced")) {
      put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, alt), RTextAreaEditorKit.rtaLineDownAction);
      put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, alt), RTextAreaEditorKit.rtaLineUpAction);
    } else {
      remove(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, alt));
      remove(KeyStroke.getKeyStroke(KeyEvent.VK_UP, alt));
    }

    remove(KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, defaultModifier));
    put(KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, moveByWordMod), RTextAreaEditorKit.rtaDeletePrevWordAction);

    if (isOSX) {
      put(KeyStroke.getKeyStroke(KeyEvent.VK_BACK_SPACE, defaultModifier), SketchTextAreaEditorKit.rtaDeleteLineToCursorAction);
      put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, defaultModifier), DefaultEditorKit.beginAction);
      put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, defaultModifier), DefaultEditorKit.endAction);
    }
  }
}
