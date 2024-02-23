#ifndef DSTREE_H
#define DSTREE_H

#include <memory>
#include <cassert>
#include <iostream>
#include "node.h"

struct NotFoundException : public std::exception {
    const char* what() const throw() {
        return "Key not found";
    }
};

template <class T>
auto operator<<(std::ostream& os, const std::unique_ptr<Node<T>>& node) -> std::ostream&;

// Functions marked with "bt" are binary tree functions.
// Functions marked with "avl" are AVL tree functions.

template <class T>
T& btInsert(std::unique_ptr<Node<T>>& root, int key, T data) {
    if (root == nullptr) {
        root = std::unique_ptr<Node<T>>(new Node<T>(key, std::move(data)));
        return root->data;
    }

    if (key == root->key) {
        root->data = std::move(data);
        return root->data;
    }

    if (key < root->key) {
        std::unique_ptr<Node<T>> left = root->popLeft();
        T& ret = btInsert(left, key, std::move(data));
        root->setLeft(std::move(left));
        return ret;
    } else {
        std::unique_ptr<Node<T>> right = root->popRight();
        T& ret = btInsert(right, key, std::move(data));
        root->setRight(std::move(right));
        return ret;
    }
}

template <class T>
std::unique_ptr<Node<T>> popRightmost(std::unique_ptr<Node<T>>& node) {
    assert(node != nullptr);

    if (node->getRight() == nullptr) {
        std::unique_ptr<Node<T>> ret = std::move(node);
        node = ret->popLeft();
        return ret;
    }

    std::unique_ptr<Node<T>> right = node->popRight();
    std::unique_ptr<Node<T>> ret = popRightmost(right);
    node->setRight(std::move(right));
    return ret;
}

template <class T>
T btRemove(std::unique_ptr<Node<T>>& root, int key, int* keyOfSwitched) {
    if (root == nullptr) {
        throw NotFoundException();
    }

    T result;
    if (root->key == key && root->getLeft() == nullptr) {
        result = std::move(root->data);
        root = root->popRight();
    } else if (root->key == key && root->getRight() == nullptr) {
        result = std::move(root->data);
        root = root->popLeft();
    } else if (root->key == key) {
        /*     5              5            5   4
         *    / \                             / \
         *    2  7    ==>    2  7    ==>     2   7
         *   / \            /|              / \
         *  1   4          1 | 4           1   3
         *     /             |      
         *    3              3      
         */
        // Find the rightmost node in the left subtree. Swap the data.
        // This should be done *carefully* so no uniqueptr is cleaned up!
        std::unique_ptr<Node<T>> left = root->popLeft();
        std::unique_ptr<Node<T>> right = root->popRight();
        std::unique_ptr<Node<T>> rightmost = popRightmost(left);
        root = std::move(rightmost);
        root->setLeft(std::move(left));
        root->setRight(std::move(right));
        if (keyOfSwitched != nullptr) {
            *keyOfSwitched = root->key;
        }
        return root->data;
    } else if (key < root->key) {
        std::unique_ptr<Node<T>> left = root->popLeft();
        result = btRemove(left, key, keyOfSwitched);
        root->setLeft(std::move(left));
    } else {
        std::unique_ptr<Node<T>> right = root->popRight();
        result = btRemove(right, key, keyOfSwitched);
        root->setRight(std::move(right));
    }
    return std::move(result);
}

template <class T>
T& btGet(Node<T>* root, int key) {
    if (root == nullptr) {
        throw NotFoundException();
    }

    if (root->key == key) {
        return root->data;
    }

    if (key < root->key) {
        return btGet(root->getLeft().get(), key);
    } else {
        return btGet(root->getRight().get(), key);
    }
}

template <class T>
void rotate(std::unique_ptr<Node<T>>& root, bool right) {
    assert(root != nullptr);
    const std::unique_ptr<Node<T>>& bRef = right ? root->getLeft() : root->getRight();
    assert(bRef != nullptr);

    // This is the action we want to perform:
    /*        A    right   B
     *       / \    ==>   / \
     *      B   C   <==  D   A
     *     / \     left     / \
     *    D   E            E   C
     */

    // Here we use the names from the diagram above (of the right rotation).
    std::unique_ptr<Node<T>> a = std::move(root);
    std::unique_ptr<Node<T>> b = right ? a->popLeft() : a->popRight();
    std::unique_ptr<Node<T>> e = right ? b->popRight() : b->popLeft();
    // C and D are not needed.

    // Now we reassemble the tree. Must go from bottom to top because of moves.
    if (right) {
        a->setLeft(std::move(e));
        b->setRight(std::move(a));
        root = std::move(b);
    } else {
        a->setRight(std::move(e));
        b->setLeft(std::move(a));
        root = std::move(b);
    }
}

