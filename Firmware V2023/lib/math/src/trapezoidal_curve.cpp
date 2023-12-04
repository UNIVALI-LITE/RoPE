#include "trapezoidal_curve.hpp"

namespace Rope
{

TrapezoidalCurve::TrapezoidalCurve(const float leftCorner, const float rightCorner, const float initialValue, const float maxValue, const float finalValue) : leftCorner_(leftCorner), rightCorner_(rightCorner), initialValue_(initialValue), maxValue_(maxValue), finalValue_(finalValue)
{
}

float TrapezoidalCurve::calculateSegment(const float initialPosition, const float finalPosition, const float initialValue, const float finalValue, const float currentPosition) const
{
    const float a = currentPosition * (initialValue - finalValue);
    const float b = initialPosition * finalValue;
    const float c = finalPosition * initialValue;
    const float d = initialPosition - finalPosition;

    return (a + b - c) / d;
}

float TrapezoidalCurve::getValue(const float position) const
{
    if (position < leftCorner_)
    {
        return calculateSegment(0.0f, leftCorner_, initialValue_, maxValue_, position);
    }
    else if (position >= leftCorner_ && position <= rightCorner_)
    {
        return calculateSegment(leftCorner_, rightCorner_, maxValue_, maxValue_, position);
    }
    else if (position > rightCorner_)
    {
        return calculateSegment(rightCorner_, 1.0f, maxValue_, finalValue_, position);
    }

    return 0;
}

} // namespace Rope
