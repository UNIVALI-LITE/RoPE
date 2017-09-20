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

#define PB0_PIN 0b00000001
#define PB1_PIN 0b00000010
#define PB2_PIN 0b00000100
#define PB3_PIN 0b00001000
#define PB4_PIN 0b00010000
#define PB5_PIN 0b00100000
#define PB6_PIN 0b01000000 // YELLOW
#define PB7_PIN 0b10000000 // RED

void pbPinMode(uint8_t pbPin, uint8_t mode)
{
  if (mode == OUTPUT)
  {
    DDRB |= pbPin;
  }
  else if (mode == INPUT)
  {
    DDRB &= ~(pbPin);
  }
}

void pbDigitalWrite(uint8_t pbPin, uint8_t state)
{
  if (state == HIGH)
  {
    PORTB |= pbPin;    
  }
  else if (state == LOW)
  {
    PORTB &= ~(pbPin);
  }
}

// the setup function runs once when you press reset or power the board
void setup() {
   pbPinMode(PB6_PIN, OUTPUT);
   pbPinMode(PB7_PIN, OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
  
  pbDigitalWrite(PB6_PIN, HIGH);
  delay(1000);
  pbDigitalWrite(PB7_PIN, HIGH);
  delay(1000);
  
  pbDigitalWrite(PB6_PIN, LOW);
  delay(1000);
  pbDigitalWrite(PB7_PIN, LOW);
  delay(1000);
}
