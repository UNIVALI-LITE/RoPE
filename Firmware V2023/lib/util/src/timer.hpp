#ifndef ROPE_UTIL_TIMER_HPP
#define ROPE_UTIL_TIMER_HPP

#include <stdint.h>
namespace Rope
{

class Timer
{
public:
  enum class UnitOfTime : uint16_t
  {
      Microseconds = 1,
      Milliseconds = 1000
  };

private:
  unsigned long startTime_;

public:
  Timer();

  void start();
  unsigned long timeElapsed(const Timer::UnitOfTime unit) const;
  void waitFor(unsigned long interval, const Timer::UnitOfTime unit) const;
};

} // namespace Rope

#endif // ROPE_UTIL_TIMER_HPP