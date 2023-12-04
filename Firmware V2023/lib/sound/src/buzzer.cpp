#include "buzzer.hpp"

#include <Arduino.h>
#include <math.h>

namespace Rope
{

void Buzzer::initialize(const uint8_t outputPin, const int8_t pitchVariation)
{
    this->outputPin_ = outputPin;
    this->pitchVariation_ = pitchVariation;

    pinMode(outputPin, OUTPUT);

    this->turnOff();
}

void Buzzer::play(float frequency, const float duration, const float pause) const
{
    if (!this->muted_)
    {
        uint8_t distanceFromC0 = round(log10(frequency / 16.35f) / log10(1.059463094359f));
        uint8_t noteValue = distanceFromC0 % 12;
        uint8_t octave = (distanceFromC0 - noteValue) / 12;
        Note note = (Note) noteValue;

        this->applyPicthVariation(note, octave, this->pitchVariation_); // Need to calculate
        
        frequency = this->getFrequency(note, octave);

        tone(this->outputPin_, round(frequency));
    }

    delay(roundf(duration * 1000));
    this->turnOff();

    if (pause > 0.0f)
    {
        delay(roundf(duration * 1000));
    }
}

void Buzzer::play(Note note, uint8_t octave, const float duration, const float pause) const
{
    float frequency = this->getFrequency(note, octave);
    
    this->play(frequency, duration, pause);
}

// This function calculates the frequency of a given note in a given octave. The frequency is
// calculated using the formula described here: https://pages.mtu.edu/~suits/NoteFreqCalcs.html
//
// The difference between the formula explained in the site and this function is that here we have
// fixed the base frequency to match the note "C0", which has a frequency of 16.35Hz according to
// this table: https://pages.mtu.edu/~suits/notefreqs.html
//
// Basically we count how many semitones the given note (and octave) is away from C0. Once we have
// get this value, just play it in the formula and you get the corresponding frequency

float Buzzer::getFrequency(const Note note, const uint8_t octave) const
{
    const uint8_t distanceFromC0 = (12 * octave) + static_cast<uint8_t>(note);

    return 16.35f * pow(1.059463094359f, distanceFromC0);
}

// This function takes one note in a given octave and turns it into another note in another octave,
// based on a specific amount of semitones
//
// In simple words, we can take a note "D7", for example, and turn it into a note "G7" by increasing
// 5 semitones: D7 -> D#7 -> E -> F -> F# -> G7
//
// We can also turn the "D7" into a "F6" by decreasing 9 semitones: D7 -> Db7 -> C7 -> B6 -> Bb6 ->
// A6 -> Ab6 -> G6 -> Gb6 -> F6
//
// To acomplish this, first we need to calculate how many semitones the target note is away from C0.
// Once we have this distance, is easy to obtain the note and octave

void Buzzer::applyPicthVariation(Note &note, uint8_t &octave, const int8_t variation) const
{
    uint8_t distanceFromC0 = (12 * octave) + static_cast<uint8_t>(note) + variation;

    octave = distanceFromC0 / 12;
    note = static_cast<Note>(distanceFromC0 % 12);
}

void Buzzer::setPitchVariation(const int8_t pitchVariation)
{
    this->pitchVariation_ = pitchVariation;
}

int8_t Buzzer::getPitchVariation() const
{
    return this->pitchVariation_;
}

bool Buzzer::isMuted() const
{
    return this->muted_;
}

void Buzzer::setMuted(const bool muted)
{
    this->muted_ = muted;
}

void Buzzer::turnOff() const
{
    noTone(this->outputPin_);
}

} // namespace Rope