#include <avr/pgmspace.h>
#include <ArduinoRobot.h>
#include "VirtualKeyboard.h"
#include "RobotTextManager.h"
#include "scripts_Hello_User.h"

const int TextManager::lineHeight=10;
const int TextManager::charWidth=6;


void TextManager::setMargin(int margin_left,int margin_top){
	this->margin_left=margin_left;
	this->margin_top=margin_top;
}
int TextManager::getLin(int lineNum){
   return lineNum*lineHeight+margin_top;
}

int TextManager::getCol(int colNum){
   return colNum*charWidth+margin_left;
}

void TextManager::writeText(int lineNum, int colNum, char* txt, bool onOff){
  if(!onOff)
	Robot.setTextColor(WHITE);

  Robot.setCursor(getCol(colNum),getLin(lineNum));
  Robot.print(txt);
  
  Robot.setTextColor(BLACK);
}

void TextManager::drawInput(bool onOff){
  if(!onOff)
    Robot.setTextColor(WHITE);

  Robot.setCursor(getCol(inputCol),getLin(inputLin)+1);
  Robot.print('_');
  
  Robot.setTextColor(BLACK);

}

void TextManager::mvInput(int dire){
  drawInput(0);
  if(dire<0){
    if(inputPos>0){
      inputPos--;
      inputCol--;
    }
  }else{
    if(inputPos<16){
      inputPos++;
      inputCol++;
    }
  }
  drawInput(1);
}

char TextManager::selectLetter(){
  static int oldVal;
  char val=map(Robot.knobRead(),0,1023,32,125);
  if(val==oldVal){
    return 0;  //No changes
  }else{
    oldVal=val;
    return val;  //Current letter
  }
}

void TextManager::refreshCurrentLetter(char letter){
  if(letter){
	writeText(inputLin,inputCol,inputPool+inputPos,false);//erase
	inputPool[inputPos]=letter;
	writeText(inputLin,inputCol,inputPool+inputPos,true);//write
  }
}


void TextManager::getInput(int lin, int col){
  writeText(lin,col,">");  //Input indicator
  
  writeText(lin, col+1, inputPool);
  
  inputLin=lin;  //Ini input cursor
  inputCol=col+1;
  inputPos=0;
  drawInput(true);
  
  Vkey.display(100);//Vkey is a object of VirtualKeyboard class
  
  while(true){
    switch(Robot.keyboardRead()){
      case BUTTON_LEFT:
        //Robot.beep(BEEP_SIMPLE);
        mvInput(-1);
        break;
      case BUTTON_RIGHT:
        //Robot.beep(BEEP_SIMPLE);
        mvInput(1);
        break;
      case BUTTON_MIDDLE:
        //Robot.beep(BEEP_DOUBLE);
        char selection=Vkey.getSelection();
        if(selection!='\0'){
          refreshCurrentLetter(selection);
          mvInput(1);
        }else{
          drawInput(false);
          return;
        }
    }
    Vkey.run();
    delay(10);
  }
}
void TextManager::setInputPool(int code){
  switch(code){
    case USERNAME:
      Robot.userNameRead(inputPool);
      break;
    case ROBOTNAME:
      Robot.robotNameRead(inputPool);
      break;
    case CITYNAME:
      Robot.cityNameRead(inputPool);
      break;
    case COUNTRYNAME:
      Robot.countryNameRead(inputPool);
      break;
  }
  for(int i=0;i<18;i++){
    if(inputPool[i]=='\0'){
      for(int j=i;j<18;j++){
        inputPool[j]='\0';
      }
      break;
    }
  }
}
void TextManager::pushInput(int code){
  switch(code){
    case USERNAME:
      Robot.userNameWrite(inputPool);
      break;
    case ROBOTNAME:
      Robot.robotNameWrite(inputPool);
      break;
    case CITYNAME:
      Robot.cityNameWrite(inputPool);
      break;
    case COUNTRYNAME:
      Robot.countryNameWrite(inputPool);
      break;
  }
  for(int i=0;i<18;i++){
    inputPool[i]='\0';
  }
}
void TextManager::input(int lin,int col, int code){
  setInputPool(code);
  getInput(lin,col);
  pushInput(code);
}

void TextManager::showPicture(char * filename, int posX, int posY){
  Robot.pause();
  Robot._drawBMP(filename,posX,posY);
  Robot.play();
}

void TextManager::getPGMtext(int seq){
  //It takes a string from program space, and fill it  
  //in the buffer
  //if(in hello user example){
  if(true){
	strcpy_P(PGMbuffer,(char*)pgm_read_word(&(::scripts_Hello_User[seq])));
  }
}

void TextManager::writeScript(int seq, int line, int col){
  //print a string from program space to a specific line,
  //column on the LCD
  
  //first fill the buffer with text from program space
  getPGMtext(seq);
  //then print it to the screen
  textManager.writeText(line,col,PGMbuffer);
}


TextManager textManager=TextManager();
