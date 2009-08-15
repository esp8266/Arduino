/*
	Launch4j (http://launch4j.sourceforge.net/)
	Cross-platform Java application wrapper for creating Windows native executables.

	Copyright (c) 2007 Ian Roberts

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
 * Created on May 10, 2005
 */
package net.sf.launch4j.binding;

import java.awt.Color;

import javax.swing.JComboBox;

import org.apache.commons.beanutils.PropertyUtils;

/**
 * @author Copyright (C) 2007 Ian Roberts
 */
public class JComboBoxBinding implements Binding {
	private final String _property;
	private final JComboBox _combo;
	private final int _defaultValue;
	private final Color _validColor;

	public JComboBoxBinding(String property, JComboBox combo, int defaultValue) {
		if (property == null || combo == null) {
			throw new NullPointerException();
		}
		if (property.equals("")
				|| combo.getItemCount() == 0
				|| defaultValue < 0 || defaultValue >= combo.getItemCount()) {
			throw new IllegalArgumentException();
		}
		_property = property;
		_combo = combo;
		_defaultValue = defaultValue;
		_validColor = combo.getBackground();
	}

	public String getProperty() {
		return _property;
	}

	public void clear(IValidatable bean) {
		select(_defaultValue);
	}

	public void put(IValidatable bean) {
		try {
			Integer i = (Integer) PropertyUtils.getProperty(bean, _property);
			if (i == null) {
				throw new BindingException(
						Messages.getString("JComboBoxBinding.property.null"));
			}
			select(i.intValue());
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}

	public void get(IValidatable bean) {
		try {
			PropertyUtils.setProperty(bean, _property, new Integer(_combo.getSelectedIndex()));
			return;
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}

	private void select(int index) {
		if (index < 0 || index >= _combo.getItemCount()) {
			throw new BindingException(
					Messages.getString("JComboBoxBinding.index.out.of.bounds"));
		}
		_combo.setSelectedIndex(index);
	}

	public void markValid() {
		_combo.setBackground(_validColor);
		_combo.requestFocusInWindow();
	}

	public void markInvalid() {
		_combo.setBackground(Binding.INVALID_COLOR);
	}
	
	public void setEnabled(boolean enabled) {
		_combo.setEnabled(enabled);
	}
}
