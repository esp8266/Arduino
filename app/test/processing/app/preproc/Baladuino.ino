/*
 * The code is released under the GNU General Public License.
 * Developed by Kristian Lauszus, TKJ Electronics 2013
 * This is the algorithm for the Balanduino balancing robot.
 * It can be controlled by either an Android app or a Processing application via Bluetooth.
 * The Android app can be found at the following link: https://github.com/TKJElectronics/BalanduinoAndroidApp
 * The Processing application can be found here: https://github.com/TKJElectronics/BalanduinoProcessingApp
 * It can also be controlled by a PS3, Wii or a Xbox controller
 * For details, see: http://balanduino.net/
 */

/* Use this to enable and disable the different options */
#define ENABLE_TOOLS
#define ENABLE_SPP
#define ENABLE_PS3
#define ENABLE_WII
#define ENABLE_XBOX
#define ENABLE_ADK

#include "Balanduino.h"
#include <Wire.h> // Official Arduino Wire library
#include <usbhub.h> // Some dongles can have a hub inside

#ifdef ENABLE_ADK
#include <adk.h>
#endif

// These are all open source libraries written by Kristian Lauszus, TKJ Electronics
// The USB libraries are located at the following link: https://github.com/felis/USB_Host_Shield_2.0
#include <Kalman.h> // Kalman filter library - see: http://blog.tkjelectronics.dk/2012/09/a-practical-approach-to-kalman-filter-and-how-to-implement-it/

#ifdef ENABLE_XBOX
#include <XBOXRECV.h>
#endif
#ifdef ENABLE_SPP
#include <SPP.h>
#endif
#ifdef ENABLE_PS3
#include <PS3BT.h>
#endif
#ifdef ENABLE_WII
#include <Wii.h>
#endif

// Create the Kalman library instance
Kalman kalman; // See https://github.com/TKJElectronics/KalmanFilter for source code

#if defined(ENABLE_SPP) || defined(ENABLE_PS3) || defined(ENABLE_WII) || defined(ENABLE_XBOX) || defined(ENABLE_ADK)
#define ENABLE_USB
USB Usb; // This will take care of all USB communication
#endif

#ifdef ENABLE_ADK
// Implementation for the Android Open Accessory Protocol. Simply connect your phone to get redirected to the Play Store
ADK adk(&Usb, "TKJ Electronics", // Manufacturer Name
              "Balanduino", // Model Name
              "Android App for Balanduino", // Description - user visible string
              "0.5.0", // Version of the Android app
              "https://play.google.com/store/apps/details?id=com.tkjelectronics.balanduino", // URL - web page to visit if no installed apps support the accessory
              "1234"); // Serial Number - this is not used
#endif

#ifdef ENABLE_XBOX
XBOXRECV Xbox(&Usb); // You have to connect a Xbox wireless receiver to the Arduino to control it with a wireless Xbox controller
#endif

#if defined(ENABLE_SPP) || defined(ENABLE_PS3) || defined(ENABLE_WII)
USBHub Hub(&Usb); // Some dongles have a hub inside
BTD Btd(&Usb); // This is the main Bluetooth library, it will take care of all the USB and HCI communication with the Bluetooth dongle
#endif

#ifdef ENABLE_SPP
SPP SerialBT(&Btd, "Balanduino", "0000"); // The SPP (Serial Port Protocol) emulates a virtual Serial port, which is supported by most computers and mobile phones
#endif

#ifdef ENABLE_PS3
PS3BT PS3(&Btd); // The PS3 library supports all three official controllers: the Dualshock 3, Navigation and Move controller
#endif

#ifdef ENABLE_WII
WII Wii(&Btd); // The Wii library can communicate with Wiimotes and the Nunchuck and Motion Plus extension and finally the Wii U Pro Controller
//WII Wii(&Btd,PAIR); // You will have to pair with your Wiimote first by creating the instance like this and the press 1+2 on the Wiimote
// or press sync if you are using a Wii U Pro Controller
// Or you can simply send "CW;" to the robot to start the pairing sequence
// This can also be done using the Android or Processing application
#endif

