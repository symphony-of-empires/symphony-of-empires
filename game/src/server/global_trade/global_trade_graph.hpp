#pragma once

#include <string>
#include <utility>
#include <set>
#include <unordered_map>

#include "boost/graph/adjacency_list.hpp"

struct RegionNode
{
    /* NODE INFORMATION */
    int region_id;
    int region_type; // 0 = ocean, 1 = land
    int region_importance;
    std::string region_owner_TAG;
    std::set<int> province_set;
    double latitude;
    double longitude;
    std::set<int> connection_set;
};

struct ConnectionEdge
{
    /* EDGE INFORMATION */
    std::pair<int, int> connection;
    int connection_type;
    double distance;
    double cost;
};

typedef boost::adjacency_list<boost::vecS, boost::vecS,
                              boost::undirectedS,
                              RegionNode,
                              ConnectionEdge>
    GlobalGraph;
typedef boost::graph_traits<GlobalGraph>::vertex_descriptor Node_Descriptor;
typedef boost::graph_traits<GlobalGraph>::edge_descriptor Edge_Descriptor;
typedef std::pair<std::pair<int, int>, Edge_Descriptor> Edge_Key_Value;

struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &p) const;
};

struct GlobalTradeGraph
{
    unsigned int total_nodes;
    GlobalGraph global_graph;
    std::unordered_map<std::pair<int, int>, Edge_Descriptor, pair_hash> edge_mapping;
    std::map<int, double> type_cost_converstion;

    GlobalTradeGraph();
    int init_graph(std::string filepath);
    int init_random_graph(int nodes, int max_degree);
    void summarize_graph(bool verbose);
    double calculate_distance_lat_lon(int n1, int n2);
    void add_edge(std::pair<int, int> connection, int connection_type);
    void update_edge(std::pair<int, int> connection, int new_connection_type);
    double calculate_edge_cost(int connection_type, double distance);
    std::string get_region_owner_TAG(int current_node);
    GlobalGraph return_country_graph(GlobalGraph &g, std::string owner_TAG);
};