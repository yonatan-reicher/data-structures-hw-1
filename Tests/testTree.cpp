#include "../tree.h"
#include <cassert>
#include <iostream>
#include <vector>
#include <set>

std::vector<int> acsendingData = std::vector<int>{ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
std::vector<int> randomData = std::vector<int>{ 23, 1, 1, 2, 23, 5, 6, 7, 3 };

int countDistinct(std::vector<int> data) {
    std::set<int> distinct;
    for (size_t i = 0; i < data.size(); i++) {
        distinct.insert(data[i]);
    }
    return distinct.size();
}

void testData(const std::vector<int>& data) {
    int distinct = countDistinct(data);
    Tree<int, int> tree;

    for (size_t i = 0; i < data.size(); i++) {
        tree.insert(data[i], data[i]);
    }
    assert(tree.size() == distinct);

    for (size_t i = 0; i < data.size(); i++) {
        assert(tree.get(data[i]) == data[i]);
    }
    for (size_t i = 0; i < data.size(); i++) {
        if (tree.contains(data[i])) {
            assert(tree.remove(data[i]) == data[i]);
        }
    }
    assert(tree.size() == 0);
}

int main() {

    testData(acsendingData);
    testData(randomData);

    std::cout << "Tree tests passed" << std::endl;

    return 0;
}
