#include <cassert>
#include <iostream>
#include <vector>
#include <set>

#include "../Olympicsa1.h"

int main() {

    Olympics olympics;

    assert(olympics.add_team(-10, -10, Sport::SWIMMING) == StatusType::INVALID_INPUT);
    assert(olympics.add_team(10, -10, Sport::SWIMMING) == StatusType::INVALID_INPUT);
    assert(olympics.add_team(-10, 0, Sport::SWIMMING) == StatusType::INVALID_INPUT);

    assert(olympics.add_team(10, 10, Sport::SWIMMING) == StatusType::FAILURE);
    assert(olympics.add_country(10, 10) == StatusType::SUCCESS);
    assert(olympics.add_team(10, 10, Sport::SWIMMING) == StatusType::SUCCESS);
    assert(olympics.add_team(20, 10, Sport::FOOTBALL) == StatusType::SUCCESS);
    assert(olympics.add_team(20, 10, Sport::SPINNING) == StatusType::FAILURE);

    std::cout << "Passed" << std::endl;

    return 0;
}

