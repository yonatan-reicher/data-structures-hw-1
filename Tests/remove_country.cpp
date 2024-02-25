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

    std::cout << "Passed" << std::endl;

    return 0;
}
