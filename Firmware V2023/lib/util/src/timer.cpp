#include "timer.hpp"

#include <Arduino.h>
#include <limits.h>

namespace Rope
{

Timer::Timer()
{
}

void Timer::start()
{
    this->startTime_ = micros();
}

// This function is not very accurate because it uses the function "micros()" to measure the
// time, which has a resolution of 8us in a 8MHz chip, according to the Arduino reference
//
// Also, the function itself takes several CPU cycles to complete, which can make the delay
// slightly higher than the desired
//
// However, for this project, the level of error is acceptable
//
// Function "micros()": https://www.arduino.cc/reference/en/language/functions/time/micros/

void Timer::waitFor(unsigned long interval, const Timer::UnitOfTime unit) const
{
    // Get interval always in Microseconds
    //
    // Example: waitFor(750, Timer::UnitOfTime::Microseconds) = 750 * 1 = 750 microseconds
    // Example: waitFor(750, Timer::UnitOfTime::Milliseconds) = 750 * 1000 = 750000 microseconds

    interval = interval * static_cast<uint16_t>(unit);

    unsigned long timeElapsed = this->timeElapsed(UnitOfTime::Microseconds);

    if (timeElapsed < interval)
    {
        unsigned long timeRemaining = interval - timeElapsed;

        if (timeRemaining > 1000)
        {
            unsigned long microseconds = timeRemaining % 1000;
            unsigned long milliseconds = timeRemaining / 1000;

            delay(milliseconds);
            delayMicroseconds(microseconds);
        }
        else
        {
            delayMicroseconds(timeRemaining);
        }
    }
}

unsigned long Timer::timeElapsed(const Timer::UnitOfTime unit) const
{
    // According to the Arduino reference, the function "micros()" will overflow after approximately
    // 70 minutes. When this occurs, the value returned from the function will start from 0 and
    // the difference between the current time and the start time will consequently be negative
    //
    // At this point, storing the result of the calculation in a UNSIGNED long variable would implicitly
    // convert it to a positive value using two's complement and preventing us to detect that a overflow
    // occurred. So, we need to store this value in a SIGNED long variable. Also, we need to handle the
    // ovetflow when it occurs
    //
    // Function "micros()": https://www.arduino.cc/reference/en/language/functions/time/micros/
    // Two's complement: https://en.wikipedia.org/wiki/Two%27s_complement

    long timeElapsed = micros() - this->startTime_;

    if (timeElapsed >= 0)
    {
        return timeElapsed / static_cast<uint16_t>(unit);
    }

    return (ULONG_MAX + 1 + timeElapsed) / static_cast<uint16_t>(unit);
}

} // namespace Rope
