#include "fheap.hpp"
#include <iostream>
#include <queue>
#include <vector>
#include <random>
#include <algorithm>
#include <map>
#include <ctime>

int main() {
   FibonacciHeap<int> heap;
   // std::random_device rd;
   // std::mt19937 g(rd());
   // size_t N = 1;

   heap.insert(1);
   heap.insert(2);
   heap.insert(3);
   // heap.extract_min();
   // heap.extract_min();
   // std::vector<int> xs{};
   // srand(time(NULL));
   // for (auto i = 1; i <= N; i++) {
   //    int temp = rand()%N;
   //    xs.emplace_back(temp);
   // }
      
   
   // // std::shuffle(xs.begin(), xs.end(), g);
   // for (auto i : xs) {
   //    heap.insert(i);
   //    std::cout << "Inserted " << i << std::endl;
   // }

   // for (int j = 0; j < N; j++){
   //    std::cout << heap.extract_min().value() << std::endl;
   //    std::cout << "**********" << std::endl;
   // }
}
