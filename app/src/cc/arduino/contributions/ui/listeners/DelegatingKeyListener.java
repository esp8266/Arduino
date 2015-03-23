package cc.arduino.contributions.ui.listeners;

import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

public class DelegatingKeyListener implements KeyListener {

  private final Component delegate;

  public DelegatingKeyListener(Component delegate) {
    this.delegate = delegate;
  }

  @Override
  public void keyTyped(final KeyEvent keyEvent) {
    if (delegate.getKeyListeners() == null) {
      return;
    }

    for (KeyListener listener : delegate.getKeyListeners()) {
      listener.keyTyped(keyEvent);
    }
  }

  @Override
  public void keyPressed(KeyEvent keyEvent) {
    if (delegate.getKeyListeners() == null) {
      return;
    }

    for (KeyListener listener : delegate.getKeyListeners()) {
      listener.keyPressed(keyEvent);
    }
  }

  @Override
  public void keyReleased(KeyEvent keyEvent) {
    if (delegate.getKeyListeners() == null) {
      return;
    }

    for (KeyListener listener : delegate.getKeyListeners()) {
      listener.keyReleased(keyEvent);
    }
  }
}