void setup() {
  /* Initialize UART */
  Serial.begin(115200);

  /* Read the PID values, target angle and other saved values in the EEPROM */
  if (!checkInitializationFlags())
    readEEPROMValues(); // Only read the EEPROM values if they have not been restored

  /* Setup encoders */
  pinMode(leftEncoder1, INPUT);
  pinMode(leftEncoder2, INPUT);
  pinMode(rightEncoder1, INPUT);
  pinMode(rightEncoder2, INPUT);
  attachInterrupt(0, leftEncoder, CHANGE);
  attachInterrupt(1, rightEncoder, CHANGE);

  /* Enable the motor drivers */
  pinMode(leftEnable, OUTPUT);
  pinMode(rightEnable, OUTPUT);
  digitalWrite(leftEnable, HIGH);
  digitalWrite(rightEnable, HIGH);

  /* Setup motor pins to output */
  sbi(pwmPortDirection, leftPWM);
  sbi(leftPortDirection, leftA);
  sbi(leftPortDirection, leftB);
  sbi(pwmPortDirection, rightPWM);
  sbi(rightPortDirection, rightA);
  sbi(rightPortDirection, rightB);

  /* Set PWM frequency to 20kHz - see the datasheet http://www.atmel.com/Images/doc8272.pdf page 128-135 */
  // Set up PWM, Phase and Frequency Correct on pin 18 (OC1A) & pin 17 (OC1B) with ICR1 as TOP using Timer1
  TCCR1B = _BV(WGM13) | _BV(CS10); // Set PWM Phase and Frequency Correct with ICR1 as TOP and no prescaling
  ICR1 = PWMVALUE; // ICR1 is the TOP value - this is set so the frequency is equal to 20kHz

  /* Enable PWM on pin 18 (OC1A) & pin 17 (OC1B) */
  // Clear OC1A/OC1B on compare match when up-counting
  // Set OC1A/OC1B on compare match when downcounting
  TCCR1A = _BV(COM1A1) | _BV(COM1B1);
  setPWM(leftPWM, 0); // Turn off PWM on both pins
  setPWM(rightPWM, 0);

  /* Setup buzzer pin */
  pinMode(buzzer, OUTPUT);

#ifdef ENABLE_USB
  if (Usb.Init() == -1) { // Check if USB Host is working
    Serial.print(F("OSC did not start"));
    digitalWrite(buzzer, HIGH);
    while (1); // Halt
  }
#endif

  /* Attach onInit function */
  // This is used to set the LEDs according to the voltage level and vibrate the controller to indicate the new connection
#ifdef ENABLE_PS3
  PS3.attachOnInit(onInit);
#endif
#ifdef ENABLE_WII
  Wii.attachOnInit(onInit);
#endif
#ifdef ENABLE_XBOX
  Xbox.attachOnInit(onInit);
#endif

  /* Setup IMU */
  Wire.begin();

  while (i2cRead(0x75, i2cBuffer, 1));
  if (i2cBuffer[0] != 0x68) { // Read "WHO_AM_I" register
    Serial.print(F("Error reading sensor"));
    digitalWrite(buzzer, HIGH);
    while (1); // Halt
  }

  i2cBuffer[0] = 19; // Set the sample rate to 400Hz - 8kHz/(19+1) = 400Hz
  i2cBuffer[1] = 0x00; // Disable FSYNC and set 260 Hz Acc filtering, 256 Hz Gyro filtering, 8 KHz sampling
  i2cBuffer[2] = 0x00; // Set Gyro Full Scale Range to ±250deg/s
  i2cBuffer[3] = 0x00; // Set Accelerometer Full Scale Range to ±2g
  while (i2cWrite(0x19, i2cBuffer, 4, false)); // Write to all four registers at once
  while (i2cWrite(0x6B, 0x09, true)); // PLL with X axis gyroscope reference, disable temperature sensor and disable sleep mode

  delay(100); // Wait for the sensor to get ready

  /* Set Kalman and gyro starting angle */
  while (i2cRead(0x3D, i2cBuffer, 4));
  accY = ((i2cBuffer[0] << 8) | i2cBuffer[1]);
  accZ = ((i2cBuffer[2] << 8) | i2cBuffer[3]);
  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // We then convert it to 0 to 2π and then from radians to degrees
  accAngle = (atan2((double)accY - cfg.accYzero, (double)accZ - cfg.accZzero) + PI) * RAD_TO_DEG;

  kalman.setAngle(accAngle); // Set starting angle
  pitch = accAngle;
  gyroAngle = accAngle;

  /* Find gyro zero value */
  calibrateGyro();

  pinMode(LED_BUILTIN, OUTPUT); // LED_BUILTIN is defined in pins_arduino.h in the hardware add-on

  /* Beep to indicate that it is now ready */
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);

  /* Setup timing */
  kalmanTimer = micros();
  pidTimer = kalmanTimer;
  encoderTimer = kalmanTimer;
  imuTimer = millis();
  reportTimer = imuTimer;
  ledTimer = imuTimer;
  blinkTimer = imuTimer;
}

