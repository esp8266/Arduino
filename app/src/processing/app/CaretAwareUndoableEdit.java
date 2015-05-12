package processing.app;

import processing.app.syntax.SketchTextArea;

import javax.swing.undo.CannotRedoException;
import javax.swing.undo.CannotUndoException;
import javax.swing.undo.UndoableEdit;

public class CaretAwareUndoableEdit implements UndoableEdit {

  private final UndoableEdit undoableEdit;
  private final int caretPosition;

  public CaretAwareUndoableEdit(UndoableEdit undoableEdit, SketchTextArea textArea) {
    this.undoableEdit = undoableEdit;
    this.caretPosition = textArea.getCaretPosition();
  }

  @Override
  public void undo() throws CannotUndoException {
    undoableEdit.undo();
  }

  @Override
  public boolean canUndo() {
    return undoableEdit.canUndo();
  }

  @Override
  public void redo() throws CannotRedoException {
    undoableEdit.redo();
  }

  @Override
  public boolean canRedo() {
    return undoableEdit.canRedo();
  }

  @Override
  public void die() {
    undoableEdit.die();
  }

  @Override
  public boolean addEdit(UndoableEdit undoableEdit) {
    return this.undoableEdit.addEdit(undoableEdit);
  }

  @Override
  public boolean replaceEdit(UndoableEdit undoableEdit) {
    return this.undoableEdit.replaceEdit(undoableEdit);
  }

  @Override
  public boolean isSignificant() {
    return undoableEdit.isSignificant();
  }

  @Override
  public String getPresentationName() {
    return undoableEdit.getPresentationName();
  }

  @Override
  public String getUndoPresentationName() {
    return undoableEdit.getUndoPresentationName();
  }

  @Override
  public String getRedoPresentationName() {
    return undoableEdit.getRedoPresentationName();
  }

  public int getCaretPosition() {
    return caretPosition;
  }
}
