#pragma once

#include <queue>

#include "global_trade_graph.hpp"

struct MatrixContainer
{
    std::vector<std::vector<double> > cost_matrix;
    int max_node_size;

    MatrixContainer(int n);
    void update_non_hub_regions(GlobalTradeGraph &gt, std::vector<std::vector<double> > &cost_matrix, std::vector<int> regions_id_set, bool parallel);
    void update_all_regions(GlobalTradeGraph &gt, std::vector<std::vector<double> > &cost_matrix, std::vector<int> region_id_set, bool parallel);
    void summarize_matrix(bool verbose);
};