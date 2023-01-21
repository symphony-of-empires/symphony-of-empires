#pragma once

#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "boost/graph/adjacency_list.hpp"

#include "world.hpp"

#define CONNECTION_TYPE_TO_COST                       \
    std::map<int, std::pair<double, std::string> >    \
    {                                                 \
        {0, {0.2, "ocean"}},                          \
            {1, {1.0, "navigable-river"}},            \
            {2, {1.0, "major canal"}},                \
            {3, {10.0, "un-improved flatland"}},      \
            {4, {12.0, "un-improved hilly"}},         \
            {5, {15.0, "un-improved mountain pass"}}, \
            {6, {5.0, "basic road"}},                 \
            {7, {4.0, "improved road"}},              \
            {8, {2.0, "inter-state highway"}},        \
            {9, {3.5, "railroad lv1"}},               \
            {10, {3.0, "railroad lv2"}},              \
            {11, {2.5, "railroad lv3"}},              \
            {12, {2.0, "railroad lv4"}},              \
            {13, {1.5, "railroad lv5"}},              \
            {14, {1.0, "railroad lv6"}},              \
            {15, {1.0, "seasonal-impasse"}},          \
            {16, {12.0, "port lv1"}},                 \
            {17, {9.0, "port lv2"}},                  \
            {18, {8.0, "port lv3"}},                  \
            {19, {7.0, "port lv4"}},                  \
            {20, {6.0, "port lv5"}},                  \
            {21, {5.0, "port lv6"}},                  \
            {22, {4.0, "port lv7"}},                  \
            {23, {2.0, "port lv8"}},                  \
            {24, {1.0, "port lv9"}},                  \
            {25, {0.5, "port lv10"}},                 \
    }

#define GEOGRAPHY_TYPE_TO_COST                     \
    std::map<int, std::pair<double, std::string> > \
    {                                              \
        {0, {1.0, "ocean"}},                       \
            {1, {1.0, "river"}},                   \
            {2, {1.0, "plains"}},                  \
            {3, {1.0, "hills"}},                   \
            {4, {1.0, "mountain"}},                \
            {5, {1.0, "broken"}},                  \
            {6, {1.0, "marsh"}},                   \
    }

#define VEGETATION_TYPE_TO_COST                    \
    std::map<int, std::pair<double, std::string> > \
    {                                              \
        {0, {1.0, "clear"}},                       \
            {1, {1.0, "steppe"}},                  \
            {2, {1.0, "woodland"}},                \
            {3, {1.0, "forest"}},                  \
            {4, {1.0, "desert"}},                  \
            {5, {1.0, "frozen"}},                  \
    }

struct location
{
    float y, x; // lat, long
};

struct RegionNode
{
    int region_id;
    int region_type;
    int region_economic_importance;
    std::string region_owner_TAG;
    std::set<int> province_id_set;
    location region_centroid;
    std::set<int> connection_set;
};

struct ConnectionEdge
{
    int connection_type;
    double distance;
    double capacity;
    double cost;
};

struct GlobalWorld
{
};

typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS,
                              RegionNode, ConnectionEdge, GlobalWorld>
    GlobalGraph;
typedef boost::graph_traits<GlobalGraph>::vertex_descriptor Node_Descriptor;
typedef boost::graph_traits<GlobalGraph>::edge_descriptor Edge_Descriptor;
typedef std::pair<std::pair<int, int>, Edge_Descriptor> Connection_Edge_Descriptor_Lookup;

struct GlobalTradeGraph
{
    GlobalGraph global_graph;
    int total_region_nodes;
    std::vector<int> currently_used_nodes;
    std::vector<int> hub_nodes;
    std::unordered_map<int, std::set<int> > region_to_service_hubs;
    std::vector<location> locations;

    GlobalTradeGraph();
    int init_graph(const World& world);
    int init_random_graph(int nodes);
    void add_edge(int n1, int n2, int connection_type);
    void update_edge(int n1, int n2, int new_connection_type);
    double calculate_distance_lat_lon(int n1, int n2);
    double calculate_edge_cost(int connection_type, double distance);
    std::set<int> get_hubs_distance(int n);
    std::string get_region_owner_TAG(int current_node);
    GlobalGraph return_country_graph(GlobalGraph &g, std::string owner_TAG);
    void summarize_graph(bool verbose);
};
