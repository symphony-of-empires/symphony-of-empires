#include <iostream>
#include <math.h>
#include <algorithm>
#include <vector>
#include <set>
#include <random>

#include "boost/graph/graph_utility.hpp"

#include "global_trade_graph.hpp"

#define PI 3.14159265358979323846
#define EARTH_RADIUS 6378.137
#define DISTANCE_MULTIPLIER 0.1

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
    std::map<int, double> type_cost_converstion{
        // palceholder values
        // cost to move 1 metric ton 1km
        {0, 0.5},   // ocean
        {1, 1.0},   // navigable-river
        {2, 1.0},   // major canal
        {3, 10.0},  // un-improved flatland
        {4, 12.0},  // un-improved hilly
        {5, 15.0},  // un-improved mountain pass
        {6, 5.0},   // basic road
        {7, 4.0},   // improved road
        {8, 2.0},   // inter-state highway
        {9, 3.5},   // railroad lv1
        {10, 3.0},  // railroad lv2
        {11, 2.5},  // railroad lv3
        {12, 2.0},  // railroad lv4
        {13, 1.5},  // railroad lv5
        {14, 1.0},  // railroad lv6
        {15, 1.0},  // special reserved index
        {16, 12.0}, // port lv1
        {17, 9.0},  // port lv2
        {18, 8.0},  // port lv3
        {19, 7.0},  // port lv4
        {20, 6.0},  // port lv5
        {21, 5.0},  // port lv6
        {22, 4.0},  // port lv7
        {23, 2.0},  // port lv8
        {24, 1.0},  // port lv9
        {25, 0.5},  // port lv10
    };
    this->type_cost_converstion = type_cost_converstion;
}

int GlobalTradeGraph::init_graph(std::string filepath) // THIS IS CURRENTLY JUST A DUMMY SIMULATION
{
    // in the future this data is read in data from file

    // STEP 1: create the graph
    int total_nodes = 3;
    this->global_graph = GlobalGraph(total_nodes); // nodes will never be added or removed from the graph

    // // STEP 2: add all node info (in the future this must come from a file)
    // this->global_graph[0].region_id = 0;
    // this->global_graph[0].region_owner_TAG = "MEX";
    // std::set<int> mex_pset;
    // this->global_graph[0].province_set = mex_pset;
    // this->global_graph[0].latitude = 19.459;
    // this->global_graph[0].longitude = -99.129;
    // std::set<int> mex_cset;
    // mex_cset.insert(1);
    // this->global_graph[0].connection_set = mex_cset;

    // this->global_graph[1].region_id = 1;
    // this->global_graph[1].region_owner_TAG = "USA";
    // std::set<int> usa_pset;
    // this->global_graph[1].province_set = usa_pset;
    // this->global_graph[1].latitude = 41.802;
    // this->global_graph[1].longitude = -87.681;
    // std::set<int> usa_cset;
    // usa_cset.insert(2);
    // usa_cset.insert(0);
    // this->global_graph[1].connection_set = usa_cset;

    // this->global_graph[2].region_id = 2;
    // this->global_graph[2].region_owner_TAG = "CAN";
    // std::set<int> can_pset;
    // this->global_graph[2].province_set = can_pset;
    // this->global_graph[2].latitude = 50.451;
    // this->global_graph[2].longitude = -104.599;
    // std::set<int> can_cset;
    // can_cset.insert(1);
    // this->global_graph[2].connection_set = can_cset;

    // // STEP 3: add all edge info (in the future this must come from a file)
    // GlobalTradeGraph::add_edge(std::make_pair(0, 1), 3);
    // GlobalTradeGraph::add_edge(std::make_pair(1, 2), 3);

    // this->total_nodes = total_nodes;
    return total_nodes;
}

int GlobalTradeGraph::init_random_graph(int nodes, int max_degree)
{
    this->global_graph = GlobalGraph(nodes);
    this->total_nodes = nodes - 1;

    for (int n = 0; n < this->total_nodes + 1; n++) //what is happening here?
    {
        this->global_graph[n].region_id = n;
        // random TAG
        static const char alphanum[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        srand((unsigned int)time(NULL));
        std::string TAG;
        for (int i = 0; i < 3; ++i)
        {
            TAG += alphanum[rand() % (sizeof(alphanum) - 1)];
        }
        this->global_graph[n].region_owner_TAG = TAG;
        // random province set
        std::random_device rd;  // obtain a random number from hardware
        std::mt19937 gen(rd()); // seed the generator
        int p_nodes = 3248;     // max provinces in vic2
        int max_ps = 5;
        std::set<int> tmp_pset;

        std::uniform_int_distribution<> distr_prov(0, p_nodes); // province IDs
        std::uniform_int_distribution<> distr_maxp(0, max_ps);  // max provinces
        for (int n = 0; n < distr_maxp(gen); ++n)
        {
            tmp_pset.insert(distr_prov(gen));
        }
        this->global_graph[n].province_set = tmp_pset;
        // random lat long
        std::uniform_real_distribution<> distr_lat(-89.9, 89.9);
        std::uniform_real_distribution<> distr_lon(-179.9, 179.9);

        double tlat = distr_lat(gen);
        double tlon = distr_lon(gen);

        this->global_graph[n].latitude = tlat;
        this->global_graph[n].longitude = tlon;
        // random connection set
        std::set<int> t_cset;
        std::set<std::pair<int, int> > connections_to_add;

        std::uniform_int_distribution<> distr_degree(1, max_degree);
        std::uniform_int_distribution<> distr_second(0, this->total_nodes);

        while (t_cset.empty())
        {
            for (int d = 0; d < distr_degree(gen); d++)
            {
                int second_n = distr_second(gen);
                if (second_n != n && t_cset.find(second_n) == t_cset.end())
                {
                    std::pair<int, int> new_connection = (n < second_n) ? std::make_pair(n, second_n) : std::make_pair(second_n, n);
                    t_cset.insert(second_n);
                    connections_to_add.insert(new_connection);
                }
            }
        }
        this->global_graph[n].connection_set = t_cset;

        // add edges
        std::uniform_int_distribution<> distr_ctype(0, 25); //connection type
        for (auto cit = connections_to_add.begin(); cit != connections_to_add.end(); cit++)
        {
            if (this->edge_mapping.find(*cit) == this->edge_mapping.end())
            {
                GlobalTradeGraph::add_edge(*cit, distr_ctype(gen));
            }
        }
    }
    return nodes;
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
    if (verbose && this->total_nodes < 10)
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
    // returns the cost to move 1 metric ton over some distance
    double cost = (distance * DISTANCE_MULTIPLIER) * this->type_cost_converstion[connection_type]; // placeholder
    return cost;
}

std::string GlobalTradeGraph::get_region_owner_TAG(int current_node)
{
    return "UNK"; // LOOKUP in the future based on province IDs
}

GlobalGraph GlobalTradeGraph::return_country_graph(GlobalGraph &g, std::string owner_TAG)
{
    // copy the graph and prune it according to diplomatic status
    GlobalGraph pruned_g = g; // shallow copy good enough?
    return pruned_g;
}
