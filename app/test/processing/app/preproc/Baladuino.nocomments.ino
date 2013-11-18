  
                                                             
                                                      
                                                            
                                                                                           
                                                                                                             
                                                                                                          
                                                               
                                           
   

                                                          
#define ENABLE_TOOLS
#define ENABLE_SPP
#define ENABLE_PS3
#define ENABLE_WII
#define ENABLE_XBOX
#define ENABLE_ADK

#include "Balanduino.h"
#include <Wire.h>                                 
#include <usbhub.h>                                      

#ifdef ENABLE_ADK
#include <adk.h>
#endif

                                                                                   
                                                                                                    
#include <Kalman.h>                                                                                                                                     

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

                                     
Kalman kalman;                                                                      

#if defined(ENABLE_SPP) || defined(ENABLE_PS3) || defined(ENABLE_WII) || defined(ENABLE_XBOX) || defined(ENABLE_ADK)
#define ENABLE_USB
USB Usb;                                                
#endif

#ifdef ENABLE_ADK
                                                                                                                        
ADK adk(&Usb, "TKJ Electronics",                     
              "Balanduino",              
              "Android App for Balanduino",                                     
              "0.5.0",                              
              "https://play.google.com/store/apps/details?id=com.tkjelectronics.balanduino",                                                                      
              "1234");                                    
#endif

#ifdef ENABLE_XBOX
XBOXRECV Xbox(&Usb);                                                                                                             
#endif

#if defined(ENABLE_SPP) || defined(ENABLE_PS3) || defined(ENABLE_WII)
USBHub Hub(&Usb);                                  
BTD Btd(&Usb);                                                                                                                        
#endif

#ifdef ENABLE_SPP
SPP SerialBT(&Btd, "Balanduino", "0000");                                                                                                                         
#endif

#ifdef ENABLE_PS3
PS3BT PS3(&Btd);                                                                                                            
#endif

#ifdef ENABLE_WII
WII Wii(&Btd);                                                                                                                                 
                                                                                                                                          
                                                        
                                                                          
                                                                    
#endif

