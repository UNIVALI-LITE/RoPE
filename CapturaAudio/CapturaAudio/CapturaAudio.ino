int var1 = A5;

int valorSensor = 0;

void setup()
{
  Serial.begin(9600);
  
  pinMode(var1, INPUT);
}

void loop()
{
  valorSensor = analogRead(var1);
  Serial.println(valorSensor);
  
  delay(50);
}
