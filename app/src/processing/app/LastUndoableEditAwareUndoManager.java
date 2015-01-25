package processing.app;

import javax.swing.undo.CannotRedoException;
import javax.swing.undo.CannotUndoException;
import javax.swing.undo.UndoManager;
import javax.swing.undo.UndoableEdit;

@SuppressWarnings("serial")
public class LastUndoableEditAwareUndoManager extends UndoManager {

  private UndoableEdit lastUndoableEdit;

  public LastUndoableEditAwareUndoManager() {
    this.lastUndoableEdit = null;
  }

  @Override
  public synchronized void undo() throws CannotUndoException {
    lastUndoableEdit = super.editToBeUndone();
    super.undo();
  }

  @Override
  public synchronized void redo() throws CannotRedoException {
    lastUndoableEdit = super.editToBeRedone();
    super.redo();
  }

  public UndoableEdit getLastUndoableEdit() {
    return lastUndoableEdit;
  }
}
