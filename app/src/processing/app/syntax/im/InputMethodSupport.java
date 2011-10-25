package processing.app.syntax.im;

import java.awt.Rectangle;
import java.awt.event.InputMethodEvent;
import java.awt.event.InputMethodListener;
import java.awt.font.TextHitInfo;
import java.awt.im.InputMethodRequests;
import java.text.AttributedCharacterIterator;

import processing.app.syntax.JEditTextArea;

/**
 * Support in-line Japanese input for PDE. (Maybe Chinese, Korean and more)
 * This class is implemented by Java Input Method Framework and handles
 * If you would like to know more about Java Input Method Framework,
 * Please see http://java.sun.com/j2se/1.5.0/docs/guide/imf/
 * 
 * This class is implemented to fix Bug #854.
 * http://dev.processing.org/bugs/show_bug.cgi?id=854
 *  
 * @author Takashi Maekawa (takachin@generative.info)
 */
public class InputMethodSupport implements InputMethodRequests,
    InputMethodListener {

  private int committed_count = 0;
  private CompositionTextManager textManager;

  public InputMethodSupport(JEditTextArea textArea) {
    textManager = new CompositionTextManager(textArea);
    textArea.enableInputMethods(true);
    textArea.addInputMethodListener(this);
  }

  public Rectangle getTextLocation(TextHitInfo offset) {
    return textManager.getTextLocation();
  }

  public TextHitInfo getLocationOffset(int x, int y) {
    return null;
  }

  public int getInsertPositionOffset() {
    return textManager.getInsertPositionOffset();
  }

  public AttributedCharacterIterator getCommittedText(int beginIndex,
      int endIndex, AttributedCharacterIterator.Attribute[] attributes) {
    return textManager.getCommittedText(beginIndex, endIndex);
  }

  public int getCommittedTextLength() {
    return committed_count;
  }

  public AttributedCharacterIterator cancelLatestCommittedText(
      AttributedCharacterIterator.Attribute[] attributes) {
    return null;
  }

  public AttributedCharacterIterator getSelectedText(
      AttributedCharacterIterator.Attribute[] attributes) {
    return null;
  }

  /**
   * Handles events from InputMethod.
   * This method judges whether beginning of input or 
   * progress of input or end and call related method.
   * 
   * @param event event from Input Method.
   */
  public void inputMethodTextChanged(InputMethodEvent event) {
    AttributedCharacterIterator text = event.getText();
    committed_count = event.getCommittedCharacterCount();
    if(isFullWidthSpaceInput(text)){
      textManager.insertFullWidthSpace();
      caretPositionChanged(event);
      return;
    }
    if(isBeginInputProcess(text, textManager)){
      textManager.beginCompositionText(text, committed_count);
      caretPositionChanged(event);
      return;
    }
    if (isInputProcess(text)){
      textManager.processCompositionText(text, committed_count);
      caretPositionChanged(event);
      return;
    }
    textManager.endCompositionText(text, committed_count);
    caretPositionChanged(event);
  }
  
  private boolean isFullWidthSpaceInput(AttributedCharacterIterator text){
    if(text == null)
      return false;
    if(textManager.getIsInputProcess())
      return false;
    return (String.valueOf(text.first()).equals("\u3000"));
  }
  
  private boolean isBeginInputProcess(AttributedCharacterIterator text, CompositionTextManager textManager){
    if(text == null)
      return false;
    if(textManager.getIsInputProcess())
      return false;
    return (isInputProcess(text));
  }

  private boolean isInputProcess(AttributedCharacterIterator text){
    if(text == null)
      return false;
    return (text.getEndIndex() - (text.getBeginIndex() + committed_count) > 0);
  }

  public void caretPositionChanged(InputMethodEvent event) {
    event.consume();
  }
}
