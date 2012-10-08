/*
 Mouse Controller HID Example
 
 Shows the output of a USB Mouse connected to the USB
 controller of an Arduino Due Board.
 
 created 8 Oct 2012
 by Cristian Maglie
 */

// Require mouse control library
#include <MouseController.h>

// Initialize USB Controller
USBHost usb;

// Attach mouse controller to USB
MouseController mouse(usb);

// This function intercepts mouse movements
void mouseMoved() {
  Serial.print("Move: ");
  Serial.print(mouse.getXChange());
  Serial.print(", ");
  Serial.println(mouse.getYChange());
}

// This function intercepts mouse movements when a button is pressed
void mouseDragged() {
  Serial.print("DRAG: ");
  Serial.print(mouse.getXChange());
  Serial.print(", ");
  Serial.println(mouse.getYChange());
}

// This function intercepts mouse button press
void mousePressed() {
  Serial.print("Pressed: ");
  if (mouse.getButton(LEFT_BUTTON))
    Serial.print("L");
  if (mouse.getButton(MIDDLE_BUTTON))
    Serial.print("M");
  if (mouse.getButton(RIGHT_BUTTON))
    Serial.print("R");
  Serial.println();
}

// This function intercepts mouse button release
void mouseReleased() {
  Serial.print("Released: ");
  if (mouse.getButton(LEFT_BUTTON))
    Serial.print("L");
  if (mouse.getButton(MIDDLE_BUTTON))
    Serial.print("M");
  if (mouse.getButton(RIGHT_BUTTON))
    Serial.print("R");
  Serial.println();
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Program started");
  delay(200);
}

void loop()
{
  // Process USB tasks
  usb.Task();
}
