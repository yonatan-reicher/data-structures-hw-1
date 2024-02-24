#ifndef DSTREE_H
#define DSTREE_H

#include <memory>
#include <cassert>
#include <iostream>
#include "node.h"

template <class K>
struct NotFoundException : public std::exception {
    K key;

    explicit NotFoundException(K key) : key(std::move(key)) {}

    const char* what() const throw() {
        return "Key not found";
    }
};

template <class K, class T>
auto operator<<(std::ostream& os, const std::unique_ptr<Node<K, T>>& node) -> std::ostream&;

// Functions marked with "bt" are binary tree functions.
// Functions marked with "avl" are AVL tree functions.

template <class K, class T>
T& btInsert(std::unique_ptr<Node<K, T>>& root, const K& key, T data, bool* alreadyExisted) {
    if (root == nullptr) {
        root = std::unique_ptr<Node<K, T>>(new Node<K, T>(key, std::move(data)));
        return root->data;
    }

    if (key == root->key) {
        root->data = std::move(data);
        if (alreadyExisted != nullptr) {
            *alreadyExisted = true;
        }
        return root->data;
    }

    if (key < root->key) {
        std::unique_ptr<Node<K, T>> left = root->popLeft();
        T& ret = btInsert(left, key, std::move(data), alreadyExisted);
        root->setLeft(std::move(left));
        return ret;
    } else {
        std::unique_ptr<Node<K, T>> right = root->popRight();
        T& ret = btInsert(right, key, std::move(data), alreadyExisted);
        root->setRight(std::move(right));
        return ret;
    }
}

template <class K, class T>
std::unique_ptr<Node<K, T>> popRightmost(std::unique_ptr<Node<K, T>>& node) {
    assert(node != nullptr);

    if (node->getRight() == nullptr) {
        std::unique_ptr<Node<K, T>> ret = std::move(node);
        node = ret->popLeft();
        return ret;
    }

    std::unique_ptr<Node<K, T>> right = node->popRight();
    std::unique_ptr<Node<K, T>> ret = popRightmost(right);
    node->setRight(std::move(right));
    return ret;
}

template <class K, class T>
T btRemove(std::unique_ptr<Node<K, T>>& root, const K& key, K* keyOfSwitched) {
    if (root == nullptr) {
        throw NotFoundException<K>(key);
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
        result = std::move(root->data);
        std::unique_ptr<Node<K, T>> left = root->popLeft();
        std::unique_ptr<Node<K, T>> right = root->popRight();
        std::unique_ptr<Node<K, T>> rightmost = popRightmost(left);
        root = std::move(rightmost);
        root->setLeft(std::move(left));
        root->setRight(std::move(right));
        if (keyOfSwitched != nullptr) {
            *keyOfSwitched = root->key;
        }
    } else if (key < root->key) {
        std::unique_ptr<Node<K, T>> left = root->popLeft();
        result = btRemove(left, key, keyOfSwitched);
        root->setLeft(std::move(left));
    } else {
        std::unique_ptr<Node<K, T>> right = root->popRight();
        result = btRemove(right, key, keyOfSwitched);
        root->setRight(std::move(right));
    }
    return std::move(result);
}

