/* ------------------------------------------------
* SERIAL COM - HANDELING MULTIPLE BYTES inside ARDUINO - 04_function development
* by beltran berrocal
*
* this prog establishes a connection with the pc and waits for it to send him
* a long string of characters like "hello Arduino!". 
* Then Arduino informs the pc that it heard the whole sentence
*
* the same as examlpe 03 but it deploys 2 reusable functions.
* for doing the same job. 
* readSerialString() and  printSerialString()
* you just need to instantiate an array that will hold all the chars of the string
* I've put a 100 value for excess, but if you exactly know how many bytes you are expecting
* simply write it down inside the brackets [yourLengthHere]
*
* created 16 Decembre 2005;
* copyleft 2005 Progetto25zero1  <http://www.progetto25zero1.com>
*
* --------------------------------------------------- */

char serInString[100];  // array that will hold the different bytes of the string. 100=100characters;
                        // -> you must state how long the array will be else it won't work properly

//read a string from the serial and store it in an array
//you must supply the array variable
void readSerialString (char *strArray) {
    int i = 0;
    if(Serial.available()) {    
       Serial.print("reading Serial String: ");  //optional: for confirmation
       while (serialAvailable()){            
          strArray[i] = Serial.read();
          i++;
          Serial.write(strArray[(i-1)]);         //optional: for confirmation
       }
       Serial.println();                          //optional: for confirmation
    }      
}

//Print the whole string at once - will be performed only if thers is data inside it
//you must supply the array variable
void printSerialString(char *strArray) {
     int i=0;
     if (strArray[i] != 0) {     
         while(strArray[i] != 0) {
            Serial.print( strArray[i] );
            strArray[i] = 0;                  // optional: flush the content
            i++;          
         }
     }
} 

//utility function to know wither an array is empty or not
boolean isStringEmpty(char *strArray) {
     if (strArray[0] == 0) {
         return true;
     } else {
         return false;
     }
}

void setup() {
  Serial.begin(9600);
}

void loop () {
  //simple feedback from Arduino
  Serial.println("Hello World"); 
  
  //read the serial port and create a string out of what you read
  readSerialString(serInString);
  
  //do somenthing else perhaps wait for other data or read another Serial string
  Serial.println("------------ arduino is doing somenthing else ");
  
  
  if( isStringEmpty(serInString) == false) { //this check is optional
      Serial.println("Arduino recorded that you said: ");
      //try to print out collected information. it will do it only if there actually is some info.
      printSerialString(serInString);
      Serial.println();
  }
  
  Serial.println();  
  //slows down the visualization in the terminal
  delay(2000);
}
