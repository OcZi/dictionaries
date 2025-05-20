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
        std::queue<Node *> q;
        q.push(node);

        while (!q.empty()) {
            auto elem = q.front();
            q.pop();
            func(elem);

            if (elem->left) q.push(elem->left);
            if (elem->right) q.push(elem->right);
        }
    }

    static void push_queue(Node* n, std::queue<Node*>& q) {
        q.push(n);
    }

    void _process() {

        while (!queue.empty()) {
            queue.pop();
        }

        if (!current) return;

        switch (type) {
            case InOrder:
                _inorder(current, [this](Node* n){ push_queue(n, queue); });
                break;
            case PostOrder:
                _postorder(current, [this](Node* n){ push_queue(n, queue); });
                break;
            case PreOrder:
                _preorder(current, [this](Node* n){ push_queue(n, queue); });
                break;
            case BFS:
                breadth_first_search(current, [this](Node* n){ push_queue(n, queue); });
                break;
        }

        if (!queue.empty()) {
            current = queue.front();
            queue.pop();
        } else {
            current = nullptr;
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
            current = other.current;
            queue = other.queue;
            type = other.type;
        }
        return *this;
    }

    bool operator !=(AVLIterator<T> other) const {
        if (current == nullptr && other.current == nullptr) return false;
        return current != other.current;
    }

    AVLIterator<T> &operator++() {
       if (queue.empty()) current = nullptr;
       else {
           current = queue.front();
           queue.pop();
       }
       return *this;
    }

    T operator*() {
        if (!current) throw std::runtime_error("Dereferencing null iterator");
        return current->data;
    }
};