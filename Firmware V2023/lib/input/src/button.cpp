#include "button.hpp"

#include <Arduino.h>

namespace Rope
{

Button::Button(const Range analogRange, const Action action) : action_(action), analogRange_(analogRange)
{
}

void Button::click() const
{
    this->action_();
}

Range Button::getRange() const
{
    return this->analogRange_;
}

} // namespace Rope
