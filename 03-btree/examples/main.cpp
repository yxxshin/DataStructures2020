#include <iostream>
#include <vector>
#include <random>

#include "btree.hpp"

int main(int argc, char *argv[]) {
    
    
    // BTree<int, 3> btree;
    // btree.insert(7);
    // std::cout << btree.format() << std::endl;
    // btree.insert(1);
    // std::cout << btree.format() << std::endl;
    // btree.insert(3);
    // std::cout << btree.format() << std::endl;
    // btree.insert(10);
    // std::cout << btree.format() << std::endl;
    // btree.insert(8);
    // std::cout << btree.format() << std::endl;
    // btree.insert(5);
    // std::cout << btree.format() << std::endl;
    // btree.insert(6);
    // std::cout << btree.format() << std::endl;
    // btree.insert(9);
    // std::cout << btree.format() << std::endl;
    // btree.insert(4);
    // std::cout << btree.format() << std::endl;
    // btree.insert(2);
    // std::cout << btree.format() << std::endl;
    // btree.insert(11);
    // std::cout << btree.format() << std::endl;
    // btree.insert(12);
    // std::cout << btree.format() << std::endl;
    // btree.insert(13);
    // std::cout << btree.format() << std::endl;
    // btree.insert(16);
    // std::cout << btree.format() << std::endl;
    // btree.insert(21);
    // std::cout << btree.format() << std::endl;
    // btree.insert(20);
    // std::cout << btree.format() << std::endl;
    // btree.insert(19);
    // std::cout << btree.format() << std::endl;
    // btree.insert(30);
    // std::cout << btree.format() << std::endl;
    // btree.insert(31);
    // std::cout << btree.format() << std::endl;
    // btree.insert(32);
    // std::cout << btree.format() << std::endl;
    // btree.insert(25);
    // std::cout << btree.format() << std::endl;
    // btree.insert(26);
    // std::cout << btree.format() << std::endl;
    // btree.insert(27);
    // std::cout << btree.format() << std::endl;

    
    // btree.remove(9);
    // std::cout << btree.format() << std::endl;
    // btree.remove(31);
    // std::cout << btree.format() << std::endl;

    BTree<int, 3> btree;
    std::vector<int> xs;
    std::vector<int> ys;

    for (auto i = 1; i <= 30; i++){
        xs.emplace_back(i);
        ys.emplace_back(i);
    }

    ys.pop_back();
    std::random_device rd;
    std::mt19937 g(rd());

    std::shuffle(xs.begin(), xs.end(), g);
    std::shuffle(ys.begin(), ys.end(), g);

    for (auto i : xs){
        std::cout << "Insert " << i << std::endl;
        btree.insert(i);
    }

    std::cout << btree.format() << '\n';

    std::cout << btree.format() << std::endl;
    for (auto i : ys){
        std::cout << "Delete " << i << std::endl;
        btree.remove(i);
        std::cout << btree.format() << std::endl;
    }
    return 0;
    
}
