/*
 Copyright (c) 2012 Arduino LLC.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <KeyboardController.h>

extern "C" {
void __keyboardControllerEmptyCallback() { }
}

void keyPressed()  __attribute__ ((weak, alias("__keyboardControllerEmptyCallback")));
void keyReleased() __attribute__ ((weak, alias("__keyboardControllerEmptyCallback")));

void KeyboardController::OnKeyDown(uint8_t _mod, uint8_t _oemKey) {
	modifiers = _mod;
	keyOem = _oemKey;
	key = OemToAscii(_mod, _oemKey);
	keyPressed();
}

void KeyboardController::OnKeyUp(uint8_t _mod, uint8_t _oemKey) {
	modifiers = _mod;
	keyOem = _oemKey;
	key = OemToAscii(_mod, _oemKey);
	keyReleased();
}
