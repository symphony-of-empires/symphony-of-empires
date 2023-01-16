#include <algorithm>
#include <cmath>
#include <iostream>
#include <limits>
#include <list>
#include <math.h>
#include <set>
#include <thread>
#include <unordered_map>

#include "boost/bind/bind.hpp"
#include "boost/graph/astar_search.hpp"
#include "boost/graph/dijkstra_shortest_paths.hpp"

#include "cost_matrix.hpp"

#define MAX_THREADS 4

/* dijkstra's algorithm */
void update_all_dijkstra(GlobalTradeGraph &g_copy, std::vector<std::vector<double> > &cost_matrix, std::vector<int> &region_id_subset)
{
    for (auto root_region : region_id_subset)
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::string owner_TAG = g_copy.get_region_owner_TAG(root_region);
        GlobalGraph g_prime = g_copy.return_country_graph(g_copy.global_graph, owner_TAG);
        std::vector<Node_Descriptor> predecessor_map(boost::num_vertices(g_prime));
        std::vector<double> cost_from_root(boost::num_vertices(g_prime));
        Node_Descriptor root = boost::vertex(root_region, g_prime);
        dijkstra_shortest_paths(g_prime, root, boost::weight_map(get(&ConnectionEdge::cost, g_prime)).distance_map(boost::make_iterator_property_map(cost_from_root.begin(), get(boost::vertex_index, g_prime))));
        cost_matrix[root_region] = cost_from_root;
        auto stop = std::chrono::high_resolution_clock::now();
        auto ms_t = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
        if (ms_t > 200) // unacceptable time
        {
            std::cout << "stop simulation: " << root_region << std::endl;
        }
    }
}

/* a_star algorithm & utiliy functions */
template <class Graph, class CostType, class LocMap>
class distance_heuristic : public boost::astar_heuristic<Graph, CostType>
{
public:
    typedef typename boost::graph_traits<Graph>::vertex_descriptor Vertex;
    distance_heuristic(LocMap l, Vertex goal)
        : m_location(l), m_goal(goal) {}
    CostType operator()(Vertex u)
    {
        CostType dx = m_location[m_goal].x - m_location[u].x;
        CostType dy = m_location[m_goal].y - m_location[u].y;
        return ::sqrt(dx * dx + dy * dy);
    }

private:
    LocMap m_location;
    Vertex m_goal;
};

struct found_goal
{
};

template <class Vertex>
class astar_goal_visitor : public boost::default_astar_visitor
{
public:
    astar_goal_visitor(Vertex goal) : m_goal(goal) {}
    template <class Graph>
    void examine_vertex(Vertex u, Graph &g)
    {
        if (u == m_goal)
            throw found_goal();
    }

private:
    Vertex m_goal;
};

double get_astar_cost(GlobalGraph &g, Node_Descriptor &start, Node_Descriptor &goal, location locations[])
{
    std::vector<GlobalGraph::vertex_descriptor> p(boost::num_vertices(g));
    std::vector<double> d(boost::num_vertices(g));
    try
    {
        astar_search_tree(g, start,
                          distance_heuristic<GlobalGraph, double, location *>(locations, goal),
                          predecessor_map(make_iterator_property_map(p.begin(), get(boost::vertex_index, g))).weight_map(get(&ConnectionEdge::cost, g)).distance_map(make_iterator_property_map(d.begin(), get(boost::vertex_index, g))).visitor(astar_goal_visitor<Node_Descriptor>(goal)));
    }
    catch (found_goal fg)
    {
        std::list<Node_Descriptor> shortest_path;
        for (Node_Descriptor v = goal;; v = p[v])
        {
            shortest_path.push_front(v);
            if (p[v] == v)
                break;
        }
        return d[goal];
    }
    return std::numeric_limits<double>::infinity();
}

/* pathfinding algorithm to hub nodes & utility functions */
template <typename T>
void merge_vecs(std::vector<T> &vec1, const std::vector<T> &vec2)
{
    std::transform(vec1.begin(), vec1.end(), vec2.begin(), vec1.begin(),
                   [](T a, T b)
                   { return std::min<T>(a, b); });
}

template <typename T>
std::vector<T> add_to_vec(T a, std::vector<T> vec1)
{
    std::vector<T> new_vec(vec1.size());
    std::transform(vec1.begin(), vec1.end(), new_vec.begin(),
                   boost::bind(std::plus<T>(), boost::placeholders::_1, a));
    return new_vec;
}

void update_path_approximate_astar(GlobalTradeGraph &gt, int region_id, std::unordered_map<int, std::set<int> > &hubs, std::vector<std::vector<double> > &cost_matrix)
{
    std::vector<double> current_vec = cost_matrix[region_id];
    std::set<int> regional_hubs = hubs[region_id];
    std::vector<std::vector<double> > tmp_costs;
    tmp_costs.reserve(regional_hubs.size());
    for (int h : regional_hubs)
    {
        current_vec = cost_matrix[region_id];
        double astar_cost_to_hub = std::numeric_limits<double>::infinity();
        Node_Descriptor start = region_id;
        Node_Descriptor goal = h;
        astar_cost_to_hub = get_astar_cost(gt.global_graph, start, goal, gt.locations.data());
        if (!std::isinf(astar_cost_to_hub))
        {
            std::vector<double> region_cost_added_astar = add_to_vec(astar_cost_to_hub, cost_matrix[h]);
            merge_vecs(current_vec, region_cost_added_astar);
        }
    }
}

/* matrix container */
MatrixContainer::MatrixContainer(int n)
{
    this->cost_matrix = std::vector<std::vector<double> >(n, std::vector<double>(n, std::numeric_limits<double>::infinity()));
}

void MatrixContainer::update_non_hub_regions(GlobalTradeGraph &gt, std::vector<std::vector<double> > &cost_matrix, std::vector<int> region_ids, bool parallel)
{
    if (parallel)
    {
        // not implemented
    }
    else
    {
        for (int current_region : region_ids)
        {
            update_path_approximate_astar(gt, current_region, gt.region_to_service_hubs, this->cost_matrix);
        }
    }
}

void MatrixContainer::update_all_regions(GlobalTradeGraph &gt, std::vector<std::vector<double> > &cost_matrix, std::vector<int> region_id_set, bool parallel)
{
    if (parallel)
    {
        // not implemented
    }
    else
    {
        update_all_dijkstra(gt, this->cost_matrix, region_id_set);
    }
}

void MatrixContainer::summarize_matrix(bool verbose)
{
    std::cout << "===| cost matrix |==================================" << std::endl;
    std::cout << "\tdimensions: "
              << "(" << this->cost_matrix.size() << ", " << this->cost_matrix[0].size() << ")" << std::endl;
    int running_avg = 0;
    int infinity_count = 0;
    int num_vals = 0;
    for (auto cv : this->cost_matrix)
    {
        for (auto c : cv)
        {
            if (c == std::numeric_limits<double>::infinity())
            {
                infinity_count++;
            }
            else
            {
                running_avg += c;
                num_vals++;
            }
        }
    }
    std::cout << "\tavg cost: " << running_avg / (double)num_vals << std::endl;
    std::cout << "\tinf count: " << infinity_count << std::endl;
    std::cout << "====================================================" << std::endl;
}