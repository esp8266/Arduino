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
import java.io.File;

import javax.swing.JFileChooser;
import javax.swing.JTextField;

import net.sf.launch4j.FileChooserFilter;

/**
 * @author Copyright (C) 2006 Grzegorz Kowal
 */
public class BrowseActionListener implements ActionListener {
	private final boolean _save;
	private final JFileChooser _fileChooser;
	private final FileChooserFilter _filter;
	private final JTextField _field;

	public BrowseActionListener(boolean save, JFileChooser fileChooser,
			FileChooserFilter filter, JTextField field) {
		_save = save;
		_fileChooser = fileChooser;
		_filter = filter;
		_field = field;
	}

	public void actionPerformed(ActionEvent e) {
		if (!_field.isEnabled()) {
			return;
		}
		_fileChooser.setFileFilter(_filter);
		_fileChooser.setSelectedFile(new File(""));
		int result = _save
				? _fileChooser.showSaveDialog(MainFrame.getInstance())
				: _fileChooser.showOpenDialog(MainFrame.getInstance());
		if (result == JFileChooser.APPROVE_OPTION) {
			_field.setText(_fileChooser.getSelectedFile().getPath());
		}
		_fileChooser.removeChoosableFileFilter(_filter);
	}
}
