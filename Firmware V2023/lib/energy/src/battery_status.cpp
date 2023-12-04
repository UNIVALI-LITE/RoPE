#include "battery_status.hpp"

#include "battery.hpp"
#include <range.hpp>

#include <Arduino.h>

namespace Rope
{

BatteryStatus::BatteryStatus()
{

}

BatteryStatus::BatteryStatus(const uint16_t index, const uint16_t analogValue, const float criticalLevel) : index(index), analogValue(analogValue), criticalLevel(criticalLevel)
{

}

uint16_t BatteryStatus::getIndex() const
{
    return index;
}

uint16_t BatteryStatus::getAnalogValue() const
{
    return analogValue;
}

float BatteryStatus::getVoltage() const
{   
    // The battery is not connected directly into the analog port. Instead it is
    // connected to a NPN transistor whose emitter is then connected to the analog 
    // port
    // 
    // This transistor when saturated causes a voltage drop of about 0.3 volts, giving
    // a wrong measurement. For this reason we need to compensate this voltage drop
    // adding 0.3f to the value read from the analog port

    // return getAdcVoltage() + 0.3f;
    // 
    // The code used to work in the past. But one day, suddenly, the transistor stopped
    // dropping the voltage and then we commented out this line of code. No one knows why!!

    return getAdcVoltage();
}

float BatteryStatus::getAdcVoltage() const
{
    return analogValue * Battery::ADC_RESOLUTION;
}

float BatteryStatus::getLevel() const
{
    uint16_t intVoltage = getVoltage() * 1000;

    if (intVoltage >= 3651)
    {
        return min(map(intVoltage, 3651, 3700, 75, 100) / 100.0f, 1.0f);
    }
    else if (intVoltage >= 3601 && intVoltage <= 3650)
    {
        return map(intVoltage, 3601, 3650, 51, 74) / 100.0f;
    }
    else if (intVoltage >= 3551 && intVoltage <= 3600)
    {
        return map(intVoltage, 3551, 3600, 25, 50) / 100.0f;
    }
    else if (intVoltage >= 3100 && intVoltage <= 3550)
    {
        return max(map(intVoltage, 3100, 3550, 0, 24) / 100.0f, 0.0f);
    }

    return 0.0f;
}

bool BatteryStatus::isEmpty() const
{
    return getLevel() <= 0.0f;
}

bool BatteryStatus::isFull() const
{
    return getLevel() >= 1.0f;
}

bool BatteryStatus::isCritical() const
{
    return getLevel() <= criticalLevel;
}

} // namespace Rope
