#include "range.hpp"

namespace Rope
{

Range::Range()
{

}

Range::Range(const uint16_t min, const uint16_t max) : min_(min), max_(max)
{
}

bool Range::contains(const uint16_t value) const
{
    return (value >= this->min_ && value <= this->max_);
}

uint16_t Range::getMin() const
{
    return this->min_;
}

uint16_t Range::getMax() const
{
    return this->max_;
}

} // namespace Rope
