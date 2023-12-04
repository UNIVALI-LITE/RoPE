#ifndef ROPE_MATH_MOVING_AVERAGE_HPP
#define ROPE_MATH_MOVING_AVERAGE_HPP

#include <stdint.h>

namespace Rope
{

class MovingAverage
{
private:
    uint16_t *samples;

    uint8_t maxSamples;    
    uint16_t average = 0;
    uint8_t index = 0;
    uint8_t totalSamples = 0;
public:
    MovingAverage(const uint8_t maxSamples);
    ~MovingAverage();
    void addSample(const uint16_t sample);
    uint16_t getValue() const;
};

} // namespace Rope

#endif // ROPE_MATH_MOVING_AVERAGE_HPP