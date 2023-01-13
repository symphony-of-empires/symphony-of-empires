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

template <typename T>
bool allEqual(std::vector<T> const &v)
{
    if (v.size() == 0)
    {
        return false;
    }
    return std::all_of(v.begin(), v.end(), [&](T const &e)
                       { return e == v.front(); });
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

int GlobalTradeGraph::init_graph(std::string filepath)
{
    // this is not implemented yet
    int total_nodes = 3;
    this->global_graph = GlobalGraph(total_nodes);
    return total_nodes;
}

int GlobalTradeGraph::init_random_graph(int nodes, int max_degree)
{
    double percent_water = .16; // 16% of the world was ocean in vic2

    this->global_graph = GlobalGraph(nodes);
    this->total_nodes = nodes - 1;

    for (int n = 0; n < this->total_nodes + 1; n++) //what is happening here?
    {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> distr_01(0, 1);
        this->global_graph[n].region_id = n;
        // random geography assignment
        double water_or_land = distr_01(gen);
        if (water_or_land <= percent_water)
        {
            this->global_graph[n].region_type = 0;
        }
        else
        {
            this->global_graph[n].region_type = 1;
        }
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
        int p_nodes = 3248; // max provinces in vic2
        int max_ps = 5;
        std::set<int> tmp_pset;
        std::uniform_int_distribution<> distr_prov(0, p_nodes);
        std::uniform_int_distribution<> distr_maxp(0, max_ps);
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

void GlobalTradeGraph::summarize_graph(bool verbose) // sanity check
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
    double cost = (distance * DISTANCE_MULTIPLIER) * this->type_cost_converstion[connection_type];
    return cost;
}

std::string GlobalTradeGraph::get_region_owner_TAG(int current_node)
{
    std::vector<std::string> p_owners;
    bool enemy_occupation = false;
    for (int p : this->global_graph[current_node].province_set)
    {
        // std::string p_owner_TAG = lookup(p);
        // p_owners.insert(p_owner_TAG);
        // if (check_diplo_status(owner_TAG, p_owner_TAG))
        // {
        //     enemy_occupation = true;
        // }
    }
    if (allEqual(p_owners))
    {
        return p_owners[0];
    }
    else if (!allEqual(p_owners) && !enemy_occupation)
    {
        return "CONTESTED_FRIENDLY";
    }
    else if (!allEqual(p_owners) && enemy_occupation)
    {
        return "CONTESTED_HOSTILE";
    }
    return "UNK";
}

GlobalGraph GlobalTradeGraph::return_country_graph(GlobalGraph &g, std::string owner_TAG)
{
    /* nation N should not be able to access RegionNodes whose owner:
        1. is at war with N
        2. is isolationist
        3. is uncivilized
        4. is another nations colony
        5. has a relationship below a certain threshold
    */
    GlobalGraph pruned_g = g;
    for (int n = 0; n < this->total_nodes; n++)
    {
        // if (check_diplo_status(owner_TAG, g[n].region_owner_TAG))
        // {
        //     if (not good relations)
        //     {
        //         boost::clear_vertex(n, pruned_g);
        //     }
        // }
    }
    return pruned_g;
}
