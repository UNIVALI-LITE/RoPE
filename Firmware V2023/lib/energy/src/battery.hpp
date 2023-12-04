#ifndef ROPE_ENERGY_BATTERY_HPP
#define ROPE_ENERGY_BATTERY_HPP

#include "battery_status.hpp"
#include <moving_average.hpp>
#include <stdint.h>

namespace Rope
{

class Battery
{
private:
  static constexpr uint8_t MAX_READS = 30;
  
  uint16_t reads = 0;
  uint8_t inputPin;
  float criticalLevel;
  
  MovingAverage *analogAverage;
  BatteryStatus lastStatus;

public:
  static constexpr float ADC_RESOLUTION = 5.0f / 1024.0f;
  
  Battery();
  ~Battery();

  void initialize(const uint8_t inputPin, const float criticalLevel);
  float getCriticalLevel() const;
  BatteryStatus readStatus();
};

} // namespace Rope

#endif // ROPE_ENERGY_BATTERY_HPP