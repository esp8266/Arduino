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

import javax.swing.JFileChooser;
import javax.swing.JRadioButton;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

import net.sf.launch4j.FileChooserFilter;
import net.sf.launch4j.binding.Bindings;
import net.sf.launch4j.config.Config;
import net.sf.launch4j.form.BasicForm;

/**
 * @author Copyright (C) 2006 Grzegorz Kowal
 */
public class BasicFormImpl extends BasicForm {

	public BasicFormImpl(Bindings bindings, JFileChooser fc) {
		bindings.add("outfile", _outfileField)
				.add("dontWrapJar", _dontWrapJarCheck)
				.add("jar", _jarField)
				.add("manifest", _manifestField)
				.add("icon", _iconField)
				.add("cmdLine", _cmdLineField)
				.add("errTitle", _errorTitleField)
				.add("downloadUrl", _downloadUrlField, Config.DOWNLOAD_URL)
				.add("supportUrl", _supportUrlField)
				.add("chdir", _chdirField)
				.add("priorityIndex", new JRadioButton[] { _normalPriorityRadio,
															_idlePriorityRadio,
															_highPriorityRadio })
				.add("customProcName", _customProcNameCheck)
				.add("stayAlive", _stayAliveCheck);

		_dontWrapJarCheck.addChangeListener(new DontWrapJarChangeListener());

		_outfileButton.addActionListener(new BrowseActionListener(true, fc,
				new FileChooserFilter("Windows executables (.exe)", ".exe"),
				_outfileField));
		_jarButton.addActionListener(new BrowseActionListener(false, fc,
				new FileChooserFilter("Jar files", ".jar"), _jarField));
		_manifestButton.addActionListener(new BrowseActionListener(false, fc,
				new FileChooserFilter("Manifest files (.manifest)", ".manifest"),
				_manifestField));
		_iconButton.addActionListener(new BrowseActionListener(false, fc,
				new FileChooserFilter("Icon files (.ico)", ".ico"), _iconField));
	}

	private class DontWrapJarChangeListener implements ChangeListener {

		public void stateChanged(ChangeEvent e) {
			boolean dontWrap = _dontWrapJarCheck.isSelected();
			if (dontWrap)  {
				_jarLabel.setIcon(loadImage("images/asterix-o.gif"));
			    _jarLabel.setText(Messages.getString("jarPath"));
			    _jarField.setToolTipText(Messages.getString("jarPathTip"));
			} else {
				_jarLabel.setIcon(loadImage("images/asterix.gif"));
				_jarLabel.setText(Messages.getString("jar"));
				_jarField.setToolTipText(Messages.getString("jarTip"));
			}
			_jarButton.setEnabled(!dontWrap);
		}
	}
}
