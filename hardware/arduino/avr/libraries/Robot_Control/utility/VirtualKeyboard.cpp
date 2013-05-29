#include "VirtualKeyboard.h"

int VirtualKeyboard::getColLin(int val){
  uint8_t col,lin;
  lin=val/10;
  col=val%10; // saving 36 bytes :(
  /*if(0<=val && 9>=val){
    col=val;
    lin=0;
  }else if(10<=val && 19>=val){
    col=val-10;
    lin=1;
  }else if(20<=val && 29>=val){
    col=val-20;
    lin=2;
  }else if(30<=val && 39>=val){
    col=val-30;
    lin=3;
  }*/
  return (col<<8)+lin;	//Put col and lin in one int
}
void VirtualKeyboard::run(){
/** visually select a letter on the keyboard
*	The selection boarder is 1px higher than the character, 
*	1px on the bottom, 2px to the left and 2px to the right.
*
*/
  if(!onOff)return;
  //Serial.println(onOff);
  static int oldColLin=0;
  uint8_t val=map(Robot.knobRead(),0,1023,0,38);
  if(val==38)val=37;	//The last value is jumpy when using batteries
  int colLin=getColLin(val);
  
  if(oldColLin!=colLin){
    uint8_t x=(oldColLin>>8 & 0xFF)*11+10;//col*11+1+9
    uint8_t y=(oldColLin & 0xFF)*11+1+top;//lin*11+1+top
    uint8_t w=9;
    if(oldColLin==1795)  //last item "Enter", col=7 lin=3
      w=33; //(5+1)*6-1+2+2  charWidth=5, charMargin=1, count("Enter")=6, lastItem_MarginRight=0, marginLeft==marginRight=2
    Robot.drawRect(x,y,w,9,hideColor);


    x=(colLin>>8 & 0xFF)*11+10;
    y=(colLin & 0xFF)*11+1+top;
    w=9;
    if(colLin==1795)  //last item "Enter", col=7 lin=3
      w=33; //(5+1)*6-1+2+2  charWidth=5, charMargin=1, count("Enter")=6, lastItem_MarginRight=0, marginLeft==marginRight=2
    Robot.drawRect(x,y,w,9,showColor);
    oldColLin=colLin;
  }
}

char VirtualKeyboard::getSelection(){
  if(!onOff)return -1;

  uint8_t val=map(Robot.knobRead(),0,1023,0,38);
  if(0<=val && 9>=val)
    val='0'+val;	
  else if(10<=val && 35>=val)
    val='A'+val-10;	
  else if(val==36)
    val=' ';
  else if(val>=37)
    val='\0';
  
  return val;
}
void VirtualKeyboard::hide(){
	onOff=false;
	Robot.fillRect(0,top,128,44,hideColor);//11*4
}

void VirtualKeyboard::display(uint8_t top, uint16_t showColor, uint16_t hideColor){
/** Display the keyboard at y position of top
*	formular:
*	When text size is 1, one character is 5*7
*	margin-left==margin-right==3,
*	margin-top==margin-bottom==2,
*	keyWidth=5+3+3==11,
*	keyHeight=7+2+2==11,
*	keyboard-margin-left=keyboard-margin-right==9
*	so character-x=11*col+9+3=11*col+12
*	character-y=11*lin+2+top
*	
**/
  this->top=top;
  this->onOff=true;
  
  this->showColor=showColor;
  this->hideColor=hideColor;
  
  for(uint8_t i=0;i<36;i++){
	Robot.setCursor(i%10*11+12,2+top+i/10*11);
	if(i<10)
		Robot.print(char('0'+i));
	else
		Robot.print(char(55+i));//'A'-10=55
  }//for saving 58 bytes :(
  
  /*for(int i=0;i<10;i++){
    Robot.setCursor(i*11+12,2+top);//11*0+2+top
    Robot.print(char('0'+i));//line_1: 0-9
  }
  for(int i=0;i<10;i++){
    Robot.setCursor(i*11+12,13+top);//11*1+2+top
    Robot.print(char('A'+i));//line_2: A-J
  }
  for(int i=0;i<10;i++){
    Robot.setCursor(i*11+12,24+top);//11*2+2+top
    Robot.print(char('K'+i));//line_3: K-T
  }
  for(int i=0;i<6;i++){
    Robot.setCursor(i*11+12,35+top);//11*3+2+top
    Robot.print(char('U'+i));//line_4: U-Z
  }*/
  //space and enter at the end of the last line.
  Robot.setCursor(78,35+top);//6*11+12=78
  Robot.print('_');//_
  
  Robot.setCursor(89,35+top);//7*11+12=89
  Robot.print("Enter");//enter
}



VirtualKeyboard Vkey=VirtualKeyboard();