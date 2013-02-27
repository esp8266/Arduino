package processing.app.helpers;

import org.fest.swing.core.ComponentMatcher;
import processing.app.syntax.JEditTextArea;

import java.awt.*;

public class JEditTextAreaComponentMatcher implements ComponentMatcher {

  private final String name;

  public JEditTextAreaComponentMatcher(String name) {
    this.name = name;
  }

  @Override
  public boolean matches(Component component) {
    return component instanceof JEditTextArea && name.equals(component.getName());
  }
}
