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
 * Created on Apr 30, 2005
 */
package net.sf.launch4j.binding;

import java.awt.Color;

import javax.swing.text.JTextComponent;

import org.apache.commons.beanutils.BeanUtils;

/**
 * Handles JEditorPane, JTextArea, JTextField
 * 
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class JTextComponentBinding implements Binding {
	private final String _property;
	private final JTextComponent _textComponent;
	private final String _defaultValue;
	private final Color _validColor;

	public JTextComponentBinding(String property, JTextComponent textComponent,
			String defaultValue) {
		if (property == null || textComponent == null || defaultValue == null) {
			throw new NullPointerException();
		}
		if (property.equals("")) {
			throw new IllegalArgumentException();
		}
		_property = property;
		_textComponent = textComponent;
		_defaultValue = defaultValue;
		_validColor = _textComponent.getBackground();
	}

	public String getProperty() {
		return _property;
	}

	public void clear(IValidatable bean) {
		_textComponent.setText(_defaultValue);
	}

	public void put(IValidatable bean) {
		try {
			String s = BeanUtils.getProperty(bean, _property);
			// XXX displays zeros as blank
			_textComponent.setText(s != null && !s.equals("0") ? s : "");
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}

	public void get(IValidatable bean) {
		try {
			BeanUtils.setProperty(bean, _property, _textComponent.getText());
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}
	
	public void markValid() {
		_textComponent.setBackground(_validColor);
		_textComponent.requestFocusInWindow();
	}

	public void markInvalid() {
		_textComponent.setBackground(Binding.INVALID_COLOR);
	}
	
	public void setEnabled(boolean enabled) {
		_textComponent.setEnabled(enabled);
	}
}
