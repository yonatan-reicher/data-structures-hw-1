#ifndef DSTREE_H
#define DSTREE_H

#include <memory>
#include <cassert>
#include "node.h"

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
        // Find the where to put the data.
        std::unique_ptr<Node>& insertAt = find(root, key);
        // Must not already exist.
        assert(insertAt == nullptr);
        insertAt = std::unique_ptr<Node>(new Node {key, data, nullptr, nullptr});
        T& result = insertAt->data;
        rebalance(insertAt);
        return result;
    }

    T remove(int key) {
        std::unique_ptr<Node>& removeAt = find(root, key);
        assert(removeAt != nullptr);
        T data = std::move(removeAt->data);
        // Binary tree removal.
        if (removeAt.get()->left == nullptr) {
            removeAt = std::move(removeAt->right);
        } else if (removeAt.get()->right == nullptr) {
            removeAt = std::move(removeAt->left);
        } else {
            /*     5              4
             *    / \            / \
             *    2  7    ==>   2   7
             *   / \           / \
             *  1   4         1   3
             *     /
             *    3 
             */
            // Find the rightmost node in the left subtree.
            std::unique_ptr<Node>* rightmost = &removeAt->left;
            while ((*rightmost)->right != nullptr) {
                rightmost = &(*rightmost)->right;
            }
            // Swap the data.
            // This should be done *carefully* so no uniqueptr is cleaned up!
            std::unique_ptr<Node> left = std::move(removeAt->left);
            std::unique_ptr<Node> right = std::move(removeAt->right);
            std::unique_ptr<Node> rightmostsLeft = std::move((*rightmost)->left);
            removeAt = std::move(*rightmost);
            removeAt->left = std::move(left);
            removeAt->right = std::move(right);
            (*rightmost) = std::move(rightmostsLeft);
        }
        removeAt = nullptr;
        return std::move(data);
    }

    T& get(int key) {
        std::unique_ptr<Node>& node = find(root, key);
        assert(node != nullptr);
        return node->data;
    }

    const T& get(int key) const {
        std::unique_ptr<Node>& node = find(root, key);
        assert(node != nullptr);
        return node->data;
    }

    bool contains(int key) const {
        return find(root, key) != nullptr;
    }

    int size() const {
        return count(root.get());
    }

private:
    // This function takes a node (with sorted children) and finds the node
    // where the key should be.
    // This must return a reference to a pointer so that we can modify the tree
    // using this function.
    static std::unique_ptr<Node>& find(std::unique_ptr<Node>& root, int key) {
        if (root == nullptr || root->key == key) {
            return root;
        }
        std::unique_ptr<Node>& next = (key < root->key) ? root->left : root->right;
        return find(next, key);
    }

    // Rebalance the tree after making a change to a given node.
    void rebalance(std::unique_ptr<Node>& node) const {
        assert(node != nullptr);
        int balance = node->balance();
        assert(balance >= -2 && balance <= 2);

        // TODO: ...
    }
};

#endif
