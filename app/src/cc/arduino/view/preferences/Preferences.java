/*
 * This file is part of Arduino.
 *
 * Copyright 2015 Arduino LLC (http://www.arduino.cc/)
 *
 * Arduino is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 */

package cc.arduino.view.preferences;

import processing.app.*;
import processing.app.helpers.FileUtils;
import processing.app.legacy.PApplet;

import javax.swing.*;
import java.awt.*;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowEvent;
import java.io.File;
import java.util.LinkedList;

import static processing.app.I18n._;

public class Preferences extends javax.swing.JDialog {

  private final Language[] languages;
  private final Language[] missingLanguages;
  private final WarningItem[] warningItems;
  private final Base base;

  public static class Language {

    private final String name;
    private final String originalName;
    private final String isoCode;

    public Language(String name, String originalName, String isoCode) {
      this.name = name;
      this.originalName = originalName;
      this.isoCode = isoCode;
    }

    public String toString() {
      if (originalName.length() == 0) {
        return name;
      }
      return originalName + " (" + name + ")";
    }

    public String getIsoCode() {
      return isoCode;
    }
  }

  private static class WarningItem {
    private final String value;
    private final String translation;

    public WarningItem(String value, String translation) {
      this.value = value;
      this.translation = translation;
    }

    public String getValue() {
      return value;
    }

    @Override
    public String toString() {
      return translation;
    }
  }

