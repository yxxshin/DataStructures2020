#include <iostream>
#include <vector>
#include <random>
#include <assert.h>

#include "hash_table.hpp"
#include "hash_funcs.hpp"

template <typename HT>
void basic_test(HT &ht) {
    int num_to_test = 3000;
    std::string value;
    bool result;

    unsigned long total_steps = 0;

    for (auto i=0; i<num_to_test; i++) {
        assert(ht.get_load_factor() <= 0.5);
        ht.put(i, std::to_string(i));
    }

    for (auto i=0; i<num_to_test; i++) {
        unsigned long steps = ht.get(i, value);
        total_steps += steps;
        assert(steps >= 0);
        assert(value == std::to_string(i));
    }

    assert(ht.remove(3) >= 0);
    assert(ht.get(3, value) == -1);

    assert(ht.remove(5) >= 0);
    assert(ht.get(5, value) == -1);
    
    std::cout << "[*] size: "<< ht.get_size() << std::endl;
    std::cout << "[*] table_size: "<< ht.get_table_size() << std::endl;
    std::cout << "[*] Load factor: "<< ht.get_load_factor() << std::endl;
}


template <typename HT>
void probe_test(HT &ht, bool is_linear) {
    int num_to_test = INITIAL_TABLE_SIZE/2;
    int arr[num_to_test];
    unsigned long total_steps = 0;

    int after_cluster = 0;
    for (auto i=0; i<num_to_test; i++) {
        if (i < num_to_test/2)
            arr[i] = i*INITIAL_TABLE_SIZE;
        else
            arr[i] = ++after_cluster;
    }

    after_cluster = 0;
    for (auto i=0; i<num_to_test; i++) {
        int step = ht.put(arr[i], std::to_string(arr[i]));
        // std::cout << "[*] " << i << ": " << arr[i] << " , " << step << std::endl;

        if (i < num_to_test/2)
            assert(step == i);
        else {
            if (is_linear)
                assert(step == (num_to_test/2-1));
            else
                assert(step < num_to_test/4);
        }
        total_steps += step;
    }

    if (is_linear)
        std::cout << "[*] probe_test(linear): average # probe: ";
    else
        std::cout << "[*] probe_test(quad): average # probe: ";

    std::cout << total_steps << "/" << num_to_test << " = "
              << (double)total_steps/num_to_test << std::endl;
}

#define LinearIntStrHt LinearProbeHashTable<int, std::string, DefaultHash>
#define QuadIntStrHt QuadProbeHashTable<int, std::string, DefaultHash>

int main() {
    LinearIntStrHt linear_basic_ht;
    basic_test<LinearIntStrHt>(linear_basic_ht);

    QuadIntStrHt quad_basic_ht;
    basic_test<QuadIntStrHt>(quad_basic_ht);

    std::cout << "---------------------------------------------" << std::endl;

    LinearIntStrHt linear_probe_ht;
    probe_test<LinearIntStrHt>(linear_probe_ht, true);
    
    QuadIntStrHt quad_probe_ht;
    probe_test<QuadIntStrHt>(quad_probe_ht, false);

    return 0;
}
