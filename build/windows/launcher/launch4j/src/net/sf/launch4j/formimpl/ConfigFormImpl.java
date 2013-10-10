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
 * Created on May 10, 2005
 */
package net.sf.launch4j.formimpl;

import javax.swing.BorderFactory;
import javax.swing.JFileChooser;
import javax.swing.JTextArea;

import net.sf.launch4j.binding.Binding;
import net.sf.launch4j.binding.Bindings;
import net.sf.launch4j.binding.IValidatable;
import net.sf.launch4j.form.ConfigForm;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class ConfigFormImpl extends ConfigForm {
	private final Bindings _bindings = new Bindings();
	private final JFileChooser _fileChooser = new FileChooser(ConfigFormImpl.class);

	public ConfigFormImpl() {
		_tab.setBorder(BorderFactory.createMatteBorder(0, -1, -1, -1, getBackground()));
		_tab.addTab(Messages.getString("tab.basic"),
				new BasicFormImpl(_bindings, _fileChooser));
		_tab.addTab(Messages.getString("tab.classpath"),
				new ClassPathFormImpl(_bindings, _fileChooser));
		_tab.addTab(Messages.getString("tab.header"),
				new HeaderFormImpl(_bindings));
		_tab.addTab(Messages.getString("tab.singleInstance"),
				new SingleInstanceFormImpl(_bindings));
		_tab.addTab(Messages.getString("tab.jre"),
				new JreFormImpl(_bindings, _fileChooser));
		_tab.addTab(Messages.getString("tab.envVars"),
				new EnvironmentVarsFormImpl(_bindings));
		_tab.addTab(Messages.getString("tab.splash"),
				new SplashFormImpl(_bindings, _fileChooser));
		_tab.addTab(Messages.getString("tab.version"),
				new VersionInfoFormImpl(_bindings, _fileChooser));
		_tab.addTab(Messages.getString("tab.messages"),
				new MessagesFormImpl(_bindings));
	}

	public void clear(IValidatable bean) {
		_bindings.clear(bean);
	}

	public void put(IValidatable bean) {
		_bindings.put(bean);
	}

	public void get(IValidatable bean) {
		_bindings.get(bean);
	}
	
	public boolean isModified() {
		return _bindings.isModified();
	}
	
	public JTextArea getLogTextArea() {
		return _logTextArea;
	}
	
	public Binding getBinding(String property) {
		return _bindings.getBinding(property);
	}
}
