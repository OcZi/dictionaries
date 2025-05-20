#ifndef AVLTree_H
#define AVLTree_H
#include <iostream>
#include "AVL_Node.h"
#include "AVL_Iterator.h"

using namespace std;

template<typename T, typename Node = NodeAVL<T> >
class AVLTree {

    Node *root;
    int nodes;

public:
    typedef AVLIterator<T> iterator;

    Node* getRoot() const { return root; }

    iterator begin(AVLIterator<int>::Type type) {
        return iterator(root, type);
    }

    iterator end() {
        return iterator(nullptr);
    }

    static T minValue(Node* node) {
        if (!node) throw std::runtime_error("Cannot get min value of nullptr");
        while (node->left != nullptr) {
            node = node->left;
        }
        return node->data;
    }

    static T maxValue(Node* node) {
        if (!node) throw std::runtime_error("Cannot get max value of nullptr");
        while (node->right != nullptr) {
            node = node->right;
        }
        return node->data;
    }

    Node *_insert(Node *n, T value) {
        if (!n) {
            return new Node(value);
        }

        if (value < n->data) {
            n->left = _insert(n->left, value);
        } else if (value > n->data) {
            n->right = _insert(n->right, value);
        } else {
            return n; // valor duplicado
        }

        updateHeight(n);
        return balance(n);
    }

    Node* _remove(Node* node, T value) {
        if (!node) return nullptr;

        if (value < node->data) {
            node->left = _remove(node->left, value);
        } else if (value > node->data) {
            node->right = _remove(node->right, value);
        } else {
            if (!node->left || !node->right) {
                // Case 1: 0 or 1 child
                Node* temp = node->left ? node->left : node->right;
                delete node;
                return temp;
            } else { //
                // Case 2: 2 children -> Se trabaja con el predecesor en este caso
                Node* predecessor = node->left;
                while (predecessor->right) {
                    predecessor = predecessor->right;
                }
                node->data = predecessor->data;
                node->left = _remove(node->left, predecessor->data);
            }
        }

        // Update height and balance
        updateHeight(node);
        return balance(node);
    }


    AVLTree() : root(nullptr), nodes(0) {
    }

    void insert(T value) {
        root = _insert(root, value);
        ++nodes;
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
            base.append(to_string(node->data) + " ");
        });

        return base;
    }

    string getPreOrder() {
        std::string base;
        _preorder(root, [&](Node* node) {
            base.append(to_string(node->data) + " ");
        });

        return base;
    }

    string getPostOrder() {
        std::string base;
        _postorder(root, [&](Node* node) {
            base.append(to_string(node->data) + " ");
        });

        return base;
    }

    [[nodiscard]] int height() const {
        return height_of(root);
    }


    T minValue() {
        return minValue(root);
    }

    T maxValue() {
        return maxValue(root);
    }


    bool isBalanced() {
        if (!root) {
            return true;
        }
        auto f = balancingFactor(root);
        return f >= -1 && f <= 1;
    }


    static int size() {
        return 0;
    }

    void remove(T value) {
        root = _remove(root, value);
        --nodes;
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
        if (this->root != nullptr) {
            this->root->killSelf();
            this->root = nullptr;
            this->nodes = 0;
        }
    }

    void displayPretty() {
        displayPretty(root);
    }


    ~AVLTree() {
        if (this->root != nullptr) {
            this->root->killSelf();
        }
    }

    static int height_of(Node *n) {
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
        n->height = 1 + std::max(height_of(n->left), height_of(n->right));
    }

    Node* balance(Node *&n) {
        if (!n) {
            throw std::runtime_error("Balance func: Node is null");
        }
        updateHeight(n);

        auto factor = balancingFactor(n);
        // Left
        if (factor > 1) {
            // LL
            if (balancingFactor(n->left) >= 0) {
                n = right_rotate(n);
            }
            // LR
            else { // balancingFactor(n->left) < 0
                n->left = left_rotate(n->left);
                n = right_rotate(n);
            }
        }
        // Right
        else if (factor < -1) {
            // RR
            if (balancingFactor(n->right) <= 0) {
                n = left_rotate(n);
            }
            // RL
            else { // balancingFactor(n->right) > 0
                n->right = right_rotate(n->right);
                n = left_rotate(n);
            }
        }
        return n;
    }

    Node *left_rotate(Node *&x) {
        if (!x || !x->right) return x;

        auto y = x->right;
        auto a = y->left;

        y ->left = x;
        x->right = a;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    Node *right_rotate(Node *&x) {
        if (!x || !x->left) return x;
        auto y = x->left;
        auto a = y->right;
        y->right = x;
        x->left = a;

        updateHeight(x);
        updateHeight(y);

        return y;
    }

    void displayPretty(Node* root, const string& prefix = "", bool isLeft = true) { //Muestra el arbol visualmente atractivo
        if (!root) return;

        cout << prefix;
        cout << (isLeft ? "L--- " : "R--- ");
        cout << root->data << endl;

        displayPretty(root->left, prefix + (isLeft ? "|    " : "     "), true);
        displayPretty(root->right, prefix + (isLeft ? "|    " : "     "), false);

    }
};

#endif
