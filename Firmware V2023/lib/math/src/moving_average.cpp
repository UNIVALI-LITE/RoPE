#include "moving_average.hpp"

#include <Arduino.h>

namespace Rope
{

MovingAverage::MovingAverage(const uint8_t maxSamples) : maxSamples(maxSamples)
{
    samples = new uint16_t[maxSamples];

    for (uint8_t i = 0; i < maxSamples; i++)
    {
        samples[i] = 0;
    }
}

MovingAverage::~MovingAverage()
{
    delete samples;
}

void MovingAverage::addSample(const uint16_t sample)
{
    totalSamples = min(totalSamples + 1, maxSamples);
    samples[index] = sample;
    index = (index + 1) % maxSamples;

    if (totalSamples < maxSamples)
    {
        average = UINT16_MAX;
        return;
    }

    average =  0;
    
    for (uint8_t i = 0; i < maxSamples; i++)
    {
        average = average + samples[i];
    }

    average = round(average / (totalSamples * 1.0f));
}

uint16_t MovingAverage::getValue() const
{
    return average;
}

} // namespace Rope
