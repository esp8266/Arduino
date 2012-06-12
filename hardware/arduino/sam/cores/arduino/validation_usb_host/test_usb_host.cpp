#include "variant.h"
#include <stdio.h>
#include <hidboot.h>

/*

#include <avr/pgmspace.h>
#include <avrpins.h>
#include <max3421e.h>
#include <usbhost.h>
#include <usb_ch9.h>
#include <Usb.h>
#include <usbhub.h>
#include <avr/pgmspace.h>
#include <address.h>
#include <hidboot.h>
#include <printhex.h>
#include <message.h>
#include <hexdump.h>
#include <parsetools.h>
*/
class MouseRptParser : public MouseReportParser
{
protected:
	virtual void OnMouseMove		(MOUSEINFO *mi);
	virtual void OnLeftButtonUp		(MOUSEINFO *mi);
	virtual void OnLeftButtonDown	(MOUSEINFO *mi);
	virtual void OnRightButtonUp	(MOUSEINFO *mi);
	virtual void OnRightButtonDown	(MOUSEINFO *mi);
	virtual void OnMiddleButtonUp	(MOUSEINFO *mi);
	virtual void OnMiddleButtonDown	(MOUSEINFO *mi);
};
void MouseRptParser::OnMouseMove(MOUSEINFO *mi)
{
    printf("Pos={%d,%d}\r\n", mi->dX, mi->dY);
};
void MouseRptParser::OnLeftButtonUp (MOUSEINFO *mi)
{
    printf("L Butt Up\r\n");
};
void MouseRptParser::OnLeftButtonDown (MOUSEINFO *mi)
{
    printf("L Butt Dn\r\n");
};
void MouseRptParser::OnRightButtonUp (MOUSEINFO *mi)
{
    printf("R Butt Up\r\n");
};
void MouseRptParser::OnRightButtonDown (MOUSEINFO *mi)
{
    printf("R Butt Dn\r\n");
};
void MouseRptParser::OnMiddleButtonUp (MOUSEINFO *mi)
{
    printf("M Butt Up\r\n");
};
void MouseRptParser::OnMiddleButtonDown (MOUSEINFO *mi)
{
    printf("M Butt Dn\r\n");
};

USBHost Usb;
HIDBoot<HID_PROTOCOL_MOUSE> HostMouse(&Usb);
MouseRptParser Prs;

void setup()
{
	cpu_irq_enable();
	printf("\r\nProgram started:\r\n");
	delay(200);

    HostMouse.SetReportParser(0,(HIDReportParser*)&Prs);
}

void loop()
{
	Usb.Task();
}
