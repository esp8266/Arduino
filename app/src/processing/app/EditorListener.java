package processing.app;

import java.awt.Toolkit;
import java.awt.event.InputEvent;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

import processing.app.syntax.SketchTextArea;

public class EditorListener implements KeyListener {
  
  private Editor editor;
  
  public EditorListener(Editor editor) {
    super();
    this.editor = editor;
  }
  
  /** ctrl-alt on windows and linux, cmd-alt on mac os x */
  private static final int CTRL = Toolkit.getDefaultToolkit().getMenuShortcutKeyMask();
  private static final int CTRL_ALT = InputEvent.ALT_MASK | CTRL;
  private static final int CTRL_SHIFT = InputEvent.SHIFT_MASK | CTRL;

  public void keyTyped(KeyEvent event) {
    char c = event.getKeyChar();

    if ((event.getModifiers() & KeyEvent.CTRL_MASK) != 0) {
      // The char is not control code when CTRL key pressed? It should be a shortcut.
      if (!Character.isISOControl(c)) {
        event.consume();
      }
    }
  }

  @Override
  public void keyPressed(KeyEvent event) {

    SketchTextArea textarea = editor.getTextArea();
    
    if (!textarea.isEditable()) return;
    
    Sketch sketch = editor.getSketch();

    int code = event.getKeyCode();
    
    // Navigation..
    if ((event.getModifiers() & CTRL) == CTRL && code == KeyEvent.VK_TAB) {
      sketch.handleNextCode();
    }

    // Navigation..
    // FIXME: not working on LINUX !!!
    if ((event.getModifiers() & CTRL_SHIFT) == CTRL_SHIFT && code == KeyEvent.VK_TAB) {
        sketch.handlePrevCode();
    }
    
    // Navigation..
    if ((event.getModifiers() & CTRL_ALT) == CTRL_ALT) {
      if (code == KeyEvent.VK_LEFT) {
        sketch.handlePrevCode();
      } else if (code == KeyEvent.VK_RIGHT) {
        sketch.handleNextCode();
      }
    }
    
//    if (event.isAltDown() && code == KeyEvent.VK_T) {
//      int line = textarea.getCaretLineNumber();
//      textarea.setActiveLineRange(line, line + 3); 
//    }
    
  }

  @Override
  public void keyReleased(KeyEvent e) {
    // TODO Auto-generated method stub
    
  }

}
