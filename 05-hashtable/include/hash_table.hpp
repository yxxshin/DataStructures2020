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
    virtual unsigned long get_next_pos(unsigned long pos, unsigned long step) = 0;
    unsigned long get_pos(const K key);
    void enlarge_table();
};

template <typename K, typename V, typename F>
class LinearProbeHashTable: public HashTable<K, V, F> {
private:
    virtual unsigned long get_next_pos(unsigned long pos, unsigned long step) {
        // TODO
        return (pos + step) % (this->get_table_size());
    }
};

template <typename K, typename V, typename F>
class QuadProbeHashTable: public HashTable<K, V, F> {
private:
    virtual unsigned long get_next_pos(unsigned long pos, unsigned long step) {
        // TODO
        return (pos + (step*(step+1)/2)) % (this->get_table_size());
    }
};

template <typename K, typename V, typename F>
HashTable<K, V, F>::HashTable(): table(), hash_func(), size(0), table_size(INITIAL_TABLE_SIZE) {
    table = new HashSlot<K, V>[table_size];
}

template <typename K, typename V, typename F>
HashTable<K, V, F>::~HashTable() {
    // TODO
    delete[] table;
}

template <typename K, typename V, typename F>
void HashTable<K, V, F>::enlarge_table() {
    // TODO
    HashSlot<K, V>* originalSlot = table;
    this->table_size *= 2;
    this->size = 0;
    table = new HashSlot<K,V>[table_size];
    
    for(size_t i = 0; i < table_size/2; i++){
        if(originalSlot[i].is_empty() == false) {
            // Rehash if there is key
            K key_ = originalSlot[i].get_key();
            V val_ = originalSlot[i].get_value();
            this->put(key_, val_);
        }
    }
    
    delete[] originalSlot;
}

template <typename K, typename V, typename F>
unsigned long HashTable<K, V, F>::get_pos(const K key) {
    // TODO
    return hash_func(key) % table_size;
}

template <typename K, typename V, typename F>
int HashTable<K, V, F>::get(const K &key, V &value) {
    // TODO
    unsigned long step = 0;
    unsigned long start_pos = get_pos(key);
    unsigned long pos = start_pos;
    while(true) {
        if( !table[pos].is_empty() && table[pos].get_key()==key ) {
            // found value : return # of probes
            value = table[pos].get_value();
            return (int)step;
        }
        else if( table[pos].is_empty() && !table[pos].is_removed() ) {
            // not found value but table is empty and not removed
            return -1;
        }
        else if(step >= this->get_table_size()) {
            // finding failed
            return -1;
        }
        step++;
        pos = get_next_pos( start_pos, step );
    }
}

template <typename K, typename V, typename F>
int HashTable<K, V, F>::put(const K &key, const V &value) {
    // TODO
    unsigned long step = 0;
    unsigned long start_pos = get_pos(key);
    unsigned long pos = start_pos;

    if(this->get_load_factor() >= 0.5 ){
        // enlarge if load_factor is same or greater than 0.5
        this->enlarge_table();
    }

    while(true) {
        if( table[pos].is_empty() ) {
            // if empty, put value in
            table[pos].set_key_value(key, value);
            this->size++;
            return (int)step;
        }

        else if( table[pos].get_key() == key ) {
            // there is already a same value in table
            return -1;
        }

        else if( step >= this->get_table_size() ) {
            // failed inserting
            return -1;
        }
        step++;
        pos = get_next_pos( start_pos, step );
    }
}

template <typename K, typename V, typename F>
int HashTable<K, V, F>::remove(const K &key) {

    // TODO
    unsigned long step = 0;
    unsigned long start_pos = get_pos(key);
    unsigned long pos = start_pos;

    while(true) {
        if( table[pos].get_key() == key ){
            // found value: remove it!
            table[pos].set_removed();
            table[pos].set_empty();
            this->size--;
            return (int)step;
        }

        else if( table[pos].is_empty() && !table[pos].is_removed() ){
            // the key is not in value
            return -1;
        }
        step++;
        pos = get_next_pos( start_pos, step );
    }
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
