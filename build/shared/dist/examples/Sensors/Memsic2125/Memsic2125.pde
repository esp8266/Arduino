// Memsic2125

#define X 2
#define Y 3

int dx = 0;
int dy = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(X, INPUT);
  pinMode(Y, INPUT);
}

void loop()
{
  int pulseX, pulseY;
  int accX, accY;
  
  // wait for previous pulse to end
  while (digitalRead(X) == HIGH) {}
  // read pulse from x-axis
  pulseX = pulseIn(X,HIGH);
  
  // wait for previous pulse to end
  while (digitalRead(Y) == HIGH) {}
  // read pulse from y-axis
  pulseY = pulseIn(Y,HIGH);
  
  // convert the pulse width into acceleration
  // accX and accY are in milli-g's: earth's gravity is 1000.
  accX = ((pulseX / 10) - 500) * 8;
  accY = ((pulseY / 10) - 500) * 8;
  
  Serial.print(accX);
  Serial.print(" ");
  Serial.print(accY);
  Serial.println();

  delay(100);
}
