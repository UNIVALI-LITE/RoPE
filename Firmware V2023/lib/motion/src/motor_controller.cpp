#include "motor_controller.hpp"

#include <timer.hpp>
#include <moving_average.hpp>

namespace Rope
{

void MotorController::initialize() const
{
    DDRB = 0b11111111;                                           // Set all PORTB pins to output
    PORTB = (this->MOTOR_A_STATES[0] | this->MOTOR_B_STATES[0]); // Set both motors to state 0

    this->turnOff();
}

void MotorController::synchronizedMove(const uint32_t steps, MotorController::Direction directions[], const float maxSpeed, const Curve *accelerationCurve, const InterruptionCallback interruptionCallback)
{
    const int8_t motorAIncrement = static_cast<int8_t>(directions[0]);
    const int8_t motorBIncrement = static_cast<int8_t>(directions[1]);

    // According to the specifications in docs/motor-28BYJ-48-specs.pdf the frequency of the
    // motor model 28BYJ-48 is 100Hz which means that it can perform a maximum of 100 steps
    // per second at full speed. Each step takes 8 microsteps to complete, so at full speed
    // the motor performs 800 microsteps in 1 second
    //
    // If we divide 1 second by 800 microsteps, we find out that each microstep must take
    // exactly 0.00125 seconds or 1250 microseconds to complete for the motor to work
    // pRoperly at full speed
    //
    // In order to guarantee that each microstep will take this exact amount of time, we declare a
    // timer object. The timer works by measuring the time passed (in microseconds) between the call
    // of the functions "start()" and "waitFor()". The timer then waits for the interval to complete

    for (uint32_t currentStep = 1; currentStep <= steps; currentStep++)
    {
        float trajectoryProgress = currentStep / (steps * 1.0f);
        float speed = maxSpeed * accelerationCurve->getValue(trajectoryProgress);

        Timer timer;

        // As described before, at maximum speed, the delay between the microsteps is 1250 microseconds.
        // However we want to be able to run the motors at a different speed. We do this by passing the
        // parameter "speed" to this function. The parameter must be a float number between 0.0f (exclusive)
        // and 1.0f (inclusive) which represents the percentual of speed that will be applied to the motors
        //
        // For example, the value 1.0f represents 100% of the speed, and so has a delay of 1250 microseconds.
        // The value 0.5f represents 50% of the speed, and so has a delay of 2500 microseconds, and so on...
        //
        // The lower the speed of the motors, the higher the delay will be. So, in order to calculate the
        // correct delay, we can't simply multiply the delay by the speed because this would have the inverse
        // effect. Instead we divide 1 by the speed and then multiply the result with the delay

        const uint32_t microstepDelay = (1.0f / speed) * 1250;

        for (uint8_t microstep = 0; microstep < 8; microstep++)
        {
            timer.start();

            this->motorACurrentState = (8 + this->motorACurrentState + motorAIncrement) % 8;
            this->motorBCurrentState = (8 + this->motorBCurrentState + motorBIncrement) % 8;

            uint8_t motorAState = this->MOTOR_A_STATES[this->motorACurrentState];
            uint8_t motorBState = this->MOTOR_B_STATES[this->motorBCurrentState];

            PORTB = (motorAState | motorBState);

            if (speed == maxSpeed && currentStep % 8 == 0 && interruptionCallback())
            {
                this->turnOff();
                return;
            }

            timer.waitFor(microstepDelay, Timer::UnitOfTime::Microseconds);
        }
    }

    this->turnOff();
}

void MotorController::turnOff() const
{
    // Turn off all PORTB pins

    PORTB = 0b00000000;
}

} // namespace Rope