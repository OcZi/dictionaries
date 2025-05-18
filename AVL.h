#ifndef AVLTree_H
#define AVLTree_H
#include <iostream>
#include "AVL_Node.h"
#include "AVL_Iterator.h"

using namespace std;

template<typename T, typename Node = NodeAVL<T> >
class AVLTree {
public:
    typedef AVLIterator<T> iterator;

    iterator begin(AVLIterator<int>::Type type) {
        return iterator(root, type);
    }

    iterator end() {
        auto n = root;
        while (n->right != nullptr) {
            n = n->right;
        }
        return iterator(n);
    }

    Node *_right_rota(Node *x) {
        auto y = x->left;
        if (!y) return x;

        auto a = y->right;
        y->right = x;
        x->left = a;
        return y;
    }

    Node *_insert(Node *n, T value) {
        if (!n) {
            return new Node(value);
        }

        auto d = n->data;
        if (d > value) {
            n->left = _insert(n->left, value);
        } else if (d < value) {
            n->right = _insert(n->right, value);
        } else {
            return nullptr;
        }
        updateHeight(n);
        return balance(n);
    }

    Node *_remove(Node *n, T value) {
        if (!n) {
            return n;
        }

        if (value < n->data) {
            n->left = _remove(n->left, value);
        } else if (value > n->data) {
            n->right = _remove(n->right, value);
        } else {
            if (!n->left and !n->right) {
                // No hijos
                delete n;
                return nullptr;
            }

            if (!n->left || !n->right) {
                // Un solo hijo
                auto aux = n->left ? n->left : n->right;
                delete n;
                return aux;
            }

            auto s = minValue(n->right);
            n->data = s;
            n->right = _remove(n->right, s);
        }

        updateHeight(n);
        return balance(n);
    }

private:
    Node *root;
    int nodes;

public:
    AVLTree() : root(nullptr), nodes(0) {
    }

    void insert(T value) {
        root = _insert(root, value);
        // ++nodes;
    }

    bool find(T value) {
        Node *curr = root;
        while (curr != nullptr) {
            auto data = curr->data;
            if (data == value) {
                return true;
            }
            if (data > value) {
                curr = curr->left;
            } else {
                curr = curr->right;
            }
        }
        return false;
    }

    string getInOrder() {
        std::string base;
        _inorder(root, [&](Node* node) {
            if (!base.empty()) {
                base.append(", ");
            }
            base.append(to_string(node->data));
        });

        return base;
    }

    string getPreOrder() {
        std::string base;
        _preorder(root, [&](Node* node) {
            if (!base.empty()) {
                base.append(", ");
            }
            base.append(to_string(node->data));
        });

        return base;
    }

    string getPostOrder() {
        std::string base;
        _postorder(root, [&](Node* node) {
            if (!base.empty()) {
                base.append(", ");
            }
            base.append(to_string(node->data));
        });

        return base;
    }

    int height() {
        return root->height;
    }

    T minValue() {
        Node *curr = root;
        while (curr->left != nullptr) {
            curr = curr->left;
        }
        return curr;
    }

    T maxValue() {
        Node *curr = root;
        while (curr->right != nullptr) {
            curr = curr->right;
        }
        return curr;
    }

    bool isBalanced() {
        auto f = balancingFactor(root);
        return f == 1 or f == -1;
    }

    int size() {
        return 0;
    }

    void remove(T value) {
        _remove(root, value);
    }


    T successor(T value) {
        Node *curr = root;
        T last_val;
        bool found = false;
        while (curr != nullptr) {
            auto data = curr->data;
            if (data > value) {
                last_val = data;
                found = true;
                curr = curr->left;
            } else {
                curr = curr->right;
            }
        }

        if (!found) {
            throw invalid_argument("No successor for " + to_string(value) + " value");
        }

        return last_val;
    }

    T predecessor(T value) {
        Node *curr = root;
        T last_val;
        bool found = false;
        while (curr != nullptr) {
            auto data = curr->data;
            if (data < value) {
                last_val = data;
                found = true;
                curr = curr->right;
            } else {
                curr = curr->left;
            }
        }

        if (!found) {
            throw invalid_argument("No predecessor for " + to_string(value) + " value");
        }

        return last_val;
    }

    void clear() {
        root->killSelf();
    }

    void displayPretty(); //Muestra el arbol visualmente atractivo

    ~AVLTree() {
        if (this->root != nullptr) {
            this->root->killSelf();
        }
    }

    int height_of(Node *n) {
        return n ? n->height : -1;
    }

private:
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

    int balancingFactor(Node *NodeAVL) {
        if (!NodeAVL) {
            throw invalid_argument("Cannot calculate balancing factor of nullptr");
        }

        return height_of(NodeAVL->left) - height_of(NodeAVL->right);
    }

    void updateHeight(Node *n) {
        if (n == nullptr) return;
        auto l = n->left, r = n->right;
        n->height = 1 + max(l ? l->height : -1, r ? r->height : -1);
    }

    void balance(Node *&n) {
        auto factor = balancingFactor(n);
        if (factor > 1) {
            // Desbalanceado a la izquierda
            if (balancingFactor(n->left) >= 0) {
                // LL
                n = right_rota(n);
            } else {
                // LR
                n->right = left_rota(n->right);
                n = right_rota(n);
            }
        }

        if (factor < -1) {
            // Desbalanceado a la derecha
            if (balancingFactor(n->left) >= 0) {
                // RR
                n = left_rota(n);
            } else {
                // RL
                n->right = right_rota(n->left);
                n = left_rota(n);
            }
        }
    }

    Node *left_rota(Node *&x) {
        auto y = x->right;
        if (!y) return x;

        auto a = y->left;
        y->left = x;
        x->right = a;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    Node *right_rota(Node *&x) {
        auto y = x->left;
        if (!y) return x;

        auto a = y->right;
        y->right = x;
        x->left = a;

        updateHeight(x);
        updateHeight(y);

        return y;
    }
};

#endif