template <class T>
void rotateLLOrRR(std::unique_ptr<Node<T>>& root, bool ll) {
    // This is just a single rotation.
    rotate(root, ll);
}

template <class T>
void rotateLROrRL(std::unique_ptr<Node<T>>& root, bool lr) {
    /* This is a double rotation.
     *      A             E
     *     / \    LR     / \
     *    B   C   ==>   B   A
     *   / \       ^   / \ / \
     *  D   E      |  D  F G  C
     *     / \     |   
     *    F   G    --- This is a left rotation on B then right rotation on A.
     */

    std::unique_ptr<Node<T>> child = lr ? root->popLeft() : root->popRight();
    rotate(child, !lr);
    if (lr) root->setLeft(std::move(child));
    else root->setRight(std::move(child));

    rotate(root, lr);
}

enum class RebalanceType {
    LL, RR, LR, RL, NONE
};

RebalanceType chooseRebalanceByBalanceFactor( int atRoot, int atLeft, int atRight) {
    assert(-2 <= atRoot && atRoot <= 2);
    assert(-1 <= atLeft && atLeft <= 1);
    assert(-1 <= atRight && atRight <= 1);

    if (atRoot == 2 && atLeft > -1) return RebalanceType::LL;
    if (atRoot == 2 && atLeft == -1) return RebalanceType::LR;
    if (atRoot == -2 && atRight < 1) return RebalanceType::RR;
    if (atRoot == -2 && atRight == 1) return RebalanceType::RL;
    return RebalanceType::NONE;
}

// Returns true iff the tree was rebalanced.
template <class T>
bool rebalanceRoot(std::unique_ptr<Node<T>>& root) {
    if (root == nullptr) {
        return false;
    }

    int atRoot = balanceFactor(root);
    int atLeft = balanceFactor(root->getLeft());
    int atRight = balanceFactor(root->getRight());

    switch (chooseRebalanceByBalanceFactor(atRoot, atLeft, atRight)) {
        case RebalanceType::LL:
            rotateLLOrRR(root, true);
            return true;
        case RebalanceType::RR:
            rotateLLOrRR(root, false);
            return true;
        case RebalanceType::LR:
            rotateLROrRL(root, true);
            return true;
        case RebalanceType::RL:
            rotateLROrRL(root, false);
            return true;
        case RebalanceType::NONE:
            return false;
        default:
            throw std::exception();
    }
}

template <class T>
bool rebalanceBranch(std::unique_ptr<Node<T>>& root, int pathKey, bool once) {
    if (root == nullptr) {
        return false;
    }

    bool rightChild = pathKey > root->key;
    std::unique_ptr<Node<T>> next = rightChild ? root->popRight() : root->popLeft();

    bool rebalanced = rebalanceBranch(next, pathKey, once);

    if (rightChild) root->setRight(std::move(next));
    else root->setLeft(std::move(next));

    if (!rebalanced || !once) {
        rebalanced = rebalanceRoot(root);
    }

    return rebalanced;
}

template <class T>
T& avlInsert(std::unique_ptr<Node<T>>& root, int key, T data) {
    T& dataReference = btInsert(root, key, std::move(data));

    rebalanceBranch(root, key, true);

    return dataReference;
}

template <class T>
int getRightmostKey(const std::unique_ptr<Node<T>>& root) {
    assert(root != nullptr);
    if (root->getRight() == nullptr) {
        return root->key;
    }
    return getRightmostKey(root->getRight());
}

template <class T>
int getAvlRemoveRebalancePath(const std::unique_ptr<Node<T>>& root, int key) {
    if (root == nullptr) return key;

    if (key == root->key) {
        if (root->getLeft() != nullptr) {
            return getRightmostKey(root->getLeft());
        } else {
            return root->key;
        }
    }
    if (key < root->key) {
        return getAvlRemoveRebalancePath(root->getLeft(), key);
    } else {
        return getAvlRemoveRebalancePath(root->getRight(), key);
    }
}

