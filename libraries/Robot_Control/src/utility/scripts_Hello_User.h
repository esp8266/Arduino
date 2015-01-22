#include <avr/pgmspace.h>

//an advanced trick for storing strings inside the program space
//as the ram of Arduino is very tiny, keeping too many string in it
//can kill the program

const char hello_user_script1[] PROGMEM="What's your name?";
const char hello_user_script2[] PROGMEM="Give me a name!";
const char hello_user_script3[] PROGMEM="And the country?";
const char hello_user_script4[] PROGMEM="The city you're in?";
const char hello_user_script5[] PROGMEM="  Plug me to\n\n  your computer\n\n  and start coding!";

const char hello_user_script6[] PROGMEM="     Hello User!\n\n It's me, your robot\n\n    I'm alive! <3";
const char hello_user_script7[] PROGMEM="  First I need some\n\n   input from you!";
const char hello_user_script8[] PROGMEM="     Use the knob\n\n   to select letters";
const char hello_user_script9[] PROGMEM="   Use L/R button\n\n to move the cursor,\n\n  middle to confirm";
const char hello_user_script10[] PROGMEM="  Press middle key\n   to continue...";
const char hello_user_script11[] PROGMEM="  Choose \"enter\" to\n\n  finish the input";

const char * const scripts_Hello_User[] PROGMEM = {
  hello_user_script1,
  hello_user_script2,
  hello_user_script3,
  hello_user_script4,
  hello_user_script5,
  hello_user_script6,
  hello_user_script7,
  hello_user_script8,
  hello_user_script9,
  hello_user_script10,
  hello_user_script11,
};

/*
void getPGMtext(int seq){
  //It takes a string from program space, and fill it  
  //in the buffer
  strcpy_P(buffer,(char*)pgm_read_word(&(scripts[seq])));
}

void writeScript(int seq, int line, int col){
  //print a string from program space to a specific line,
  //column on the LCD
  
  //first fill the buffer with text from program space
  getPGMtext(seq);
  //then print it to the screen
  textManager.writeText(line,col,buffer);
}

*/
