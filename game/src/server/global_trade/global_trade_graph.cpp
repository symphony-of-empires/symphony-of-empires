#include <algorithm>
#include <ctime>
#include <chrono>
#include <math.h>
#include <iostream>
#include <set>
#include <vector>

#include "boost/random.hpp"
#include "boost/random/uniform_real_distribution.hpp"
#include "boost/random/uniform_int_distribution.hpp"
#include "boost/graph/graph_utility.hpp"

#include "global_trade_graph.hpp"
#include "province.hpp"

#define PI 3.14159265358979323846
#define EARTH_RADIUS 6378.137
#define GAME_DISTANCE_MULTIPLIER 0.1

/* Useful Utility Functions */
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

location random_location(float lat1, float lon1, float lat2, float lon2, double seed)
{
    boost::random::mt19937 gen(seed);
    float lat_t1 = (lat1 < lat2) ? lat1 : lat2;
    float lat_t2 = (lat1 < lat2) ? lat2 : lat1;
    boost::random::uniform_real_distribution<> dist_lat(lat_t1, lat_t2); // bounding box west to east
    boost::random::uniform_real_distribution<> dist_lon(lon1, lon2);
    return {static_cast<float>(dist_lat(gen)), static_cast<float>(dist_lon(gen))};
};

std::set<int> random_connection(int degree, int min, int max, double seed)
{
    boost::random::mt19937 gen(seed);
    boost::random::uniform_int_distribution<> dist_range(min, max);
    boost::random::uniform_int_distribution<> dist_degree(3, degree);
    int try_degree = dist_degree(gen);
    int length = ((max - min) < try_degree) ? (max - min) : try_degree;
    std::set<int> connection;
    for (int i = 0; i < length; ++i)
    {
        while (connection.size() != length)
        {
            int element = dist_range(gen);
            connection.insert(element);
        }
    }
    return connection;
}

/* GLOBAL TRADE GRAPH */
GlobalTradeGraph::GlobalTradeGraph()
{
}

int GlobalTradeGraph::init_graph(const World& world)
{
    /* setup graph */
    int total_region_nodes = world.provinces.size();
    this->global_graph = GlobalGraph(total_region_nodes);
    this->currently_used_nodes.resize(total_region_nodes);
    iota(this->currently_used_nodes.begin(), this->currently_used_nodes.end(), 0);
    int max_node_index = total_region_nodes - 1;
    int num_hub_nodes = 25;     // we need to calculate hub nodes
    std::vector<location> locations_vec; // we need to get locations

    /* add information to empty nodes */
    for(const auto& province : world.provinces) {
        for(const auto neighbour_id : province.neighbour_ids) {
            /* add node info & connect them */
            int p_id = province.get_id();
            this->global_graph[p_id].region_id = p_id;
            this->global_graph[p_id].region_type = (!province.is_coastal) ? (int) province.terrain_type_id % 25 : 0; // for safety
            this->global_graph[p_id].region_centroid = { province.get_pos().y, province.get_pos().x };
            this->global_graph[p_id].connection_set.insert(neighbour_id);
            this->add_edge(p_id, neighbour_id, 0);
            /* add locations to graph */
            this->locations.push_back(this->global_graph[p_id].region_centroid);
            /* if hub nodes push back */
            if(province < num_hub_nodes) {
                this->hub_nodes.push_back(p_id);
            }
        }
    }
    return total_region_nodes;
}

