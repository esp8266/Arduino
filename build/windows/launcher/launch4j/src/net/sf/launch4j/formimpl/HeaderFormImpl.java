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

import javax.swing.JRadioButton;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import net.sf.launch4j.binding.Binding;
import net.sf.launch4j.binding.Bindings;
import net.sf.launch4j.config.Config;
import net.sf.launch4j.config.ConfigPersister;
import net.sf.launch4j.form.HeaderForm;

/**
 * @author Copyright (C) 2006 Grzegorz Kowal
 */
public class HeaderFormImpl extends HeaderForm {
	private final Bindings _bindings;

	public HeaderFormImpl(Bindings bindings) {
		_bindings = bindings;
		_bindings.add("headerTypeIndex", new JRadioButton[] { _guiHeaderRadio,
														_consoleHeaderRadio })
				.add("headerObjects", "customHeaderObjects", _headerObjectsCheck,
															_headerObjectsTextArea)
				.add("libs", "customLibs", _libsCheck, _libsTextArea);

		_guiHeaderRadio.addChangeListener(new HeaderTypeChangeListener());
		_headerObjectsCheck.addActionListener(new HeaderObjectsActionListener());
		_libsCheck.addActionListener(new LibsActionListener());
	}

	private class HeaderTypeChangeListener implements ChangeListener {
		public void stateChanged(ChangeEvent e) {
			Config c = ConfigPersister.getInstance().getConfig();
			c.setHeaderType(_guiHeaderRadio.isSelected() ? Config.GUI_HEADER
														: Config.CONSOLE_HEADER);
			if (!_headerObjectsCheck.isSelected()) {
				Binding b = _bindings.getBinding("headerObjects");
				b.put(c);
			}
		}
	}

	private class HeaderObjectsActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			if (!_headerObjectsCheck.isSelected()) {
				ConfigPersister.getInstance().getConfig().setHeaderObjects(null);
				Binding b = _bindings.getBinding("headerObjects");
				b.put(ConfigPersister.getInstance().getConfig());
			}
		}
	}

	private class LibsActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			if (!_libsCheck.isSelected()) {
				ConfigPersister.getInstance().getConfig().setLibs(null);
				Binding b = _bindings.getBinding("libs");
				b.put(ConfigPersister.getInstance().getConfig());
			}
		}
	}
}
