
#include "Contestant.h"

Contestant::Contestant(int id, Sport sport, Country *country, int strength) {
    m_id = id;
    m_sport = sport;
    m_country = country;
    m_numOfTeams = 0;
    m_strength = strength;
}
