#ifndef ROPE_MATH_CURVE_HPP
#define ROPE_MATH_CURVE_HPP

namespace Rope
{

class Curve
{
  public:
    Curve();

    virtual float getValue(const float position) const = 0;
};

} // namespace Rope

#endif // ROPE_MATH_CURVE_HPP