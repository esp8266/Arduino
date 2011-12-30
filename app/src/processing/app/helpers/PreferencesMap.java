/*
 PreferencesMap - A Map<String, String> with some useful features 
 to handle preferences.
 Part of the Arduino project - http://www.arduino.cc/

 Copyright (c) 2011 Cristian Maglie

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software Foundation,
 Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 $Id$
 */
package processing.app.helpers;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.util.HashMap;
import java.util.Map;

import processing.core.PApplet;

public class PreferencesMap extends HashMap<String, String> {

	/**
	 * Parse a property list file and put kev/value pairs into the Map
	 * 
	 * @param file
	 * @throws FileNotFoundException
	 * @throws IOException
	 */
	public void load(File file) throws FileNotFoundException, IOException {
		load(new FileInputStream(file));
	}

	/**
	 * Parse a property list stream and put key/value pairs into the Map
	 * 
	 * @param input
	 * @throws IOException
	 */
	public void load(InputStream input) throws IOException {
		String[] lines = PApplet.loadStrings(input);
		for (String line : lines) {
			if (line.length() == 0 || line.charAt(0) == '#')
				continue;

			int equals = line.indexOf('=');
			if (equals != -1) {
				String key = line.substring(0, equals);
				String value = line.substring(equals + 1);
				put(key.trim(), value.trim());
			}
		}
	}

	public Map<String, PreferencesMap> createFirstLevelMap() {
		Map<String, PreferencesMap> res = new HashMap<String, PreferencesMap>();
		for (String key : keySet()) {
			int dot = key.indexOf('.');
			if (dot == -1)
				continue;

			String parent = key.substring(0, dot);
			String child = key.substring(dot + 1);

			if (!res.containsKey(parent))
				res.put(parent, new PreferencesMap());
			res.get(parent).put(child, get(key));
		}
		return res;
	}

	private static final long serialVersionUID = 2330591567444282843L;
}
