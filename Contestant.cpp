
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
            m_teams[i] = m_teams[m_numOfTeams - 1];
            m_numOfTeams--;
            return true;
        }
    }
    return false;
}

bool Contestant::addTeam(Team* team) {
    if (m_numOfTeams == NUM_OF_TEAMS) {
        return false;
    }
    m_teams[m_numOfTeams] = team;
    m_numOfTeams++;
    return true;
}

bool Contestant::canBeDeleted() const {
    return m_numOfTeams == 0;
}
