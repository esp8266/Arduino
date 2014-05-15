package cc.arduino.packages.contributions.ui;

import java.awt.Color;
import java.awt.Font;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

import javax.swing.JTextField;
import javax.swing.UIManager;
import javax.swing.event.DocumentEvent;
import javax.swing.event.DocumentListener;

@SuppressWarnings("serial")
public class FilterJTextField extends JTextField {
  private String filterHint;
  boolean showingHint;
  List<String> filters;

  public FilterJTextField(String hint) {
    super(hint);
    filterHint = hint;

    showingHint = true;
    filters = new ArrayList<String>();
    updateStyle();

    addFocusListener(new FocusListener() {
      public void focusLost(FocusEvent focusEvent) {
        if (getText().isEmpty()) {
          showingHint = true;
        }
        updateStyle();
      }

      public void focusGained(FocusEvent focusEvent) {
        if (showingHint) {
          showingHint = false;
          setText("");
        }
        updateStyle();
      }
    });

    getDocument().addDocumentListener(new DocumentListener() {
      public void removeUpdate(DocumentEvent e) {
        applyFilter();
      }

      public void insertUpdate(DocumentEvent e) {
        applyFilter();
      }

      public void changedUpdate(DocumentEvent e) {
        applyFilter();
      }
    });
  }

  public void applyFilter() {
    String filter = getFilterText();
    filter = filter.toLowerCase();

    // Replace anything but 0-9, a-z, or : with a space
    filter = filter.replaceAll("[^\\x30-\\x39^\\x61-\\x7a^\\x3a]", " ");
    filters = Arrays.asList(filter.split(" "));
    // filterLibraries(category, filters);
  }

  public String getFilterText() {
    return showingHint ? "" : getText();
  }

  public void updateStyle() {
    if (showingHint) {
      setText(filterHint);
      setForeground(Color.gray);
      setFont(getFont().deriveFont(Font.ITALIC));
    } else {
      setForeground(UIManager.getColor("TextField.foreground"));
      setFont(getFont().deriveFont(Font.PLAIN));
    }
  }
}
