#include <iostream>
using namespace std;

const int maxColision = 3;

template <typename TK, typename TV>
class HashTable;

namespace std {
    inline std::string to_string(const std::string& s) {
        return s;
    }
}

//itera sobre el hashtable manteniendo el orden de insercion
template <typename TK, typename TV>
class HashIterator {
private:
    HashTable<TK, TV>* hashtable;
    typename HashTable<TK, TV>::Node* current;

public:
    HashIterator(HashTable<TK,TV>* ht,
                 typename HashTable<TK,TV>::Node* node)
      : hashtable(ht), current(node) {}

    bool operator!=(const HashIterator<TK, TV>& other) const {
        return current != other.current;
    }
    HashIterator<TK, TV>& operator++() {
        if (current) current = current->next;
        return *this;
    }
    pair<TK, TV> operator*() {
        return {current->key, hashtable->at(current->key)};
    }
};

template <typename TK, typename TV>
class HashTable
{
public:
    typedef HashIterator<TK, TV>  iterator;
    friend class HashIterator<TK, TV>;
    iterator begin() { return iterator(this, list_head); }
    iterator end() { return iterator(this, nullptr); }

private:
    struct Node {
        TK key;
        Node* prev;
        Node* next;
        Node(TK k) : key(k), prev(nullptr), next(nullptr) {}
    };

    struct Entry {
        TK key;
        TV value;
        Node* list_node;
        Entry(TK k, TV v, Node* ln) : key(k), value(v), list_node(ln) {}
    };

    struct Bucket {
        struct BucketEntry {
            Entry* entry;
            BucketEntry* next;
            BucketEntry(Entry* e, BucketEntry* n = nullptr) : entry(e), next(n) {}
        };
        BucketEntry* head;
        int count;

        Bucket() : head(nullptr), count(0) {}
    };

    Bucket* buckets;
    int capacity;//capacidad del hash table
    int size;//total de elementos
    Node* list_head;
    Node* list_tail;

    size_t hash(TK key) const {
        return std::hash<TK>{}(key) % capacity;
    }


public:
    HashTable(int _cap = 5) : capacity(_cap), size(0), list_head(nullptr), list_tail(nullptr) {
        buckets = new Bucket[capacity];
    }
    ~HashTable() {
        delete[] buckets;
        Node* current = list_head;
        while (current) {
            Node* next = current->next;
            delete current;
            current = next;
        }
    }
    void insert(TK key, TV value) {
        // Si ya existe, actualizar y salir
        if (find(key)) {
            at(key) = value;
            return;
        }

        // Antes de insertar, comprobamos si ese bucket excede maxColision
        size_t idx = hash(key);
        if (buckets[idx].count >= maxColision) {
            rehashing();
            idx = std::hash<TK>{}(key) % capacity;
        }

        // 2.1) añadimos un Node al final de la lista de inserción
        Node* ln = new Node(key);
        if (!list_head) {
            list_head = list_tail = ln;
        } else {
            list_tail->next = ln;
            ln->prev        = list_tail;
            list_tail      = ln;
        }

        // 2.2) creamos la Entry y la encadenamos en el bucket idx
        Entry* entry = new Entry(key, value, ln);
        auto& bucket = buckets[idx];
        auto be_new   = new typename Bucket::BucketEntry(entry, bucket.head);
        bucket.head  = be_new;
        bucket.count += 1;

        // 2.3) ajustamos tamaño total
        size += 1;
    };
    void insert(pair<TK, TV> item) {
        insert(item.first, item.second);
    };
    TV& at(TK key) {
        int idx = hash(key);
        auto& bucket = buckets[idx];
        auto be = bucket.head;
        while (be) {
            if (be->entry->key == key) return be->entry->value;
            be = be->next;
        }
        throw std::out_of_range("Key not found in HashTable::at()");
    }

    TV& operator[](TK key) {
        try {
            return at(key);
        } catch (out_of_range&) {
            insert(key, TV());
            return at(key);
        }
    }

    bool find(TK key) {
        int idx = hash(key);
        auto& bucket = buckets[idx];
        auto be = bucket.head;
        while (be) {
            if (be->entry->key == key) return true;
            be = be->next;
        }
        return false;
    }

    bool remove(TK key) {
        int idx = hash(key);
        auto& bucket = buckets[idx];
        typename Bucket::BucketEntry* prev = nullptr;
        auto current = bucket.head;

        while (current) {
            if (current->entry->key == key) {
                if (prev) prev->next = current->next;
                else bucket.head = current->next;

                Entry* entry = current->entry;
                Node* list_node = entry->list_node;

                if (list_node->prev) list_node->prev->next = list_node->next;
                else list_head = list_node->next;
                if (list_node->next) list_node->next->prev = list_node->prev;
                else list_tail = list_node->prev;

                delete list_node;
                delete entry;
                delete current;

                bucket.count--;
                size--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    int getSize() { return size; }

    /*itera sobre el hashtable manteniendo el orden de insercion*/
    vector<TK> getAllKeys() {
        vector<TK> keys;
        Node* current = list_head;
        while (current) {
            keys.push_back(current->key);
            current = current->next;
        }
        return keys;
    }

    vector<pair<TK, TV>> getAllElements() {
        vector<pair<TK, TV>> elements;
        Node* current = list_head;
        while (current) {
            elements.push_back({current->key, at(current->key)});
            current = current->next;
        }
        return elements;
    }
private:
    /*Si una lista colisionada excede maxColision, redimensionar el array*/
    void rehashing() {
        int new_cap = capacity * 2;
        Bucket* new_buckets = new Bucket[new_cap];

        for (int i = 0; i < capacity; ++i) {
            auto current = buckets[i].head;
            while (current) {
                Entry* entry = current->entry;

                size_t raw_hash = std::hash<TK>{}(entry->key);
                int new_idx = raw_hash % new_cap;

                auto& new_bucket = new_buckets[new_idx];
                auto new_be = new typename Bucket::BucketEntry(entry, new_bucket.head);
                new_bucket.head = new_be;
                new_bucket.count++;

                auto old_be = current;
                current = current->next;
                delete old_be;
            }
            buckets[i].head = nullptr;
        }

        delete[] buckets;
        buckets = new_buckets;
        capacity = new_cap;
        }
};


