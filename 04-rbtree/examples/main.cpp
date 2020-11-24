#include <random>
#include <vector>
#include <algorithm>
#include <bitset>

#include "rbtree.hpp"

int main(int argc, char *argv[]) {
    
    std::vector<int> xs{};
    RBTree<int> rbtree{};
    size_t N = 5;
    for (auto i = 1; i <= N; i++){
        xs.emplace_back(i);
    }

    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(xs.begin(), xs.end(), g);
    
    for(auto i : xs) {
        rbtree.insert(i);
    }
    std::cout << rbtree << '\n';

    rbtree.remove_max();
    std::cout << rbtree << '\n';

    // std::vector<int> xs{};
    // std::vector<int> ys{};
    // RBTree<int> rbtree{};
    // size_t N = 100;

    // for (auto i = 1; i <= N; i++){
    //     xs.emplace_back(i);
    //     ys.emplace_back(i);
    // }
        

    // std::random_device rd;
    // std::mt19937 g(rd());

    // std::shuffle(xs.begin(), xs.end(), g);
    // std::shuffle(ys.begin(), ys.end(), g);

    // for (auto i : xs) {
    //     rbtree.insert(i);
    //     std::cout << rbtree << '\n';
    // }

    // for (auto i : ys) {
    //     rbtree.remove(i);
    //     std::cout << rbtree << '\n';
    // }

    
    // RBTree<int> rbtree{};
    // rbtree.insert(2);
    // rbtree.insert(5);
    // rbtree.insert(10);
    // rbtree.insert(7);
    // rbtree.insert(1);
    // rbtree.insert(3);
    // rbtree.insert(9);
    // rbtree.insert(8);
    // rbtree.insert(4);
    // rbtree.insert(6);
    // std::cout << rbtree << std::endl;
    // rbtree.remove(3);
    // std::cout << rbtree << std::endl;
    // rbtree.remove(2);
    // std::cout << rbtree << std::endl;
    // rbtree.remove(4);
    // std::cout << rbtree << std::endl;
    

/* 
    const auto& all_leaves = rbtree.collect_all_leaves();
    for (const auto &[p, n] : all_leaves)
        std::cout << "path: " << std::bitset<16>{p.p_}
                  << "\tlen: " << p.len_
                  << "\tblack: " << p.num_black_ << '\n';
*/
    return 0;
}
