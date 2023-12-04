#ifndef ROPE_PROGRAM_HPP
#define ROPE_PROGRAM_HPP

#include <stdint.h>

namespace Rope
{

class Program
{
public:
  enum class Termination
  {
    Normal,
    Canceled
  };

  typedef void (*Instruction)();
  typedef void (*StartCallback)(Program &program);
  typedef void (*TerminationCallback)(Program &program, const Termination result);
  typedef void (*InstructionCallback)(Program &program, const Instruction instruction, const uint8_t index);

private:
  Instruction instructions_[PROGRAM_SIZE];

  uint8_t capacity_;
  uint8_t instructionsAdded_ = 0;
  uint8_t currentInstruction_ = 0;

  bool executing_ = false;
  bool canceled_ = false;

  StartCallback startCallback_;
  InstructionCallback instructionCallback_;
  TerminationCallback terminationCallback_;

public:
  void initialize(const uint8_t capacity, const StartCallback startCallback, const InstructionCallback instructionCallback, const TerminationCallback terminationCallback);
  bool addInstruction(const Instruction instruction);
  bool isFull() const;
  bool isEmpty() const;
  void clear();
  void execute();
  void cancel();
  bool isExecuting() const;
  bool checkForDemoMode(const Instruction Left, const Instruction Right) const;

}; // namespace Rope

}
#endif // ROPE_PROGRAM_HPP