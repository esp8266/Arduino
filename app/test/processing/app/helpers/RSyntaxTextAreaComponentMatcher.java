package processing.app.helpers;

import java.awt.Component;

import org.fest.swing.core.ComponentMatcher;
import org.fife.ui.rsyntaxtextarea.RSyntaxTextArea;

public class RSyntaxTextAreaComponentMatcher implements ComponentMatcher {

  private final String name;

  public RSyntaxTextAreaComponentMatcher(String name) {
    this.name = name;
  }

  @Override
  public boolean matches(Component component) {
    return component instanceof RSyntaxTextArea && name.equals(component.getName());
  }
}
