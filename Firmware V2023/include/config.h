#ifndef ROPE_CONFIG_H
#define ROPE_CONFIG_H

#include <stdint.h>
#include <range.hpp>
#include <trapezoidal_curve.hpp>

namespace Rope
{

class Config
{
  public:

    // Defines how much the buzzer will change the picth of played notes, that is,
    // how many semitones the buzzer will increase or decrease the original note
    // before playing it
    //
    // For example, suppose that the buzzer is playing the note A4
    //
    // - If the pitch variation is set to 1, then the note A4 will be incrceased by
    //   one semitone, becoming the next note in the musical scale, which is A#4 or Bb4
    //   
    // - If the picth variation is set to 2, then the note A4 will be increased by 
    //   two semitones in the musical scale, becoming the note B4
    //
    // - If the pitch variation is set to -1, then the note A4 will be decreased by
    //   one semitone, becoming the prior note in the musical scale, which is Ab4 or G#4
    //   
    // - If the picth variation is set to -2, then the note A4 will be decreased by 
    //   two semitones in the musical scale, becoming the note G4
    //
    // - If the pitch variation is set to 0, then no changes will be made to the note
    //   and it will still be A4
    //
    // Once set, the pitch variation affects all the notes played on the buzzer. So, by
    // changing the pitch variation, is possible to change the sonority of all melodies
    // at once
    static constexpr int8_t buzzerPitchVariation = 0;

   // static constexpr uint8_t programSize = 100;

    static constexpr uint16_t delayBetweenInstructions = 150;

    // interval to alert user of idle status (5min = 300000) 
    static constexpr uint32_t idleAlertInterval = 300000;
   
    // The number of times that the critical battery feedback will play before sleeping 
    // the CPU. Max: 255 repetitions
    static constexpr uint8_t batteryWarningRepetitions = 20;

    // The delay in seconds between each critical battery feedback. Max: 255 seconds
    static constexpr uint8_t delayBetweenBatteryWarnings = 30;

    // Distance in millimeters (D) = 92
    // Whell width (W) = 6
    // Distance between wheels = D - W = 92 - 6 = 86
    static constexpr float distanceBetweenWheels = 69.00;

    static constexpr float wheelsDiameter = 70.10f;

    static constexpr float maxMovingSpeed = 2.3f;

    static constexpr float maxTurningSpeed = 2.0f;

    static constexpr float movingDistance = 120.0f; // mm

    static constexpr int8_t deadZoneCorrection = 2;

    // This variable defines the minimum level that the battery can reach before the program entering 
    // "Critical Mode". The value represents a percentual of the total charge (voltage) of the battery 
    // and ranges from 0.0f to 1.0f, meaning 0% and 100% respectivelly.
    //
    // For example, a value of 0.12f means that the program will enter "Critical Mode" if the battery 
    // drops below 12% of its total charge.
    //
    // In "Critical Mode", the microcontroller power off itself and stop processing commands. Interrupts may
    // still occur from external sources such as keyboard, serial or timers and thus wake up the microcontroller.
    // However, they will simply be ignored and the microcontroller will power off again right after the interrupt.
    //
    // The recommended value is usually 15% of the battery voltage because below this treshold the battery lifespan 
    // tends to decrease. This affirmative is valid in the majority of the cases and can be used as a rule of thumb,
    // however specific batteries may demand different tresholds
    static constexpr float criticalBatteryLevel = 0.10f;

    static const Curve *movingCurve;

    static const Curve *turningCurve;
};

const Curve* Config::movingCurve = new TrapezoidalCurve(0.15f, 0.90f, 0.30f, 1.0f, 0.0f);

const Curve* Config::turningCurve = new TrapezoidalCurve(0.30f, 0.70f, 0.30f, 1.0f, 0.0f);

} // namespace Rope

#endif // ROPE_CONFIG_H