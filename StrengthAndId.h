#ifndef STRENGTHANDID_H
#define STRENGTHANDID_H

#include "Contestant.h"

// Needed because of circular dependency
class Contestant;

struct StrengthAndId {
    int m_strength;
    int m_id;

    StrengthAndId(int power, int id);
    StrengthAndId(const Contestant& contestant);

    bool operator<(const StrengthAndId& other) const;
    bool operator>(const StrengthAndId& other) const;
    bool operator==(const StrengthAndId& other) const;
    bool operator!=(const StrengthAndId& other) const;
};

#endif
