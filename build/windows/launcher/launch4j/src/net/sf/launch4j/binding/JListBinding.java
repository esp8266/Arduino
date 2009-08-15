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
package net.sf.launch4j.binding;

import java.awt.Color;
import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import javax.swing.DefaultListModel;
import javax.swing.JList;

import org.apache.commons.beanutils.PropertyUtils;

/**
 * @author Copyright (C) 2006 Grzegorz Kowal
 */
public class JListBinding implements Binding {
	private final String _property;
	private final JList _list;
	private final Color _validColor;

	public JListBinding(String property, JList list) {
		if (property == null || list == null) {
			throw new NullPointerException();
		}
		if (property.equals("")) {
			throw new IllegalArgumentException();
		}
		_property = property;
		_list = list;
		_validColor = _list.getBackground();
	}

	public String getProperty() {
		return _property;
	}

	public void clear(IValidatable bean) {
		_list.setModel(new DefaultListModel());
	}

	public void put(IValidatable bean) {
		try {
			DefaultListModel model = new DefaultListModel();
			List list = (List) PropertyUtils.getProperty(bean, _property);
			if (list != null) {
				for (Iterator iter = list.iterator(); iter.hasNext();) {
					model.addElement(iter.next());
				}
			}
			_list.setModel(model);
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}

	public void get(IValidatable bean) {
		try {
			DefaultListModel model = (DefaultListModel) _list.getModel();
			final int size = model.getSize();
			List list = new ArrayList(size);
			for (int i = 0; i < size; i++) {
				list.add(model.get(i));
			}
			PropertyUtils.setProperty(bean, _property, list);
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}
	
	public void markValid() {
		_list.setBackground(_validColor);
		_list.requestFocusInWindow();
	}

	public void markInvalid() {
		_list.setBackground(Binding.INVALID_COLOR);
	}
	
	public void setEnabled(boolean enabled) {
		_list.setEnabled(enabled);
	}
}
