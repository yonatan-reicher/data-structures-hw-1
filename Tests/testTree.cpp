#include "../tree.h"
#include <cassert>
#include <iostream>

int main() {

    Tree<int> tree;
    tree.insert(1, 2);
    tree.insert(2, 3);
    tree.insert(3, 4);

    assert(tree.get(3) == 4);
    assert(tree.size() == 3);
    assert(tree.remove(3) == 4);
    assert(tree.size() == 2);

    auto x = new int(5);

    std::cout << "Tree tests passed" << std::endl;

    return 0;
}
