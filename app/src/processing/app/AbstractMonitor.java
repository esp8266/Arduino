package processing.app;

import processing.app.debug.MessageConsumer;
import processing.app.legacy.PApplet;

import javax.swing.*;
import javax.swing.border.EmptyBorder;
import javax.swing.text.DefaultCaret;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import static processing.app.I18n._;

@SuppressWarnings("serial")
public abstract class AbstractMonitor extends JFrame implements MessageConsumer {

  protected final JLabel noLineEndingAlert;
  protected JTextArea textArea;
  protected JScrollPane scrollPane;
  protected JTextField textField;
  protected JButton sendButton;
  protected JCheckBox autoscrollBox;
  protected JComboBox lineEndings;
  protected JComboBox serialRates;

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

    textArea = new JTextArea(16, 40);
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
            "19200", "57600", "115200"
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
}
