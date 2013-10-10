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
 * Created on 2005-05-09
 */
package net.sf.launch4j.formimpl;

import java.awt.BorderLayout;
import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.io.File;

import javax.swing.ImageIcon;
import javax.swing.JButton;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JToolBar;
import javax.swing.UIManager;

import com.jgoodies.looks.Options;
import com.jgoodies.looks.plastic.PlasticXPLookAndFeel;

import foxtrot.Task;
import foxtrot.Worker;

import net.sf.launch4j.Builder;
import net.sf.launch4j.BuilderException;
import net.sf.launch4j.ExecException;
import net.sf.launch4j.FileChooserFilter;
import net.sf.launch4j.Log;
import net.sf.launch4j.Main;
import net.sf.launch4j.Util;
import net.sf.launch4j.binding.Binding;
import net.sf.launch4j.binding.BindingException;
import net.sf.launch4j.binding.InvariantViolationException;
import net.sf.launch4j.config.Config;
import net.sf.launch4j.config.ConfigPersister;
import net.sf.launch4j.config.ConfigPersisterException;

/**
 * @author Copyright (C) 2005 Grzegorz Kowal
 */
public class MainFrame extends JFrame {
	private static MainFrame _instance;

	private final JToolBar _toolBar;
	private final JButton _runButton;
	private final ConfigFormImpl _configForm;
	private final JFileChooser _fileChooser = new FileChooser(MainFrame.class);
	private File _outfile;
	private boolean _saved = false;

	public static void createInstance() {
		try {
			Toolkit.getDefaultToolkit().setDynamicLayout(true);
			System.setProperty("sun.awt.noerasebackground","true");
	
			// JGoodies
			Options.setDefaultIconSize(new Dimension(16, 16));		// menu icons
			Options.setUseNarrowButtons(false);
			Options.setPopupDropShadowEnabled(true);

			UIManager.setLookAndFeel(new PlasticXPLookAndFeel());
			_instance = new MainFrame();
		} catch (Exception e) {
			System.err.println(e);
		}
	}

	public static MainFrame getInstance() {
		return _instance;
	}

	public MainFrame() {
		showConfigName(null);
		setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
		addWindowListener(new MainFrameListener());
		setGlassPane(new GlassPane(this));
		_fileChooser.setFileFilter(new FileChooserFilter(
				Messages.getString("MainFrame.config.files"),
				new String[] {".xml", ".cfg"}));

		_toolBar = new JToolBar();
		_toolBar.setFloatable(false);
		_toolBar.setRollover(true);
		addButton("images/new.png",	Messages.getString("MainFrame.new.config"),
				new NewActionListener());
		addButton("images/open.png", Messages.getString("MainFrame.open.config"),
				new OpenActionListener());
		addButton("images/save.png", Messages.getString("MainFrame.save.config"),
				new SaveActionListener());
		_toolBar.addSeparator();
		addButton("images/build.png", Messages.getString("MainFrame.build.wrapper"),
				new BuildActionListener());
		_runButton = addButton("images/run.png",
				Messages.getString("MainFrame.test.wrapper"),
				new RunActionListener());
		setRunEnabled(false);
		_toolBar.addSeparator();
		addButton("images/info.png", Messages.getString("MainFrame.about.launch4j"),
				new AboutActionListener());

		_configForm = new ConfigFormImpl();
		getContentPane().setLayout(new BorderLayout());
		getContentPane().add(_toolBar, BorderLayout.NORTH);
		getContentPane().add(_configForm, BorderLayout.CENTER);
		pack();
		Dimension scr = Toolkit.getDefaultToolkit().getScreenSize();
		Dimension fr = getSize();
		fr.width += 25;
		fr.height += 100;
		setBounds((scr.width - fr.width) / 2, (scr.height - fr.height) / 2,
				fr.width, fr.height);
		setVisible(true);
	}

	private JButton addButton(String iconPath, String tooltip, ActionListener l) {
		ImageIcon icon = new ImageIcon(MainFrame.class.getClassLoader()
				.getResource(iconPath));
		JButton b = new JButton(icon);
		b.setToolTipText(tooltip);
		b.addActionListener(l);
		_toolBar.add(b);
		return b;
	}
	
	public void info(String text) {
		JOptionPane.showMessageDialog(this, 
									text,
									Main.getName(),
									JOptionPane.INFORMATION_MESSAGE);
	}

	public void warn(String text) {
		JOptionPane.showMessageDialog(this, 
									text,
									Main.getName(),
									JOptionPane.WARNING_MESSAGE);
	}

	public void warn(InvariantViolationException e) {
		Binding b = e.getBinding(); 
		if (b != null) {
			b.markInvalid();
		}
		warn(e.getMessage());
		if (b != null) {
			e.getBinding().markValid();
		}
	}

