#ifndef DSNODE_H
#define DSNODE_H

#include <memory>

template <class T>
struct Node;

// These are functions that cannot be methods because they can be called on a nullptr

template <class T>
int height(const Node<T>* root) {
    if (root == nullptr) {
        return 0;
    }
    return root->m_height;
}

template <class T>
int count(const Node<T>* root) {
    if (root == nullptr) {
        return 0;
    }
    return root->m_count;
}

template <class T>
int balance(const Node<T>* root) {
    if (root == nullptr) {
        return 0;
    }
    return height(root->m_left.get()) - height(root->m_right.get());
}

template <class T>
struct Node {
    std::unique_ptr<Node> m_left;
    std::unique_ptr<Node> m_right;
    int m_height;
    int m_count;
    int key;
    T data;

    Node(int key, T data) {
        key = key;
        data = std::move(data);
        m_left = nullptr;
        m_right = nullptr;
        updateHeightAndCount();
    }

    void updateHeightAndCount() {
        m_height = 1 + std::max(height(m_left.get()), height(m_right.get()));
        m_count = 1 + count(m_left.get()) + count(m_right.get());
    }
};

#endif
