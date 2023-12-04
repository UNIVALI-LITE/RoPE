#ifndef ROPE_INPUT_BUTTON_HPP
#define ROPE_INPUT_BUTTON_HPP

#include <stdint.h>
#include <range.hpp>

namespace Rope
{

class Button
{
public:
  typedef void (*Action)();

private:
  const Action action_;
  const Range analogRange_;

public:  
  Button(const Range analogRange, const Action action);
  void click() const;
  Range getRange() const;
};

} // namespace Rope

#endif // ROPE_INPUT_BUTTON_HPP