	public boolean confirm(String text) {
		return JOptionPane.showConfirmDialog(MainFrame.this,
				text,
				Messages.getString("MainFrame.confirm"),
				JOptionPane.YES_NO_OPTION) == JOptionPane.YES_OPTION;
	}

	private boolean isModified() {
		return (!_configForm.isModified())
				|| confirm(Messages.getString("MainFrame.discard.changes"));
	}

	private boolean save() {
		// XXX
		try {
			_configForm.get(ConfigPersister.getInstance().getConfig());
			if (_fileChooser.showSaveDialog(MainFrame.this) == JOptionPane.YES_OPTION) {
				File f = _fileChooser.getSelectedFile();
				if (!f.getPath().endsWith(".xml")) {
					f = new File(f.getPath() + ".xml");
				}
				ConfigPersister.getInstance().save(f);
				_saved = true;
				showConfigName(f);
				return true;
			}
			return false;
		} catch (InvariantViolationException ex) {
			warn(ex);
			return false;
		} catch (BindingException ex) {
			warn(ex.getMessage());
			return false;
		} catch (ConfigPersisterException ex) {
			warn(ex.getMessage());
			return false;
		}
	}

	private void showConfigName(File config) {
		setTitle(Main.getName() + " - " 	+ (config != null ? config.getName()
						: Messages.getString("MainFrame.untitled")));
	}

	private void setRunEnabled(boolean enabled) {
		if (!enabled) {
			_outfile = null;
		}
		_runButton.setEnabled(enabled);
	}

	private void clearConfig() {
		ConfigPersister.getInstance().createBlank();
		_configForm.clear(ConfigPersister.getInstance().getConfig());
	}

	private class MainFrameListener extends WindowAdapter {
		public void windowOpened(WindowEvent e) {
			clearConfig();
		}

		public void windowClosing(WindowEvent e) {
			if (isModified()) {
				System.exit(0);
			}
		}
	}

	private class NewActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			if (isModified()) {
				clearConfig();
			}
			_saved = false;
			showConfigName(null);
			setRunEnabled(false);
		}
	}

	private class OpenActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			try {
				if (isModified() && _fileChooser.showOpenDialog(MainFrame.this)
									== JOptionPane.YES_OPTION) {
					final File f = _fileChooser.getSelectedFile(); 
					if (f.getPath().endsWith(".xml")) {
						ConfigPersister.getInstance().load(f);	
						_saved = true;
					} else {
						ConfigPersister.getInstance().loadVersion1(f);
						_saved = false;
					}
					_configForm.put(ConfigPersister.getInstance().getConfig());
					showConfigName(f);
					setRunEnabled(false);
				}
			} catch (ConfigPersisterException ex) {
				warn(ex.getMessage());
			} catch (BindingException ex) {
				warn(ex.getMessage());
			}
		}
	}
	
	private class SaveActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			save();
		}
	}
	
	private class BuildActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			final Log log = Log.getSwingLog(_configForm.getLogTextArea());
			try {
				if ((!_saved || _configForm.isModified())
						&& !save()) {
					return;
				}
				log.clear();
				ConfigPersister.getInstance().getConfig().checkInvariants();
				Builder b = new Builder(log);
				_outfile = b.build();
				setRunEnabled(ConfigPersister.getInstance().getConfig()
						.getHeaderType() == Config.GUI_HEADER	// TODO fix console app test
						&& (Util.WINDOWS_OS || !ConfigPersister.getInstance()
												.getConfig().isDontWrapJar()));
			} catch (InvariantViolationException ex) {
				setRunEnabled(false);
				ex.setBinding(_configForm.getBinding(ex.getProperty()));
				warn(ex);
			} catch (BuilderException ex) {
				setRunEnabled(false);
				log.append(ex.getMessage());
			}
		}
	}
	
	private class RunActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			try {
				getGlassPane().setVisible(true);
				Worker.post(new Task() {
					public Object run() throws ExecException {
						Log log = Log.getSwingLog(_configForm.getLogTextArea());
						log.clear();
						String path = _outfile.getPath();
						if (Util.WINDOWS_OS) {
							log.append(Messages.getString("MainFrame.executing") + path);
							Util.exec(new String[] { path }, log);
						} else {
							log.append(Messages.getString("MainFrame.jar.integrity.test")
									+ path);
							Util.exec(new String[] { "java", "-jar", path }, log);
						}
						return null;
					}
				});
			} catch (Exception ex) {
				// XXX errors logged by exec
			} finally {
				getGlassPane().setVisible(false);
			}
		};
	}

	private class AboutActionListener implements ActionListener {
		public void actionPerformed(ActionEvent e) {
			info(Main.getDescription());
		}
	}
}
