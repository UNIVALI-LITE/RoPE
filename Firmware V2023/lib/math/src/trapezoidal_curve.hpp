#ifndef ROPE_MATH_TRAPEZOIDAL_CURVE_HPP
#define ROPE_MATH_TRAPEZOIDAL_CURVE_HPP

#include "curve.hpp"

namespace Rope
{

class TrapezoidalCurve: public Curve
{
  private:
    const float leftCorner_;
    const float rightCorner_;
    const float initialValue_;
    const float maxValue_;
    const float finalValue_;

    float calculateSegment(const float initialPosition, const float finalPosition, const float initialValue, const float finalValue, const float currentPosition) const;

  public:
    TrapezoidalCurve(const float leftCorner, const float rightCorner, const float initialValue, const float maxValue, const float finalValue);

    float getValue(const float position) const;
};

} // namespace Rope

#endif // ROPE_MATH_TRAPEZOIDAL_CURVE_HPP