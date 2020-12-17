#include <string>
#include <cstddef>
#include <iostream>
#include <vector>
#include <tuple>

#include "graph.hpp"
#include "topological_sort.hpp"
#include "dijkstra_shortest_paths.hpp"
#include "prim_minimum_spanning_tree.hpp"

int main(int argc, char *argv[]) {
    edges_t prim_edges = {{0, 1, 4.0f},
                              {0, 7, 8.0f},
                              {1, 2, 8.0f},
                              {1, 7, 11.0f},
                              {2, 3, 7.0f},
                              {2, 8, 2.0f},
                              {2, 5, 4.0f},
                              {3, 4, 9.0f},
                              {3, 5, 14.0f},
                              {4, 5, 10.0f},
                              {5, 6, 2.0f},
                              {6, 7, 1.0f},
                              {6, 8, 6.0f},
                              {7, 8, 7.0f}};

        Graph g(9, prim_edges, GraphType::UNDIRECTED);
        std::optional<edges_t> prim_result
            = prim_minimum_spanning_tree(g, 2);

    edges_t result = prim_result.value();
    for(size_t i = 0; i < result.size(); i++) {
        std::cout << std::get<0>(result[i]) << " :: " << std::get<1>(result[i]) << " 거리= " << std::get<2>(result[i]) << std::endl;
    }
}
