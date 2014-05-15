/*
 * This file is part of Arduino.
 *
 * Copyright 2014 Arduino LLC (http://www.arduino.cc/)
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
package cc.arduino.packages.contributions.ui;

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dialog;
import java.awt.Dimension;
import java.awt.FlowLayout;
import java.awt.Window;
import java.awt.event.ActionListener;

import javax.swing.Box;
import javax.swing.BoxLayout;
import javax.swing.JButton;
import javax.swing.JDialog;
import javax.swing.JLabel;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.border.EmptyBorder;

import cc.arduino.packages.contributions.ContributionInstaller;
import cc.arduino.packages.contributions.ContributionInstaller.Listener;

@SuppressWarnings("serial")
public class ContributionInstallerUI extends JDialog {

  private final JPanel contentPanel = new JPanel();
  private JButton cancelButton;
  private JProgressBar progressBar;
  private JLabel operationLabel;

  public ContributionInstallerUI(Window parent) {
    super(parent, "Installer progress", Dialog.ModalityType.APPLICATION_MODAL);

    setBounds(100, 100, 450, 300);
    getContentPane().setLayout(new BorderLayout());
    contentPanel.setBorder(new EmptyBorder(5, 5, 5, 5));
    getContentPane().add(contentPanel, BorderLayout.CENTER);
    contentPanel.setLayout(new BoxLayout(contentPanel, BoxLayout.Y_AXIS));
    {
      Box verticalBox = Box.createVerticalBox();
      contentPanel.add(verticalBox);
      {
        Component verticalGlue = Box.createVerticalGlue();
        verticalBox.add(verticalGlue);
      }
      {
        Box horizontalBox = Box.createHorizontalBox();
        verticalBox.add(horizontalBox);
        {
          Component horizontalGlue = Box.createHorizontalGlue();
          horizontalBox.add(horizontalGlue);
        }
        {
          JLabel lblNewLabel = new JLabel("Description");
          horizontalBox.add(lblNewLabel);
        }
        {
          Component horizontalGlue = Box.createHorizontalGlue();
          horizontalBox.add(horizontalGlue);
        }
      }
      {
        Component verticalGlue = Box.createVerticalGlue();
        verticalBox.add(verticalGlue);
      }
      {
        Box horizontalBox = Box.createHorizontalBox();
        verticalBox.add(horizontalBox);
        {
          operationLabel = new JLabel("Current running operation");
          horizontalBox.add(operationLabel);
        }
        {
          Component horizontalGlue = Box.createHorizontalGlue();
          horizontalBox.add(horizontalGlue);
        }
      }
      {
        Component verticalStrut = Box.createVerticalStrut(20);
        verticalStrut.setPreferredSize(new Dimension(0, 5));
        verticalStrut.setMinimumSize(new Dimension(0, 5));
        verticalBox.add(verticalStrut);
      }
      {
        progressBar = new JProgressBar();
        progressBar.setStringPainted(true);
        progressBar.setValue(10);
        progressBar.setSize(new Dimension(0, 30));
        progressBar.setMinimumSize(new Dimension(10, 30));
        progressBar.setMaximumSize(new Dimension(32767, 30));
        verticalBox.add(progressBar);
      }
    }
    {
      JPanel buttonPane = new JPanel();
      buttonPane.setLayout(new FlowLayout(FlowLayout.RIGHT));
      getContentPane().add(buttonPane, BorderLayout.SOUTH);
      {
        cancelButton = new JButton("Cancel");
        cancelButton.setActionCommand("Cancel");
        buttonPane.add(cancelButton);
      }
    }
  }

  public void onCancel(ActionListener listener) {
    cancelButton.addActionListener(listener);
  }

  public void setOperationText(String message) {
    operationLabel.setText(message);
  }

  public void setProgress(int progress) {
    progressBar.setValue(progress);
  }

  public void attach(ContributionInstaller installer) {
    installer.setListener(new Listener() {
      @Override
      public void onProgress(double progress, String message) {
        setOperationText(message);
        setProgress((int) progress);
      }
    });
  }
}
