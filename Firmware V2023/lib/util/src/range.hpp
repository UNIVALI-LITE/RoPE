#ifndef ROPE_UTIL_RANGE_HPP
#define ROPE_UTIL_RANGE_HPP

#include <stdint.h>

namespace Rope
{
  
class Range
{
private:
  uint16_t min_;
  uint16_t max_;

public:
  Range();
  Range(const uint16_t min, const uint16_t max);
  bool contains(const uint16_t value) const;
  uint16_t getMin() const;
  uint16_t getMax() const;
};

} // namespace Rope

#endif // ROPE_UTIL_RANGE_HPP