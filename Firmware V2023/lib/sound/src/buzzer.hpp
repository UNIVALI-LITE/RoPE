#ifndef ROPE_SOUND_BUZZER_HPP
#define ROPE_SOUND_BUZZER_HPP

#include "note.hpp"

#include <stdint.h>

namespace Rope
{

class Buzzer
{
private:
  uint8_t outputPin_;
  int8_t pitchVariation_ = 0;
  bool muted_ = false;

  void turnOff() const;
  void applyPicthVariation(Note &note, uint8_t &octave, const int8_t variation) const;

public:
  void initialize(const uint8_t outputPin, const int8_t pitchVariation = 0);
  void play(float frequency, const float duration, const float pause = 0.0f) const;
  void play(Note note, uint8_t octave, const float duration, const float pause = 0.0f) const;
  float getFrequency(const Note note, const uint8_t octave) const;
  void setPitchVariation(const int8_t pitchVariation);
  int8_t getPitchVariation() const;
  
  bool isMuted() const;
  void setMuted(const bool muted);
};

} // namespace Rope

#endif // ROPE_SOUND_BUZZER_HPP