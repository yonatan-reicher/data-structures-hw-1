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
        T& ret = btInsert(left, key, data);
        root->setLeft(std::move(left));
        return ret;
    } else {
        std::unique_ptr<Node<T>> right = root->popRight();
        T& ret = btInsert(right, key, data);
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
T btRemove(std::unique_ptr<Node<T>>& root, int key) {
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
        return root->data;
    } else if (key < root->key) {
        std::unique_ptr<Node<T>> left = root->popLeft();
        result = btRemove(left, key);
        root->setLeft(std::move(left));
    } else {
        std::unique_ptr<Node<T>> right = root->popRight();
        result = btRemove(right, key);
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
T& avlInsert(std::unique_ptr<Node<T>>& root, int key, T data) {
    T& dataReference = btInsert(root, key, std::move(data));

    rebalanceAt(root, key);

    return dataReference;
}

template <class T>
class Tree {
private:
    using Node = Node<T>;

    std::unique_ptr<Node> root;

public:
    Tree() {
        root = nullptr;
    }
    virtual ~Tree() {
        root = nullptr;
    }

    T& insert(int key, T data) {
        return btInsert(root, key, data);
    }

    T remove(int key) {
        return btRemove(root, key);
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
        return count(root.get());
    }

private:
    // Rebalance the tree after making a change at a given key.
    bool rebalanceAt(int pathKey) {
        return rebalanceAt(root, pathKey);
    }

    bool rebalanceAtHelper(Node& root, int pathKey) {
        // Recurse down. If a rebalance already happened, don't do more.
        // (another one won't happen)
        Node& next = root.key > pathKey ? root.m_left : root.m_right;
        if (rebalanceAtHelper(next, pathKey)) return true;

        if (root.m_left == nullptr || root.m_right == nullptr) return false;
        int balanceFactor = height(root.m_left) - height(root.m_right);
        int leftBalanceFactor = height(root.m_left->left) - height(root.m_left->right);
        int rightBalanceFactor = height(root.m_right->left) - height(root.m_right->right);

        return dispatchRebalance(balanceFactor, leftBalanceFactor, rightBalanceFactor);
    }

    bool dispatchRebalance(int atRoot, int atLeft, int atRight) {
        assert(-2 <= atRoot && atRoot <= 2);
        assert(-1 <= atLeft && atLeft <= 1);
        assert(-1 <= atRight && atRight <= 1);

        if (atRoot != 2 && atRoot != -2) {
            return false;
        }

        /*
        if (atRoot == 2 && atLeft > -1) ll();
        if (atRoot == 2 && atLeft == -1) lr();
        if (atRoot == -2 && atRight < 1) rr();
        if (atRoot == -2 && atRight == 1) rl();
        */
        return false;
    }

    friend auto operator<<(std::ostream& os, const Tree& tree) -> std::ostream& { 
        return os << tree.root.get();
    }
};

template <class T>
auto operator<<(std::ostream& os, const Node<T>* node) -> std::ostream& { 
    os << '[';
    
    if (node->getLeft() != nullptr) {
        os << node->getLeft().get();
    }

    if (node != nullptr) {
        os << node->key << ":" << node->data;
    }

    if (node->getRight() != nullptr) {
        os << node->getRight().get();
    }

    return os << ']';
}

#endif
