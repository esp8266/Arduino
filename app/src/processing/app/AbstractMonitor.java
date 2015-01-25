package processing.app;

import static processing.app.I18n._;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.Rectangle;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.AbstractAction;
import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JCheckBox;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextField;
import javax.swing.KeyStroke;
import javax.swing.SwingUtilities;
import javax.swing.Timer;
import javax.swing.border.EmptyBorder;
import javax.swing.text.DefaultCaret;

import processing.app.debug.TextAreaFIFO;
import processing.app.legacy.PApplet;

@SuppressWarnings("serial")
public abstract class AbstractMonitor extends JFrame implements ActionListener {

  protected final JLabel noLineEndingAlert;
  protected TextAreaFIFO textArea;
  protected JScrollPane scrollPane;
  protected JTextField textField;
  protected JButton sendButton;
  protected JCheckBox autoscrollBox;
  protected JComboBox lineEndings;
  protected JComboBox serialRates;

  private Timer updateTimer;
  private StringBuffer updateBuffer;

  public AbstractMonitor(String title) {
    super(title);

    addWindowListener(new WindowAdapter() {
      public void windowClosing(WindowEvent event) {
        try {
          close();
        } catch (Exception e) {
          // ignore
        }
      }
    });

    // obvious, no?
    KeyStroke wc = Editor.WINDOW_CLOSE_KEYSTROKE;
    getRootPane().getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT).put(wc, "close");
    getRootPane().getActionMap().put("close", (new AbstractAction() {
      public void actionPerformed(ActionEvent event) {
        try {
          close();
        } catch (Exception e) {
          // ignore
        }
        setVisible(false);
      }
    }));

    getContentPane().setLayout(new BorderLayout());

    Font consoleFont = Theme.getFont("console.font");
    Font editorFont = Preferences.getFont("editor.font");
    Font font = new Font(consoleFont.getName(), consoleFont.getStyle(), editorFont.getSize());

    textArea = new TextAreaFIFO(8000000);
    textArea.setRows(16);
    textArea.setColumns(40);
    textArea.setEditable(false);
    textArea.setFont(font);

    // don't automatically update the caret.  that way we can manually decide
    // whether or not to do so based on the autoscroll checkbox.
    ((DefaultCaret) textArea.getCaret()).setUpdatePolicy(DefaultCaret.NEVER_UPDATE);

    scrollPane = new JScrollPane(textArea);

    getContentPane().add(scrollPane, BorderLayout.CENTER);

    JPanel upperPane = new JPanel();
    upperPane.setLayout(new BoxLayout(upperPane, BoxLayout.X_AXIS));
    upperPane.setBorder(new EmptyBorder(4, 4, 4, 4));

    textField = new JTextField(40);
    sendButton = new JButton(_("Send"));

    upperPane.add(textField);
    upperPane.add(Box.createRigidArea(new Dimension(4, 0)));
    upperPane.add(sendButton);

    getContentPane().add(upperPane, BorderLayout.NORTH);

    final JPanel pane = new JPanel();
    pane.setLayout(new BoxLayout(pane, BoxLayout.X_AXIS));
    pane.setBorder(new EmptyBorder(4, 4, 4, 4));

    autoscrollBox = new JCheckBox(_("Autoscroll"), true);

    noLineEndingAlert = new JLabel(I18n.format(_("You've pressed {0} but nothing was sent. Should you select a line ending?"), _("Send")));
    noLineEndingAlert.setToolTipText(noLineEndingAlert.getText());
    noLineEndingAlert.setForeground(pane.getBackground());
    Dimension minimumSize = new Dimension(noLineEndingAlert.getMinimumSize());
    minimumSize.setSize(minimumSize.getWidth() / 3, minimumSize.getHeight());
    noLineEndingAlert.setMinimumSize(minimumSize);