/* realistic simulation (this will be deleted after testing is done) */
int GlobalTradeGraph::init_random_graph(int nodes)
{
    this->global_graph = GlobalGraph(nodes);
    this->currently_used_nodes.resize(nodes);
    iota(this->currently_used_nodes.begin(), this->currently_used_nodes.end(), 0);
    int max_node_index = nodes - 1;
    /* setup realistic simulation */
    double percent_ocean = .16; // 16% of the world was ocean in vic2
    int num_hub_nodes = 25;     //reserve 0-25 for hub nodes
    int hubs_per_node = 2;
    std::vector<location> locations_vec;
    typedef std::pair<location, std::pair<location, location> > region_bounding_box_struct;
    std::map<int, std::pair<region_bounding_box_struct, std::string> > continents{
        {0, {{{43.33f, -108.325f}, {{62.25f, -143.79f}, {24.41f, -72.86f}}}, "North America"}},
        {1, {{{-22.45f, -59.285f}, {{2.20f, -82.38f}, {-47.10f, -36.19f}}}, "South America"}},
        {2, {{{53.87f, 7.935f}, {{64.41f, -17.68f}, {43.33f, 33.55f}}}, "Europe"}},
        {3, {{{43.385f, 98.665f}, {{65.02f, 68.53f}, {21.75f, 128.8f}}}, "Asia"}},
        {4, {{{0.425f, 16.465f}, {{33.04f, -10.82f}, {-32.19f, 43.75f}}}, "Africa"}},
        {5, {{{-24.025f, 141.925f}, {{-6.18f, 116.26f}, {-41.87f, 167.59f}}}, "Oceania"}},
    };
    int max_nodes_per_continent = ((nodes - num_hub_nodes) + continents.size() - 1) / continents.size();
    /* first generate all the hub nodes */
    for (int h = 0; h < num_hub_nodes; h++)
    {
        this->global_graph[h].region_id = h;
        this->global_graph[h].region_type = 0;
        auto now = std::chrono::high_resolution_clock::now();
        this->global_graph[h].region_centroid = random_location(-90, -180, 90, 180,
                                                                std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
        this->global_graph[h].connection_set = random_connection(3, 0, num_hub_nodes - 1,
                                                                 std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
        this->locations.push_back(this->global_graph[h].region_centroid);
        /* connect them */
        for (int c : this->global_graph[h].connection_set)
        {
            this->add_edge(c, h, 0);
        }
        this->hub_nodes.push_back(h);
    }
    /* next generate all continents */
    boost::random::mt19937 gen(std::time(0));
    boost::random::uniform_int_distribution<> dist_ctype(1, 25);
    int n = num_hub_nodes;
    int continent = 0;
    while (continent < continents.size())
    {
        int min_index = (continent * max_nodes_per_continent) + num_hub_nodes;
        int max_index = std::min((min_index + (max_nodes_per_continent - 1)), max_node_index);
        if (min_index > max_index)
        {
            break;
        }
        for (int n = min_index; n <= max_index; n++)
        {
            region_bounding_box_struct continent_bounding_box = continents[continent].first;
            this->global_graph[n].region_id = n;
            this->global_graph[n].region_type = 2;
            auto now = std::chrono::high_resolution_clock::now();
            this->global_graph[n].region_centroid = random_location(continent_bounding_box.second.first.y,
                                                                    continent_bounding_box.second.first.x,
                                                                    continent_bounding_box.second.second.y,
                                                                    continent_bounding_box.second.second.x,
                                                                    std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
            std::set<int> new_conns = random_connection(5,
                                                        min_index,
                                                        std::min(max_node_index, max_index),
                                                        std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count());
            this->global_graph[n].connection_set = new_conns;
            this->locations.push_back(this->global_graph[n].region_centroid);
            /* connect them */
            for (int c : this->global_graph[n].connection_set)
            {
                this->add_edge(c, n, dist_ctype(gen));
            }
        }
        continent++;
    }
    /* link hub nodes to continents */
    for (int n = num_hub_nodes; n < nodes; n++)
    {
        for (int h : this->get_hubs_distance(n))
        {
            auto now = std::chrono::high_resolution_clock::now();
            auto seed = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
            boost::random::mt19937 gen(seed);
            boost::random::uniform_real_distribution<> dist_rand_hc_chance(0, 1);
            if (dist_rand_hc_chance(gen) < .9 || n % max_nodes_per_continent)
            {
                this->add_edge(n, h, 0);
            }
            this->region_to_service_hubs[n].insert(h);
        }
    }
    /* add a few random connections */
    for (int n = 0; n < nodes; n++)
    {
        boost::random::uniform_int_distribution<> dist_all_nodes(0, max_node_index);
        boost::random::uniform_real_distribution<> dist_rand_rc_chance(0, 1);
        if (dist_rand_rc_chance(gen) < .99)
        {
            this->add_edge(dist_all_nodes(gen), dist_all_nodes(gen), dist_ctype(gen));
        }
    }
    return nodes;
}

double GlobalTradeGraph::calculate_distance_lat_lon(int n1, int n2)
{
    double lat1 = this->global_graph[n1].region_centroid.y;
    double lon1 = this->global_graph[n1].region_centroid.x;
    double lat2 = this->global_graph[n2].region_centroid.y;
    double lon2 = this->global_graph[n2].region_centroid.x;

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
}

void GlobalTradeGraph::add_edge(int n1, int n2, int connection_type)
{
    int n1t, n2t;
    if (n1 == n2)
    {
        return;
    }
    if (n1 < n2)
    {
        n1t = n1, n2t = n2;
    }
    else
    {
        n1t = n2, n2t = n1;
    }
    std::pair<Edge_Descriptor, bool> get_edge = boost::edge(n1t, n2t, global_graph);
    if (!get_edge.second)
    {
        std::pair<Edge_Descriptor, bool> added_edge = boost::add_edge(n1t, n2t, this->global_graph);
        this->global_graph[added_edge.first].connection_type = connection_type;
        this->global_graph[added_edge.first].distance = this->calculate_distance_lat_lon(n1t, n2t);
        this->global_graph[added_edge.first].cost = this->calculate_edge_cost(connection_type, this->global_graph[added_edge.first].distance);
    }
}

void GlobalTradeGraph::update_edge(int n1, int n2, int new_connection_type)
{
    Edge_Descriptor edge_desc;
    std::pair<Edge_Descriptor, bool> get_edge = boost::edge(n1, n2, global_graph);
    if (get_edge.second)
    {
        edge_desc = get_edge.first;
        this->global_graph[edge_desc].connection_type = new_connection_type;
        this->global_graph[edge_desc].cost = this->calculate_edge_cost(new_connection_type, this->global_graph[edge_desc].distance);
    }
}

/* returns the cost to move 1 metric ton over some distance value */
double GlobalTradeGraph::calculate_edge_cost(int connection_type, double distance)
{
    return (distance * GAME_DISTANCE_MULTIPLIER) * CONNECTION_TYPE_TO_COST[connection_type].first;
}

std::set<int> GlobalTradeGraph::get_hubs_distance(int n)
{
    std::set<int> hubs;
    std::vector<double> cost_to_hubs;
    for (int h : this->hub_nodes)
    {
        cost_to_hubs.push_back(this->calculate_distance_lat_lon(n, h));
    }
    auto min_cost_idx = std::min_element(cost_to_hubs.begin(), cost_to_hubs.end());
    auto second_min_cost_idx = std::min_element(cost_to_hubs.begin(), cost_to_hubs.end(),
                                                [&min_cost_idx](const double &a, const double &b)
                                                { return a != *min_cost_idx && a < b; });
    hubs.insert(std::distance(cost_to_hubs.begin(), min_cost_idx));
    hubs.insert(std::distance(cost_to_hubs.begin(), second_min_cost_idx));
    return hubs;
}

std::string GlobalTradeGraph::get_region_owner_TAG(int current_node)
{
    if (this->global_graph[current_node].province_id_set.empty())
    {
        return "INTERNATIONAL ZONE";
    }
    std::vector<std::string> p_owners;
    bool enemy_occupation = false;
    bool same_economic_market = true;
    for (int p : this->global_graph[current_node].province_id_set)
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
        return p_owners.front();
    }
    else if (!allEqual(p_owners) && same_economic_market)
    {
        return "CONTESTED_FRIENDLY";
    }
    else if (!allEqual(p_owners) && !enemy_occupation)
    {
        return "CONTESTED_NEUTRAL";
    }
    else if (!allEqual(p_owners) && enemy_occupation)
    {
        return "CONTESTED_HOSTILE";
    }
    return "UNK";
}

GlobalGraph GlobalTradeGraph::return_country_graph(GlobalGraph &g, std::string owner_TAG)
{
    /* owner_TAG T should not be able to access RegionNodes whose owner:
        1. is at war with T
        2. is isolationist
        3. is uncivilized
        4. is another nations colony
        5. has a relationship below a certain threshold
        6. is CONTESTED_HOSTILE
    */
    GlobalGraph pruned_g = g;
    for (int n = 0; n < this->total_region_nodes; n++)
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
