#ifndef ROPE_ENERGY_CPU
#define ROPE_ENERGY_CPU

#include <stdint.h>

namespace Rope
{

class Cpu
{
public:
  typedef void (*Callback)();

private:
  uint8_t interruptPin_;
  static void interruptDetected();

  void wakeUp() const;

public:
  void initialize(const uint8_t interruptPin);
  void sleep() const;
  void sleep(const Callback beforeSleepCallback, const Callback afterWakeCallback) const;
};

} // namespace Rope

#endif // ROPE_ENERGY_CPU