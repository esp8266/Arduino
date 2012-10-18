/*
  Arduino Starter Kit example
 Project 3  - Love-O-Meter
 
 This sketch is written to accompany Project 3 in the
 Arduino Starter Kit
 
 Parts required:
 1 TMP36 temperature sensor 
 3 red LEDs
 3 220 ohm resistors
 
 Created 13 September 2012
 by Scott Fitzgerald
 
 http://arduino.cc/starterKit
 
 This example code is part of the public domain 
 */

// named constant for the pin the sensor is connected to
const int sensorPin = A0;
// room temperature in Celcius
const float baselineTemp = 20.0;

void setup(){
  // open a serial connection to display values
  Serial.begin(9600);
  // set the LED pins as outputs
  // the for() loop saves some extra coding
  for(int pinNumber = 2; pinNumber<5; pinNumber++){
    pinMode(pinNumber,OUTPUT);
    digitalWrite(pinNumber, LOW);
  }
}

void loop(){
  // read the value on AnalogIn pin 0 
  // and store it in a variable
  int sensorVal = analogRead(sensorPin);

  // send the 10-bit sensor value out the serial port
  Serial.print("sensor Value: ");
  Serial.print(sensorVal); 

  // convert the ADC reading to voltage
  float voltage = (sensorVal/1024.0) * 5.0;

  // Send the voltage level out the Serial port
  Serial.print(", Volts: ");
  Serial.print(voltage);

  // convert the voltage to temperature in degrees C
  // the sensor changes 10 mV per degree
  // the datasheet says there's a 500 mV offset
  // ((volatge - 500mV) times 100)
  Serial.print(", degrees C: "); 
  float temperature = (voltage - .5) * 100;
  Serial.println(temperature);

  // if the current temperature is lower than the baseline
  // turn off all LEDs
  if(temperature < baselineTemp){
    digitalWrite(2, LOW);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  } // if the temperature rises 2-4 degrees, turn an LED on 
  else if(temperature >= baselineTemp+2 && temperature < baselineTemp+4){
    digitalWrite(2, HIGH);
    digitalWrite(3, LOW);
    digitalWrite(4, LOW);
  } // if the temperature rises 4-6 degrees, turn a second LED on  
  else if(temperature >= baselineTemp+4 && temperature < baselineTemp+6){
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, LOW);
  } // if the temperature rises more than 6 degrees, turn all LEDs on
  else if(temperature >= baselineTemp+6){
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
  }
  delay(1);
}
