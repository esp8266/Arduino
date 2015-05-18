package processing.app;

import javax.swing.undo.CannotRedoException;
import javax.swing.undo.CannotUndoException;

import org.fife.ui.rtextarea.RUndoManager;

import processing.app.syntax.SketchTextArea;

public class LastUndoableEditAwareUndoManager extends RUndoManager {
  
  private Editor editor;

  public LastUndoableEditAwareUndoManager(SketchTextArea textarea, Editor editor) {
    super(textarea);
    this.editor = editor;
  }

  @Override
  public synchronized void undo() throws CannotUndoException {
    super.undo();
  }

  @Override
  public synchronized void redo() throws CannotRedoException {
    super.redo();
  }
  
  @Override
  public void updateActions() {
    super.updateActions();
    editor.undoAction.updateUndoState();
    editor.redoAction.updateRedoState();
  }


}
