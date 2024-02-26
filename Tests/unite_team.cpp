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
    int ids[N] = { 17, 31, 74, 95, 11, 13, 58, 27, 29, 77, 15, 35, 49, 70, 41, 1, 93, 69, 68, 78, 5, 65, 9, 62, 57, 54, 25 };
    int strenghs[N] = { 33, 73, 48, 31, 57, 29, 88, 46, 75, 1, 61, 4, 23, 25, 77, 93, 82, 29, 56, 93, 74, 8, 76, 63, 47, 14, 87 };
    for (int i = 0; i < N; i++) {
        assert(olympics3.add_contestant(ids[i], 1, Sport::SWIMMING, strenghs[i]) == StatusType::SUCCESS);
        assert(olympics3.add_contestant_to_team(i % 3 == 0 ? 1 : 2, ids[i]) == StatusType::SUCCESS);
    }
    /*
    ids1: [1, 17, 49, 57, 58, 65, 68, 77, 95]
    strs1: [93, 33, 23, 47, 88, 8, 56, 1, 31]
    total strength: 93 + 88 + 56 = 237

    ids2: [5, 9, 11, 13, 15, 25, 27, 29, 31, 35, 41, 54, 62, 69, 70, 74, 78, 93]
    strs2: [74, 76, 57, 29, 61, 87, 46, 75, 73, 4, 77, 14, 63, 29, 25, 48, 93, 82]
    total strength: 76 + 87 + 75 + 77 + 63 + 93 = 257

    [1, 5, 9, 11, 13, 15, 17, 25, 27, 29, 31, 35, 41, 49, 54, 57, 58, 62, 65, 68, 69, 70, 74, 77, 78, 93, 95]
    [93, 74, 76, 57, 29, 61, 33, 87, 46, 75, 73, 4, 77, 23, 14, 47, 88, 63, 8, 56, 29, 25, 48, 1, 93, 82, 31]
    strs after unite: 274
    */
    assert(olympics3.get_team_strength(1).status() == StatusType::SUCCESS);
    assert(olympics3.get_team_strength(1).ans() == 237);
    assert(olympics3.get_team_strength(2).status() == StatusType::SUCCESS);
    assert(olympics3.get_team_strength(2).ans() == 257);
    assert(olympics3.unite_teams(1, 2) == StatusType::SUCCESS);
    assert(olympics3.get_team_strength(1).status() == StatusType::SUCCESS);
    assert(olympics3.get_team_strength(1).ans() == 274);

    std::cout << "Passed" << std::endl;

    return 0;
}

