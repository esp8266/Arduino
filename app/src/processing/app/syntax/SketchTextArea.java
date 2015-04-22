/* -*- mode: java; c-basic-offset: 2; indent-tabs-mode: nil -*- */

package processing.app.syntax;

import java.awt.AWTKeyStroke;
import java.awt.KeyboardFocusManager;
import java.awt.event.KeyEvent;
import java.io.File;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.HashSet;
import java.util.Set;
import java.util.logging.Logger;

import javax.swing.JPopupMenu;
import javax.swing.KeyStroke;
import javax.swing.event.HyperlinkEvent;
import javax.swing.text.BadLocationException;
import javax.swing.text.Document;
import javax.swing.text.Segment;
import javax.swing.undo.UndoManager;

import org.fife.ui.rsyntaxtextarea.*;
import org.fife.ui.rsyntaxtextarea.Token;
import org.fife.ui.rsyntaxtextarea.focusabletip.FocusableTip;
import org.fife.ui.rtextarea.RUndoManager;

import processing.app.Base;
import processing.app.BaseNoGui;
import processing.app.EditorLineStatus;
import processing.app.EditorListener;

/**
 * Arduino Sketch code editor based on RSyntaxTextArea (http://fifesoft.com/rsyntaxtextarea)
 * @author Ricardo JL Rufino (ricardo@criativasoft.com.br) 
 * @date 20/04/2015
 * @since 1.6.4
 */
public class SketchTextArea extends RSyntaxTextArea {
  
  private final static Logger LOG = Logger.getLogger(SketchTextArea.class.getName());
  
  /** The last docTooltip displayed. */
  private FocusableTip docTooltip;
  
  /**
   * The component that tracks the current line number.
   */
  protected EditorLineStatus editorLineStatus;

  private EditorListener editorListener;
  
  public SketchTextArea() {
    super();
    installFeatures();
  }
  
  
  protected void installFeatures(){
    FileInputStream defaultXmlInputStream = null;
    try {
      defaultXmlInputStream = new FileInputStream(new File(BaseNoGui.getContentFile("lib"), "theme/syntax/default.xml"));
      Theme theme = Theme.load(defaultXmlInputStream);
      theme.apply(this);
    } catch (IOException e) {
      e.printStackTrace();
    } finally {
      if (defaultXmlInputStream != null) {
        try {
          defaultXmlInputStream.close();
        } catch (IOException e) {
          e.printStackTrace();
        }
      }
    }
    
    setLinkGenerator(new DocLinkGenerator());
    
    fixControlTab();
    installTokenMaker();  
  }
  
  // Removing the default focus traversal keys
  // This is because the DefaultKeyboardFocusManager handles the keypress and consumes the event
  protected void fixControlTab(){
    KeyStroke ctrlTab = KeyStroke.getKeyStroke("ctrl TAB");
    KeyStroke ctrlShiftTab = KeyStroke.getKeyStroke("ctrl shift TAB");
 
    // Remove ctrl-tab from normal focus traversal
    Set<AWTKeyStroke> forwardKeys = new HashSet<AWTKeyStroke>(this.getFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS));
    forwardKeys.remove(ctrlTab);
    this.setFocusTraversalKeys(KeyboardFocusManager.FORWARD_TRAVERSAL_KEYS, forwardKeys);
 
