#include "variant.h"
#include <stdio.h>
#include <hidboot.h>

class KbdRptParser : public KeyboardReportParser
{
	void PrintKey(uint8_t mod, uint8_t key);

protected:
	virtual void OnKeyDown	(uint8_t mod, uint8_t key);
	virtual void OnKeyUp	(uint8_t mod, uint8_t key);
	virtual void OnKeyPressed(uint8_t key);
};

void KbdRptParser::PrintKey(uint8_t m, uint8_t key)
{
    MODIFIERKEYS mod;

    *((uint8_t*)&mod) = m;
    printf((mod.bmLeftCtrl   == 1) ? "C" : " ");
    printf((mod.bmLeftShift  == 1) ? "S" : " ");
    printf((mod.bmLeftAlt    == 1) ? "A" : " ");
    printf((mod.bmLeftGUI    == 1) ? "G" : " ");

    printf("<%c>", key);

    printf((mod.bmRightCtrl   == 1) ? "C" : " ");
    printf((mod.bmRightShift  == 1) ? "S" : " ");
    printf((mod.bmRightAlt    == 1) ? "A" : " ");
    printf((mod.bmRightGUI    == 1) ? "G" : " ");
};

void KbdRptParser::OnKeyDown(uint8_t mod, uint8_t key)
{
    printf("DN ");
    PrintKey(mod, key);
    uint8_t c = OemToAscii(mod, key);

    if (c)
        OnKeyPressed(c);
}

void KbdRptParser::OnKeyUp(uint8_t mod, uint8_t key)
{
    printf("UP ");
    PrintKey(mod, key);
}

void KbdRptParser::OnKeyPressed(uint8_t key)
{
    printf("ASCII: %c", key);
};

USBHost Usb;
HIDBoot<HID_PROTOCOL_KEYBOARD> Kbd(&Usb);
KbdRptParser Prs;

void setup()
{
	cpu_irq_enable();
	printf("\r\nProgram started:\r\n");
	delay(200);

    Kbd.SetReportParser(0, (HIDReportParser*)&Prs);
}

void loop()
{
	Usb.Task();
}
