/*
  Blink

  Turns an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the UNO, MEGA and ZERO
  it is attached to digital pin 13, on MKR1000 on pin 6. LED_BUILTIN is set to
  the correct LED pin independent of which board is used.
  If you want to know what pin the on-board LED is connected to on your Arduino
  model, check the Technical Specs of your board at:
  https://www.arduino.cc/en/Main/Products

  modified 8 May 2014
  by Scott Fitzgerald
  modified 2 Sep 2016
  by Arturo Guadalupi
  modified 8 Sep 2016
  by Colby Newman

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/Blink
*/
long initTime = 0;

// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(9600);
  initTime = millis();
}

uint32_t counter = 0;
void loop() {

  if (millis() - initTime >= 1000)
  {
    Serial.print("Counter: ");
    Serial.println(counter);

    counter = 0;

    initTime = millis();
  }
  
  counter = counter + 1;
}
