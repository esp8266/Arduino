// This sketch demonstrate how to use low-level drivers (only for advanced users).

#include <hidboot.h>

class MouseRptParser : public MouseReportParser
{
protected:
  virtual void OnMouseMove(MOUSEINFO *mi);
  virtual void OnLeftButtonUp(MOUSEINFO *mi);
  virtual void OnLeftButtonDown(MOUSEINFO *mi);
  virtual void OnRightButtonUp(MOUSEINFO *mi);
  virtual void OnRightButtonDown(MOUSEINFO *mi);
  virtual void OnMiddleButtonUp(MOUSEINFO *mi);
  virtual void OnMiddleButtonDown(MOUSEINFO *mi);
};

void MouseRptParser::OnMouseMove(MOUSEINFO *mi)
{
  Serial1.print("Moving { ");
  Serial1.print(mi->dX);
  Serial1.print(", ");
  Serial1.print(mi->dY);
  Serial1.println(" }");
};

void MouseRptParser::OnLeftButtonUp(MOUSEINFO *mi)
{
  Serial1.println("Left Button UP");
};

void MouseRptParser::OnLeftButtonDown(MOUSEINFO *mi)
{
  Serial1.println("Left Button DOWN");
};

void MouseRptParser::OnRightButtonUp(MOUSEINFO *mi)
{
  Serial1.println("Right Button UP");
};

void MouseRptParser::OnRightButtonDown(MOUSEINFO *mi)
{
  Serial1.println("Right Button DOWN");
};

void MouseRptParser::OnMiddleButtonUp(MOUSEINFO *mi)
{
  Serial1.println("Middle Button UP");
};

void MouseRptParser::OnMiddleButtonDown(MOUSEINFO *mi)
{
  Serial1.println("Middle Button DOWN");
};

USBHost Usb;
HIDBoot<HID_PROTOCOL_MOUSE> HostMouse(&Usb);
MouseRptParser Prs;

void setup()
{
  Serial1.begin(115200);
  Serial1.println("Program started:");
  delay(200);

  HostMouse.SetReportParser(0, &Prs);
}

void loop()
{
  Usb.Task();
}
