#ifndef __TOPOLOGICAL_SORT_H_
#define __TOPOLOGICAL_SORT_H_

#include <vector>
#include <queue>

#include "graph.hpp"
#include "catch.hpp"

/* Feel free to add more standard library headers */

/* Return _a_ valid topologically sorted sequence of vertex descriptors */
std::vector<vertex_t> topological_sort(Graph& g) {
    edges_t graph_edges = g.get_edges();
    size_t graph_num_vertices = g.get_num_vertices();
    std::vector<size_t> numlink;    // numlink[i] : number of edges coming into vertex i
    for(size_t i = 0; i < graph_num_vertices; i++){
        numlink.push_back(0);
    }

    // set numlink
    for(size_t i = 0; i < graph_edges.size(); i++) {
        numlink[std::get<1>(graph_edges[i])]++;
    }

    // use queue
    std::queue<vertex_t> queue;
    std::vector<vertex_t> result; 
    
    // Topological Sort
    for(size_t i = 0; i < graph_num_vertices; i++){
        if(numlink[i] == 0) {
            queue.push(i);
        }
    }

    while(!queue.empty()) {
        vertex_t current_vertex = queue.front();
        result.push_back(current_vertex);
        queue.pop();
        for(size_t i = 0; i < graph_edges.size(); i++) {
            // delete all edges from current_vertex
            if(std::get<0>(graph_edges[i]) == current_vertex) {
                numlink[std::get<1>(graph_edges[i])]--;
                // if numlink = 0, push to queue
                if(numlink[std::get<1>(graph_edges[i])] == 0) {
                    queue.push(std::get<1>(graph_edges[i]));
                }
            }
        }
    }

    return result;
}

#endif // __TOPOLOGICAL_SORT_H_
