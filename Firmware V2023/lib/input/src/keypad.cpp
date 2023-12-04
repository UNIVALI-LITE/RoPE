#include "keypad.hpp"
#include <Arduino.h>
#include <timer.hpp>

#define INITIAL_REPEAT_DELAY 750
#define REPEAT_DELAY 200

namespace Rope
{

void Keypad::initialize(const uint8_t inputPin)
{
    this->inputPin_ = inputPin;
    pinMode(inputPin, INPUT);
}

void Keypad::registerButton(const Range &range, const Button::Action action)
{
    this->buttons_[this->installedButtons_] = new Button(range, action);
    this->installedButtons_++;
}

void Keypad::listen()
{
    uint16_t value = this->debounce();

    for (uint8_t i = 0; i < this->installedButtons_; i++)
    {
        Button *button = this->buttons_[i];

        if (button->getRange().contains(value))
        {
            button->click();

            Timer repeatTimer;
            repeatTimer.start();

            while (buttonPressed(i) && repeatTimer.timeElapsed(Timer::UnitOfTime::Milliseconds) < INITIAL_REPEAT_DELAY)
            {
                // Do nothing
            }

            while (buttonPressed(i))
            {
                if (repeatTimer.timeElapsed(Timer::UnitOfTime::Milliseconds) >= REPEAT_DELAY)
                {
                    button->click();
                    repeatTimer.start();
                }
            }

            return;
        }
    }
}

void Keypad::waitRelease(const Button *button) const
{
    uint16_t value = this->debounce();

    while (value > 0)
    {
        delayMicroseconds(10);
        value = this->debounce();
    }
}

void Keypad::waitRelease(const uint8_t button, const bool useDebounce) const
{
    if (button == 255)
    {
        while (anyButtonPressed())
        {
            delayMicroseconds(10);    
        }
    }
    else
    {
        while (buttonPressed(button, useDebounce))
        {
            delayMicroseconds(10);
        }
    }
}

Button *Keypad::button(uint8_t id) const
{
    Button *button = this->buttons_[id];
    return button;
}

bool Keypad::buttonPressed(uint8_t button, const bool useDebounce) const
{
    uint16_t value;
    
    if (useDebounce)
    {
        value = this->debounce();
    }
    else
    {
        value = analogRead(inputPin_);
    }     

    return this->buttons_[button]->getRange().contains(value);
}

bool Keypad::anyButtonPressed() const
{
    for (uint8_t i = 0; i < this->installedButtons_; i++)
    {
        if (this->buttonPressed(i))
        {
            return true;
        }
    }

    return false;
}

// This function debounces the buttons by analyzing the variation between the analog
// reads
//
// While the variation is above the defined treshold, we assume that signal is still
// bouncing and thus the value read is not correct
//
// When the variation between the reads fall below the treshold, we assume that the analog
// signal stopped bouncing and thus is secure to assume that the value is correct
//
// In order to achieve a more precise read, we calculate an average of the values read. This
// guarantee that the final value is closer to the desired value

uint16_t Keypad::debounce() const
{
    uint32_t reads = 2; // We start with 2 because of the two reads before the loop
    uint32_t total = 0;

    float previousValue = analogRead(this->inputPin_); // First read
    float currentValue = analogRead(this->inputPin_);  // Second read

    float variation = abs(1.0f - (currentValue / previousValue));

    total += previousValue;
    total += currentValue;

    while (variation > Keypad::debounceTreshold_)
    {
        previousValue = analogRead(this->inputPin_);
        currentValue = analogRead(this->inputPin_);
        variation = abs(1.0f - (currentValue / previousValue));

        total += previousValue;
        total += currentValue;
        reads += 1;
    }

    return total / reads; // Returns an average os the values read
}

} // namespace Rope