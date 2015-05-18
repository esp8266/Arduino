package processing.app;

import java.io.File;

import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;
import javax.swing.text.Document;
import javax.swing.undo.UndoManager;

public class SketchCodeDocument implements DocumentListener{

  private SketchCode code;
  private Sketch sketch;
  private Document document;

  // Undo Manager for this tab, each tab keeps track of their own Editor.undo
  // will be set to this object when this code is the tab that's currently the
  // front.
  private UndoManager undo;

  // saved positions from last time this tab was used
  private int selectionStart;
  private int selectionStop;
  private int scrollPosition;

  public SketchCodeDocument(Sketch sketch, SketchCode code) {
    this.code = code;
    this.sketch = sketch;
    this.code.setMetadata(this);
  }

  public SketchCodeDocument(Sketch sketch, File file) {
    this.code = new SketchCode(file, this);
    this.sketch = sketch;
  }

  public UndoManager getUndo() {
    return undo;
  }

  public void setUndo(UndoManager undo) {
    this.undo = undo;
  }

  public int getSelectionStart() {
    return selectionStart;
  }

  public void setSelectionStart(int selectionStart) {
    this.selectionStart = selectionStart;
  }

  public int getSelectionStop() {
    return selectionStop;
  }

  public void setSelectionStop(int selectionStop) {
    this.selectionStop = selectionStop;
  }

  public int getScrollPosition() {
    return scrollPosition;
  }

  public void setScrollPosition(int scrollPosition) {
    this.scrollPosition = scrollPosition;
  }

  public SketchCode getCode() {
    return code;
  }

  public void setCode(SketchCode code) {
    this.code = code;
  }

  public Document getDocument() {
    return document;
  }

  public void setDocument(Document document) {
    this.document = document;
    document.addDocumentListener(this);
  }

  @Override
  public void insertUpdate(DocumentEvent e) {
    if(!code.isModified()) sketch.setModified(true);
  }


  @Override
  public void removeUpdate(DocumentEvent e) {
    if(!code.isModified()) sketch.setModified(true);
  }

  @Override
  public void changedUpdate(DocumentEvent e) {
     // Callback for when styles in the current document change.
     // This method is never called.
  }
  
}
