/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2004, 2007 Grzegorz Kowal

	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification,
	are permitted provided that the following conditions are met:

	    * Redistributions of source code must retain the above copyright notice,
	      this list of conditions and the following disclaimer.
	    * Redistributions in binary form must reproduce the above copyright notice,
	      this list of conditions and the following disclaimer in the documentation
	      and/or other materials provided with the distribution.
	    * Neither the name of the Launch4j nor the names of its contributors
	      may be used to endorse or promote products derived from this software without
	      specific prior written permission.

	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
	"AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
	LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
	A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
	CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
	EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
	PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
	PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
	LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
	NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
	SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

/*
 * Created on May 1, 2006
 */
package net.sf.launch4j.formimpl;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.io.File;
import java.io.IOException;
import java.util.jar.Attributes;
import java.util.jar.JarFile;

import javax.swing.DefaultListModel;
import javax.swing.JFileChooser;
import javax.swing.JTextField;
import javax.swing.ListSelectionModel;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import javax.swing.event.ListSelectionEvent;
import javax.swing.event.ListSelectionListener;

import net.sf.launch4j.FileChooserFilter;
import net.sf.launch4j.binding.Bindings;
import net.sf.launch4j.binding.Validator;
import net.sf.launch4j.config.ClassPath;
import net.sf.launch4j.form.ClassPathForm;

/**
 * @author Copyright (C) 2006 Grzegorz Kowal
 */
public class ClassPathFormImpl extends ClassPathForm {
	private final JFileChooser _fileChooser;
	private final FileChooserFilter _filter
			= new FileChooserFilter("Executable jar", ".jar");

	public ClassPathFormImpl(Bindings bindings, JFileChooser fc) {
		bindings.addOptComponent("classPath", ClassPath.class, _classpathCheck)
				.add("classPath.mainClass", _mainclassField)
				.add("classPath.paths", _classpathList);
		_fileChooser = fc;

		ClasspathCheckListener cpl = new ClasspathCheckListener();
		_classpathCheck.addChangeListener(cpl);
		cpl.stateChanged(null);

		_classpathList.setModel(new DefaultListModel());
		_classpathList.setSelectionMode(ListSelectionModel.MULTIPLE_INTERVAL_SELECTION);
		_classpathList.addListSelectionListener(new ClasspathSelectionListener());

		_newClasspathButton.addActionListener(new NewClasspathListener());
		_acceptClasspathButton.addActionListener(
				new AcceptClasspathListener(_classpathField));
		_removeClasspathButton.addActionListener(new RemoveClasspathListener());
		_importClasspathButton.addActionListener(new ImportClasspathListener());
		_classpathUpButton.addActionListener(new MoveUpListener());
		_classpathDownButton.addActionListener(new MoveDownListener());
	}

	private class ClasspathCheckListener implements ChangeListener {
		public void stateChanged(ChangeEvent e) {
			boolean on = _classpathCheck.isSelected();
			_importClasspathButton.setEnabled(on);
			_classpathUpButton.setEnabled(on);
			_classpathDownButton.setEnabled(on);
			_classpathField.setEnabled(on);
			_newClasspathButton.setEnabled(on);
			_acceptClasspathButton.setEnabled(on);
			_removeClasspathButton.setEnabled(on);
		}
	}

	private class NewClasspathListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			_classpathList.clearSelection();
			_classpathField.setText("");
			_classpathField.requestFocusInWindow();
		}
	}

	private class AcceptClasspathListener extends AbstractAcceptListener {
		public AcceptClasspathListener(JTextField f) {
			super(f, true);
		}

		public void actionPerformed(ActionEvent e) {
			String cp = getText();
			if (Validator.isEmpty(cp)) {
				signalViolation(Messages.getString("specifyClassPath"));
				return;
			}
			DefaultListModel model = (DefaultListModel) _classpathList.getModel();
			if (_classpathList.isSelectionEmpty()) {
				model.addElement(cp);
				clear();
			} else {
				model.setElementAt(cp, _classpathList.getSelectedIndex());
			}
		}
	}

	private class ClasspathSelectionListener implements ListSelectionListener {
		public void valueChanged(ListSelectionEvent e) {
			if (e.getValueIsAdjusting()) {
				return;
			}
			if (_classpathList.isSelectionEmpty()) {
				_classpathField.setText("");
			} else {
				_classpathField.setText((String) _classpathList.getSelectedValue());
			}
			_classpathField.requestFocusInWindow();
		}
	}
	
	private class RemoveClasspathListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			if (_classpathList.isSelectionEmpty()
					|| !MainFrame.getInstance().confirm(
							Messages.getString("confirmClassPathRemoval"))) {
				return;
			}
			DefaultListModel model = (DefaultListModel) _classpathList.getModel();
			while (!_classpathList.isSelectionEmpty()) {
				model.remove(_classpathList.getSelectedIndex());
			}
		}
	}
	
	private class MoveUpListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			int x = _classpathList.getSelectedIndex();
			if (x < 1) {
				return;
			}
			DefaultListModel model = (DefaultListModel) _classpathList.getModel();
			Object o = model.get(x - 1);
			model.set(x - 1, model.get(x));
			model.set(x, o);
			_classpathList.setSelectedIndex(x - 1);
		}
	}
	
	private class MoveDownListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			DefaultListModel model = (DefaultListModel) _classpathList.getModel();
			int x = _classpathList.getSelectedIndex();
			if (x == -1 || x >= model.getSize() - 1) {
				return;
			}
			Object o = model.get(x + 1);
			model.set(x + 1, model.get(x));
			model.set(x, o);
			_classpathList.setSelectedIndex(x + 1);
		}
	}

	private class ImportClasspathListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			try {
				_fileChooser.setFileFilter(_filter);
				_fileChooser.setSelectedFile(new File(""));
				if (_fileChooser.showOpenDialog(MainFrame.getInstance())
						== JFileChooser.APPROVE_OPTION) {
					JarFile jar = new JarFile(_fileChooser.getSelectedFile());
					if (jar.getManifest() == null) {
						jar.close();
						MainFrame.getInstance().info(Messages.getString("noManifest"));
						return;
					}
					Attributes attr = jar.getManifest().getMainAttributes();
					String mainClass = (String) attr.getValue("Main-Class");
					String classPath = (String) attr.getValue("Class-Path");
					jar.close();
					_mainclassField.setText(mainClass != null ? mainClass : "");
					DefaultListModel model = new DefaultListModel();
					if (classPath != null) {
						String[] paths = classPath.split(" ");
						for (int i = 0; i < paths.length; i++) {
							model.addElement(paths[i]);
						}
					}
					_classpathList.setModel(model);
				}
			} catch (IOException ex) {
				MainFrame.getInstance().warn(ex.getMessage());
			}
		}
	}
}
