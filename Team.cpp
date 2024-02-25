#include "Team.h"

Team::Team(int id, Sport sport, Country* country) {
    m_id = id;
    m_sport = sport;
    m_country = country;
    m_strength = 0;
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
