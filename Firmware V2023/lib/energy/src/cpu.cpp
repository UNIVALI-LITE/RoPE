#include "cpu.hpp"

#include <Arduino.h>
#include <avr/power.h>
#include <avr/sleep.h>

namespace Rope
{

void Cpu::initialize(const uint8_t interruptPin)
{
    this->interruptPin_ = interruptPin;

pinMode(interruptPin, INPUT);
    attachInterrupt(digitalPinToInterrupt(interruptPin), Cpu::interruptDetected, RISING);
}

void Cpu::interruptDetected()
{
    // Do nothing here, the wake proccess will be handled by the function "wakeUp()" when
    // the CPU resumes
    //
    // There are two main reasons why we don't do the wake up process here:
    //
    // 1. According to the Arduino reference, this function must be as short as possible
    //
    // 2. According to the Arduino reference, some features and functions won't work here,
    //    for example: "millis()", "micros()", "delay()"
    //
    // Arduino interrupts reference:
    // https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
}

void Cpu::sleep(const Callback beforeSleepCallback, const Callback afterWakeCallback) const
{
    // Execute some actions before sleeping
    beforeSleepCallback();

    sleep();

    // Execute some actions after program resumes
    afterWakeCallback();
}

void Cpu::sleep() const
{
    // Disable all chip hardware that will not be used during sleep to save energy: USART serial,
    // ADC converter, ISP, TWI, etc...
    power_all_disable();

    // Choose the sleep mode. SLEEP_MODE_PWR_DOWN is the deepest level of sleep
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);

    // Allow the CPU to sleep. Without this, the call to function "sleep_mode()" does nothing
    sleep_enable();

    // Sleep the CPU now and wait until an interrupt occurs! When an interrupt occurs, the function
    // "interruptDetected()" will be executed and then the program will be resumed
    sleep_mode();

    // Thw CPU was interrupted, time to wake up and execute some post sleep initializations!! 
    wakeUp();
}

void Cpu::wakeUp() const
{
    // Prevent the CPU from sleeping after the wake up process. We only allow the cpu to sleep again when we
    // splicitly call the function "Cpu::sleep()"
    sleep_disable();

    // Some tutorials instruct to call "noInterrupts()" or "cli() after waking up. But disabling interrupts
    // also messes up with the timing functions like "millis()" and "delay()". This leads to several problems
    // and so it's not a good idea
    //
    // Arduino interrupts reference:
    // https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/
    //
    // Do not uncomment this function!! Never!! We're living this here just for documentation purposes
    // noInterrupts();

    // Re-enable all chip hardware that was disabled before sleeping
    power_all_enable();

    // If we had disabled interrupts, this function would be called to re-enable them after doing the wake up
    // process. We could also call the function "sei()" as this function has the same effect that "interrupts()"
    //
    // Do not uncomment this function!! Never!! We're living this here just for documentation purposes
    // interrupts();
}

} // namespace Rope
