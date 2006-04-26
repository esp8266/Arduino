/* Driving two DC motors 
 * ----------------------- 
 * 
 *  Custom timer originally intended to DC motor  
 *  control purposes and made from two nested loops: 
 *   - "scan cycle" is the main loop  
 *   - "control cycle" is the internal loop  
 *  Timing id adjusted by changing number of iterations en each loop 
 *  and the internal delay of each control cycle iteration (tic). If scan 
 *  cycle code takes significative time to jam current control cycle, this 
 *  delay could be easily reduced or bypassed. 
 *
 * (copyleft) 2005 by Quique
 * <mailto:info@spindesk.com>
 * posted to the Arduino Forum
 * <http://www.arduino.cc>
 * 
 */

int ledPin0 = 13;      // LED connected to digital pin 13 
boolean ledValue = LOW;
int motorPin1 = 9;    // Motor 1 connected to digital pin 9 
int motorPin2 = 8;     // Motor 1 connected to digital pin 8 

int potPin1   = 5;     // Potentiometer1 connected to analog pin 5 ( 5 is 1 in some boards) 
int potPin2   = 4;     // Potentiometer1 connected to analog pin 5 ( 4 is 2 in some boards) 

// Timing Setup 

int scanCycle = 10;    // Number of control cycles in a scan cycle    
int controlCycle = 200;    // Control cycle iterations 
int tic = 6;     // Control cycle iteration aditional delay in microseconds 

int currentSCycle = 0;      // Current scan cycle iteration 
int currentCCycle = 0;      // Current control cycle iteration 
boolean scanEnable = true;  // Allows read analog & digital inputs 

// End timing setup 

int counter = 0;       // Scan cycle counter used to change led status 
int motor1_PW;    // motor 1 Pulse Width 
int motor2_PW;    // motor 2 Pulse Width 

/* 
 * Switch the boolean value assigned to any variable 
 */
boolean boolSwitch (boolean *target) 
{ 
  if ( *target ) {
    *target = false;
  }  
  else { 
    *target = true; 
  } 
  return *target; 
} 

void setup() 
{ 
  pinMode (ledPin0, OUTPUT); // sets the digital pin as output 
  pinMode (motorPin1, OUTPUT);    // sets the digital pin as output 
  pinMode (motorPin2, OUTPUT);    // sets the digital pin as output 
} 

void loop() 
{ 
  // Scan cycle 

  if (scanEnable) 
  { 
    // 
    // Scan cycle logic here 
    // 
    motor1_PW = analogRead (potPin1)/5; // Pot 1 read scale change 
    motor2_PW = analogRead (potPin2)/5; // Pot 1 read scale change 

    // Swith led in pin 13 each 10 scan cycles. We can assume that while  
    // Led is on (or off), porgram has taken 10 scan cycles. So, if we adjust   
    // blink time to 1 sec, we are able to scanning inputs each 100 msec.  
    if (counter++ >= 10) 
    {  
      digitalWrite (ledPin0, boolSwitch (&ledValue)); // Led blink each 10 scan cycles i.e. if 
      // led is on 1 second, we are scaning knobs each 
      // 100 miliseconds  
      counter =0; 
    } 
  } 

  // Control cycle    
  for (currentCCycle = 0; currentCCycle < controlCycle; currentCCycle ++) 
  {  
    delayMicroseconds (tic); 
    // 
    // Control cycle logic here 
    // 
    if ( motor1_PW > currentCCycle ) 
    { 
      digitalWrite ( motorPin1, LOW); 
    } 
    else  
    { 
      digitalWrite ( motorPin1, HIGH); 
    } 

    if ( motor2_PW > currentCCycle ) 
    { 
      digitalWrite ( motorPin2, LOW); 
    } 
    else  
    { 
      digitalWrite ( motorPin2, HIGH); 
    } 

  } 

  // Detect completed scan cycle    
  if ( currentSCycle ++ > scanCycle) 
  { 
    scanEnable = true; // Allow readings of external inputs  
    currentSCycle = 0; // Reset scan cycle counter 
  } 
  else 
  { 
    scanEnable = false; //  
  } 
} 

