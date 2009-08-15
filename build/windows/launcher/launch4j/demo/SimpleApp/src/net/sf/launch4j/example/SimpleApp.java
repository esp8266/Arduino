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

package net.sf.launch4j.example;

import java.awt.Dimension;
import java.awt.Toolkit;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;

import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.UIManager;

public class SimpleApp extends JFrame {
    public SimpleApp(String[] args) {
        super("Java Application");
        final int inset = 100;
        Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();
		setBounds (inset, inset,
				screenSize.width - inset * 2, screenSize.height - inset * 2);

		JMenu menu = new JMenu("File");
		menu.add(new JMenuItem("Open"));
		menu.add(new JMenuItem("Save"));
		JMenuBar mb = new JMenuBar();
		mb.setOpaque(true);
		mb.add(menu);
		setJMenuBar(mb);

		this.addWindowListener(new WindowAdapter() {
	    	public void windowClosing(WindowEvent e) {
				System.exit(123);
		}});
		setVisible(true);

		StringBuffer sb = new StringBuffer("Java version: ");
		sb.append(System.getProperty("java.version"));
		sb.append("\nJava home: ");
		sb.append(System.getProperty("java.home"));
		sb.append("\nCurrent dir: ");
		sb.append(System.getProperty("user.dir"));
		if (args.length > 0) {
			sb.append("\nArgs: ");
			for (int i = 0; i < args.length; i++) {
				sb.append(args[i]);
				sb.append(' ');
			}
		}
		JOptionPane.showMessageDialog(this,
				sb.toString(),
				"Info",
				JOptionPane.INFORMATION_MESSAGE);
    }

	public static void setLAF() {
		JFrame.setDefaultLookAndFeelDecorated(true);
		Toolkit.getDefaultToolkit().setDynamicLayout(true);
		System.setProperty("sun.awt.noerasebackground","true");
		try {
			UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
		} catch (Exception e) {
			System.err.println("Failed to set LookAndFeel");
		}
	}

   	public static void main(String[] args) {
   		setLAF();
		new SimpleApp(args);
	}
}
