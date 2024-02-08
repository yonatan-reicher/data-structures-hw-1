#ifndef DSNODE_H
#define DSNODE_H

#include <memory>

template <class T>
class Node;

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
class Node {
    std::unique_ptr<Node> m_left;
    std::unique_ptr<Node> m_right;
    int m_height;
    int m_count;
public:
    int key;
    T data;

    Node(int key, T data) {
        key = key;
        data = std::move(data);
        m_left = nullptr;
        m_right = nullptr;
        updateHeightAndCount();
    }

    std::unique_ptr<Node> setLeft(std::unique_ptr<Node> newLeft) {
        std::unique_ptr<Node> ret = std::move(m_left);
        m_left = std::move(newLeft);
        updateHeightAndCount();
        return std::move(ret);
    }

    std::unique_ptr<Node> setRight(std::unique_ptr<Node> newRight) {
        std::unique_ptr<Node> ret = std::move(m_right);
        m_right = std::move(newRight);
        updateHeightAndCount();
        return std::move(ret);
    }

    std::unique_ptr<Node> popLeft() {
        return setLeft(nullptr);
    }

    std::unique_ptr<Node> popRight() {
        return setRight(nullptr);
    }

    const std::unique_ptr<Node>& getLeft() const {
        return m_left;
    }

    const std::unique_ptr<Node>& getRight() const {
        return m_right;
    }

private:
    void updateHeightAndCount() {
        m_height = 1 + std::max(height(m_left.get()), height(m_right.get()));
        m_count = 1 + count(m_left.get()) + count(m_right.get());
    }

    friend int height(const Node<T>*);
    friend int count(const Node<T>*);
};

#endif
