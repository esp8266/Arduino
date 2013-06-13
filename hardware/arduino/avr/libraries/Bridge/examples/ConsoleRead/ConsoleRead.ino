/*
 Console.read() example:
 read data coming from bridge using the Console.read() function
 and store it in a string.
 
 created 13 Jun 2013
 by Angelo Scialabba 
 
 This example code is in the public domain.
 */

#include <Console.h>

String name;
int current_char;

void setup() {
  //Initialize Console and wait for port to open:
  Bridge.begin();
  Console.begin(); 
  
  while (!Console) {
    ; // wait for Console port to connect.
  }
  Console.println("Hi, who are you?");
} 

void loop() {
  current_char = Console.read(); //read the next char received
  //look for the newline character, this is the last character in the string
  if (current_char == '\n') {
    //print text with the name received
    Console.print("Hi ");
    Console.print(name);
    Console.println("! Nice to meet you!");
    Console.println();
    //Ask again for name and clear the old name
    Console.println("Hi, who are you?");
    name = "";
  } else if (current_char != -1) {   //if the buffer is empty Cosole.read returns -1
    name = name + (char)current_char; //current_char is int, treat him as char and add it to the name string
  }
}
