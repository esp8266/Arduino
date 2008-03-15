int encoder = 4;
unsigned long speed = 0;

void setup() 
{ 
  pinMode(encoder, INPUT);
}

void loop() 
{ 
    speed = getspeed();
    delay(1000);  
} 

unsigned long getspeed()
{
  unsigned long duration = 0;
  duration = pulseIn(encoder, HIGH);
  return(duration);
} 

