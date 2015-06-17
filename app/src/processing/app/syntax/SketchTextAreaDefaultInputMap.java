package processing.app.syntax;

import org.fife.ui.rsyntaxtextarea.RSyntaxTextAreaDefaultInputMap;
import org.fife.ui.rsyntaxtextarea.RSyntaxTextAreaEditorKit;
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
    int shift = InputEvent.SHIFT_MASK;
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

      put(KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, defaultModifier | shift), DefaultEditorKit.selectionBeginLineAction);
      put(KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, defaultModifier | shift), DefaultEditorKit.selectionEndLineAction);

      remove(KeyStroke.getKeyStroke(KeyEvent.VK_J, defaultModifier));

      put(KeyStroke.getKeyStroke(KeyEvent.VK_OPEN_BRACKET, defaultModifier), DefaultEditorKit.insertTabAction);
      put(KeyStroke.getKeyStroke(KeyEvent.VK_CLOSE_BRACKET, defaultModifier), RSyntaxTextAreaEditorKit.rstaDecreaseIndentAction);

      put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, defaultModifier | shift), DefaultEditorKit.selectionBeginAction);
      put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, defaultModifier | shift), DefaultEditorKit.selectionEndAction);
    }

    put(KeyStroke.getKeyStroke(KeyEvent.VK_DIVIDE, defaultModifier), RSyntaxTextAreaEditorKit.rstaToggleCommentAction);

    put(KeyStroke.getKeyStroke(KeyEvent.VK_KP_LEFT, 0), DefaultEditorKit.backwardAction);
    put(KeyStroke.getKeyStroke(KeyEvent.VK_KP_RIGHT, 0), DefaultEditorKit.forwardAction);
    put(KeyStroke.getKeyStroke(KeyEvent.VK_KP_DOWN, 0), DefaultEditorKit.downAction);
    put(KeyStroke.getKeyStroke(KeyEvent.VK_KP_UP, 0), DefaultEditorKit.upAction);

  }
}
