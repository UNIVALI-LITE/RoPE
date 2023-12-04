#include "program.hpp"

#include <debug.h>
#include <Arduino.h>

namespace Rope
{

void Program::initialize(const uint8_t capacity, const StartCallback startCallback, const InstructionCallback instructionCallback, const TerminationCallback terminationCallback)
{
    this->capacity_ = capacity;
    this->startCallback_ = startCallback;
    this->instructionCallback_ = instructionCallback;
    this->terminationCallback_ = terminationCallback;
}

bool Program::addInstruction(const Instruction instruction)
{
    if (this->instructionsAdded_ < this->capacity_)
    {
        this->instructions_[this->instructionsAdded_] = instruction;
        this->instructionsAdded_++;

        return true;
    }

    return false;
}

bool Program::isFull() const
{
    return this->instructionsAdded_ == this->capacity_;
}

void Program::clear()
{
    this->instructionsAdded_ = 0;
}

void Program::execute()
{
    this->canceled_ = false;
    this->executing_ = true;
    this->currentInstruction_ = 0;

    this->startCallback_(*this);

    while (this->executing_ && this->currentInstruction_ < this->instructionsAdded_)
    {
        __debug__(Serial.print("Executing instruction "));
        __debug__(Serial.print(this->currentInstruction_ + 1));
        __debug__(Serial.print(" of "));
        __debug__(Serial.println(this->instructionsAdded_));
        __debug__(Serial.flush());

        Instruction instruction = this->instructions_[this->currentInstruction_];

        instruction();
        this->instructionCallback_(*this, instruction, this->currentInstruction_);

        this->currentInstruction_++;
    }

    this->executing_ = false;

    this->terminationCallback_(*this, (this->canceled_) ? Termination::Canceled : Termination::Normal);
}

void Program::cancel()
{
    this->executing_ = false;
    this->canceled_ = true;
}

bool Program::isEmpty() const
{
    return this->instructionsAdded_ == 0;
}

bool Program::isExecuting() const
{
    return this->executing_;
}

bool  Program::checkForDemoMode(const Instruction Left, const Instruction Right) const
{
 
    Instruction pattern [12] = {Left, Right, Left, Left, Right, Right, Right, Left, Left, Left, Left, Left}; 
    int8_t match = 0;

    for (int8_t i=0; i < 12; i++){
        if (this->instructions_[i] == pattern [i]){
            match++;
        }
    }
    return (match==12);
}
} // namespace Rope