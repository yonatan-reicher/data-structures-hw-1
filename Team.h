#ifndef TEAM_H
#define TEAM_H

#include "wet1util.h"
#include "Country.h"
#include "Contestant.h"
#include "StrengthAndId.h"
#include "tree.h"
#include <iostream>

// Needed because of circular dependency
class Contestant;
class StrengthAndId;

#define NUM_OF_TREES 3

class Team {
public:
    int m_id;
    Sport m_sport;
    Country* m_country;
    // TODO: Remove this field!
    int m_strength;
    int m_points;
    Tree<int, Contestant*> m_contestantIds[NUM_OF_TREES];
    Tree<StrengthAndId, Contestant*> m_contestantPowers[NUM_OF_TREES];
    // TODO: We must update this everytime we make an important change that
    // affects the contestants. For example, in add_contestant_to_team,
    // remove_contestant_from_team, update_contestant_strength, etc.
    int m_cachedAusterity;

    Team(int id, Sport sport, Country* country);
    // Teams should not be copied
    Team(const Team& other) = delete;
    Team(Team&& other) = default;
    Team& operator=(const Team& other) = delete;
    Team& operator=(Team&& other) = default;

    bool canBeDeleted() const;

    int size() const;
};

std::ostream& operator<<(std::ostream& os, const Team& contestant);

#endif
