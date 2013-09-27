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

import java.beans.PropertyChangeEvent;
import java.beans.PropertyChangeListener;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JList;
import javax.swing.JRadioButton;
import javax.swing.JTextArea;
import javax.swing.JToggleButton;
import javax.swing.text.JTextComponent;

import org.apache.commons.beanutils.PropertyUtils;

/**
 * Creates and handles bindings.
 * 
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class Bindings implements PropertyChangeListener {
	private final Map _bindings = new HashMap();
	private final Map _optComponents = new HashMap();
	private boolean _modified = false;

	/**
	 * Used to track component modifications.
	 */
	public void propertyChange(PropertyChangeEvent evt) {
		String prop = evt.getPropertyName();
		if ("AccessibleValue".equals(prop)
				|| "AccessibleText".equals(prop)
				|| "AccessibleVisibleData".equals(prop)) {
			_modified = true;
		}
	}

	/** 
	 * Any of the components modified?
	 */
	public boolean isModified() {
		return _modified;
	}

	public Binding getBinding(String property) {
		return (Binding) _bindings.get(property);
	}

	private void registerPropertyChangeListener(JComponent c) {
		c.getAccessibleContext().addPropertyChangeListener(this);
	}

	private void registerPropertyChangeListener(JComponent[] cs) {
		for (int i = 0; i < cs.length; i++) {
			cs[i].getAccessibleContext().addPropertyChangeListener(this);
		}
	}

	private boolean isPropertyNull(IValidatable bean, Binding b) {
		try {
			for (Iterator iter = _optComponents.keySet().iterator(); iter.hasNext();) {
				String property = (String) iter.next();
				if (b.getProperty().startsWith(property)) {
					return PropertyUtils.getProperty(bean, property) == null;
				}
			}
			return false;
		} catch (Exception e) {
			throw new BindingException(e);
		}
	}

	/**
	 * Enables or disables all components bound to properties that begin with given prefix.
	 */
	public void setComponentsEnabled(String prefix, boolean enabled) {
		for (Iterator iter = _bindings.values().iterator(); iter.hasNext();) {
			Binding b = (Binding) iter.next();
			if (b.getProperty().startsWith(prefix)) {
				b.setEnabled(enabled);
			}
		}
	}

	/**
	 * Clear all components, set them to their default values.
	 * Clears the _modified flag.
	 */
	public void clear(IValidatable bean) {
		for (Iterator iter = _optComponents.values().iterator(); iter.hasNext();) {
			((Binding) iter.next()).clear(bean);
		}
		for (Iterator iter = _bindings.values().iterator(); iter.hasNext();) {
			((Binding) iter.next()).clear(bean);
		}
		_modified = false;
	}

	/**
	 * Copies data from the Java Bean to the UI components.
	 * Clears the _modified flag.
	 */
	public void put(IValidatable bean) {
		for (Iterator iter = _optComponents.values().iterator(); iter.hasNext();) {
			((Binding) iter.next()).put(bean);
		}
		for (Iterator iter = _bindings.values().iterator(); iter.hasNext();) {
			Binding b = (Binding) iter.next();
			if (isPropertyNull(bean, b)) {
				b.clear(null);
			} else {
				b.put(bean);
			}
		}
		_modified = false;
	}

	/**
	 * Copies data from UI components to the Java Bean and checks it's class invariants.
	 * Clears the _modified flag.
	 * @throws InvariantViolationException
	 * @throws BindingException
	 */
	public void get(IValidatable bean) {
		try {
			for (Iterator iter = _optComponents.values().iterator(); iter.hasNext();) {
				((Binding) iter.next()).get(bean);
			}
			for (Iterator iter = _bindings.values().iterator(); iter.hasNext();) {
				Binding b = (Binding) iter.next();
				if (!isPropertyNull(bean, b)) {
					b.get(bean);
				}
			}
			bean.checkInvariants();
			for (Iterator iter = _optComponents.keySet().iterator(); iter.hasNext();) {
				String property = (String) iter.next();
				IValidatable component = (IValidatable) PropertyUtils.getProperty(bean,
						property);
				if (component != null) {
					component.checkInvariants();
				}
			}
			_modified = false;	// XXX
		} catch (InvariantViolationException e) {
			e.setBinding(getBinding(e.getProperty()));
			throw e;
		} catch (Exception e) {
			throw new BindingException(e);
		} 
	}

	private Bindings add(Binding b) {
		if (_bindings.containsKey(b.getProperty())) {
			throw new BindingException(Messages.getString("Bindings.duplicate.binding"));
		}
		_bindings.put(b.getProperty(), b);
		return this;
	}

	/**
	 * Add an optional (nullable) Java Bean component of type clazz.
	 */
	public Bindings addOptComponent(String property, Class clazz, JToggleButton c,
			boolean enabledByDefault) {
		Binding b = new OptComponentBinding(this, property, clazz, c, enabledByDefault);
		if (_optComponents.containsKey(property)) {
			throw new BindingException(Messages.getString("Bindings.duplicate.binding"));
		}
		_optComponents.put(property, b);
		return this;
	}

	/**
	 * Add an optional (nullable) Java Bean component of type clazz.
	 */
	public Bindings addOptComponent(String property, Class clazz, JToggleButton c) {
		return addOptComponent(property, clazz, c, false);
	}

	/**
	 * Handles JEditorPane, JTextArea, JTextField
	 */
	public Bindings add(String property, JTextComponent c, String defaultValue) {
		registerPropertyChangeListener(c);
		return add(new JTextComponentBinding(property, c, defaultValue));
	}

	/**
	 * Handles JEditorPane, JTextArea, JTextField
	 */
	public Bindings add(String property, JTextComponent c) {
		registerPropertyChangeListener(c);
		return add(new JTextComponentBinding(property, c, ""));
	}

	/**
	 * Handles JToggleButton, JCheckBox 
	 */
	public Bindings add(String property, JToggleButton c, boolean defaultValue) {
		registerPropertyChangeListener(c);
		return add(new JToggleButtonBinding(property, c, defaultValue));
	}

	/**
	 * Handles JToggleButton, JCheckBox
	 */
	public Bindings add(String property, JToggleButton c) {
		registerPropertyChangeListener(c);
		return add(new JToggleButtonBinding(property, c, false));
	}

	/**
	 * Handles JRadioButton
	 */
	public Bindings add(String property, JRadioButton[] cs, int defaultValue) {
		registerPropertyChangeListener(cs);
		return add(new JRadioButtonBinding(property, cs, defaultValue));
	}
	
	/**
	 * Handles JRadioButton
	 */
	public Bindings add(String property, JRadioButton[] cs) {
		registerPropertyChangeListener(cs);
		return add(new JRadioButtonBinding(property, cs, 0));
	}

	/**
	 * Handles JTextArea
	 */
	public Bindings add(String property, JTextArea textArea, String defaultValue) {
		registerPropertyChangeListener(textArea);
		return add(new JTextComponentBinding(property, textArea, defaultValue));
	}

	/**
	 * Handles JTextArea lists
	 */
	public Bindings add(String property, JTextArea textArea) {
		registerPropertyChangeListener(textArea);
		return add(new JTextAreaBinding(property, textArea));
	}

	/**
	 * Handles Optional JTextArea lists
	 */
	public Bindings add(String property, String stateProperty, 
			JToggleButton button, JTextArea textArea) {
		registerPropertyChangeListener(button);
		registerPropertyChangeListener(textArea);
		return add(new OptJTextAreaBinding(property, stateProperty, button, textArea));
	}

	/**
	 * Handles JList
	 */
	public Bindings add(String property, JList list) {
		registerPropertyChangeListener(list);
		return add(new JListBinding(property, list));
	}

	/**
	 * Handles JComboBox
	 */
	public Bindings add(String property, JComboBox combo, int defaultValue) {
		registerPropertyChangeListener(combo);
		return add(new JComboBoxBinding(property, combo, defaultValue));
	}

	/**
	 * Handles JComboBox
	 */
	public Bindings add(String property, JComboBox combo) {
		registerPropertyChangeListener(combo);
		return add(new JComboBoxBinding(property, combo, 0));
	}
}
