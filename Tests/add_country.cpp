#include <cassert>
#include <iostream>
#include <vector>
#include <set>

#include "../Olympicsa1.h"

int main() {

    Olympics olympics;

    assert(olympics.get_medals(1).status() == StatusType::FAILURE);
    assert(olympics.add_country(1, 0) == StatusType::SUCCESS);
    assert(olympics.get_medals(1).status() == StatusType::SUCCESS);
    assert(olympics.get_medals(1).ans() == 0);
    assert(olympics.add_country(1, 0) == StatusType::FAILURE);

    std::cout << "Passed" << std::endl;

    return 0;
}
