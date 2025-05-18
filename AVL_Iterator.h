#pragma once

#include <queue>
#include "AVL_Node.h"

template<typename T>
class AVLIterator {
public:
    enum Type {
        PreOrder, InOrder, PostOrder, BFS
    };

private:
    using Node = NodeAVL<T>;
    Node* current;
    std::queue<Node*> queue;
    Type type;

    template<typename Fun>
    void _preorder(Node *node, Fun func) {
        if (!node) return;

        func(node);
        _preorder(node->left, func);
        _preorder(node->right, func);
    }

    template<typename Fun>
    void _inorder(Node *node, Fun func) {
        if (!node) return;

        _inorder(node->left, func);
        func(node);
        _inorder(node->right, func);
    }

    template<typename Fun>
    void _postorder(Node *node, Fun func) {
        if (!node) return;

        _postorder(node->left, func);
        _postorder(node->right, func);
        func(node);
    }

    template<typename Fun>
    void breadth_first_search(Node *node, Fun func) {
        if (!node) return;
        std::queue<Node *> queue;
        queue.push(node);

        while (!queue.empty()) {
            auto elem = queue.front();
            queue.pop();
            func(elem);

            if (elem->left) queue.push(elem->left);
            if (elem->right) queue.push(elem->right);
        }
    }

    void push_queue(Node* n) {
        queue.push(n);
    }

    void _process() {
        switch (type) {
            case InOrder:
                _inorder(current, push_queue);
            case PostOrder:
                _postorder(current, push_queue);
            case PreOrder:
                _preorder(current, push_queue);
            case BFS:
                breadth_first_search(current, push_queue);
        }
    }

public:
    AVLIterator() : current(nullptr), type(InOrder) {
    };

    explicit AVLIterator(NodeAVL<T> *current, Type type = InOrder) : current(current), type(type) {
        _process();
    }

    AVLIterator<T> &operator =(const AVLIterator<T> &other) {
        if (this != &other) {
            std::swap(current, other.current);
            std::swap(type, other.type);
            _process();
        }
        return *this;
    }

    bool operator !=(AVLIterator<T> other) {
        return current != other;
    }

    AVLIterator<T> &operator++() {
        current = queue.front();
        queue.pop();
        return *this;
    }

    T operator*() {
        return current->data;
    }
};