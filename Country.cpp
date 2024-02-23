#include "Country.h"

Country::Country(int id, int numOfMedals) {
    m_id = id;
    m_numOfMedals = numOfMedals;
    m_numOfTeams = 0;
    m_numOfContestants = 0;
}

bool Country::canBeDeleted() const {
    return m_numOfTeams == 0 && m_numOfContestants == 0;
}