    lineEndings = new JComboBox(new String[]{_("No line ending"), _("Newline"), _("Carriage return"), _("Both NL & CR")});
    lineEndings.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent event) {
        Preferences.setInteger("serial.line_ending", lineEndings.getSelectedIndex());
        noLineEndingAlert.setForeground(pane.getBackground());
      }
    });
    if (Preferences.get("serial.line_ending") != null) {
      lineEndings.setSelectedIndex(Preferences.getInteger("serial.line_ending"));
    }
    lineEndings.setMaximumSize(lineEndings.getMinimumSize());

    String[] serialRateStrings = {
            "300", "1200", "2400", "4800", "9600",
            "19200", "38400", "57600", "115200"
    };

    serialRates = new JComboBox();
    for (String rate : serialRateStrings) {
      serialRates.addItem(rate + " " + _("baud"));
    }

    serialRates.setMaximumSize(serialRates.getMinimumSize());

    pane.add(autoscrollBox);
    pane.add(Box.createHorizontalGlue());
    pane.add(noLineEndingAlert);
    pane.add(Box.createRigidArea(new Dimension(8, 0)));
    pane.add(lineEndings);
    pane.add(Box.createRigidArea(new Dimension(8, 0)));
    pane.add(serialRates);

    this.setMinimumSize(new Dimension(pane.getMinimumSize().width, this.getPreferredSize().height));

    getContentPane().add(pane, BorderLayout.SOUTH);

    pack();

    Dimension screen = Toolkit.getDefaultToolkit().getScreenSize();
    if (Preferences.get("last.screen.height") != null) {
      // if screen size has changed, the window coordinates no longer
      // make sense, so don't use them unless they're identical
      int screenW = Preferences.getInteger("last.screen.width");
      int screenH = Preferences.getInteger("last.screen.height");
      if ((screen.width == screenW) && (screen.height == screenH)) {
        String locationStr = Preferences.get("last.serial.location");
        if (locationStr != null) {
          int[] location = PApplet.parseInt(PApplet.split(locationStr, ','));
          setPlacement(location);
        }
      }
    }
    
    updateBuffer = new StringBuffer(1048576);
    updateTimer = new Timer(33, this);  // redraw serial monitor at 30 Hz
    updateTimer.start();
  }

  public void onSerialRateChange(ActionListener listener) {
    serialRates.addActionListener(listener);
  }

  public void onSendCommand(ActionListener listener) {
    textField.addActionListener(listener);
    sendButton.addActionListener(listener);
  }

  protected void setPlacement(int[] location) {
    setBounds(location[0], location[1], location[2], location[3]);
  }

  protected int[] getPlacement() {
    int[] location = new int[4];

    // Get the dimensions of the Frame
    Rectangle bounds = getBounds();
    location[0] = bounds.x;
    location[1] = bounds.y;
    location[2] = bounds.width;
    location[3] = bounds.height;

    return location;
  }

  public void message(final String s) {
    SwingUtilities.invokeLater(new Runnable() {
      public void run() {
        textArea.append(s);
        if (autoscrollBox.isSelected()) {
          textArea.setCaretPosition(textArea.getDocument().getLength());
        }
      }
    });
  }

  public boolean requiresAuthorization() {
    return false;
  }

  public String getAuthorizationKey() {
    return null;
  }

  public abstract void open() throws Exception;

  public abstract void close() throws Exception;
  
  public synchronized void addToUpdateBuffer(char buff[], int n) {
    updateBuffer.append(buff, 0, n);
  }

  private synchronized String consumeUpdateBuffer() {
    String s = updateBuffer.toString();
    updateBuffer.setLength(0);
    return s;
  }

  public void actionPerformed(ActionEvent e) {
    final String s = consumeUpdateBuffer();
    if (s.length() > 0) {
      //System.out.println("gui append " + s.length());
      if (autoscrollBox.isSelected()) {
        textArea.appendTrim(s);
        textArea.setCaretPosition(textArea.getDocument().getLength());
      } else {
        textArea.appendNoTrim(s);
      }
    }
  }

}