  public Preferences(Window parent, Base base) {
    super(parent);
    this.base = base;

    this.languages = new Language[]{
      new Language(_("System Default"), "", ""),
      new Language(_("Albanian"), "shqip", "sq"),
      new Language(_("Arabic"), "العربية", "ar"),
      new Language(_("Aragonese"), "Aragonés", "an"),
      new Language(_("Belarusian"), "Беларуская мова", "be"),
      new Language(_("Bulgarian"), "български", "bg"),
      new Language(_("Catalan"), "Català", "ca"),
      new Language(_("Chinese Simplified"), "简体中文", "zh_CN"),
      new Language(_("Chinese Traditional"), "繁體中文", "zh_TW"),
      new Language(_("Croatian"), "Hrvatski", "hr_HR"),
      new Language(_("Czech (Czech Republic)"), "český (Czech Republic)", "cs_CZ"),
      new Language(_("Danish (Denmark)"), "Dansk (Denmark)", "da_DK"),
      new Language(_("Dutch"), "Nederlands", "nl"),
      new Language(_("English"), "English", "en"),
      new Language(_("English (United Kingdom)"), "English (United Kingdom)", "en_GB"),
      new Language(_("Estonian"), "Eesti", "et"),
      new Language(_("Estonian (Estonia)"), "Eesti keel", "et_EE"),
      new Language(_("Filipino"), "Pilipino", "fil"),
      new Language(_("Finnish"), "Suomi", "fi"),
      new Language(_("French"), "Français", "fr"),
      new Language(_("Canadian French"), "Canadienne-français", "fr_CA"),
      new Language(_("Galician"), "Galego", "gl"),
      new Language(_("Georgian"), "საქართველოს", "ka_GE"),
      new Language(_("German"), "Deutsch", "de_DE"),
      new Language(_("Greek"), "ελληνικά", "el_GR"),
      new Language(_("Hebrew"), "עברית", "he"),
      new Language(_("Hindi"), "हिंदी", "hi"),
      new Language(_("Hungarian"), "Magyar", "hu"),
      new Language(_("Indonesian"), "Bahasa Indonesia", "id"),
      new Language(_("Italian"), "Italiano", "it_IT"),
      new Language(_("Japanese"), "日本語", "ja_JP"),
      new Language(_("Korean"), "한국어", "ko_KR"),
      new Language(_("Latvian"), "Latviešu", "lv_LV"),
      new Language(_("Lithuaninan"), "Lietuvių Kalba", "lt_LT"),
      new Language(_("Norwegian Bokmål"), "Norsk bokmål", "nb_NO"),
      new Language(_("Persian"), "فارسی", "fa"),
      new Language(_("Polish"), "Język Polski", "pl"),
      new Language(_("Portuguese (Brazil)"), "Português (Brazil)", "pt_BR"),
      new Language(_("Portuguese (Portugal)"), "Português (Portugal)", "pt_PT"),
      new Language(_("Romanian"), "Română", "ro"),
      new Language(_("Russian"), "Русский", "ru"),
      new Language(_("Slovenian"), "Slovenščina", "sl_SI"),
      new Language(_("Spanish"), "Español", "es"),
      new Language(_("Swedish"), "Svenska", "sv"),
      new Language(_("Tamil"), "தமிழ்", "ta"),
      new Language(_("Turkish"), "Türk", "tr"),
      new Language(_("Ukrainian"), "Український", "uk"),
      new Language(_("Vietnamese"), "Tiếng Việt", "vi"),
    };

    this.missingLanguages = new Language[]{
      new Language(_("Afrikaans"), "Afrikaans", "af"),
      new Language(_("Armenian"), "Հայերեն", "hy"),
      new Language(_("Asturian"), "Asturianu", "ast"),
      new Language(_("Basque"), "Euskara", "eu"),
      new Language(_("Bengali (India)"), "বাংলা (India)", "bn_IN"),
      new Language(_("Bosnian"), "Bosanski", "bs"),
      new Language(_("Burmese (Myanmar)"), "ဗမာစကား", "my_MM"),
      new Language(_("Chinese (China)"), "", "zh_CN"),
      new Language(_("Chinese (Hong Kong)"), "", "zh_HK"),
      new Language(_("Chinese (Taiwan)"), "", "zh_TW"),
      new Language(_("Chinese (Taiwan) (Big5)"), "", "zh_TW.Big5"),
      new Language(_("Czech"), "český", "cs"),
      new Language(_("Danish"), "Dansk", "da"),
      new Language(_("Dutch (Netherlands)"), "Nederlands", "nl_NL"),
      new Language(_("Galician (Spain)"), "Galego (Spain)", "gl_ES"),
      new Language(_("Nepali"), "नेपाली", "ne"),
      new Language(_("N'Ko"), "ߒߞߏ", "nqo"),
      new Language(_("Marathi"), "मराठी", "mr"),
      new Language(_("Malay (Malaysia)"), "بهاس ملايو (Malaysia)", "ms_MY"),
      new Language(_("Norwegian"), "Norsk", "no"),
      new Language(_("Norwegian Nynorsk"), "Norsk Nynorsk", "nn"),
      new Language(_("Portugese"), "Português", "pt"),
      new Language(_("Persian (Iran)"), "فارسی (Iran)", "fa_IR"),
      new Language(_("Slovak"), "Slovenčina", "sk"),
      new Language(_("Swahili"), "كِسوَهِل", "sw"),
      new Language(_("Talossan"), "Talossan", "tzl"),
      new Language(_("Urdu (Pakistan)"), "اردو (Pakistan)", "ur_PK"),
      new Language(_("Western Frisian"), "Western Frisian", "fy"),
    };

    this.warningItems = new WarningItem[]{
      new WarningItem("none", _("None")),
      new WarningItem("default", _("Default")),
      new WarningItem("more", _("More")),
      new WarningItem("all", _("All"))
    };

    initComponents();

    Base.registerWindowCloseKeys(getRootPane(), new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        cancelButtonActionPerformed(e);
      }
    });

    showPrerefencesData();
  }

  /**
   * This method is called from within the constructor to initialize the form.
   * WARNING: Do NOT modify this code. The content of this method is always
   * regenerated by the Form Editor.
   */
  @SuppressWarnings("unchecked")
  // <editor-fold defaultstate="collapsed" desc="Generated Code">//GEN-BEGIN:initComponents
  private void initComponents() {

    javax.swing.JLabel sketchbookLocationLabel = new javax.swing.JLabel();
    sketchbookLocationField = new javax.swing.JTextField();
    javax.swing.JButton browseButton = new javax.swing.JButton();
    javax.swing.JLabel comboLanguageLabel = new javax.swing.JLabel();
    comboLanguage = new JComboBox(languages);
    javax.swing.JLabel requiresRestartLabel = new javax.swing.JLabel();
    javax.swing.JLabel fontSizeLabel = new javax.swing.JLabel();
    fontSizeField = new javax.swing.JTextField();
    javax.swing.JLabel showVerboseLabel = new javax.swing.JLabel();
    verboseCompilationBox = new javax.swing.JCheckBox();
    verboseUploadBox = new javax.swing.JCheckBox();
    javax.swing.JLabel comboWarningsLabel = new javax.swing.JLabel();
    comboWarnings = new JComboBox(warningItems);
    javax.swing.JLabel additionalBoardsManagerLabel = new javax.swing.JLabel();
    additionalBoardsManagerField = new javax.swing.JTextField();
    javax.swing.JButton extendedAdditionalUrlFieldWindow = new javax.swing.JButton();
    javax.swing.JLabel morePreferencesLabel = new javax.swing.JLabel();
    preferencesFileLabel = new javax.swing.JLabel();
    javax.swing.JLabel arduinoNotRunningLabel = new javax.swing.JLabel();
    javax.swing.JButton okButton = new javax.swing.JButton();
    javax.swing.JButton cancelButton = new javax.swing.JButton();
    javax.swing.JPanel checkboxesContainer = new javax.swing.JPanel();
    displayLineNumbersBox = new javax.swing.JCheckBox();
    enableCodeFoldingBox = new javax.swing.JCheckBox();
    verifyUploadBox = new javax.swing.JCheckBox();
    externalEditorBox = new javax.swing.JCheckBox();
    checkUpdatesBox = new javax.swing.JCheckBox();
    updateExtensionBox = new javax.swing.JCheckBox();
    saveVerifyUploadBox = new javax.swing.JCheckBox();

    setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
    setTitle(_("Preferences"));
    setModal(true);
    setResizable(false);

    sketchbookLocationLabel.setText(_("Sketchbook location:"));

    sketchbookLocationField.setColumns(40);

    browseButton.setText(I18n.PROMPT_BROWSE);
    browseButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        browseButtonActionPerformed(evt);
      }
    });

    comboLanguageLabel.setText(_("Editor language: "));

    requiresRestartLabel.setText(_("  (requires restart of Arduino)"));

    fontSizeLabel.setText(_("Editor font size: "));

    fontSizeField.setColumns(4);

    showVerboseLabel.setText(_("Show verbose output during: "));

    verboseCompilationBox.setText(_("compilation "));

    verboseUploadBox.setText(_("upload"));

    comboWarningsLabel.setText(_("Compiler warnings: "));

    additionalBoardsManagerLabel.setText(_("Additional Boards Manager URLs: "));
    additionalBoardsManagerLabel.setToolTipText(_("Enter a comma separated list of urls"));

    additionalBoardsManagerField.setToolTipText(_("Enter a comma separated list of urls"));

    extendedAdditionalUrlFieldWindow.setIcon(new ImageIcon(Base.getThemeImage("newwindow.gif", this)));
    extendedAdditionalUrlFieldWindow.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        extendedAdditionalUrlFieldWindowActionPerformed(evt);
      }
    });

    morePreferencesLabel.setForeground(Color.GRAY);
    morePreferencesLabel.setText(_("More preferences can be edited directly in the file"));

    preferencesFileLabel.setText(PreferencesData.getPreferencesFile().getAbsolutePath());
    preferencesFileLabel.addMouseListener(new java.awt.event.MouseAdapter() {
      public void mousePressed(java.awt.event.MouseEvent evt) {
        preferencesFileLabelMousePressed(evt);
      }

      public void mouseExited(java.awt.event.MouseEvent evt) {
        preferencesFileLabelMouseExited(evt);
      }

      public void mouseEntered(java.awt.event.MouseEvent evt) {
        preferencesFileLabelMouseEntered(evt);
      }
    });

    arduinoNotRunningLabel.setForeground(Color.GRAY);
    arduinoNotRunningLabel.setText(_("(edit only when Arduino is not running)"));

    okButton.setText(I18n.PROMPT_OK);
    okButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        okButtonActionPerformed(evt);
      }
    });

    cancelButton.setText(I18n.PROMPT_CANCEL);
    cancelButton.addActionListener(new java.awt.event.ActionListener() {
      public void actionPerformed(java.awt.event.ActionEvent evt) {
        cancelButtonActionPerformed(evt);
      }
    });

    checkboxesContainer.setLayout(new javax.swing.BoxLayout(checkboxesContainer, javax.swing.BoxLayout.Y_AXIS));

    displayLineNumbersBox.setText(_("Display line numbers"));
    checkboxesContainer.add(displayLineNumbersBox);

    enableCodeFoldingBox.setText(_("Enable Code Folding"));
    checkboxesContainer.add(enableCodeFoldingBox);

    verifyUploadBox.setText(_("Verify code after upload"));
    checkboxesContainer.add(verifyUploadBox);

    externalEditorBox.setText(_("Use external editor"));
    checkboxesContainer.add(externalEditorBox);

    checkUpdatesBox.setText(_("Check for updates on startup"));
    checkboxesContainer.add(checkUpdatesBox);

    updateExtensionBox.setText(_("Update sketch files to new extension on save (.pde -> .ino)"));
    checkboxesContainer.add(updateExtensionBox);

    saveVerifyUploadBox.setText(_("Save when verifying or uploading"));
    checkboxesContainer.add(saveVerifyUploadBox);

    javax.swing.GroupLayout layout = new javax.swing.GroupLayout(getContentPane());
    getContentPane().setLayout(layout);
    layout.setHorizontalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
        .addGroup(layout.createSequentialGroup()
          .addContainerGap()
          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createSequentialGroup()
              .addComponent(sketchbookLocationField)
              .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
              .addComponent(browseButton))
            .addComponent(checkboxesContainer, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, Short.MAX_VALUE)
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
              .addComponent(okButton)
              .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
              .addComponent(cancelButton))
            .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
              .addComponent(additionalBoardsManagerLabel)
              .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
              .addComponent(additionalBoardsManagerField, javax.swing.GroupLayout.PREFERRED_SIZE, 500, javax.swing.GroupLayout.PREFERRED_SIZE)
              .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
              .addComponent(extendedAdditionalUrlFieldWindow, javax.swing.GroupLayout.PREFERRED_SIZE, 36, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addGroup(layout.createSequentialGroup()
              .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                .addComponent(sketchbookLocationLabel)
                .addGroup(layout.createSequentialGroup()
                  .addComponent(comboWarningsLabel)
                  .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                  .addComponent(comboWarnings, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
                .addGroup(layout.createSequentialGroup()
                  .addComponent(showVerboseLabel)
                  .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                  .addComponent(verboseCompilationBox)
                  .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                  .addComponent(verboseUploadBox))
                .addGroup(layout.createSequentialGroup()
                  .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(comboLanguageLabel)
                    .addComponent(fontSizeLabel))
                  .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                  .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
                    .addComponent(fontSizeField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                    .addGroup(layout.createSequentialGroup()
                      .addComponent(comboLanguage, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
                      .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
                      .addComponent(requiresRestartLabel))))
                .addComponent(arduinoNotRunningLabel)
                .addComponent(morePreferencesLabel)
                .addComponent(preferencesFileLabel))
              .addGap(0, 0, Short.MAX_VALUE)))
          .addContainerGap())
    );
    layout.setVerticalGroup(
      layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
        .addGroup(javax.swing.GroupLayout.Alignment.TRAILING, layout.createSequentialGroup()
          .addContainerGap()
          .addComponent(sketchbookLocationLabel)
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
            .addComponent(sketchbookLocationField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addComponent(browseButton))
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
            .addComponent(comboLanguageLabel)
            .addComponent(comboLanguage, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
            .addComponent(requiresRestartLabel))
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
            .addComponent(fontSizeLabel)
            .addComponent(fontSizeField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
            .addComponent(showVerboseLabel)
            .addComponent(verboseCompilationBox)
            .addComponent(verboseUploadBox))
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
            .addComponent(comboWarningsLabel)
            .addComponent(comboWarnings, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addComponent(checkboxesContainer, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE)
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.LEADING)
            .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
              .addComponent(additionalBoardsManagerLabel)
              .addComponent(additionalBoardsManagerField, javax.swing.GroupLayout.PREFERRED_SIZE, javax.swing.GroupLayout.DEFAULT_SIZE, javax.swing.GroupLayout.PREFERRED_SIZE))
            .addComponent(extendedAdditionalUrlFieldWindow, javax.swing.GroupLayout.PREFERRED_SIZE, 27, javax.swing.GroupLayout.PREFERRED_SIZE))
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addComponent(morePreferencesLabel)
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addComponent(preferencesFileLabel)
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addComponent(arduinoNotRunningLabel)
          .addPreferredGap(javax.swing.LayoutStyle.ComponentPlacement.RELATED)
          .addGroup(layout.createParallelGroup(javax.swing.GroupLayout.Alignment.BASELINE)
            .addComponent(cancelButton)
            .addComponent(okButton))
          .addContainerGap())
    );

    pack();
  }// </editor-fold>//GEN-END:initComponents

  private void browseButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_browseButtonActionPerformed
    File dflt = new File(sketchbookLocationField.getText());
    File file = Base.selectFolder(_("Select new sketchbook location"), dflt, this);
    if (file != null) {
      String path = file.getAbsolutePath();
      if (BaseNoGui.getPortableFolder() != null) {
        path = FileUtils.relativePath(BaseNoGui.getPortableFolder().toString(), path);
        if (path == null) {
          path = BaseNoGui.getPortableSketchbookFolder();
        }
      }
      sketchbookLocationField.setText(path);
    }
  }//GEN-LAST:event_browseButtonActionPerformed

  private void extendedAdditionalUrlFieldWindowActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_extendedAdditionalUrlFieldWindowActionPerformed
    final AdditionalBoardsManagerURLTextArea additionalBoardsManagerURLTextArea = new AdditionalBoardsManagerURLTextArea(this);
    additionalBoardsManagerURLTextArea.setText(additionalBoardsManagerField.getText());
    additionalBoardsManagerURLTextArea.onOk(new ActionListener() {
      @Override
      public void actionPerformed(ActionEvent e) {
        additionalBoardsManagerField.setText(additionalBoardsManagerURLTextArea.getText());
      }
    });
    additionalBoardsManagerURLTextArea.setVisible(true);
  }//GEN-LAST:event_extendedAdditionalUrlFieldWindowActionPerformed

  private void preferencesFileLabelMouseEntered(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_preferencesFileLabelMouseEntered
    preferencesFileLabel.setForeground(new Color(0, 0, 140));
  }//GEN-LAST:event_preferencesFileLabelMouseEntered

  private void preferencesFileLabelMousePressed(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_preferencesFileLabelMousePressed
    Base.openFolder(PreferencesData.getPreferencesFile().getParentFile());
  }//GEN-LAST:event_preferencesFileLabelMousePressed

  private void preferencesFileLabelMouseExited(java.awt.event.MouseEvent evt) {//GEN-FIRST:event_preferencesFileLabelMouseExited
    preferencesFileLabel.setForeground(Color.BLACK);
  }//GEN-LAST:event_preferencesFileLabelMouseExited

  private void cancelButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_cancelButtonActionPerformed
    dispatchEvent(new WindowEvent(this, WindowEvent.WINDOW_CLOSING));
  }//GEN-LAST:event_cancelButtonActionPerformed

  private void okButtonActionPerformed(java.awt.event.ActionEvent evt) {//GEN-FIRST:event_okButtonActionPerformed
    java.util.List<String> errors = validateData();
    if (!errors.isEmpty()) {
      Base.showWarning(_("Error"), errors.get(0), null);
      return;
    }

    savePreferencesData();
    for (Editor editor : base.getEditors()) {
      editor.applyPreferences();
    }
    cancelButtonActionPerformed(evt);
  }//GEN-LAST:event_okButtonActionPerformed

  // Variables declaration - do not modify//GEN-BEGIN:variables
  private javax.swing.JTextField additionalBoardsManagerField;
  private javax.swing.JCheckBox checkUpdatesBox;
  private javax.swing.JComboBox comboLanguage;
  private javax.swing.JComboBox comboWarnings;
  private javax.swing.JCheckBox displayLineNumbersBox;
  private javax.swing.JCheckBox enableCodeFoldingBox;
  private javax.swing.JCheckBox externalEditorBox;
  private javax.swing.JTextField fontSizeField;
  private javax.swing.JLabel preferencesFileLabel;
  private javax.swing.JCheckBox saveVerifyUploadBox;
  private javax.swing.JTextField sketchbookLocationField;
  private javax.swing.JCheckBox updateExtensionBox;
  private javax.swing.JCheckBox verboseCompilationBox;
  private javax.swing.JCheckBox verboseUploadBox;
  private javax.swing.JCheckBox verifyUploadBox;
  // End of variables declaration//GEN-END:variables

  private java.util.List<String> validateData() {
    java.util.List<String> errors = new LinkedList<String>();
    if (FileUtils.isSubDirectory(new File(sketchbookLocationField.getText()), new File(PreferencesData.get("runtime.ide.path")))) {
      errors.add(_("The specified sketchbook folder contains your copy of the IDE.\nPlease choose a different folder for your sketchbook."));
    }
    return errors;
  }

  private void savePreferencesData() {
    String oldPath = PreferencesData.get("sketchbook.path");
    String newPath = sketchbookLocationField.getText();
    if (newPath.isEmpty()) {
      if (BaseNoGui.getPortableFolder() == null) {
        newPath = base.getDefaultSketchbookFolderOrPromptForIt().toString();
      } else {
        newPath = BaseNoGui.getPortableSketchbookFolder();
      }
    }
    if (!newPath.equals(oldPath)) {
      base.rebuildSketchbookMenus();
      PreferencesData.set("sketchbook.path", newPath);
    }

    Language newLanguage = (Language) comboLanguage.getSelectedItem();
    PreferencesData.set("editor.languages.current", newLanguage.getIsoCode());

    String newSizeText = fontSizeField.getText();
    try {
      int newSize = Integer.parseInt(newSizeText.trim());
      String pieces[] = PApplet.split(PreferencesData.get("editor.font"), ',');
      pieces[2] = String.valueOf(newSize);
      PreferencesData.set("editor.font", PApplet.join(pieces, ','));

    } catch (Exception e) {
      System.err.println(I18n.format(_("ignoring invalid font size {0}"), newSizeText));
    }

    // put each of the settings into the table
    PreferencesData.setBoolean("build.verbose", verboseCompilationBox.isSelected());
    PreferencesData.setBoolean("upload.verbose", verboseUploadBox.isSelected());

    WarningItem warningItem = (WarningItem) comboWarnings.getSelectedItem();
    PreferencesData.set("compiler.warning_level", warningItem.getValue());

    PreferencesData.setBoolean("editor.linenumbers", displayLineNumbersBox.isSelected());

    PreferencesData.setBoolean("editor.code_folding", enableCodeFoldingBox.isSelected());

    PreferencesData.setBoolean("upload.verify", verifyUploadBox.isSelected());

    PreferencesData.setBoolean("editor.save_on_verify", saveVerifyUploadBox.isSelected());

    PreferencesData.setBoolean("editor.external", externalEditorBox.isSelected());

    PreferencesData.setBoolean("update.check", checkUpdatesBox.isSelected());

    PreferencesData.setBoolean("editor.update_extension", updateExtensionBox.isSelected());

    PreferencesData.setBoolean("editor.save_on_verify", saveVerifyUploadBox.isSelected());

    PreferencesData.set("boardsmanager.additional.urls", additionalBoardsManagerField.getText().replace("\r\n", "\n").replace("\r", "\n").replace("\n", ","));

    //editor.applyPreferences();
  }

  private void showPrerefencesData() {
    sketchbookLocationField.setText(PreferencesData.get("sketchbook.path"));

    String currentLanguageISOCode = PreferencesData.get("editor.languages.current");
    for (Language language : languages) {
      if (language.getIsoCode().equals(currentLanguageISOCode)) {
        comboLanguage.setSelectedItem(language);
      }
    }

    Font editorFont = PreferencesData.getFont("editor.font");
    fontSizeField.setText(String.valueOf(editorFont.getSize()));

    verboseCompilationBox.setSelected(PreferencesData.getBoolean("build.verbose"));
    verboseUploadBox.setSelected(PreferencesData.getBoolean("upload.verbose"));

    String currentWarningLevel = PreferencesData.get("compiler.warning_level", "none");
    for (WarningItem item : warningItems) {
      if (currentWarningLevel.equals(item.getValue())) {
        comboWarnings.setSelectedItem(item);
      }
    }

    displayLineNumbersBox.setSelected(PreferencesData.getBoolean("editor.linenumbers"));

    enableCodeFoldingBox.setSelected(PreferencesData.getBoolean("editor.code_folding"));

    verifyUploadBox.setSelected(PreferencesData.getBoolean("upload.verify"));

    externalEditorBox.setSelected(PreferencesData.getBoolean("editor.external"));

    checkUpdatesBox.setSelected(PreferencesData.getBoolean("update.check"));

    updateExtensionBox.setSelected(PreferencesData.get("editor.update_extension") == null || PreferencesData.getBoolean("editor.update_extension"));

    saveVerifyUploadBox.setSelected(PreferencesData.getBoolean("editor.save_on_verify"));

    additionalBoardsManagerField.setText(PreferencesData.get("boardsmanager.additional.urls"));
  }
}
