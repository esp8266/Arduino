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
 * Created on May 11, 2005
 */
package net.sf.launch4j.binding;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.Arrays;

import javax.swing.JToggleButton;

import org.apache.commons.beanutils.PropertyUtils;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class OptComponentBinding implements Binding, ActionListener {
	private final Bindings _bindings;
	private final String _property;
	private final Class _clazz;
	private final JToggleButton _button;
	private final boolean _enabledByDefault;

	public OptComponentBinding(Bindings bindings, String property, Class clazz,
								JToggleButton button, boolean enabledByDefault) {
		if (property == null || clazz == null || button == null) {
			throw new NullPointerException();
		}
		if (property.equals("")) {
			throw new IllegalArgumentException();
		}
		if (!Arrays.asList(clazz.getInterfaces()).contains(IValidatable.class)) {
			throw new IllegalArgumentException(
					Messages.getString("OptComponentBinding.must.implement")
					+ IValidatable.class);
		}
		_bindings = bindings;
		_property = property;
		_clazz = clazz;
		_button = button;
		_button.addActionListener(this);
		_enabledByDefault = enabledByDefault;
	}

	public String getProperty() {
		return _property;
	}

	public void clear(IValidatable bean) {
		_button.setSelected(_enabledByDefault);
		updateComponents();
	}

	public void put(IValidatable bean) {
		try {
			Object component = PropertyUtils.getProperty(bean, _property);
			_button.setSelected(component != null);
			updateComponents();
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}

	public void get(IValidatable bean) {
		try {
			PropertyUtils.setProperty(bean, _property, _button.isSelected()
					? _clazz.newInstance() : null);
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}

	public void markValid() {}

	public void markInvalid() {}

	public void setEnabled(boolean enabled) {} // XXX implement?

	public void actionPerformed(ActionEvent e) {
		updateComponents();
	}
	
	private void updateComponents() {
		_bindings.setComponentsEnabled(_property, _button.isSelected());
	}
}
