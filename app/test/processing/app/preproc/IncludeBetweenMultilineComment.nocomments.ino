#include <CapacitiveSensorDue.h>
  
                 
  
CapacitiveSensorDue cs_13_8 = CapacitiveSensorDue(13,8);
void setup()
{
	Serial.begin(9600);
}
void loop()
{
	long total1 = cs_13_8.read(30);
	Serial.println(total1);
	delay(100);
}