template <class T>
T avlRemove(std::unique_ptr<Node<T>>& root, int key) {
    std::cout << "Removing " << key << std::endl;
    std::cout << "Before:          " << root << std::endl;
    int pathKey = key;
    T data = btRemove(root, key, &pathKey);
    std::cout << "After remove:    " << root << std::endl;

    pathKey = getAvlRemoveRebalancePath(root, pathKey);
    rebalanceBranch(root, pathKey, false);
    std::cout << "After rebalance: " << root << std::endl;

    return data;
}

template <class T>
const std::unique_ptr<Node<T>>& getMaximum(const std::unique_ptr<Node<T>>& root) {
    if (root->getRight() == nullptr) {
        return root;
    }
    return getMaximum(root->getRight());
}

template <class T>
const std::unique_ptr<Node<T>>& getMinimum(const std::unique_ptr<Node<T>>& root) {
    if (root->getLeft() == nullptr) {
        return root;
    }
    return getMinimum(root->getLeft());
}

// Returns the number of elements written
template <class T>
int toArray(const std::unique_ptr<Node<T>>& root, T* array) {
    if (root == nullptr) {
        return 0;
    }

    int written = 0;
    written += toArray(root->getLeft(), array);
    array[written] = root->data;
    written++;
    written += toArray(root->getRight(), array + written);
    return written;
}

template <class T>
std::unique_ptr<Node<T>> fromArray(T* array, int size) {
    if (size == 0) {
        return nullptr;
    }

    if (size % 2 == 1) {
        int leftSize = size / 2;
        int rightSize = size / 2;
        std::unique_ptr<Node<T>> root = std::make_unique<Node<T>>(array[leftSize]);
        root->setLeft(fromArray(array, leftSize));
        root->setRight(fromArray(array + leftSize + 1, rightSize));
        return root;
    }

    if (size % 2 == 0) {
        int leftSize = size / 2 - 1;
        int rightSize = size / 2;
        std::unique_ptr<Node<T>> root = std::make_unique<Node<T>>(array[leftSize]);
        root->setLeft(fromArray(array, leftSize));
        root->setRight(fromArray(array + leftSize + 1, rightSize));
        return root;
    }
}

template <class T>
class Tree {
private:
    using Node = Node<T>;

    std::unique_ptr<Node> root;
    int m_size;
    std::unique_ptr<Node>* m_minimum;
    std::unique_ptr<Node>* m_maximum;

public:
    Tree() {
        root = nullptr;
        m_size = 0;
        m_minimum = &root;
        m_maximum = &root;
    }

    T& insert(int key, T data) {
        T& dataReference = avlInsert(root, key, std::move(data));
        m_size++; // Must happen after the insert so we don't count failed calls.
        // Update minimum and maximum. O(log n) time so it's fine.
        m_minimum = &getMinimum(root);
        m_maximum = &getMaximum(root);
        return dataReference;
    }

    T remove(int key) {
        T data = avlRemove(root, key);
        m_size--; // Must happen after the remove so we don't count failed calls.
        // Update minimum and maximum. O(log n) time so it's fine.
        m_minimum = &getMinimum(root);
        m_maximum = &getMaximum(root);
        return data;
    }

    T& get(int key) {
        return btGet(root.get(), key);
    }

    const T& get(int key) const {
        return btGet(root.get(), key);
    }

    bool contains(int key) const {
        return find(root, key) != nullptr;
    }

    int size() const {
        return m_size;
    }

    T* minimum() {
        return (*m_minimum)->data;
    }

    int minimumKey() {
        return (*m_minimum)->key;
    }

    T* maximum() {
        return (*m_maximum)->data;
    }

    int maximumKey() {
        return (*m_maximum)->key;
    }

    // Return value must be deleted by the caller using delete[].
    T* toArray() {
        T* array = new T[m_size];
        int written = toArray(root, array);
        assert(written == m_size);
        return array;
    }

    // Input array must be deleted by the caller using delete[].
    static Tree fromArray(T* array, int size) {
        return treeFromArray(array, size);
    }

    friend auto operator<<(std::ostream& os, const Tree& tree) -> std::ostream& { 
        return os << tree.root;
    }
};

template <class T>
auto operator<<(std::ostream& os, const std::unique_ptr<Node<T>>& node) -> std::ostream& { 
    os << '[';
    
    if (node) {
        if (node->getLeft() != nullptr) {
            os << node->getLeft();
        }

        os << node->key << ":" << node->data;

        if (node->getRight() != nullptr) {
            os << node->getRight();
        }
    }

    return os << ']';
}

#endif