template <class K, class T>
T& btGet(Node<K, T>* root, const K& key) {
    if (root == nullptr) {
        throw NotFoundException<K>(key);
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

template <class K, class T>
void rotate(std::unique_ptr<Node<K, T>>& root, bool right) {
    assert(root != nullptr);
    const std::unique_ptr<Node<K, T>>& bRef = right ? root->getLeft() : root->getRight();
    assert(bRef != nullptr);

    // This is the action we want to perform:
    /*        A    right   B
     *       / \    ==>   / \
     *      B   C   <==  D   A
     *     / \     left     / \
     *    D   E            E   C
     *
     *    (Here C, D, and E can all be nulls)
     */

    // Here we use the names from the diagram above (of the right rotation).
    std::unique_ptr<Node<K, T>> a = std::move(root);
    std::unique_ptr<Node<K, T>> b = right ? a->popLeft() : a->popRight();
    std::unique_ptr<Node<K, T>> e = right ? b->popRight() : b->popLeft();
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

template <class K, class T>
void rotateLLOrRR(std::unique_ptr<Node<K, T>>& root, bool ll) {
    // This is just a single rotation.
    rotate(root, ll);
}

template <class K, class T>
void rotateLROrRL(std::unique_ptr<Node<K, T>>& root, bool lr) {
    /* This is a double rotation.
     *      A             E
     *     / \    LR     / \
     *    B   C   ==>   B   A
     *   / \       ^   / \ / \
     *  D   E      |  D  F G  C
     *     / \     |   
     *    F   G    --- This is a left rotation on B then right rotation on A.
     */

    std::unique_ptr<Node<K, T>> child = lr ? root->popLeft() : root->popRight();
    rotate(child, !lr);
    if (lr) root->setLeft(std::move(child));
    else root->setRight(std::move(child));

    rotate(root, lr);
}

enum class RebalanceType {
    LL, RR, LR, RL, NONE
};

RebalanceType chooseRebalanceByBalanceFactor(int atRoot, int atLeft, int atRight) {
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
template <class K, class T>
bool rebalanceRoot(std::unique_ptr<Node<K, T>>& root) {
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

template <class K, class T>
bool rebalanceBranch(std::unique_ptr<Node<K, T>>& root, const K& pathKey, bool once) {
    if (root == nullptr) {
        return false;
    }

    bool rightChild = pathKey > root->key;
    std::unique_ptr<Node<K, T>> next = rightChild ? root->popRight() : root->popLeft();

    bool rebalanced = rebalanceBranch(next, pathKey, once);

    if (rightChild) root->setRight(std::move(next));
    else root->setLeft(std::move(next));

    if (!rebalanced || !once) {
        rebalanced = rebalanceRoot(root);
    }

    return rebalanced;
}

template <class K, class T>
const K& getRightmostKey(const std::unique_ptr<Node<K, T>>& root) {
    assert(root != nullptr);
    if (root->getRight() == nullptr) {
        return root->key;
    }
    return getRightmostKey(root->getRight());
}

template <class K, class T>
const K& getAvlRemoveRebalancePath(const std::unique_ptr<Node<K, T>>& root, const K& key) {
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

template <class K, class T>
T& avlInsert(std::unique_ptr<Node<K, T>>& root, const K& key, T data, bool* wasInserted) {
    T& dataReference = btInsert(root, key, std::move(data), wasInserted);

    rebalanceBranch(root, key, true);

    return dataReference;
}

template <class K, class T>
T avlRemove(std::unique_ptr<Node<K, T>>& root, const K& key) {
    K pathKey = key;
    T data = btRemove(root, key, &pathKey);
    pathKey = getAvlRemoveRebalancePath(root, pathKey);
    rebalanceBranch(root, pathKey, false);

    return data;
}

template <class K, class T>
const std::unique_ptr<Node<K, T>>& getMaximum(const std::unique_ptr<Node<K, T>>& root) {
    if (root == nullptr) {
        return root;
    }
    if (root->getRight() == nullptr) {
        return root;
    }
    return getMaximum(root->getRight());
}

template <class K, class T>
const std::unique_ptr<Node<K, T>>& getMinimum(const std::unique_ptr<Node<K, T>>& root) {
    if (root == nullptr) {
        return root;
    }
    if (root->getLeft() == nullptr) {
        return root;
    }
    return getMinimum(root->getLeft());
}

// Returns the number of elements written
template <class K, class T>
int toArray(const std::unique_ptr<Node<K, T>>& root, T* array) {
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

template <class K, class T, class F>
std::unique_ptr<Node<K, T>> fromArray(T* array, int size, F& keyGenerator) {
    if (size == 0) {
        return nullptr;
    }

    if (size % 2 == 1) {
        int leftSize = size / 2;
        int rightSize = size / 2;
        std::unique_ptr<Node<K, T>> root = std::unique_ptr<Node<K, T>>(
            new Node<K, T>(keyGenerator(array[leftSize]), array[leftSize])
        );
        root->setLeft(fromArray(array, leftSize));
        root->setRight(fromArray(array + leftSize + 1, rightSize));
        return root;
    }

    if (size % 2 == 0) {
        int leftSize = size / 2 - 1;
        int rightSize = size / 2;
        std::unique_ptr<Node<K, T>> root = std::unique_ptr<Node<K, T>>(
            new Node<K, T>(keyGenerator(array[leftSize]), array[leftSize])
        );
        root->setLeft(fromArray(array, leftSize));
        root->setRight(fromArray(array + leftSize + 1, rightSize));
        return root;
    }
}

template <class K, class T>
class Tree {
private:
    using Node = Node<K, T>;

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

    T& insert(const K& key, T data) {
        bool wasAlreadyInserted = false;
        T& dataReference = avlInsert(root, key, std::move(data), &wasAlreadyInserted);
        if (!wasAlreadyInserted) {
            m_size++; // Must happen after the insert so we don't count failed calls.
        }
        // Update minimum and maximum. O(log n) time so it's fine.
        m_minimum = &const_cast<std::unique_ptr<Node>&>(getMinimum(root));
        m_maximum = &const_cast<std::unique_ptr<Node>&>(getMaximum(root));
        return dataReference;
    }

    T remove(const K& key) {
        T data = avlRemove(root, key);
        m_size--; // Must happen after the remove so we don't count failed calls.
        // Update minimum and maximum. O(log n) time so it's fine.
        m_minimum = &const_cast<std::unique_ptr<Node>&>(getMinimum(root));
        m_maximum = &const_cast<std::unique_ptr<Node>&>(getMaximum(root));
        return data;
    }

    T& get(const K& key) {
        return btGet(root.get(), key);
    }

    const T& get(const K& key) const {
        return btGet(root.get(), key);
    }

    bool contains(const K& key) const {
        try {
            get(key);
            return true;
        } catch (const NotFoundException<K>& e) {
            return false;
        }
    }

    int size() const {
        return m_size;
    }

    T* minimum() {
        return (*m_minimum)->data;
    }

    const K& minimumKey() {
        return (*m_minimum)->key;
    }

    T* maximum() {
        return (*m_maximum)->data;
    }

    const K& maximumKey() {
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

template <class K, class T>
auto operator<<(std::ostream& os, const std::unique_ptr<Node<K, T>>& node) -> std::ostream& { 
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
