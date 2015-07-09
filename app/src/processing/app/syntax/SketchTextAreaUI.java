package processing.app.syntax;

import org.fife.ui.rsyntaxtextarea.RSyntaxTextAreaUI;

import javax.swing.*;
import javax.swing.text.EditorKit;
import javax.swing.text.JTextComponent;

public class SketchTextAreaUI extends RSyntaxTextAreaUI {

  private static final EditorKit defaultKit = new SketchTextAreaEditorKit();

  public SketchTextAreaUI(JComponent rSyntaxTextArea) {
    super(rSyntaxTextArea);
  }

  @Override
  public EditorKit getEditorKit(JTextComponent tc) {
    return defaultKit;
  }
}
