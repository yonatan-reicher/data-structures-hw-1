#ifndef CONTESTANT_H
#define CONTESTANT_H

#include "wet1util.h"
#include "Country.h"
#include "Team.h"
#include <iostream>

// Needed because of circular dependency
class Team;

#define MAX_NUM_OF_TEAMS 3

class Contestant {
public:
    int m_id;
    Sport m_sport;
    Country* m_country;
    Team* m_teams[MAX_NUM_OF_TEAMS];
    int m_numOfTeams;
    int m_helperIndex;
    int m_strength;

    Contestant(int id, Sport sport, Country* country, int strength);

    bool removeTeam(Team* team);
    bool addTeam(Team* team);
    bool canBeDeleted() const;

};

std::ostream& operator<<(std::ostream& os, const Contestant& contestant);

#endif
