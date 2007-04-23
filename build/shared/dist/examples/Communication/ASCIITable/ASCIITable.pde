// ASCII Table 
// by Nicholas Zambetti <http://www.zambetti.com> 

void setup() 
{ 
  Serial.begin(9600); 
  
  // prints title with ending line break 
  Serial.println("ASCII Table ~ Character Map"); 
 
  // wait for the long string to be sent 
  delay(100); 
} 
 
int number = 33; // first visible character '!' is #33 
 
void loop() 
{ 
  Serial.print(number, BYTE);    // prints value unaltered, first will be '!' 
  
  Serial.print(", dec: "); 
  Serial.print(number);          // prints value as string in decimal (base 10) 
  // Serial.print(number, DEC);  // this also works 
  
  Serial.print(", hex: "); 
  Serial.print(number, HEX);     // prints value as string in hexadecimal (base 16) 
  
  Serial.print(", oct: "); 
  Serial.print(number, OCT);     // prints value as string in octal (base 8) 
  
  Serial.print(", bin: "); 
  Serial.println(number, BIN);   // prints value as string in binary (base 2) 
                                 // also prints ending line break 
 
  // if printed last visible character '~' #126 ... 
  if(number == 126) { 
    // loop forever 
    while(true) { 
      continue; 
    } 
  } 
 
  number++; // to the next character 
  
  delay(100); // allow some time for the Serial data to be sent 
} 