void loop() {
#ifdef ENABLE_WII
  if (Wii.wiimoteConnected) // We have to read much more often from the Wiimote to decrease latency
    Usb.Task();
#endif

  /* Calculate pitch */
  while (i2cRead(0x3D, i2cBuffer, 8));
  accY = ((i2cBuffer[0] << 8) | i2cBuffer[1]);
  accZ = ((i2cBuffer[2] << 8) | i2cBuffer[3]);
  gyroX = ((i2cBuffer[6] << 8) | i2cBuffer[7]);

  // atan2 outputs the value of -π to π (radians) - see http://en.wikipedia.org/wiki/Atan2
  // We then convert it to 0 to 2π and then from radians to degrees
  accAngle = (atan2((double)accY - cfg.accYzero, (double)accZ - cfg.accZzero) + PI) * RAD_TO_DEG;

  uint32_t timer = micros();
  // This fixes the 0-360 transition problem when the accelerometer angle jumps between 0 and 360 degrees
  if ((accAngle < 90 && pitch > 270) || (accAngle > 270 && pitch < 90)) {
    kalman.setAngle(accAngle);
    pitch = accAngle;
    gyroAngle = accAngle;
  } else {
    gyroRate = ((double)gyroX - gyroXzero) / 131.0; // Convert to deg/s
    double dt = (double)(timer - kalmanTimer) / 1000000.0;
    gyroAngle += gyroRate * dt; // Gyro angle is only used for debugging
    if (gyroAngle < 0 || gyroAngle > 360)
      gyroAngle = pitch; // Reset the gyro angle when it has drifted too much
    pitch = kalman.getAngle(accAngle, gyroRate, dt); // Calculate the angle using a Kalman filter
  }
  kalmanTimer = timer;
  //Serial.print(accAngle);Serial.print('\t');Serial.print(gyroAngle);Serial.print('\t');Serial.println(pitch);

#ifdef ENABLE_WII
  if (Wii.wiimoteConnected) // We have to read much more often from the Wiimote to decrease latency
    Usb.Task();
#endif

  /* Drive motors */
  timer = micros();
  // If the robot is laying down, it has to be put in a vertical position before it starts balancing
  // If it's already balancing it has to be ±45 degrees before it stops trying to balance
  if ((layingDown && (pitch < cfg.targetAngle - 10 || pitch > cfg.targetAngle + 10)) || (!layingDown && (pitch < cfg.targetAngle - 45 || pitch > cfg.targetAngle + 45))) {
    layingDown = true; // The robot is in a unsolvable position, so turn off both motors and wait until it's vertical again
    stopAndReset();
  } else {
    layingDown = false; // It's no longer laying down
    updatePID(cfg.targetAngle, targetOffset, turningOffset, (double)(timer - pidTimer) / 1000000.0);
  }
  pidTimer = timer;

  /* Update encoders */
  timer = micros();
  if (timer - encoderTimer >= 100000) { // Update encoder values every 100ms
    encoderTimer = timer;
    int32_t wheelPosition = getWheelsPosition();
    wheelVelocity = wheelPosition - lastWheelPosition;
    lastWheelPosition = wheelPosition;
    //Serial.print(wheelPosition);Serial.print('\t');Serial.print(targetPosition);Serial.print('\t');Serial.println(wheelVelocity);
    if (abs(wheelVelocity) <= 40 && !stopped) { // Set new targetPosition if braking
      targetPosition = wheelPosition;
      stopped = true;
    }

    batteryCounter++;
    if (batteryCounter > 10) { // Measure battery every 1s
      batteryCounter = 0;
      batteryVoltage = (double)analogRead(VBAT) / 63.050847458; // VBAT is connected to analog input 5 which is not broken out. This is then connected to a 47k-12k voltage divider - 1023.0/(3.3/(12.0/(12.0+47.0))) = 63.050847458
      if (batteryVoltage < 10.2 && batteryVoltage > 5) // Equal to 3.4V per cell - don't turn on if it's below 5V, this means that no battery is connected
        digitalWrite(buzzer, HIGH);
      else
        digitalWrite(buzzer, LOW);
    }
  }

  /* Read the Bluetooth dongle and send PID and IMU values */
#ifdef ENABLE_USB
  readUsb();
#endif
#ifdef ENABLE_TOOLS
  checkSerialData();
#endif
#if defined(ENABLE_TOOLS) || defined(ENABLE_SPP)
  printValues();
#endif

#if defined(ENABLE_SPP) || defined(ENABLE_PS3) || defined(ENABLE_WII)
  if (Btd.isReady()) {
    timer = millis();
    if ((Btd.watingForConnection && timer - blinkTimer > 1000) || (!Btd.watingForConnection && timer - blinkTimer > 100)) {
      blinkTimer = timer;
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState); // Used to blink the built in LED, starts blinking faster upon an incoming Bluetooth request
    }
  } else if (ledState) { // The LED is on
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState); // This will turn it off
  }
#endif
}