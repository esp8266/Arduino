/*
 * Calibration
 *
 * Demonstrates one techinque for calibrating sensor input.  The
 * sensor readings during the first five seconds of the sketch
 * execution define the minimum and maximum of expected values.
 */

int sensorPin = 2;
int ledPin = 9;

int val = 0;
int sensorMin = 1023, sensorMax = 0;

void setup() {
  // signal the start of the calibration period
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);
  
  // calibrate during the first five seconds 
  while (millis() < 5000) {
    val = analogRead(sensorPin);
    
    // record the maximum sensor value
    if (val > sensorMax) {
      sensorMax = val;
    }
    
    // record the minimum sensor value
    if (val < sensorMin) {
      sensorMin = val;
    }
  }
  
  // signal the end of the calibration period
  digitalWrite(13, LOW);
}

void loop() {
  val = analogRead(sensorPin);
  
  // apply the calibration to the sensor reading
  val = map(val, sensorMin, sensorMax, 0, 255);
  
  // in case the sensor value is outside the range seen during calibration
  val = constrain(val, 0, 255);
  
  analogWrite(ledPin, val);
}
