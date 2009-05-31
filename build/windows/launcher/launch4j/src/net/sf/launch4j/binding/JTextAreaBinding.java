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
 * Created on Jun 14, 2006
 */
package net.sf.launch4j.binding;

import java.awt.Color;
import java.util.ArrayList;
import java.util.List;

import javax.swing.JTextArea;

import org.apache.commons.beanutils.PropertyUtils;

/**
 * @author Copyright (C) 2006 Grzegorz Kowal
 */
public class JTextAreaBinding implements Binding {
	private final String _property;
	private final JTextArea _textArea;
	private final Color _validColor;

	public JTextAreaBinding(String property, JTextArea textArea) {
		if (property == null || textArea == null) {
			throw new NullPointerException();
		}
		if (property.equals("")) {
			throw new IllegalArgumentException();
		}
		_property = property;
		_textArea = textArea;
		_validColor = _textArea.getBackground();
	}

	public String getProperty() {
		return _property;
	}

	public void clear(IValidatable bean) {
		put(bean);
	}

	public void put(IValidatable bean) {
		try {
			List list = (List) PropertyUtils.getProperty(bean, _property);
			StringBuffer sb = new StringBuffer();
			if (list != null) {
				for (int i = 0; i < list.size(); i++) {
					sb.append(list.get(i));
					if (i < list.size() - 1) {
						sb.append("\n");
					}
				}
			}
			_textArea.setText(sb.toString());
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}

	public void get(IValidatable bean) {
		try {
			String text = _textArea.getText();
			if (!text.equals("")) {
				String[] items = text.split("\n");
				List list = new ArrayList();
				for (int i = 0; i < items.length; i++) {
					list.add(items[i]);
				}
				PropertyUtils.setProperty(bean, _property, list);
			} else {
				PropertyUtils.setProperty(bean, _property, null);
			}
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}

	public void markValid() {
		_textArea.setBackground(_validColor);
		_textArea.requestFocusInWindow();
	}

	public void markInvalid() {
		_textArea.setBackground(Binding.INVALID_COLOR);
	}
	
	public void setEnabled(boolean enabled) {
		_textArea.setEnabled(enabled);
	}
}
