#ifndef __DIJKSTRA_SHORTEST_PATHS_H_
#define __DIJKSTRA_SHORTEST_PATHS_H_

#include <unordered_map>
#include <optional>
#include <tuple>
#include <queue>
#include <map>
#include <algorithm>
/* Feel free to add more standard library headers */

#include "graph.hpp"
#include "catch.hpp"

#define INF (vertex_t)-1

/* Given a vertex `v`, `dijkstra_shortest_path` is:
 * - `nullopt` if `v` is not reacheble from `src`.
 * - {`u`, `d`} where `u` is the predecessor of `v` and `d` is the distance
 *   from `src`
 */

std::unordered_map<vertex_t,
    std::optional<std::tuple<vertex_t, edge_weight_t>>>
dijkstra_shortest_path(Graph& g, vertex_t src) {
    // get informations from g
    std::unordered_map<vertex_t, std::optional< std::tuple<vertex_t, edge_weight_t>>> dist_map; // dist_map[i] is the minimum distance going to node i, gets updated.
    edges_t graph_edges = g.get_edges();
    size_t graph_num_vertices = g.get_num_vertices();
    GraphType graph_type = g.get_type();

    for( size_t i = 0; i < graph_num_vertices; i++) {
        dist_map[i] = std::make_optional(std::make_tuple(NULL,INF));
    }

    // use priority_queue for heap
    std::priority_queue< std::pair<edge_weight_t, vertex_t> > queue;

    // initialize (start from src)
    queue.push( std::make_pair(0, src));
    dist_map[src] = std::make_optional(std::make_tuple(INF, 0));

    // get connection matrix (conn_map[from](to, distance))
    std::vector<std::pair<vertex_t, edge_weight_t>> conn_mat[10000];  // Assumed that the maximum num of vertex are 10000
    for(size_t i = 0; i < graph_edges.size(); i++) {
        // push back
        conn_mat[std::get<0>(graph_edges[i])].push_back(std::make_pair( std::get<1>(graph_edges[i]), std::get<2>(graph_edges[i]) ));
        if(graph_type == GraphType::UNDIRECTED) {
            // if undirected graph, push the opposite way too
            conn_mat[std::get<1>(graph_edges[i])].push_back(std::make_pair( std::get<0>(graph_edges[i]), std::get<2>(graph_edges[i]) ));
        }
    }

    // Dijkstra Algorithm
    while (!queue.empty()) {
        vertex_t current_vertex = queue.top().second;
        edge_weight_t cost = (-1) * queue.top().first;      // use priority_queue as minimum heap
        queue.pop();

        // update values
        for(size_t i = 0; i < conn_mat[current_vertex].size(); i++) {
            vertex_t test_vertex = conn_mat[current_vertex][i].first;
            edge_weight_t new_cost = cost + conn_mat[current_vertex][i].second;
            edge_weight_t before_cost = std::get<1>(dist_map[test_vertex].value());

            // if new_cost < before_cost : update
            if(new_cost < before_cost) {
                dist_map[test_vertex] = std::make_optional(std::make_tuple(current_vertex, new_cost));
                queue.push( std::make_pair( -1 * new_cost, test_vertex));
            }
        }
    }

    return dist_map;
}

#endif // __DIJKSTRA_SHORTEST_PATHS_H_
