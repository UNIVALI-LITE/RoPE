#ifndef ROPE_ENERGY_BATTERY_STATUS_HPP
#define ROPE_ENERGY_BATTERY_STATUS_HPP

#include <stdint.h>

namespace Rope
{
class BatteryStatus
{

private:  
  uint16_t index = 0;
  uint16_t analogValue = 1023;
  float criticalLevel = 0.0f;

public:
  BatteryStatus();
  BatteryStatus(const uint16_t index, const uint16_t analogValue, const float criticalLevel);

  uint16_t getIndex() const;
  uint16_t getAnalogValue() const;
  float getVoltage() const;
  float getAdcVoltage() const;
  float getLevel() const;
  bool isEmpty() const;
  bool isFull() const;
  bool isCritical() const;
};

} // namespace Rope

#endif // ROPE_ENERGY_BATTERY_STATUS_HPP