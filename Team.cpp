#include "Team.h"

Team::Team(int id, Sport sport, Country* country) {
    m_id = id;
    m_sport = sport;
    m_country = country;
    m_strength = 0;
    m_cachedAusterity = 0;
}

bool Team::canBeDeleted() const {
    return size() == 0;
}

int Team::size() const {
    int size = 0;
    for (int i = 0; i < NUM_OF_TREES; i++) {
        size += m_contestantIds[i].size();
    }
    return size;
}

std::ostream& operator<<(std::ostream& os, const Team& team) {
    return os << "Team with trees " << team.m_contestantIds[0] << " " << team.m_contestantIds[1] << " " << team.m_contestantIds[2];
}
