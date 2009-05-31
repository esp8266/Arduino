/* -*- mode: jde; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
  Tool - interface implementation for the Processing tools menu
  Part of the Processing project - http://processing.org

  Copyright (c) 2008 Ben Fry and Casey Reas

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
*/

package processing.app.tools;

import processing.app.Editor;


/**
 * Interface for items to be shown in the Tools menu.
 */
public interface Tool extends Runnable {

  public void init(Editor editor);
  
  public void run();

  // Not doing shortcuts for now, no way to resolve between tools.
  // Also would need additional modifiers for shift and alt.
  //public char getShortcutKey();

  public String getMenuTitle();
}

