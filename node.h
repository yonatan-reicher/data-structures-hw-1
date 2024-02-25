#ifndef DSNODE_H
#define DSNODE_H

#include <memory>

template <class K, class T>
class Node;

// These are functions that cannot be methods because they can be called on a nullptr

template <class K, class T>
int height(const std::unique_ptr<Node<K, T>>& root) {
    if (root == nullptr) {
        return -1;
    }
    return root->m_height;
}

template <class K, class T>
int balanceFactor(const std::unique_ptr<Node<K, T>>& root) {
    if (root == nullptr) {
        return 0;
    }
    return height(root->m_left) - height(root->m_right);
}

template <class K, class T>
class Node {
    std::unique_ptr<Node> m_left;
    std::unique_ptr<Node> m_right;
    int m_height;
public:
    K key;
    T data;

    Node(K key, T data)
        : m_left(nullptr), m_right(nullptr)
        , m_height(0)
        , key(std::move(key))
        , data(std::move(data))
    {
        updateHeight();
    }

    std::unique_ptr<Node> setLeft(std::unique_ptr<Node> newLeft) {
        std::unique_ptr<Node> ret = std::move(m_left);
        m_left = std::move(newLeft);
        updateHeight();
        return ret;
    }

    std::unique_ptr<Node> setRight(std::unique_ptr<Node> newRight) {
        std::unique_ptr<Node> ret = std::move(m_right);
        m_right = std::move(newRight);
        updateHeight();
        return ret;
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
    void updateHeight() {
        m_height = 1 + std::max(height(m_left), height(m_right));
    }

    template <class L, class U>
    friend int height(const std::unique_ptr<Node<L, U>>&);
    template <class L, class U>
    friend int balanceFactor(const std::unique_ptr<Node<L, U>>&);
};

#endif
