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

import javax.swing.DefaultComboBoxModel;
import javax.swing.JFileChooser;
import javax.swing.JTextField;

import net.sf.launch4j.binding.Bindings;
import net.sf.launch4j.binding.Validator;
import net.sf.launch4j.form.JreForm;
import net.sf.launch4j.config.Jre;

/**
 * @author Copyright (C) 2006 Grzegorz Kowal
 */
public class JreFormImpl extends JreForm {

	public JreFormImpl(Bindings bindings, JFileChooser fc) {
		_jdkPreferenceCombo.setModel(new DefaultComboBoxModel(new String[] {
				Messages.getString("jdkPreference.jre.only"),
				Messages.getString("jdkPreference.prefer.jre"),
				Messages.getString("jdkPreference.prefer.jdk"),
				Messages.getString("jdkPreference.jdk.only")}));
		bindings.add("jre.path", _jrePathField)
				.add("jre.minVersion", _jreMinField)
				.add("jre.maxVersion", _jreMaxField)
				.add("jre.jdkPreferenceIndex", _jdkPreferenceCombo,
						Jre.DEFAULT_JDK_PREFERENCE_INDEX)
				.add("jre.initialHeapSize", _initialHeapSizeField)
				.add("jre.initialHeapPercent", _initialHeapPercentField)
				.add("jre.maxHeapSize", _maxHeapSizeField)
				.add("jre.maxHeapPercent", _maxHeapPercentField)
				.add("jre.options", _jvmOptionsTextArea);

		_varCombo.setModel(new DefaultComboBoxModel(new String[] {
				"EXEDIR", "EXEFILE", "PWD", "OLDPWD",
				"HKEY_CLASSES_ROOT", "HKEY_CURRENT_USER", "HKEY_LOCAL_MACHINE",
				"HKEY_USERS", "HKEY_CURRENT_CONFIG" }));

		_varCombo.addActionListener(new VarComboActionListener());
		_varCombo.setSelectedIndex(0);

		_propertyButton.addActionListener(new PropertyActionListener());
		_optionButton.addActionListener(new OptionActionListener());

		_envPropertyButton.addActionListener(new EnvPropertyActionListener(_envVarField));
		_envOptionButton.addActionListener(new EnvOptionActionListener(_envVarField));
	}

	private class VarComboActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			_optionButton.setEnabled(((String) _varCombo.getSelectedItem())
					.startsWith("HKEY_"));
		}
	}

	private class PropertyActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			final int pos = _jvmOptionsTextArea.getCaretPosition();
			final String var = (String) _varCombo.getSelectedItem();
			if (var.startsWith("HKEY_")) {
				_jvmOptionsTextArea.insert("-Dreg.key=\"%"
						+ var + "\\\\...%\"\n", pos);
			} else {
				_jvmOptionsTextArea.insert("-Dlaunch4j." + var.toLowerCase()
						+ "=\"%" + var + "%\"\n", pos);
			}
		}
	}

	private class OptionActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			final int pos = _jvmOptionsTextArea.getCaretPosition();
			final String var = (String) _varCombo.getSelectedItem();
			if (var.startsWith("HKEY_")) {
				_jvmOptionsTextArea.insert("%" + var + "\\\\...%\n", pos);
			} else {
				_jvmOptionsTextArea.insert("%" + var + "%\n", pos);
			}
		}
	}

	private abstract class EnvActionListener extends AbstractAcceptListener {
		public EnvActionListener(JTextField f, boolean listen) {
			super(f, listen);
		}

		public void actionPerformed(ActionEvent e) {
			final int pos = _jvmOptionsTextArea.getCaretPosition();
			final String var = getText()
					.replaceAll("\"", "")
					.replaceAll("%", "");
			if (Validator.isEmpty(var)) {
				signalViolation(Messages.getString("specifyVar"));
				return;
			}
			add(var, pos);
			clear();
		}

		protected abstract void add(String var, int pos);
	}

	private class EnvPropertyActionListener extends EnvActionListener {
		public EnvPropertyActionListener(JTextField f) {
			super(f, true);
		}

		protected void add(String var, int pos) {
			final String prop = var
					.replaceAll(" ", ".")
					.replaceAll("_", ".")
					.toLowerCase();
			_jvmOptionsTextArea.insert("-Denv." + prop + "=\"%" + var
					+ "%\"\n", pos);
		}
	}

	private class EnvOptionActionListener extends EnvActionListener {
		public EnvOptionActionListener(JTextField f) {
			super(f, false);
		}

		protected void add(String var, int pos) {
			_jvmOptionsTextArea.insert("%" + var + "%\n", pos);	
		}
	}
}
