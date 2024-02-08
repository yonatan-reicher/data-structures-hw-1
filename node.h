#ifndef DSNODE_H
#define DSNODE_H

#include <memory>

template <class T>
struct Node;

// These are functions that cannot be methods because they can be called on a nullptr

template <class T>
int height(Node<T>* root) {
    if (root == nullptr) {
        return 0;
    }
    return 1 + std::max(height(root->left.get()), height(root->right.get()));
}

template <class T>
int count(Node<T>* root) {
    if (root == nullptr) {
        return 0;
    }
    return 1 + count(root->left.get()) + count(root->right.get());
}

template <class T>
struct Node {
    int key;
    T data;
    std::unique_ptr<Node> left;
    std::unique_ptr<Node> right;
    //Node* parent;

    int balance() {
        return height(left.get()) - height(right.get());
    }
};

#endif
