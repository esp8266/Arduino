package processing.app.syntax;

import org.fife.ui.rsyntaxtextarea.RSyntaxTextAreaEditorKit;
import org.fife.ui.rtextarea.RTextArea;
import org.fife.ui.rtextarea.RecordableTextAction;

import javax.swing.*;
import javax.swing.text.BadLocationException;
import javax.swing.text.TextAction;
import javax.swing.text.Utilities;
import java.awt.event.ActionEvent;

public class SketchTextAreaEditorKit extends RSyntaxTextAreaEditorKit {

  public static final String rtaDeleteNextWordAction = "RTA.DeleteNextWordAction";

  private static final Action[] defaultActions = {
    new DeleteNextWordAction()
  };

  @Override
  public Action[] getActions() {
    return TextAction.augmentList(super.getActions(), SketchTextAreaEditorKit.defaultActions);
  }

  public static class DeleteNextWordAction extends RecordableTextAction {

    public DeleteNextWordAction() {
      super(rtaDeleteNextWordAction);
    }

    @Override
    public void actionPerformedImpl(ActionEvent e, RTextArea textArea) {
      if (!textArea.isEditable() || !textArea.isEnabled()) {
        UIManager.getLookAndFeel().provideErrorFeedback(textArea);
        return;
      }
      try {
        int start = textArea.getSelectionStart();
        int end = getNextWordStart(textArea, start);
        if (end > start) {
          textArea.getDocument().remove(start, end - start);
        }
      } catch (BadLocationException ex) {
        UIManager.getLookAndFeel().provideErrorFeedback(textArea);
      }
    }

    @Override
    public String getMacroID() {
      return rtaDeleteNextWordAction;
    }

    /**
     * Returns the starting offset to delete.  Exists so subclasses can
     * override.
     */
    protected int getNextWordStart(RTextArea textArea, int end)
      throws BadLocationException {
      return Utilities.getNextWord(textArea, end);
    }

  }

}
