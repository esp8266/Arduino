

#include "Arduino.h"
int s_Tick=0;
int number = 0;
#define RAMFUNC __attribute__((section(".entry.text")))
    #include "gdbstub.h"

int RAMFUNC add(int a, int b)
{
  int c;
  c = a+b;
  return c;
}
int RAMFUNC mult(int a, int b){
  int c;
  c = a*b;
  return c;
}
int RAMFUNC calculate(int a, int b, int c){
  a = add(a,b);
  c = mult(a,c);
  return c;
}
void RAMFUNC function(){
	int val;
	//toggleIoPins();
	val = calculate(number++,2,3);
	ets_printf("Tick: %d  Value:%d\r\n", ++s_Tick, val);
	delay(1000);
}

void setup() {
	//uart_div_modify(0, UART_CLK_FREQ / 115200);
	Serial.begin(115200);
 Serial.print("program started");
	gdbstub_init();
    pinMode(4, OUTPUT);     // Initialize the pin as an output
    pinMode(5, OUTPUT);     // Initialize the pin as an output
}

void RAMFUNC toggleIoPins() {
	digitalWrite(4, LOW);
	digitalWrite(5, LOW);
	delay(100);
	digitalWrite(4, HIGH);
	digitalWrite(5, HIGH);
	delay(200);
}
int loopCount=0;
// the loop function runs over and over again forever
void RAMFUNC loop() {
  loopCount++;
	function();
  ets_printf("Loopcount: %d",loopCount);
}
