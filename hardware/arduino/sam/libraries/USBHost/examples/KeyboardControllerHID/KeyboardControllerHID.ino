#include <hidboot.h>

class KbdRptParser : 
public KeyboardReportParser
{
  void PrintKey(uint8_t mod, uint8_t key);

protected:
  virtual void OnKeyDown(uint8_t mod, uint8_t key);
  virtual void OnKeyUp(uint8_t mod, uint8_t key);
  virtual void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
  MODIFIERKEYS mod;

  *((uint8_t*)&mod) = m;
  Serial1.print((mod.bmLeftCtrl   == 1) ? "C" : " ");
  Serial1.print((mod.bmLeftShift  == 1) ? "S" : " ");
  Serial1.print((mod.bmLeftAlt    == 1) ? "A" : " ");
  Serial1.print((mod.bmLeftGUI    == 1) ? "G" : " ");

  Serial1.print("<");
  Serial1.print(key);
  Serial1.print(">");

  Serial1.print((mod.bmRightCtrl   == 1) ? "C" : " ");
  Serial1.print((mod.bmRightShift  == 1) ? "S" : " ");
  Serial1.print((mod.bmRightAlt    == 1) ? "A" : " ");
  Serial1.print((mod.bmRightGUI    == 1) ? "G" : " ");
}

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
  Serial1.print("DOWN ");
  PrintKey(mod, key);
  uint8_t c = OemToAscii(mod, key);

  if (c)
    OnKeyPressed(c);
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
  Serial1.print("UP ");
  PrintKey(mod, key);
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
  Serial1.print("ASCII: ");
  Serial1.println(key);
}

USBHost Usb;
HIDBoot<HID_PROTOCOL_KEYBOARD> Kbd(&Usb);
KbdRptParser Prs;

void setup()
{
  Serial1.begin(115200);
  Serial1.println("Program started!");
  delay(200);

  Kbd.SetReportParser(0, &Prs);
}

void loop()
{
  Usb.Task();
}

