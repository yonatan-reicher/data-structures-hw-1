#include <cassert>
#include <iostream>
#include <vector>
#include <set>

#include "../Olympicsa1.h"

int main() {

    Olympics olympics1;

    // Unite empty teams
    assert(olympics1.add_country(1, 0) == StatusType::SUCCESS);
    assert(olympics1.unite_teams(1, 2) == StatusType::FAILURE);
    assert(olympics1.add_team(1, 1, Sport::SWIMMING) == StatusType::SUCCESS);
    assert(olympics1.add_team(2, 1, Sport::SWIMMING) == StatusType::SUCCESS);
    assert(olympics1.unite_teams(1, 1) == StatusType::INVALID_INPUT);
    assert(olympics1.unite_teams(1, 2) == StatusType::SUCCESS);
    assert(olympics1.get_team_strength(1).status() == StatusType::SUCCESS);
    assert(olympics1.get_team_strength(1).ans() == 0);
    assert(olympics1.get_team_strength(2).status() == StatusType::FAILURE);

    // Unite empty team to non-empty team
    Olympics olympics2;
    assert(olympics2.add_country(1, 0) == StatusType::SUCCESS);
    assert(olympics2.add_team(1, 1, Sport::SWIMMING) == StatusType::SUCCESS);
    assert(olympics2.add_team(2, 1, Sport::SWIMMING) == StatusType::SUCCESS);
    assert(olympics2.add_contestant(1, 1, Sport::SWIMMING, 1) == StatusType::SUCCESS);
    assert(olympics2.add_contestant(2, 1, Sport::SWIMMING, 2) == StatusType::SUCCESS);
    assert(olympics2.add_contestant(3, 1, Sport::SWIMMING, 3) == StatusType::SUCCESS);
    assert(olympics2.add_contestant_to_team(1, 1) == StatusType::SUCCESS);
    assert(olympics2.add_contestant_to_team(1, 2) == StatusType::SUCCESS);
    assert(olympics2.add_contestant_to_team(1, 3) == StatusType::SUCCESS);
    assert(olympics2.unite_teams(1, 2) == StatusType::SUCCESS);
    assert(olympics2.get_team_strength(1).status() == StatusType::SUCCESS);
    assert(olympics2.get_team_strength(1).ans() == 6);
    assert(olympics2.get_team_strength(2).status() == StatusType::FAILURE);

    // General test
    Olympics olympics3;
    assert(olympics3.add_country(1, 10) == StatusType::SUCCESS);
    assert(olympics3.add_team(1, 1, Sport::SWIMMING) == StatusType::SUCCESS);
    assert(olympics3.add_team(2, 1, Sport::SWIMMING) == StatusType::SUCCESS);
    // 30 Random contestants with unique ids
    constexpr int N = 27;
    int ids[N] =      { 17, 31, 74, 95, 11, 13, 58, 27, 29, 77, 15, 35, 49, 70, 41, 1, 93, 69, 68, 78, 5, 65, 9, 62, 57, 54, 25 };
    int strenghs[N] = { 33, 73, 48, 31, 57, 29, 88, 46, 75, 1,  61, 4, 23, 25, 77, 93, 82, 29, 56, 73, 74, 8, 76, 63, 97, 14, 87 };
    for (int i = 0; i < N; i++) {
        assert(olympics3.add_contestant(ids[i], 1, Sport::SWIMMING, strenghs[i]) == StatusType::SUCCESS);
        assert(olympics3.add_contestant_to_team(i % 3 == 0 ? 1 : 2, ids[i]) == StatusType::SUCCESS);
    }
    assert(olympics3.get_team_strength(1).status() == StatusType::SUCCESS);
    assert(olympics3.get_team_strength(1).ans() == 246);
    assert(olympics3.get_team_strength(2).status() == StatusType::SUCCESS);
    assert(olympics3.get_team_strength(2).ans() == 246);
    assert(olympics3.unite_teams(1, 2) == StatusType::SUCCESS);
    assert(olympics3.get_team_strength(1).status() == StatusType::SUCCESS);
    assert(olympics3.get_team_strength(1).ans() == 272);

    // Also test austerity measures
    assert(olympics3.austerity_measures(1).status() == StatusType::SUCCESS);
    assert(olympics3.austerity_measures(1).ans() == 278);

    std::cout << "Passed" << std::endl;

    return 0;
}