void setup() {
                       
  Serial.begin(115200);

                                                                              
  if (!checkInitializationFlags())
    readEEPROMValues();                                                              

                      
  pinMode(leftEncoder1, INPUT);
  pinMode(leftEncoder2, INPUT);
  pinMode(rightEncoder1, INPUT);
  pinMode(rightEncoder2, INPUT);
  attachInterrupt(0, leftEncoder, CHANGE);
  attachInterrupt(1, rightEncoder, CHANGE);

                                
  pinMode(leftEnable, OUTPUT);
  pinMode(rightEnable, OUTPUT);
  digitalWrite(leftEnable, HIGH);
  digitalWrite(rightEnable, HIGH);

                                  
  sbi(pwmPortDirection, leftPWM);
  sbi(leftPortDirection, leftA);
  sbi(leftPortDirection, leftB);
  sbi(pwmPortDirection, rightPWM);
  sbi(rightPortDirection, rightA);
  sbi(rightPortDirection, rightB);

                                                                                                           
                                                                                                           
  TCCR1B = _BV(WGM13) | _BV(CS10);                                                                          
  ICR1 = PWMVALUE;                                                                          

                                                   
                                                      
                                                     
  TCCR1A = _BV(COM1A1) | _BV(COM1B1);
  setPWM(leftPWM, 0);                             
  setPWM(rightPWM, 0);

                        
  pinMode(buzzer, OUTPUT);

#ifdef ENABLE_USB
  if (Usb.Init() == -1) {                                
    Serial.print(F("OSC did not start"));
    digitalWrite(buzzer, HIGH);
    while (1);        
  }
#endif

                              
                                                                                                                          
#ifdef ENABLE_PS3
  PS3.attachOnInit(onInit);
#endif
#ifdef ENABLE_WII
  Wii.attachOnInit(onInit);
#endif
#ifdef ENABLE_XBOX
  Xbox.attachOnInit(onInit);
#endif

                 
  Wire.begin();

  while (i2cRead(0x75, i2cBuffer, 1));
  if (i2cBuffer[0] != 0x68) {                            
    Serial.print(F("Error reading sensor"));
    digitalWrite(buzzer, HIGH);
    while (1);        
  }

  i2cBuffer[0] = 19;                                                      
  i2cBuffer[1] = 0x00;                                                                                     
  i2cBuffer[2] = 0x00;                                          
  i2cBuffer[3] = 0x00;                                             
  while (i2cWrite(0x19, i2cBuffer, 4, false));                                       
  while (i2cWrite(0x6B, 0x09, true));                                                                                          

  delay(100);                                    

                                          
  while (i2cRead(0x3D, i2cBuffer, 4));
  accY = ((i2cBuffer[0] << 8) | i2cBuffer[1]);
  accZ = ((i2cBuffer[2] << 8) | i2cBuffer[3]);
                                                                                          
                                                                   
  accAngle = (atan2((double)accY - cfg.accYzero, (double)accZ - cfg.accZzero) + PI) * RAD_TO_DEG;

  kalman.setAngle(accAngle);                      
  pitch = accAngle;
  gyroAngle = accAngle;

                            
  calibrateGyro();

  pinMode(LED_BUILTIN, OUTPUT);                                                                   

                                             
  digitalWrite(buzzer, HIGH);
  delay(100);
  digitalWrite(buzzer, LOW);

                    
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
  if (Wii.wiimoteConnected)                                                                        
    Usb.Task();
#endif

                       
  while (i2cRead(0x3D, i2cBuffer, 8));
  accY = ((i2cBuffer[0] << 8) | i2cBuffer[1]);
  accZ = ((i2cBuffer[2] << 8) | i2cBuffer[3]);
  gyroX = ((i2cBuffer[6] << 8) | i2cBuffer[7]);

                                                                                          
                                                                   
  accAngle = (atan2((double)accY - cfg.accYzero, (double)accZ - cfg.accZzero) + PI) * RAD_TO_DEG;

  uint32_t timer = micros();
                                                                                                         
  if ((accAngle < 90 && pitch > 270) || (accAngle > 270 && pitch < 90)) {
    kalman.setAngle(accAngle);
    pitch = accAngle;
    gyroAngle = accAngle;
  } else {
    gyroRate = ((double)gyroX - gyroXzero) / 131.0;                    
    double dt = (double)(timer - kalmanTimer) / 1000000.0;
    gyroAngle += gyroRate * dt;                                         
    if (gyroAngle < 0 || gyroAngle > 360)
      gyroAngle = pitch;                                                     
    pitch = kalman.getAngle(accAngle, gyroRate, dt);                                             
  }
  kalmanTimer = timer;
                                                                                                               

#ifdef ENABLE_WII
  if (Wii.wiimoteConnected)                                                                        
    Usb.Task();
#endif

                    
  timer = micros();
                                                                                                    
                                                                                         
  if ((layingDown && (pitch < cfg.targetAngle - 10 || pitch > cfg.targetAngle + 10)) || (!layingDown && (pitch < cfg.targetAngle - 45 || pitch > cfg.targetAngle + 45))) {
    layingDown = true;                                                                                                     
    stopAndReset();
  } else {
    layingDown = false;                              
    updatePID(cfg.targetAngle, targetOffset, turningOffset, (double)(timer - pidTimer) / 1000000.0);
  }
  pidTimer = timer;

                       
  timer = micros();
  if (timer - encoderTimer >= 100000) {                                     
    encoderTimer = timer;
    int32_t wheelPosition = getWheelsPosition();
    wheelVelocity = wheelPosition - lastWheelPosition;
    lastWheelPosition = wheelPosition;
                                                                                                                                   
    if (abs(wheelVelocity) <= 40 && !stopped) {                                     
      targetPosition = wheelPosition;
      stopped = true;
    }

    batteryCounter++;
    if (batteryCounter > 10) {                            
      batteryCounter = 0;
      batteryVoltage = (double)analogRead(VBAT) / 63.050847458;                                                                                                                                                                     
      if (batteryVoltage < 10.2 && batteryVoltage > 5)                                                                                                    
        digitalWrite(buzzer, HIGH);
      else
        digitalWrite(buzzer, LOW);
    }
  }

                                                             
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
      digitalWrite(LED_BUILTIN, ledState);                                                                                             
    }
  } else if (ledState) {                 
    ledState = !ledState;
    digitalWrite(LED_BUILTIN, ledState);                         
  }
#endif
}

