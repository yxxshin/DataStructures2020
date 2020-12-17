#ifndef __PRIM_MINIMUM_SPANNING_TREE_H_
#define __PRIM_MINIMUM_SPANNING_TREE_H_

#include <optional>
#include <vector>
#include <queue>
#include <algorithm>

#include "catch.hpp"

#define INF (vertex_t)-1

/* Feel free to add more standard library headers */

/* Returns the vector of edges in the MST, or std::nullopt if MST does
 * not exist */

std::optional<edges_t>
prim_minimum_spanning_tree(Graph& g, vertex_t src) {
	// get informations from g
	edges_t graph_edges = g.get_edges();
	size_t graph_num_vertices = g.get_num_vertices();
	GraphType graph_type = g.get_type();
	
	// make distance & parent vector (updated!)
	std::vector<edge_weight_t> dist;	// dist[i] is the distance of vertex i
	std::vector<vertex_t> parent;		// parent[i] is the parent of vertex i
	std::vector<bool> hasVisited;		// hasVisited[i] = true if visited before
	for(size_t i = 0; i < graph_num_vertices; i++) {
		dist.push_back(INF);
		parent.push_back(graph_num_vertices);	// not existing vertex
		hasVisited.push_back(false);
	}

	// use priority_queue for heap
	std::priority_queue< std::pair<edge_weight_t, vertex_t>> queue;

	// start from src
	queue.push( std::make_pair(0, src));
	dist[src] = 0;
	hasVisited[src] = true;

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
	
	// Prim's Algorithm
	while(!queue.empty()) {
		vertex_t current_vertex = queue.top().second;
		edge_weight_t cost = (-1) * queue.top().first;		// use priority_queue as minimum heap
		hasVisited[current_vertex] = true;
		queue.pop();

		// update values
		for(size_t i = 0; i < conn_mat[current_vertex].size(); i++) {
			vertex_t test_vertex = conn_mat[current_vertex][i].first;
			// if already visited, don't need to check
			if(hasVisited[test_vertex]) continue;
			// if not visited: 
			edge_weight_t new_dist = conn_mat[current_vertex][i].second;
			if(new_dist < dist[test_vertex]) {
				dist[test_vertex] = new_dist;
				parent[test_vertex] = current_vertex;
				queue.push( std::make_pair( -1 * new_dist, test_vertex));
			}
		}
	}

	// There is no MST
	bool isMST = true;
	for(size_t i = 0; i < graph_num_vertices; i++ ) {
		// If there is a not-visited node after the Algorithm, it means no MST
		if(!hasVisited[i]) isMST = false;
	}

	if(!isMST) {
		// no MST: return std::nullopt
		return std::nullopt;
	}
	
	edges_t temp_result;
	edges_t result;		// returning result

	// make temp_result: in bidirectional way
	for(size_t i = 0; i < graph_num_vertices; i++) {
		if(parent[i] == graph_num_vertices) continue;
		else {
			edge_t temp2 = std::make_tuple(parent[i], i, dist[i]);
			temp_result.push_back(temp2);
		}
	}

	// sorting edges to pass the test :(
	size_t edge_num = graph_num_vertices - 1;
	vertex_t now_vertex = src;
	
	std::vector<bool> didVisit;
	for(size_t i = 0; i < graph_num_vertices; i++){
		didVisit.push_back(false);
	}
	
	// push in the first edge
	size_t temp = 1;
	result.push_back(temp_result[0]);
	didVisit[std::get<0>(temp_result[0])] = true;
	didVisit[std::get<1>(temp_result[0])] = true;
	edge_num--;

	// put in the other edges
	while(edge_num > 0) {
		vertex_t start = std::get<0>(temp_result[temp]);
		vertex_t end = std::get<1>(temp_result[temp]);
		// only select when one of the nodes are visited
		if((!didVisit[start]&&didVisit[end]) || (didVisit[start]&&!didVisit[end])) {
			didVisit[start] = true;
			didVisit[end] = true;
			result.push_back(temp_result[temp]);
			edge_num--;
		}
		temp++;
		if(temp >= graph_num_vertices - 1) temp = 1;
	}
	return make_optional(result);
}
#endif // __PRIM_MINIMUM_SPANNING_TREE_H_
