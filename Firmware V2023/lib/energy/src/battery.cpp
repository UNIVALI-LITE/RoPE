#include "battery.hpp"

#include <Arduino.h>

namespace Rope
{

Battery::Battery()
{
    analogAverage = new MovingAverage(MAX_READS);
}

Battery::~Battery()
{
    delete analogAverage;
}

void Battery::initialize(const uint8_t inputPin, const float criticalLevel)
{
    this->inputPin = inputPin;
    this->criticalLevel = criticalLevel;

    pinMode(inputPin, INPUT);
}

float Battery::getCriticalLevel() const
{
    return criticalLevel;
}

BatteryStatus Battery::readStatus()
{
    uint16_t currentValue = analogRead(inputPin);

    analogAverage->addSample(currentValue);
    
    if (analogAverage->getValue() < lastStatus.getAnalogValue())
    {
        lastStatus = BatteryStatus(++reads, analogAverage->getValue(), criticalLevel);
    }

    return lastStatus;
}

} // namespace Rope
