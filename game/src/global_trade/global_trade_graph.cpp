#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
#include <set>

#include "boost/graph/graph_utility.hpp"

#include "global_trade_graph.hpp"

#define PI 3.14159265358979323846
#define EARTH_RADIUS 6378.137

/* Graph Features (Vertex & Edge) */

/* Useful Utility Functions */
template <class T1, class T2>
std::size_t pair_hash::operator()(const std::pair<T1, T2> &p) const
{
    std::size_t seed = 0;
    // build hash
    boost::hash_combine(seed, p.first);
    boost::hash_combine(seed, p.second);
    return seed;
}

/* GLOBAL TRADE GRAPH */
GlobalTradeGraph::GlobalTradeGraph()
{
}

void GlobalTradeGraph::init_graph(std::string filepath)
{
    // in the future this data is read in data from file

    // STEP 1: create the graph
    int total_nodes = 3;
    this->global_graph = GlobalGraph(total_nodes); // nodes will never be added or removed from the graph

    // STEP 2: add all node info (in the future this must come from a file)
    this->global_graph[0].region_id = 0;
    std::set<int> mex_pset;
    this->global_graph[0].province_set = mex_pset;
    this->global_graph[0].latitude = 19.459;
    this->global_graph[0].longitude = -99.129;
    std::set<int> mex_cset;
    mex_cset.insert(1);
    this->global_graph[0].connection_set = mex_cset;

    this->global_graph[1].region_id = 1;
    std::set<int> usa_pset;
    this->global_graph[1].province_set = usa_pset;
    this->global_graph[1].latitude = 41.802;
    this->global_graph[1].longitude = -87.681;
    std::set<int> usa_cset;
    usa_cset.insert(2);
    usa_cset.insert(0);
    this->global_graph[1].connection_set = usa_cset;

    this->global_graph[2].region_id = 2;
    std::set<int> can_pset;
    this->global_graph[2].province_set = can_pset;
    this->global_graph[2].latitude = 50.451;
    this->global_graph[2].longitude = -104.599;
    std::set<int> can_cset;
    can_cset.insert(1);
    this->global_graph[2].connection_set = can_cset;

    // STEP 3: add all edge info (in the future this must come from a file)
    GlobalTradeGraph::add_edge(std::make_pair(0, 1), 1);
    GlobalTradeGraph::add_edge(std::make_pair(1, 2), 1);
    GlobalTradeGraph::update_edge(std::make_pair(1, 2), 2);
}

double GlobalTradeGraph::calculate_distance_lat_lon(int n1, int n2)
{
    double lat1 = this->global_graph[n1].latitude;
    double lon1 = this->global_graph[n1].longitude;
    double lat2 = this->global_graph[n2].latitude;
    double lon2 = this->global_graph[n2].longitude;

    double dlat = ((lat2 - lat1) * PI) / 180.0;
    double dlon = ((lon2 - lon1) * PI) / 180.0;
    lat1 = (lat1 * PI) / 180.0;
    lat2 = (lat2 * PI) / 180.0;

    double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double b = 2 * asin(sqrt(a));
    return EARTH_RADIUS * b;
}

void GlobalTradeGraph::summarize_graph(bool verbose)
{
    boost::print_graph(this->global_graph);
    if (verbose)
    {
        GlobalGraph::vertex_iterator vit, vend;
        for (boost::tie(vit, vend) = boost::vertices(this->global_graph); vit != vend; ++vit)
        {
            std::cout << "\tregion [ region_id: " << this->global_graph[*vit].region_id << " degree: " << boost::degree(*vit, this->global_graph) << " (" << this->global_graph[*vit].latitude << ", " << this->global_graph[*vit].longitude << ") ]" << std::endl;
        }
        GlobalGraph::edge_iterator eit, eend;
        for (boost::tie(eit, eend) = boost::edges(this->global_graph); eit != eend; ++eit)
        {
            std::cout << "\tedge [ <" << this->global_graph[*eit].connection.first << "," << this->global_graph[*eit].connection.second << "> connection_type: " << this->global_graph[*eit].connection_type << " distance: " << this->global_graph[*eit].distance << " cost: " << this->global_graph[*eit].cost << "]" << std::endl;
        }
    }
}

void GlobalTradeGraph::add_edge(std::pair<int, int> connection, int connection_type)
{
    std::pair<Edge_Descriptor, bool> added_info = boost::add_edge(connection.first, connection.second, this->global_graph);
    Edge_Descriptor added_descriptor = added_info.first;
    bool first_add = added_info.second;
    if (first_add)
    {
        this->global_graph[added_descriptor].connection = connection;
        this->global_graph[added_descriptor].connection_type = connection_type;
        double distance = GlobalTradeGraph::calculate_distance_lat_lon(connection.first, connection.second);
        this->global_graph[added_descriptor].distance = distance;
        this->global_graph[added_descriptor].cost = GlobalTradeGraph::calculate_edge_cost(connection_type, distance);
        Edge_Key_Value new_add = std::make_pair(connection, added_descriptor);
        this->edge_mapping.insert(new_add);
    }
}

void GlobalTradeGraph::update_edge(std::pair<int, int> connection, int new_connection_type)
{
    Edge_Descriptor to_update = this->edge_mapping[connection];
    this->global_graph[to_update].connection_type = new_connection_type;
    this->global_graph[to_update].cost = GlobalTradeGraph::calculate_edge_cost(this->global_graph[to_update].connection_type, this->global_graph[to_update].distance);
}

double GlobalTradeGraph::calculate_edge_cost(int connection_type, double distance)
{
    double cost = (distance / 10.0) / connection_type; // placeholder
    return cost;
}
