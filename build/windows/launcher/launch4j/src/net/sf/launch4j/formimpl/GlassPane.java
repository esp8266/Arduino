package net.sf.launch4j.formimpl;

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.Cursor;
import java.awt.Toolkit;
import java.awt.Window;
import java.awt.event.AWTEventListener;
import java.awt.event.KeyAdapter;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;

import javax.swing.JComponent;
import javax.swing.SwingUtilities;

/**
 * This is the glass pane class that intercepts screen interactions during
 * system busy states.
 * 
 * Based on JavaWorld article by Yexin Chen.
 */
public class GlassPane extends JComponent implements AWTEventListener {
	private final Window _window;

	public GlassPane(Window w) {
		_window = w;
		addMouseListener(new MouseAdapter() {});
		addKeyListener(new KeyAdapter() {});
		setCursor(Cursor.getPredefinedCursor(Cursor.WAIT_CURSOR));
	}

	/**
	 * Receives all key events in the AWT and processes the ones that originated
	 * from the current window with the glass pane.
	 * 
	 * @param event
	 *            the AWTEvent that was fired
	 */
	public void eventDispatched(AWTEvent event) {
		Object source = event.getSource();
		if (event instanceof KeyEvent
				&& source instanceof Component) {
			/*
			 * If the event originated from the window w/glass pane,
			 * consume the event.
			 */
			if ((SwingUtilities.windowForComponent((Component) source) == _window)) {
				((KeyEvent) event).consume();
			}
		}
	}

	/**
	 * Sets the glass pane as visible or invisible. The mouse cursor will be set
	 * accordingly.
	 */
	public void setVisible(boolean visible) {
		if (visible) {
			// Start receiving all events and consume them if necessary
			Toolkit.getDefaultToolkit().addAWTEventListener(this, AWTEvent.KEY_EVENT_MASK);
		} else {
			// Stop receiving all events
			Toolkit.getDefaultToolkit().removeAWTEventListener(this);
		}
		super.setVisible(visible);
	}
}
