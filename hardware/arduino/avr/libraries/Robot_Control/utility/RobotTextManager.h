#ifndef ROBOTTEXTMANAGER_H
#define ROBOTTEXTMANAGER_H

#define USERNAME  0
#define ROBOTNAME  1
#define CITYNAME  2
#define COUNTRYNAME  3
#define EMPTY 4

class TextManager{
  //The TextManager class is a collection of features specific for Hello 
  //User example. 
  //
  //- It includes solution for setting text position based on
  //  line/column. The original Robot.text(), or the more low level 
  //  print() function can only set text position on pixels from left,
  //  top. 
  //
  //- The process of accepting input with the virtual keyboard, saving
  //  into or reading from EEPROM is delt with here.
  //
  //- A workflow for stop the music while displaying image. Trouble
  //  will happen otherwise.
  
  public:
	//add some margin to the text, left side only atm.
	void setMargin(int margin_left,int margin_top);
    
    //print text based on line, column. 
    void writeText(int lineNum, int colNum, char* txt, bool onOff=true);
	
	//print a script from the scripts library
	void writeScript(int seq, int line, int col);

	//The whole process of getting input
	void input(int lin,int col, int code);
    //Print a cursor and virtual keyboard on screen, and save the user's input
    void getInput(int lin, int col);  
    //Get user name, robot name, city name or country name from EEPROM
    //and store in the input pool.
    void setInputPool(int code);
    //save user input to EEPROM
    void pushInput(int code);
	
    //Replaces Robot.drawPicture(), as this one solves collision between
    //image and music
    void showPicture(char * filename, int posX, int posY);

  private:
    int margin_left,margin_top;
	int getLin(int lineNum);  //Convert line to pixels from top
    int getCol(int colNum);  //Convert line to pixels from left
  
    static const int lineHeight;//8+2=10
    static const int charWidth;//5+1=6
    
    int inputPos;
    int inputLin;
    int inputCol;
	
    void drawInput(bool onOff);
    void mvInput(int dire);

    char selectLetter();
    void refreshCurrentLetter(char letter);

	void getPGMtext(int seq);
	
	char PGMbuffer[85];  //the buffer for storing strings
    char inputPool[18];
};

//a trick for removing the need of creating an object of TextManager.
//So you can call me.somefunction() directly in the sketch.
extern TextManager textManager;

#endif
