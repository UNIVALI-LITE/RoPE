#ifndef ROPE_MOTION_MOTOR_CONTROLLER_HPP
#define ROPE_MOTION_MOTOR_CONTROLLER_HPP

#include <curve.hpp>

#include <stdint.h>
#include <Arduino.h>

namespace Rope
{

class MotorController
{
public:
  enum class Direction : int8_t
  {
    Clockwise = 1,
    Counterclockwise = -1
  };

  typedef bool (*InterruptionCallback)();

  // According to the specifications in lib/motion/docs/motor-28BYJ-48-specs.pdf, the motor model
  // 28BYJ-48 has a stride angle of 5.625 degrees, this means that the motor takes 64 microsteps to
  // make one revolution
  //
  // The specifications also states that the motor has a gear reduction of ratio 64:1. This means that
  // for each microstep of the motor, the speed and angle is reduced 64 times, resulting in an angle of
  // 0.087890625 degrees by microstep
  //
  // In order to calculate the correct number of microsteps taken to make one revolution, we need to
  // multiply the original value by the gear reduction ratio. In other words, the amount of microsteps
  // needed to make one revolution is: 64 * 64 = 4096 microsteps
  //
  // However, we are operating the motors at the STEP level instead of the microstep level. So, to calculate
  // the number of STEPS needed to make one revolution, we need to divide by 8 because each STEP takes 8
  // microsteps to complete. The result is: 4096 / 8 = 512 steps per revolution

  static const uint16_t StepsPerRevolution = 512;

private:
  
  uint8_t motorACurrentState = 0;
  uint8_t motorBCurrentState = 0;

  #ifndef MOTORS_CONFIG_INVERTED

  const uint8_t MOTOR_A_STATES[8] = {0b00000100, 0b00001100, 0b00001000, 0b00011000, 0b00010000, 0b00110000, 0b00100000, 0b00100100};
  const uint8_t MOTOR_B_STATES[8] = {0b10000010, 0b00000010, 0b00000011, 0b00000001, 0b01000001, 0b01000000, 0b11000000, 0b10000000};
  
  #endif // MOTORS_CONFIG_INVERTED

  #ifdef MOTORS_CONFIG_INVERTED

  const uint8_t MOTOR_A_STATES[8] = {0b00001000, 0b00001100, 0b00000100, 0b00100100, 0b00100000, 0b00110000, 0b00010000, 0b00011000};
  const uint8_t MOTOR_B_STATES[8] = {0b01000001, 0b00000001, 0b00000011, 0b00000010, 0b10000010, 0b10000000, 0b11000000, 0b01000000};
  
  #endif // MOTORS_CONFIG_INVERTED


public:
  void initialize() const;
  void synchronizedMove(const uint32_t steps, MotorController::Direction directions[], const float maxSpeed, const Curve *accelerationCurve, const InterruptionCallback interruptionCallback);
  void turnOff() const;
};

} // namespace Rope

#endif // ROPE_MOTION_MOTOR_CONTROLLER_HPP