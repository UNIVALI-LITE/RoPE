#ifndef ROPE_INPUT_KEYPAD_HPP
#define ROPE_INPUT_KEYPAD_HPP

#include <button.hpp>
#include <stdint.h>

namespace Rope
{

class Keypad
{
  
private:

  // Defines the maximum amount of variation that will be tolerated between the
  // analog reads during the detection of button presses
  static constexpr float debounceTreshold_ = 0.02f;

  static constexpr uint8_t MAX_BUTTONS = 10;

  uint8_t inputPin_;
  uint8_t installedButtons_ = 0;

  Button *buttons_[MAX_BUTTONS];

  void waitRelease(const Button *button) const;
  uint16_t debounce() const;

public:
  void initialize(const uint8_t inputPin);
  void listen();
  void registerButton(const Range &range, const Button::Action action);
  Button *button(uint8_t id) const;
  bool buttonPressed(uint8_t button, const bool useDebounce = true) const;
  bool anyButtonPressed() const;
  void waitRelease(const uint8_t button = 255, const bool useDebounce = true) const;
};

} // namespace Rope

#endif // ROPE_INPUT_KEYPAD_HPP