    // Remove ctrl-shift-tab from normal focus traversal
    Set<AWTKeyStroke> backwardKeys = new HashSet<AWTKeyStroke>(this.getFocusTraversalKeys(KeyboardFocusManager.BACKWARD_TRAVERSAL_KEYS));
    backwardKeys.remove(ctrlShiftTab);
  }


  
  public void setEditorLineStatus(EditorLineStatus editorLineStatus) {
    this.editorLineStatus = editorLineStatus;
  }
  
  @Override
  public void select(int selectionStart, int selectionEnd) {
    super.select(selectionStart, selectionEnd);
    if(editorLineStatus != null) editorLineStatus.set(selectionStart, selectionEnd);
  }

  public boolean isSelectionActive() {
    return this.getSelectedText() != null;
  }
  
  public void setSelectedText(String text){
    
    int old = getTextMode();
    setTextMode(OVERWRITE_MODE);
    replaceSelection(text);
    setTextMode(old);
    
  }

  protected void installTokenMaker(){
    AbstractTokenMakerFactory atmf = (AbstractTokenMakerFactory)TokenMakerFactory.getDefaultInstance();
    atmf.putMapping(SYNTAX_STYLE_CPLUSPLUS, "processing.app.syntax.SketchTokenMaker");
    setSyntaxEditingStyle(SYNTAX_STYLE_CPLUSPLUS);
  }
  
  public void processKeyEvent(KeyEvent evt) {
  
    // this had to be added because the menu key events weren't making it up to the frame.
    
    switch(evt.getID()) {
    case KeyEvent.KEY_TYPED:
      if (editorListener != null) editorListener.keyTyped(evt);
      break;
    case KeyEvent.KEY_PRESSED:
      if (editorListener != null) editorListener.keyPressed(evt);
      break;
    case KeyEvent.KEY_RELEASED:
      // inputHandler.keyReleased(evt);
      break;
    }
    
    if(!evt.isConsumed()){
       super.processKeyEvent(evt);
    }
  }
  
  public void switchDocument(Document document, UndoManager newUndo) {
    
    // HACK: Dont discard changes on curret UndoManager.
    // BUG: https://github.com/bobbylight/RSyntaxTextArea/issues/84
    setUndoManager(null); // bypass reset current undo manager...
    
    super.setDocument(document);
    
    setUndoManager((RUndoManager) newUndo);
    
    // HACK: Complement previous hack (hide code folding on switch) | Drawback: Lose folding state
//  if(sketch.getCodeCount() > 1 && textarea.isCodeFoldingEnabled()){
//    textarea.setCodeFoldingEnabled(false);
//    textarea.setCodeFoldingEnabled(true);
//  }
    
    
  }

  @Override
  protected JPopupMenu createPopupMenu() {
    JPopupMenu menu = super.createPopupMenu();
    return menu;
  }
  
  @Override
  protected void configurePopupMenu(JPopupMenu popupMenu) {
    super.configurePopupMenu(popupMenu);
  }
  
  public void getTextLine(int line, Segment segment) {
    try {
      int offset = getLineStartOffset(line);
      int end = getLineEndOffset(line);
      getDocument().getText(offset, end - offset, segment);
    } catch (BadLocationException e) {
    }
  }
  
  public String getTextLine(int line) {
    try {
      int offset = getLineStartOffset(line);
      int end = getLineEndOffset(line);
      return getDocument().getText(offset, end - offset);
    } catch (BadLocationException e) {
      return null;
    }
  }
  

  public void setEditorListener(EditorListener editorListener) {
    this.editorListener = editorListener;
  }
  
  private static class DocLinkGenerator implements LinkGenerator{
    
     @Override
      public LinkGeneratorResult isLinkAtOffset(RSyntaxTextArea textArea, final int offs) {
        
        final Token token = textArea.modelToToken(offs);
        
        final String reference = PdeKeywords.getReference(token.getLexeme());

        // LOG.fine("reference: " + reference + ", match: " + (token.getType() == TokenTypes.DATA_TYPE || token.getType() == TokenTypes.VARIABLE || token.getType() == TokenTypes.FUNCTION));
        
        if(token != null && (reference != null || (token.getType() == TokenTypes.DATA_TYPE || token.getType() == TokenTypes.VARIABLE || token.getType() == TokenTypes.FUNCTION) )){
          
          LinkGeneratorResult generatorResult = new LinkGeneratorResult() {
            
            @Override
            public int getSourceOffset() {
              return offs;
            }
            
            @Override
            public HyperlinkEvent execute() {
              
              LOG.fine("Open Reference: " + reference);
              
              Base.showReference("Reference/" + reference);
              
              return null;
            }
          };
          
          return generatorResult;
        }
        
        return null;
      }
    };

}
