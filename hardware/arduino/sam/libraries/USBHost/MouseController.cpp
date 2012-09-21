/*
 Copyright (c) 2012 Arduino.  All right reserved.

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

#include <MouseController.h>

extern "C" {
void __mouseControllerEmptyCallback() { }
}

void mouseClicked()  __attribute__ ((weak, alias("__mouseControllerEmptyCallback")));
void mouseDragged()  __attribute__ ((weak, alias("__mouseControllerEmptyCallback")));
void mouseMoved()    __attribute__ ((weak, alias("__mouseControllerEmptyCallback")));
void mousePressed()  __attribute__ ((weak, alias("__mouseControllerEmptyCallback")));
void mouseReleased() __attribute__ ((weak, alias("__mouseControllerEmptyCallback")));

int mouseX = 0;
int mouseY = 0;
MouseButton mouseButton;
bool mouseButtonPressed = false;

void MouseController::OnMouseMove(MOUSEINFO *mi) {
	mouseX += mi->dX;
	mouseY += mi->dY;
	if (mouseX < 0)
		mouseX = 0;
	if (mouseX > maxX)
		mouseX = maxX;
	if (mouseY < 0)
		mouseY = 0;
	if (mouseY > maxY)
		mouseY = maxY;
	if (mouseButtonPressed)
		mouseDragged();
	else
		mouseMoved();
}

void MouseController::OnLeftButtonUp(MOUSEINFO *mi) {
	buttons--;
	mouseButtonPressed = (buttons > 0);
	mouseButton = LEFT_BUTTON;
	mouseReleased();
	mouseClicked();
}

void MouseController::OnLeftButtonDown(MOUSEINFO *mi) {
	buttons++;
	mouseButtonPressed = (buttons > 0);
	mouseButton = LEFT_BUTTON;
	mousePressed();
}

void MouseController::OnMiddleButtonUp(MOUSEINFO *mi) {
	buttons--;
	mouseButtonPressed = (buttons > 0);
	mouseButton = MIDDLE_BUTTON;
	mouseReleased();
	mouseClicked();
}

void MouseController::OnMiddleButtonDown(MOUSEINFO *mi) {
	buttons++;
	mouseButtonPressed = (buttons > 0);
	mouseButton = MIDDLE_BUTTON;
	mousePressed();
}

void MouseController::OnRightButtonUp(MOUSEINFO *mi) {
	buttons--;
	mouseButtonPressed = (buttons > 0);
	mouseButton = RIGHT_BUTTON;
	mouseReleased();
	mouseClicked();
}

void MouseController::OnRightButtonDown(MOUSEINFO *mi) {
	buttons++;
	mouseButtonPressed = (buttons > 0);
	mouseButton = RIGHT_BUTTON;
	mousePressed();
}
