#include <cassert>
#include <iostream>
#include <vector>
#include <set>

#include "../Olympicsa1.h"

int main() {

    Olympics olympics;

    assert(olympics.remove_country(1) == StatusType::FAILURE);
    assert(olympics.remove_country(2) == StatusType::FAILURE);
    assert(olympics.remove_country(-1) == StatusType::INVALID_INPUT);
    assert(olympics.add_country(1, 0) == StatusType::SUCCESS);
    assert(olympics.remove_country(2) == StatusType::FAILURE);
    assert(olympics.remove_country(1) == StatusType::SUCCESS);
    assert(olympics.remove_country(1) == StatusType::FAILURE);

    assert(olympics.add_country(1, 0) == StatusType::SUCCESS);
    assert(olympics.remove_country(1) == StatusType::SUCCESS);

    // Now test that the country can't be removed if it has a team or contestants
    
    assert(olympics.add_country(1, 0) == StatusType::SUCCESS);
    assert(olympics.add_team(1, 1, Sport::SWIMMING) == StatusType::SUCCESS);
    assert(olympics.remove_country(1) == StatusType::FAILURE);
    assert(olympics.remove_team(1) == StatusType::SUCCESS);
    assert(olympics.remove_country(1) == StatusType::SUCCESS);

    assert(olympics.add_country(1, 0) == StatusType::SUCCESS);
    assert(olympics.add_contestant(1, 1, Sport::FOOTBALL, 1) == StatusType::SUCCESS);
    assert(olympics.remove_country(1) == StatusType::FAILURE);
    assert(olympics.remove_contestant(1) == StatusType::SUCCESS);
    assert(olympics.remove_country(1) == StatusType::SUCCESS);

    std::cout << "Passed" << std::endl;

    return 0;
}
