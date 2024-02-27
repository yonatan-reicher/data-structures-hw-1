
#include "Contestant.h"

Contestant::Contestant(int id, Sport sport, Country *country, int strength) {
    m_id = id;
    m_sport = sport;
    m_country = country;
    m_numOfTeams = 0;
    m_strength = strength;
}

bool Contestant::removeTeam(Team* team) {
    for (int i = 0; i < m_numOfTeams; i++) {
        if (m_teams[i] == team) {
            for (int j = i; j < m_numOfTeams - 1; j++) {
                m_teams[j] = m_teams[j + 1];
            }
            m_numOfTeams--;
            return true;
        }
    }
    return false;
}

bool Contestant::addTeam(Team* team) {
    if (m_numOfTeams == MAX_NUM_OF_TEAMS) {
        return false;
    }

    // Don't add the team if it's already in the list
    for (int i = 0; i < m_numOfTeams; i++) {
        if (m_teams[i] == team) {
            return false;
        }
    }

    m_teams[m_numOfTeams] = team;
    m_numOfTeams++;
    return true;
}

bool Contestant::canBeDeleted() const {
    return m_numOfTeams == 0;
}

std::ostream& operator<<(std::ostream& os, const Contestant& contestant) {
    os << "Contestant with strength " << contestant.m_strength << " of teams";
    for (int i = 0; i < contestant.m_numOfTeams; i++) {
        os << " " << contestant.m_teams[i]->m_id;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Contestant* contestant) {
    return os << *contestant;
}
