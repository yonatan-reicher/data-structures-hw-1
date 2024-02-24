
#include "Team.h"

Team::Team(int id, Sport sport, Country* country) {
    m_id = id;
    m_sport = sport;
    m_country = country;
    m_points = 0;
    m_strength = 0;
}

bool Team::canBeDeleted() const {
    for (int i = 0; i < NUM_OF_TREES; i++) {
        if (m_contestantIds->size() != 0)
            return false;
    }
    return true;
}
