#include "StrengthAndId.h"

StrengthAndId::StrengthAndId(int strength, int id) {
    this->m_strength = strength;
    this->m_id = id;
}

StrengthAndId::StrengthAndId(const Contestant& contestant)
: StrengthAndId(contestant.m_strength, contestant.m_id)
{}

bool StrengthAndId::operator<(const StrengthAndId& other) const {
    if (m_strength < other.m_strength)
        return true;
    if (m_strength > other.m_strength)
        return false;
    return m_id < other.m_id;
}

bool StrengthAndId::operator>(const StrengthAndId& other) const {
    return other < *this;
}

bool StrengthAndId::operator==(const StrengthAndId& other) const {
    return m_strength == other.m_strength && m_id == other.m_id;
}

