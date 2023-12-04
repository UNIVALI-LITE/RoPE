#ifdef UNIT_TEST

#include <stdint.h>
#include <unity.h>
#include <pinout.h>

#include <keypad.hpp>
#include <buzzer.hpp>
#include <motor_controller.hpp>

#define ON HIGH
#define OFF LOW

#define LED_DELAY 500
#define TEST_START_DELAY 1000

Rope::Keypad keypad;
Rope::Buzzer buzzer;
Rope::MotorController motorController;

void turnLeds(const uint8_t state);
void blinkLed(const uint8_t ledPin);
void waitButton(const uint8_t buttonId);

void setupLeds();
void setupKeypad();
void setupBuzzer();
void setupMotors();

void testLeds();
void testKeypad();
void testBuzzer();
void testMotors();
void testCharger();

void setup()
{
    setupLeds();
    setupKeypad();
    setupBuzzer();
    setupMotors();
    
    testLeds();
    testkeypad();
    testBuzzer();
    testMotors();
    testCharger();
}

void testLeds()
{
    turnOffLeds();

    delay(TEST_START_DELAY);

    blinkLed(FRONT_LED_PIN);
    blinkLed(RIGHT_LED_PIN);
    blinkLed(BACK_LED_PIN);
    blinkLed(LEFT_LED_PIN);
    blinkLed(EXECUTE_LED_PIN);
}

void blinkLed(const uint8_t ledPin)
{
    digitalWrite(FRONT_LED_PIN, LOW);
    digitalWrite(FRONT_LED_PIN, HIGH);
    delay(LED_DELAY);
    digitalWrite(FRONT_LED_PIN, LOW);
}

void setLeds(const uint8_t state)
{
    digitalWrite(FRONT_LED_PIN, state);
    digitalWrite(RIGHT_LED_PIN, state);
    digitalWrite(BACK_LED_PIN, state);
    digitalWrite(LEFT_LED_PIN, state);
    digitalWrite(EXECUTE_LED_PIN, state);
}

void doNothing()
{

}

void setupLeds()
{
    pinMode(FRONT_LED_PIN, OUTPUT);
    pinMode(RIGHT_LED_PIN, OUTPUT);
    pinMode(BACK_LED_PIN, OUTPUT);
    pinMode(LEFT_LED_PIN, OUTPUT);
    pinMode(EXECUTE_LED_PIN, OUTPUT);

    setLeds(LOW);
}

void setupKeypad()
{
    keypad.initialize(KEYPAD_PIN);
    keypad.registerButton(Range(886, 968), doNothing);
    keypad.registerButton(Range(720, 802), doNothing);
    keypad.registerButton(Range(637, 719), doNothing);
    keypad.registerButton(Range(803, 885), doNothing);
    keypad.registerButton(Range(553, 636), doNothing);
}



void setupMotors()
{

}

void loop()
{
    // Do nothing
}

#endif // UNIT_TEST