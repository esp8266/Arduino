/*
  Serial Call and Response in ASCII
 Language: Wiring/Arduino
 
 This program sends an ASCII A (byte of value 65) on startup
 and repeats that until it gets some data in.
 Then it waits for a byte in the serial port, and 
 sends three ASCII-encoded, comma-separated sensor values, 
 truncated by a linefeed and carriage return, 
 whenever it gets a byte in.
 
 Thanks to Greg Shakar and Scott Fitzgerald for the improvements
 
  The circuit:
 * potentiometers attached to analog inputs 0 and 1 
 * pushbutton attached to digital I/O 2
 
 
 
 Created 26 Sept. 2005
 by Tom Igoe
 modified 26 Oct 2011
 by Tom Igoe and Scott Fitzgerald
 
 This example code is in the public domain.
 
 http://www.arduino.cc/en/Tutorial/SerialCallResponseASCII
 
 */

int firstSensor = 0;    // first analog sensor
int secondSensor = 0;   // second analog sensor
int thirdSensor = 0;    // digital sensor
int inByte = 0;         // incoming serial byte

void setup()
{
  // start serial port at 9600 bps:
  Serial.begin(9600);
  pinMode(2, INPUT);   // digital sensor is on digital pin 2
  establishContact();  // send a byte to establish contact until receiver responds 
}

void loop()
{
  // if we get a valid byte, read analog ins:
  if (Serial.available() > 0) {
    // get incoming byte:
    inByte = Serial.read();
    // read first analog input:
    firstSensor = analogRead(A0);
    // read second analog input:
    secondSensor = analogRead(A1);
    // read  switch, map it to 0 or 255L
    thirdSensor = map(digitalRead(2), 0, 1, 0, 255);  
    // send sensor values:
    Serial.print(firstSensor);
    Serial.print(",");
    Serial.print(secondSensor);
    Serial.print(",");
    Serial.println(thirdSensor);               
  }
}

void establishContact() {
  while (Serial.available() <= 0) {
    Serial.println("0,0,0");   // send an initial string
    delay(300);
  }
}


/*
Processing code to run with this example:

// This example code is in the public domain.

import processing.serial.*;     // import the Processing serial library
Serial myPort;                  // The serial port

float bgcolor;			// Background color
float fgcolor;			// Fill color
float xpos, ypos;	        // Starting position of the ball

void setup() {
  size(640,480);

  // List all the available serial ports
  println(Serial.list());

  // I know that the first port in the serial list on my mac
  // is always my  Arduino module, so I open Serial.list()[0].
  // Change the 0 to the appropriate number of the serial port
  // that your microcontroller is attached to.
  myPort = new Serial(this, Serial.list()[0], 9600);

  // read bytes into a buffer until you get a linefeed (ASCII 10):
  myPort.bufferUntil('\n');
  
  // draw with smooth edges:
  smooth();
}

void draw() {
  background(bgcolor);
  fill(fgcolor);
  // Draw the shape
  ellipse(xpos, ypos, 20, 20);
}

// serialEvent  method is run automatically by the Processing applet
// whenever the buffer reaches the  byte value set in the bufferUntil() 
// method in the setup():

void serialEvent(Serial myPort) { 
  // read the serial buffer:
  String myString = myPort.readStringUntil('\n');
  // if you got any bytes other than the linefeed:
    myString = trim(myString);
 
    // split the string at the commas
    // and convert the sections into integers:
    int sensors[] = int(split(myString, ','));

    // print out the values you got:
    for (int sensorNum = 0; sensorNum < sensors.length; sensorNum++) {
      print("Sensor " + sensorNum + ": " + sensors[sensorNum] + "\t"); 
    }
    // add a linefeed after all the sensor values are printed:
    println();
    if (sensors.length > 1) {
      xpos = map(sensors[0], 0,1023,0,width);
      ypos = map(sensors[1], 0,1023,0,height);
      fgcolor = sensors[2];
    }
    // send a byte to ask for more data:
    myPort.write("A");
  }

*/

