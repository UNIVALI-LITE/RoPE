#ifndef ROPE_PINOUT_H
#define ROPE_PINOUT_H

// Pin used for the keypad interrupt

#define INTERRUPT_PIN 2

// Pins for the four directional LEDs

#define LEFT_LED_PIN 6
#define BACK_LED_PIN 5
#define RIGHT_LED_PIN 4
#define FRONT_LED_PIN 7
#define EXECUTE_LED_PIN 3

// Pin used to measure the battery tension/charge

#define BATTERY_PIN A7

// Pin used by the buzzer to play sounds

#define BUZZZER_PIN A2

// Analog pin used by the keypad to read the buttons pressed

#define KEYPAD_PIN A5

// Pin used to turn enable and disable the battery charger circuit

#define CHARGER_TOGGLE_PIN 17


#endif // ROPE_PINOUT_H