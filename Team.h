#ifndef TEAM_H
#define TEAM_H

#include "wet1util.h"
#include "Country.h"
#include "tree.h"

#define NUM_OF_TREES 3

class Team {
public:
    int m_id;
    Sport m_sport;
    Country* m_country;
    int m_strength;
    int m_points;
    Tree<Node<int>> m_contestantIds[NUM_OF_TREES];
    Tree<Node<int>> m_contestantPowers[NUM_OF_TREES];

    Team(int id, Sport sport, Country* country);
    Team(const Team& other) = delete;
    Team(Team&& other) = delete;

    bool canBeDeleted() const;
};

#endif