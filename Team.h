#ifndef TEAM_H
#define TEAM_H

#include "wet1util.h"
#include "Country.h"
#include "Contestant.h"
#include "StrengthAndId.h"
#include "tree.h"

// Needed because of circular dependency
class Contestant;
class StrengthAndId;

#define NUM_OF_TREES 3

class Team {
public:
    int m_id;
    Sport m_sport;
    Country* m_country;
    int m_strength;
    int m_points;
    Tree<int, Contestant*> m_contestantIds[NUM_OF_TREES];
    Tree<StrengthAndId, Contestant*> m_contestantPowers[NUM_OF_TREES];

    Team(int id, Sport sport, Country* country);
    // Teams should not be copied
    Team(const Team& other) = delete;
    Team(Team&& other) = default;
    Team& operator=(const Team& other) = delete;
    Team& operator=(Team&& other) = default;

    bool canBeDeleted() const;

    int size() const;
};

#endif
