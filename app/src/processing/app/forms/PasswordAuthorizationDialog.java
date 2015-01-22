package processing.app.forms;

import processing.app.Base;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;

import static processing.app.I18n._;

public class PasswordAuthorizationDialog extends JDialog {

  protected final JButton uploadButton;
  protected final JButton cancelButton;
  protected final JLabel typePasswordLabel;
  protected final JLabel passwordLabel;
  protected final JLabel icon;
  protected final JPasswordField passwordField;

  protected boolean cancelled;
  protected String password;

  public PasswordAuthorizationDialog(Frame parent, String dialogText) {
    super(parent, true);

    this.cancelled = false;
    this.password = null;

    typePasswordLabel = new JLabel();
    icon = new JLabel();
    passwordLabel = new JLabel();
    passwordField = new JPasswordField();
    uploadButton = new JButton();
    cancelButton = new JButton();

    setDefaultCloseOperation(WindowConstants.DISPOSE_ON_CLOSE);

    typePasswordLabel.setText(dialogText);

    icon.setIcon(new ImageIcon(new File(Base.getContentFile("lib"), "theme/lock.png").getAbsolutePath()));

    passwordLabel.setText(_("Password:"));

    passwordField.setText("");
    passwordField.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        uploadButtonPressed(evt);
      }
    });

    uploadButton.setText(_("Upload"));
    uploadButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        uploadButtonPressed(evt);
      }
    });

    cancelButton.setText(_("Cancel"));
    cancelButton.addActionListener(new ActionListener() {
      public void actionPerformed(ActionEvent evt) {
        cancelButtonPressed(evt);
      }
    });

    GroupLayout layout = new GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
            layout.createParallelGroup(GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                            .addContainerGap()
                            .addComponent(icon, GroupLayout.PREFERRED_SIZE, 66, GroupLayout.PREFERRED_SIZE)
                            .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                            .addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
                                    .addComponent(typePasswordLabel)
                                    .addGroup(layout.createSequentialGroup()
                                            .addComponent(passwordLabel)
                                            .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                                            .addComponent(passwordField, GroupLayout.PREFERRED_SIZE, 300, GroupLayout.PREFERRED_SIZE)))
                            .addContainerGap(GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
                    .addGroup(GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                            .addContainerGap(GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
                            .addComponent(cancelButton)
                            .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                            .addComponent(uploadButton)
                            .addContainerGap())
    );
    layout.setVerticalGroup(
            layout.createParallelGroup(GroupLayout.Alignment.LEADING)
                    .addGroup(layout.createSequentialGroup()
                            .addContainerGap()
                            .addComponent(typePasswordLabel)
                            .addGroup(layout.createParallelGroup(GroupLayout.Alignment.LEADING)
                                    .addGroup(layout.createSequentialGroup()
                                            .addGap(53, 53, 53)
                                            .addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
                                                    .addComponent(passwordLabel)
                                                    .addComponent(passwordField, GroupLayout.PREFERRED_SIZE, GroupLayout.DEFAULT_SIZE, GroupLayout.PREFERRED_SIZE))
                                            .addGap(18, 18, 18))
                                    .addGroup(GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
                                            .addPreferredGap(LayoutStyle.ComponentPlacement.RELATED)
                                            .addComponent(icon)
                                            .addGap(9, 9, 9)))
                            .addGroup(layout.createParallelGroup(GroupLayout.Alignment.BASELINE)
                                    .addComponent(uploadButton)
                                    .addComponent(cancelButton))
                            .addContainerGap(GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE))
    );

    pack();
  }

  private void cancelButtonPressed(ActionEvent event) {
    this.cancelled = true;
    this.dispose();
  }

  public void uploadButtonPressed(ActionEvent event) {
    this.password = new String(passwordField.getPassword());
    this.dispose();
  }

  public String getPassword() {
    return this.password;
  }

  public boolean isCancelled() {
    return cancelled;
  }
}