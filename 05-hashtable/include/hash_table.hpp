#include <algorithm>
#include <iostream>
#include <vector>
#include <functional>
#include <iterator>
#include <memory>

#define INITIAL_TABLE_SIZE 64

#include "hash_slot.hpp"

/* Fill in the TODO sections in the following code. */

template <typename K, typename V, typename F>
class HashTable {
public:
    HashTable();
    ~HashTable();
    int get(const K &key, V &value);
    int put(const K &key, const V &value);
    int remove(const K &key);
    size_t get_table_size();
    size_t get_size();
    double get_load_factor();

protected:
    size_t table_size;
    
private:
    HashTable(const HashTable & other);
    const HashTable & operator=(const HashTable & other);
    F hash_func;
    size_t size;
    HashSlot<K, V> *table;

    // Should be overriden by the derived class
    virtual unsigned long get_next_pos(unsigned long pos,
                                       unsigned long step) = 0;
    unsigned long get_pos(const K key);
    void enlarge_table();
};

template <typename K, typename V, typename F>
class LinearProbeHashTable: public HashTable<K, V, F> {
private:
    virtual unsigned long get_next_pos(unsigned long pos, unsigned long step) {
        // TODO
        return 0;
    }
};

template <typename K, typename V, typename F>
class QuadProbeHashTable: public HashTable<K, V, F> {
private:
    virtual unsigned long get_next_pos(unsigned long pos, unsigned long step) {
        // TODO
        return 0;
    }
};

template <typename K, typename V, typename F>
HashTable<K, V, F>::HashTable(): table(), hash_func(),
                                 size(0), table_size(INITIAL_TABLE_SIZE) {
    table = new HashSlot<K, V>[table_size];
}

template <typename K, typename V, typename F>
HashTable<K, V, F>::~HashTable() {
    // TODO
}

template <typename K, typename V, typename F>
void HashTable<K, V, F>::enlarge_table() {
    // TODO
}

template <typename K, typename V, typename F>
unsigned long HashTable<K, V, F>::get_pos(const K key) {
    // TODO
    return 0;
}

template <typename K, typename V, typename F>
int HashTable<K, V, F>::get(const K &key, V &value) {
    // TODO
    return 0;
}

template <typename K, typename V, typename F>
int HashTable<K, V, F>::put(const K &key, const V &value) {
    // TODO
    return 0;
}

template <typename K, typename V, typename F>
int HashTable<K, V, F>::remove(const K &key) {
    // TODO    
    return 0;
}

template <typename K, typename V, typename F>
size_t HashTable<K, V, F>::get_table_size() {
    return table_size;
}

template <typename K, typename V, typename F>
size_t HashTable<K, V, F>::get_size() {
    return size;
}

template <typename K, typename V, typename F>
double HashTable<K, V, F>::get_load_factor() {
    return (double)size/table_size;
}
