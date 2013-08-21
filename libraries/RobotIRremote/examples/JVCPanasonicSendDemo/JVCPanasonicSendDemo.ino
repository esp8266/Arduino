/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 * JVC and Panasonic protocol added by Kristian Lauszus (Thanks to zenwheel and other people at the original blog post)
 */
#include <IRremote.h>
 
#define PanasonicAddress      0x4004     // Panasonic address (Pre data) 
#define PanasonicPower        0x100BCBD  // Panasonic Power button

#define JVCPower              0xC5E8

IRsend irsend;

void setup()
{
}

void loop() {
  irsend.sendPanasonic(PanasonicAddress,PanasonicPower); // This should turn your TV on and off
  
  irsend.sendJVC(JVCPower, 16,0); // hex value, 16 bits, no repeat
  delayMicroseconds(50); // see http://www.sbprojects.com/knowledge/ir/jvc.php for information
  irsend.sendJVC(JVCPower, 16,1); // hex value, 16 bits, repeat
  delayMicroseconds(50);
}
