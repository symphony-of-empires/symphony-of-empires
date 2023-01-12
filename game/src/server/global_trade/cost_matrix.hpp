#pragma once

#include <queue>

#include "global_trade_graph.hpp"

struct MatrixContainer
{
    int node_size;
    std::vector<std::vector<double> > cost_matrix; // there is almost certainly a better way to do this

    MatrixContainer(int node_size);

    void update_all_paths(std::vector<std::vector<double> > &cost_matrix, GlobalTradeGraph &g, int option, bool parallel); // run at game start only
    void update_subset_paths(std::vector<std::vector<double> > &cost_matrix, GlobalTradeGraph &g, std::priority_queue<int> pq, int option, bool parallel);
    void update_matrix_dijkstra(std::vector<std::vector<double> > &cost_matrix, GlobalTradeGraph &gt, int region_id, std::string owner_TAG);

    void summarize_matrix(bool verbose);
};