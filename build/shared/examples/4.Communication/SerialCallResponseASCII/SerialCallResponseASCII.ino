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
 modified 30 Aug 2011
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
    // read first analog input, divide by 4 to make the range 0-255:
    firstSensor = analogRead(A0)/4;
    // delay 10ms to let the ADC recover:
    delay(10);
    // read second analog input, divide by 4 to make the range 0-255:
    secondSensor = analogRead(A1)/4;
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
2726.3oc2bkziaiiE9bU+J3XjCciwo.WzZeqPCL.4vLG5zXlCIAAzRz1bhrj
aI5pRkF8+89QRIWR1VxxqQNoPrL0B0G+dK7QxG8ed+cilj8UQwHzuf9.5t69
y6u6Nyozm3txx2MZA+qQI7BysMJU7b1j++nw1KoDeUYN8z7rEEurXRVR0kxV
oRDJ0KKE1pezHzmJuzRtJZtLc1myEQJ6UodtOfGibCLG7czeRwOfW+LxXy6A
d2u0u5ULMKUkxWXdAidLWxW+xixRxxs0LQWU3GBBwjPO82vkeiTq1SWsPlBv
0zFI0p9B42LUOgBUy56011r2r9j+082q+X7owgRUFuU1Slp5EA5oAJxwqKBL
3BSfruSD32RPyxyVsrcUvw8TMjEZOzEK5reVrAqPODVgd9XkBQBz9PDLhvZk
YlvSmoYmpicyRXK8fMpF9tcwRD7ARSGj0G6HnoEhhB9LwV7jm6w45h4Z7V4D
XXCF8AWvzHX2rQ3kTmgbV0YPj8przKZw03Ph0oCIugloTT1hEBvi6l7x6EZL
g9UdRxa+MQwxrzBA5w2+qu6cnOl9wz2KRiKPbzjWTBD.AjZt.UXelkY4pwHd
ZLJWvg6hU9bhzrXQ7Xj9UxgadIOmqDwHjLcLRkuJMxTbxKP8lHSESEh3GPuy
T2ov8qJPSyxEyxVAUsws8XzWWlUXeWu.eCkMEd1HYdTh.sp.DSFb8DOYkn.P
iZUdJ7FzcguIfe.YZW+mLk3WP+9bYAxHoQ.OsBrifamaajwhmjQPaN0TJC9H
GZYw5W8FUIBpjYYFPjAmGtGnUCEArYosoVjg7bQ+jkhd7m0UbghmqVs7A.GP
E9EgFGOyk11uEI5JXbEwXDEokr7inmgyJdBfkTAOFn2fV.zFJlq3OXZjQfbQ
yzDGziKyAcUb3GSAZ+8QYJE5eIUealHmmDa30eG3p2MKasWDsjIBDAJqpX6l
ENVmld9FOnNX8AhOc21EtWRem3yncgJWNCXGzOARhOn9zOqEIQZkK4r4p2lH
lp.UyzmfGUBlLfV0iIIV8lb9yZcAMmtLOCdFi94yR35y4KWBRxIBs9M5ey+J
nq9GfJKH5.2Vk5uOf9eZwsRqaVghoxbAn+CB5szB.cNdwWPOlGuRllYzbpUW
6TZx5niPqONOpoKPmxCs3626lQZlKjoRE.K3kVXDSy.KiBiIDpzaAXPxM12S
2Io0gE.wFiOydfvrkbZgzbtUHsn4hnuT4KR.ZYQRYomLvkFnjo4Gs92DwLYp
wc+pTI3bGrHzFDSUZeSVdu4U0dLWviMd1fuNIIK5Knh4q.6f3rmSOXsVGaDa
LeiyGZU3KsH.XCMAPKgrrD8wQZuIF121Y2GGcjCFkYhYw2NX.pmIZWRXKDDc
mDz+UjGyS4i583ivsEUWcbJxKIlRlApCYhtWsBPOo1ce2nWaMV4an0SksCGm
fZAhA78LsJkvzlvUmLVL8PpiLvU8q2O1NlwZez7NkoKAWzfYjQAey2KeUh5y
6lbZd8o7HQqObKhh6FMKWFmkpgQimUe5pWn10t03nNxM2QJe4NdXUVVxDd9S
xB4jDQCIAnMySkK.OnJoEQPnEUWTtXYtzZwt9bhTNTGyKhxyRRZTU1q7zNth
M9qmkwp4l55U9pwL7TSyogcViy243k1bZelMAHI2p+W+lZ2lq0gLXcLQbMJM
gAlB07Ks0Hv8q+9Z0+TqieXaiPxCtgtj.+lO3liw5tJmV1uL9RQrX8emFkRK
oTq5ra3doTuaZJsQeCaOjVsVZ2DZqyCRq5rXaH71Cd1g4R5ffcakf2vOUa8r
1QuqsMCIWd1cIGhIAeNzXsF+kJrj.7dIT1QSnNGCgdkTW+mHR2DY8IDt8Ipq
txoy94R5Qykzez4xRGo8lJI2tTYWcLAgUyyGiVxANKCFdC5MDzann2vPuw4r
X3Wcft2tpv2zcUYvyIqU55qmt4R0wsCYcy4SJnURoCMqZWoZvu5JbzfiLHz5
Fm6mMB+glMX3ChM7vCT1v95Fsmd5nAOvf+43L17890jI8JVqMlxhCnI+5PG0
yMSKiByzZzWe2bQQ1p7nJ4d0ndPMaSwhBkLc8Xo+vqMuMtw4x33lib0P2x3k
YfJUI.QepWRuCF2d2n3Feahau9ha8PRFP3V6Vte3ldihaXDKHxvA2A8E2CK8
69ia1vxeB8PvMa3faxgfa5vA2taYt0Bt8GV5It8ku8FV3l0WbiOi3t7jPCvt
bIE7mDweFdMPHhelqT4xIqT1Pvpu7Im9pIz4E22hYLKIaBOobocVWKMp6sP0
l008uxDmmURCezIgHyFyMKj8ZpP0VN+35eijKT+i21QpPsOFwobPXtjdvHt2
HLhNa..Opjia0UKojxke1syT800YnQIprYyRDmr9fclJd8yc13Yct.6WZwK9
HW7baxg5zKwK9VJeHwmVBQVo2acN5zctEGLzxHxjn3Va9IxBkt4WcTaDLte4
XQ.obVZ7VeXW7AK7.LEbNexckNKDS5zZumIKsG0llMzMW3fFMS2CNWRHeuRE
1m3Iq8OsqIl1l779kQD32UylbYa0GURFsZwDQ99D7F69Ns4Cn0XAWuNE92Tx
dZGx9xDgrex9fgmvuilMoilMMzu2MaJ9GVcdlqeu04ozgmR+YhFpRhvRZvsS
ZX.Z62ROhqRqmpGH793oVOzCtyKDWKALak7Burjm6YeqXg6wdqoe6wFZoSFW
aHFcERIavsQrZMSpSjfF1bQtIcoiRxJDatIR5vKbYRDxvk63nN23QTualzKu
Aony+zCfSJG5AsLap1Cm3Oz3j11wdFUiibS6YsbJ0RXakWjMHDxPaTpsQHl8
WE+HYDmvZ5HNjtXDxfaeL1lYyu1vrYlPY1EcEJ8dxnlsSQmQyVeeRPw9cZ7L
zrcNw4qh53X2gZVNfV84N0JHeelup+XgPkms24moGMypNR6dGMSuYbGX1ZAG
m2fxXtrJ81cuaqdCX2LyhmwfmiB8v3SaRcr5KLlwrHnyI4jbQ2Bamyg0+aBd
bkWQY5xUJzjUSmp2IuOIxeQ+KHvBYB38TDkkFWbn66uxrxpz+IA2019ibyrW
Iscvn2Jy5smbWyNfusKUe61ZgzY2HjqVLXl2dyzSyRGatrukjxVK2qd3WutZ
+srTuzF47v1Ky6tWh2sDQGDlb1ClXWUHwZjEBsQSgWeZBuOLcc4IWbOvDAeU
wjeOfDy8vfD02QuVvdjx.OBVW5DAaPO.q+Uk9b5AhBtpHhzGkLmCTfZEgtzP
yZ7aEulRmCvROyfsDdkKGUsRmJXo8w7045JsI8ASW2dnHrK.Ow7Cr5dtlCtr
0kNUzFdIPqqAsLLqFZMkN0t0HWBzBiARiOpWczpKcpAFzGeQazjt3Aqf6QvJ
jvgmUL6.CLnxFobZ.sxXSoSEs8oSO2lz7EOzJVewzAvfNXS+cN1ILrrSPSoq
BC5bXLXkcqcIJcbbVW5DQqWe52iccsX5i31fa50aDGz9hoqmYAs27DdfYp5d
cwjaeMHOB2G1ewWc7Br4NX8RL6OpBk2ooz0nKW2q6fjb6yfLcYGZPKFGbNj5
Lnoz4X1LN2gXUMHX2xYa1lC.MJwpRWPqabh6o63tGMjvgmsu2Q1KsMHVO15R
mHXCGdC2yI3BXIcpxz9DLiyoLIHPg+59+Fv1JXFJ
-----------end_max5_patcher-----------
*/
