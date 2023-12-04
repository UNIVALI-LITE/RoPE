#ifdef ROPE_TEST

#include <stdint.h>
#include <pinout.h>

#include <config.h>
#include <range.hpp>
#include <keypad.hpp>
#include <buzzer.hpp>
#include <motor_controller.hpp>

#define ON HIGH
#define OFF LOW

#define BUTTON_GO_FORWARD 0
#define BUTTON_TURN_RIGHT 1
#define BUTTON_GO_BACKWARD 2
#define BUTTON_TURN_LEFT 3
#define BUTTON_EXECUTE 4


#define LED_DELAY 750
#define TEST_START_DELAY 1000

Rope::Keypad keypad;
Rope::Buzzer buzzer;
Rope::MotorController motorController;

void turnLeds(const uint8_t state);
void blinkLed(const uint8_t ledPin);
void waitButton(const uint8_t buttonId);
void finishTests();

void setupLeds();
void setupKeypad();
void setupBuzzer();
void setupMotors();
void setupCharger();

void testLeds();
void testKeypad();
void testButton(const uint8_t buttonId, const uint8_t buttonLedPin);
void testBuzzer();
void testMotors();
void testCharger();

void setup()
{
    setupLeds();
    setupKeypad();
    setupBuzzer();
    setupMotors();
    setupCharger();

    waitButton(BUTTON_EXECUTE);
}

void loop()
{
    testLeds();
    testKeypad();
    testBuzzer();
    testMotors();
    // testCharger();
    
    finishTests();
}

void testLeds()
{
    blinkLed(FRONT_LED_PIN);
    blinkLed(RIGHT_LED_PIN);
    blinkLed(BACK_LED_PIN);
    blinkLed(LEFT_LED_PIN);
    blinkLed(EXECUTE_LED_PIN);

    waitButton(BUTTON_EXECUTE);
}

void testKeypad()
{
    testButton(BUTTON_GO_FORWARD, FRONT_LED_PIN);
    testButton(BUTTON_TURN_RIGHT, RIGHT_LED_PIN);
    testButton(BUTTON_GO_BACKWARD, BACK_LED_PIN);
    testButton(BUTTON_TURN_LEFT, LEFT_LED_PIN);
    testButton(BUTTON_EXECUTE, EXECUTE_LED_PIN);  

    delay(1000);

    waitButton(BUTTON_EXECUTE);
}

void testButton(const uint8_t buttonId, const uint8_t buttonLedPin)
{
    digitalWrite(buttonLedPin, HIGH);
    waitButton(buttonId);
    digitalWrite(buttonLedPin, LOW);
}

void testBuzzer()
{
    turnLeds(ON);

    const float duration = 0.035f; // seconds

    for (uint8_t octave = 4; octave <= 7; octave++)
    {
        buzzer.play(Rope::Note::C, octave, duration);
        buzzer.play(Rope::Note::D, octave, duration);
        buzzer.play(Rope::Note::E, octave, duration);
        buzzer.play(Rope::Note::F, octave, duration);
        buzzer.play(Rope::Note::G, octave, duration);
        buzzer.play(Rope::Note::A, octave, duration);
        buzzer.play(Rope::Note::B, octave, duration);
    }
    
    turnLeds(OFF);
    waitButton(BUTTON_EXECUTE);
}

bool dummy()
{
    return false;
}

Rope::Curve * curve = new Rope::TrapezoidalCurve(0.0f, 0.1f, 1.0f, 1.0f, 1.0f);

void testMotors()
{
    turnLeds(ON);

    const uint8_t revolutions = 2;

    Rope::MotorController::Direction clockwise[2] = {Rope::MotorController::Direction::Clockwise, Rope::MotorController::Direction::Clockwise};
    Rope::MotorController::Direction* directions = clockwise;

    motorController.synchronizedMove(Rope::MotorController::StepsPerRevolution * revolutions, directions, 1.2f, curve, dummy);
    // motorController.synchronizedMove(Rope::MotorController::StepsPerRevolution * revolutions, directions, Rope::Config::maxMotorsSpeed, curve, dummy);
    
    turnLeds(OFF);
    waitButton(BUTTON_EXECUTE);
}

void testCharger()
{
    waitButton(BUTTON_EXECUTE);
}

void blinkLed(const uint8_t ledPin)
{
    digitalWrite(ledPin, LOW);
    digitalWrite(ledPin, HIGH);
    delay(LED_DELAY);
    digitalWrite(ledPin, LOW);
}

void turnLeds(const uint8_t state)
{
    digitalWrite(FRONT_LED_PIN, state);
    digitalWrite(RIGHT_LED_PIN, state);
    digitalWrite(BACK_LED_PIN, state);
    digitalWrite(LEFT_LED_PIN, state);
    digitalWrite(EXECUTE_LED_PIN, state);
}

void setupLeds()
{
    pinMode(FRONT_LED_PIN, OUTPUT);
    pinMode(RIGHT_LED_PIN, OUTPUT);
    pinMode(BACK_LED_PIN, OUTPUT);
    pinMode(LEFT_LED_PIN, OUTPUT);
    pinMode(EXECUTE_LED_PIN, OUTPUT);

    turnLeds(OFF);
}

void doNothing()
{

}

void setupKeypad()
{
    keypad.initialize(KEYPAD_PIN);
    keypad.registerButton(Rope::Range(886, 968), doNothing);
    keypad.registerButton(Rope::Range(720, 802), doNothing);
    keypad.registerButton(Rope::Range(637, 719), doNothing);
    keypad.registerButton(Rope::Range(803, 885), doNothing);
    keypad.registerButton(Rope::Range(553, 636), doNothing);
}

void setupBuzzer()
{
    buzzer.initialize(BUZZZER_PIN);
    buzzer.setPitchVariation(0);
}

void setupMotors()
{
    motorController.initialize();
}

void setupCharger()
{
    pinMode(CHARGER_TOGGLE_PIN, HIGH);
    analogReference(DEFAULT);
}

void finishTests()
{
    for (uint8_t i = 1; i <= 3; i++)
    {
        turnLeds(ON);
        buzzer.play(Rope::Note::Cs, 7, 0.12f);
        turnLeds(OFF);
        buzzer.play(Rope::Note::C, 7, 0.10f);        
    }

    waitButton(BUTTON_EXECUTE);
}

void waitButton(const uint8_t buttonId)
{
    while (!keypad.buttonPressed(buttonId))
    {
        
    }
}

#endif // ROPE_TEST