#ifndef ROPE_SOUND_NOTE_HPP
#define ROPE_SOUND_NOTE_HPP

#include <stdint.h>

namespace Rope
{
// This enum defines constants for all possible basic notes.
//
// Each constant represents the distance from C0 (16.35Hz) in semi tones as
// defined in this table: http://pages.mtu.edu/~suits/notefreqs.html
enum class Note : uint8_t
{
    C = 0,
    Cs = 1,
    Db = 1,
    D = 2,
    Ds = 3,
    Eb = 3,
    E = 4,
    F = 5,
    Fs = 6,
    Gb = 6,
    G = 7,
    Gs = 8,
    Ab = 8,
    A = 9,
    As = 10,
    Bb = 10,
    B = 11
};

} // namespace Rope

#endif // ROPE_SOUND_NOTE_HPP