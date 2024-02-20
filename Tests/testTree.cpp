#include "../tree.h"
#include <cassert>
#include <iostream>

int main() {

    // Hardcoded *random* numbers without duplicates to test the tree
    int x[] = { 56, 23, 78, 12, 34, 67, 89, 46, 2, 9, 1 };
    int xSize = sizeof(x) / sizeof(x[0]);
    Tree<int> tree;

    for (int i = 0; i < xSize; i++) {
        tree.insert(x[i], x[i]);
    }

    assert(tree.size() == xSize);
    for (int i = 0; i < xSize; i++) {
        assert(tree.get(x[i]) == x[i]);
        tree.remove(x[i]);
    }
    assert(tree.size() == 0);


    std::cout << "Tree tests passed" << std::endl;

    return 0;
}
