  
                                                             
                                                      
                                                            
                                                                                           
                                                                                                             
                                                                                                          
                                                               
                                           
   

                                                          
                    
                  
                  
                  
                   
                  

                       
                                                  
                                                         

                 
                
      

                                                                                   
                                                                                                    
                                                                                                                                                        

                  
                     
      
                 
                
      
                 
                  
      
                 
                
      

                                     
Kalman kalman;                                                                      

                                                                                                                    
                  
USB Usb;                                                
      

                 
                                                                                                                        
ADK adk(&Usb,                  ,                     
                          ,              
                                          ,                                     
                     ,                              
                                                                                           ,                                                                      
                    );                                    
      

                  
XBOXRECV Xbox(&Usb);                                                                                                             
      

                                                                     
USBHub Hub(&Usb);                                  
BTD Btd(&Usb);                                                                                                                        
      

                 
SPP SerialBT(&Btd,             ,       );                                                                                                                         
      

                 
PS3BT PS3(&Btd);                                                                                                            
      

                 
WII Wii(&Btd);                                                                                                                                 
                                                                                                                                          
                                                        
                                                                          
                                                                    
      

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

                 
  if (Usb.Init() == -1) {                                
    Serial.print(F(                   ));
    digitalWrite(buzzer, HIGH);
    while (1);        
  }
      

                              
                                                                                                                          
                 
  PS3.attachOnInit(onInit);
      
                 
  Wii.attachOnInit(onInit);
      
                  
  Xbox.attachOnInit(onInit);
      

                 
  Wire.begin();

  while (i2cRead(0x75, i2cBuffer, 1));
  if (i2cBuffer[0] != 0x68) {                            
    Serial.print(F(                      ));
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
                 
  if (Wii.wiimoteConnected)                                                                        
    Usb.Task();
      

                       
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
                                                                                                               

                 
  if (Wii.wiimoteConnected)                                                                        
    Usb.Task();
      

                    
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

                                                             
                 
  readUsb();
      
                   
  checkSerialData();
      
                                                
  printValues();
      

                                                                     
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
      
}
