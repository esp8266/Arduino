
// Require mouse control library
#include <MouseController.h>

// Initialize USB Controller
USBHost usb;

// Attach mouse controller to USB
MouseController mouse(usb);

// This function intercepts mouse movements
void mouseMoved() {
  Serial1.print("Moving mouse: ");
  Serial1.print(mouseX);
  Serial1.print(", ");
  Serial1.println(mouseY);
}

// This function intercepts mouse movements when a button is pressed
void mouseDragged() {
  Serial1.print("DRAG: ");
  Serial1.print(mouseX);
  Serial1.print(", ");
  Serial1.println(mouseY);
}

// This function intercepts mouse button press
void mousePressed() {
  Serial1.print("Pressed: ");
  Serial1.println(mouseButton);
}

// This function intercepts mouse button release
void mouseReleased() {
  Serial1.print("Released: ");
  Serial1.println(mouseButton);
}

void setup()
{
  Serial1.begin(115200);
  Serial1.println("Program started");
  delay(200);
}

void loop()
{
  // Process USB tasks
  usb.Task();
}

