//#include <ArduinoRobotMotorBoard.h>
#include "LineFollow.h"

//#define KP 19 //0.1 units
//#define KD 14
//#define ROBOT_SPEED 100 //percentage

//#define KP 11
//#define KD 5
//#define ROBOT_SPEED 50

//#define INTEGRATION_TIME 10 //En ms

/*uint8_t KP=11;
uint8_t KD=5;
uint8_t robotSpeed=50; //percentage
uint8_t intergrationTime=10;*/

#define NIVEL_PARA_LINEA 50 

/*int lectura_sensor[5], last_error=0, acu=0;

//Estos son los arrays que hay que rellenar con los valores de los sensores
//de suelo sobre blanco y negro. 
int sensor_blanco[]={
  0,0,0,0,0};
int sensor_negro[]={
  1023,1023,1023,1023,1023};
*/
//unsigned long time;

//void mueve_robot(int vel_izq, int vel_der);
//void para_robot();
//void doCalibration(int speedPct, int time);
//void ajusta_niveles();	//calibrate values

LineFollow::LineFollow(){
	/*KP=11;
	KD=5;
	robotSpeed=50; //percentage
	intergrationTime=10;*/
	config(11,5,50,10);
		
	for(int i=0;i<5;i++){
		sensor_blanco[i]=0;
		sensor_negro[i]=1023;
	}
}

void LineFollow::config(uint8_t KP, uint8_t KD, uint8_t robotSpeed, uint8_t intergrationTime){
	this->KP=KP;
	this->KD=KD;
	this->robotSpeed=robotSpeed;
	this->intergrationTime=intergrationTime;
	/*Serial.print("LFC: ");
	Serial.print(KP);
	Serial.print(' ');
	Serial.print(KD);
	Serial.print(' ');
	Serial.print(robotSpeed);
	Serial.print(' ');
	Serial.println(intergrationTime);*/
	
}
void LineFollow::calibIRs(){
	static bool isInited=false;//So only init once
	if(isInited)return ;
	
	delay(1000);
	
	doCalibration(30,500);
	doCalibration(-30,800);
	doCalibration(30,500);
	
	delay(1000);
	isInited=true;
}

void LineFollow::runLineFollow(){
  for(int count=0; count<5; count++)
  {
    lectura_sensor[count]=map(IRread(count),sensor_negro[count],sensor_blanco[count],0,127);
    acu+=lectura_sensor[count];
  }

  //Serial.println(millis());
  if (acu > NIVEL_PARA_LINEA)
  {
    acu/=5;

    int error = ((lectura_sensor[0]<<6)+(lectura_sensor[1]<<5)-(lectura_sensor[3]<<5)-(lectura_sensor[4]<<6))/acu;

    error = constrain(error,-100,100);

    //Calculamos la correcion de velocidad mediante un filtro PD
    int vel = (error * KP)/10 + (error-last_error)*KD;

    last_error = error;

    //Corregimos la velocidad de avance con el error de salida del filtro PD
    int motor_left = constrain((robotSpeed + vel),-100,100);
    int motor_right =constrain((robotSpeed - vel),-100,100);

    //Movemos el robot
    //motorsWritePct(motor_left,motor_right);
    motorsWritePct(motor_left,motor_right);

    //Esperamos un poquito a que el robot reaccione
    delay(intergrationTime);
  }
  else
  {
    //Hemos encontrado una linea negra
    //perpendicular a nuestro camino
    //paramos el robot
    motorsStop();

    //y detenemos la ejecución del programa
    //while(true);
	reportActionDone();
	//setMode(MODE_SIMPLE);
  }
}  


void LineFollow::doCalibration(int speedPct, int time){
	motorsWritePct(speedPct, -speedPct);
	unsigned long beginTime = millis();
	while((millis()-beginTime)<time)
		ajusta_niveles();
	motorsStop();
}
void LineFollow::ajusta_niveles()
{
	int lectura=0;

	for(int count=0; count<5; count++){
		lectura=IRread(count);

		if (lectura > sensor_blanco[count])
			sensor_blanco[count]=lectura;

		if (lectura < sensor_negro[count])
			sensor_negro[count]=lectura;
	} 
}