/*

Max/MSP version 5 patch to run with this example:

----------begin_max5_patcher----------
3141.3oc2cs0iaiaE9YO+JXMxC6hNYfHonj0hhhNH.EHOzVfMKZeHyh.ZIZa
sQVxqD8LYxh8+dO7h7H6wRV1VVxIIHiFpKTG9ctSxixebynwSy9hnXL5mPeD
MZzebynQ5SoNwHa6QiWx+RXBuPeaiCyVtTjJGeq4ZRwWjlyuHKqPf3nBQdLO
AsJKWhlkmsDIWDWffmYc4yjtdY1ZYhPp6QG6YmkkJKh+pPcNL4txSuJWT.uO
tLNK8S4hPogX8I9vsfHSvpCN1ef90xmhKCWDmNu1mf3XZU8ghizijro+1ao9
iqPVo7kZxZ78pgVkgQbZ4n.qN2edyMpebaKgxTwSv65UHoE.C7bbP+C9ZYV1
JQJxY+nGoYzybmxmWILfv3XEqCMdbSHEw0UgMTpFVwtrlPJraOfTKEEE74hW
Kzk.xb6GXvGIvzHhPMxNTGO8g.MtLY+.BAej.Bo6Dcz3wTd57tASz8Ti3RfV
RAaN30rfh2QhKNcGtjjwi5QXw0iVEVbZBVHtCmkFv1JXZIRaqtGzh7IF0GpV
NwOnQXYRO.Kqq2qD8Dsqtks0XoXo4M82DKWIe9uugC.On0qaS.lmAorlcHNM
JHQwCufDek5POHJYQFBSaWNXRi.CY3.FIJLQvy6FDwzUMpgYkTHZ6OtzFgE5
vAKfhQdOJkXfCRyQ7Mf1gyggknAqvGaPdGLBOKvfCzVT7HMBLdCGvnxen34k
SyR5ZokvrjrbyYsYPLIvAG3oSiv9a3lSnfXh2gMw3NysIPzeH00x3cC5o7wc
I.Pi7mqWS.3wFN.s6.vulflCZnq5IcyRTwjkAqQa3tCWpFEhDfjQXGDl1MgP
YBP+kiMiRNF3wQK742blGNCXJpdr9H+Tl15iqYtMnj6X0meZvfJyfvciZTqD
RJgEl1.CsQCLd8.rT2bm8AyL97NdRxa+YQwprzBA59O7t2+dzCoOj9AHl5BD
GM8YHJA.IPxEhpSy1sHND6ctfC2E09bhzrHQzsH0qTMmbq34boHBghSuEIyW
mFpaN8YneShSEyDhn6PuW22ov8KKPyxxEyyVCcs1V+snurJqv7tdF9MT1L3Y
CiygvRQqK.jWSWOxSVKJ.pQtNOEdC1o.bKB9Njdb8uyjheB8KpoGTy7P.NAo
jU.2N2LHiDOFGBi4TcqL3G4HUdFvqdmtDAcx7LMQjAmWkKBRQS.sYfsYFJC4
wP+fAht+GUcbgjmKWu5NfNfN7yBEc7DO1L9MThpCtsDXzrhTKJeO5I3rhGAx
JVBOFH6hVBvFJhK42oGjg.eQgzXWz8qxUIwE7PJ.6eHLSJQ+yX4WmKx4IQZb
8W.r58yyDOjtdUj9wnNn+SnLapP8n3MJOJVVHvazxOX5IMwp6YdKLVS0ZKLW
yr+Mwa+5KrdvlZc5Kx734.pg9AfC8Q4u9iJVUnRnKd9B4aSDyfztxReDdzXP
UBj1tOIwHOkyeRIinv5U4YvynkaeJFt9BcxqnoBkbO5ew+BHC+6fnLvRAyYV
456P+OkXfdJsKohYw4BPu.D.dkzAHKxK9L597n0woYZIpJ80dYlcEujXr84Q
ztFcsGpwmf2PxMyEwowR3MAiqBMaZNH5WnUsPVqHfhJWq2OaufFoi.MrIVdp
4.1N8Kt0.ZtCHnEtPD94RaTBPJKLN1ZgCL0ABYJ7QI+MULONU6FXcZbndYTT
JDyjJaVFbe6qprjtjGos3A+5zjrvOiJVrFzChxdJ8xJ055axE.aLDUFVSMLf
AbFLVgdXrxZhF8MNAeX7QDomyIFoGgpAHe6bs6YVJhfZ.n8OyW+WQdDOku+7
lp9pE4VfwhLP+Dmn71U.xIUFZiFC1Nqb5QUdDEb9alLEY2t4Tf0v7pcvHvJv
iwkOef4j.S5lxKVN9MRD1IV23hxcRkP5.14bkDqHph.3nwQhYGSenVwsWtem
pKw3Vqyn4NiSqtHk69d4qSjeZ+79su9Ldnn1GtFl2nwyyiixRUjwVOq5zkuP
ky.i5TUJWeGo7U64gkYYIS44JFxzDwVLZPYgmFuDLPKiMTDDQS4EiWtJO1XP
Xy4DobnOVTDlmkjrUWYtxi64Jlv9dJNRtP2WufWasb0UkxppFu04aLMscmso
4SAPhU9up2zdUoesZM9NV.COwux0qW+t1zVLoyhYNuL41Uyl6UyBf+3cnm8Z
KrlbXFUZSzpxco.VG0eGJHkXgTih.K3fPp2U.jtkqmWmIsRJsA.kVGftwA7Q
NML0JtZVWJr4fmIKabCXKqVrcGia62A9kGcWwgPdbNYI1C.vGDPM58VYU7Dm
CBnzSFPc6G.8uhvW.fr57QW+7DVU3z8vXI4jwRx26Xo0FZqgR7UAT1jiI8Jp
dKZEGF9YP1Sn2fQugfdCE8F2gxUkU6u7.4ftpbtBbUo601gXmjnGyWMk1VYt
WGL6tyM0jZGzUUBJysV8Xi+FBLvjpZfGFMB9tFMnNGEZ34LnngYHVOb3b.2Y
jI2Qg+35dq92a2PFWe.UGwP9kTGUS8SMYgooY002OVTjsNOrj6VF6BZaJORT
HiS2jK8GewL2N23h3nnsyb0L2BQqx.AmM6upVw8NZ5l0V5165itwsgt8utna
bawak.E45gtIGCcSuZnakKz1S2NeCR23qK5N3Xva72fzM4pBu8bZKcyttrC1
V5165R9dOtA+1ft8GB51dxxE.YbA+QQzmfWCDH3m3RYd7z0RSHXUWcl4IYS4
I1ktYSVfaVpf8tHCWtUlnwKV2BibyKHQ2rPcNWf893dqPHyZEGPeYGXU2VMh
4ObaAq+xaqaGX00HhqMuLFtEHBa3PD0lM.bnh6gsomERn9NlcXSS6GMVer2N
kYymmbfBn6jY7jMkqRCiR5kdg5srWmdf8Z2Sptsh85LvaCSmdbeXVJ2GzhxR
wcRO.LIg0TjRt0MNShKzE3U4wVUGsl8pwDrcFZ71bXu0YQvkuXj7XnjdrNhM
5CdDyt3gr6BFr0nmczaUktb6T8HOgNti1CSLyd122FQfeSCabuLpgNXZcEb0
Ix3ObMvNgTAEXdMx7GZdOoq3818YY638SF5gMtqF1j.+VOrINe2JySY9sVlm
P9tEFJ2ihVXf0HLzO5903QbcZ0s5QcAIxNMbp1Cs.67LUloc+cVK3ELjfmsB
vbpK0oKZ8eY+hbPLwVcfJX0YH2n1IwfD1BQtd2Xq+.crasqzUVgs6avMet.Z
rdmHdWMfh96ayEBSrodPr6hpCfI9CXgBpqhIdeTrftAUfDWbSeLavACXYw8t
sjRlKj5ZUqPppvgJhJEclrB1tGBbZwWHp93iJwEY5YrecmvlYofPJ+4dY+3K
sSjeeoPlmU6zQz4eZdri9R+FMW2vjdP3uVyA5cReOXMvFhgY5pL0HasVCnCn
0feVviJU6iSWsVhltd1LUAg9nH+YUgnuLNALOHByRiJtnkIlci2XMPXrZTWU
hgGxRqyXAUUQrppbsyx1y23yv2TYB3FsShOGep5Nb6sajAN1cY7rfxqV9NxV
aGfZV5N0mvtJ2zqV1tcWxNeJSsXF1r8nAZEncZYCOkQ02opkYM8dEissiDMQ
hOvHg5cbiDpefYjfMQQaVplsZoK.XUcR4Z9nB3c1iDhaa3IziajnJbP0BLYH
SJHZV15bwcuVPsD7QRsNTC8g0TKX0sr0YRsz1PsziTduWnIZ+RSsQFjR5WZh
1FZB2uzD4JjlvsvR3NF9uzzDlbA7ybtzD8BPSkVV80t.s92zsNWpM3RPsLM0
RslasdiUsNWeX3KA0Z7CPMqW1lXGb6lXGND0pVQ5JpUWboyVQStGGMUhYlXw
bsACvN+nunrK.+lf0RmdLaRWlY9cqVzx0N1LtvrydjDb8Ynh0FQA290ftaaR
bf0uAsvZiAcuiMRTcF.ttFSOdzMsNShsMA8Ezuvm+EfkRoZaLtXOSQHEro04
ZwwoMTqe+hfr1Zy9Dj.Me.Mc8lX1CUUaQ8pht5V8gpzNC2KM5RZk5c+lSBgz
VZp+hbn0vDo2HIuVklTv0m2BlyI5tv78YxD.it04FNXaBnldjTaY5I1+aavN
MKNcjw4CkBJte42sxgAoeSUuUbUb+ZRSsTSULMTSzlGofluNJ.lcONVoUWPr
NcLwZzZYj.St6taZctwL2BZseihuMAw2ugP0lXPorSR5yC6Vwnrt045BoMRe
Xm9Mhj1j.KommMYValQxf9GmbOjjV+SRGxxK4b7F.M9ya9+PS8xmX
-----------end_max5_patcher-----------

*/
