#include <iostream>
#include <limits>
#include <queue>
#include <thread>

#include "boost/graph/dijkstra_shortest_paths.hpp"

#include "cost_matrix.hpp"

#define MAX_THREAD 2

void update_cm_dijkstra(std::vector<int> &t, GlobalTradeGraph &copy_g, std::vector<std::vector<double> > &cm)
{
    for (int root_region : t)
    {
        if (root_region <= copy_g.total_nodes)
        {
            std::string owner_TAG = copy_g.get_region_owner_TAG(root_region);
            GlobalGraph g_prime = copy_g.return_country_graph(copy_g.global_graph, owner_TAG);
            std::vector<Node_Descriptor> predecessor_map(boost::num_vertices(g_prime));
            std::vector<double> cost_from_root(boost::num_vertices(g_prime));
            Node_Descriptor root = boost::vertex(root_region, g_prime);
            dijkstra_shortest_paths(g_prime, root, boost::weight_map(get(&ConnectionEdge::cost, g_prime)).distance_map(boost::make_iterator_property_map(cost_from_root.begin(), get(boost::vertex_index, g_prime))));
            cm[root_region] = cost_from_root;
        }
    }
}

MatrixContainer::MatrixContainer(int node_size)
{
    std::vector<std::vector<double> > cm;
    for (int i = 0; i < node_size; i++) // there is almost certainly a better way to do this
    {
        std::vector<double> tmp;
        for (int j = 0; j < node_size; j++)
        {
            tmp.push_back(std::numeric_limits<double>::infinity());
        }
        cm.push_back(tmp);
    }
    this->cost_matrix = cm;
}

void MatrixContainer::update_all_paths(std::vector<std::vector<double> > &cost_matrix, GlobalTradeGraph &g, int option, bool parallel)
{
    int nodes = (int)g.total_nodes + 1;
    std::priority_queue<int> pq;
    for (int i = 0; i < nodes; i++)
    {
        if (g.global_graph[i].region_type > 0)
        {
            pq.push(i);
        }
    }
    if (option == 0)
    {
        if (parallel)
        {
            // not super important
        }
        else
        {
            while (!pq.empty())
            {
                int current_node = pq.top();
                std::vector<int> current_node_v{current_node};
                update_matrix_dijkstra(cost_matrix, g, current_node_v);
                pq.pop();
            }
        }
    }
}

void MatrixContainer::update_subset_paths(std::vector<std::vector<double> > &cost_matrix, GlobalTradeGraph &g, std::priority_queue<int> pq, int option, bool parallel)
{
    if (option == 0)
    {
        if (parallel)
        {
            int max_thread_quota = pq.size() / MAX_THREAD + (pq.size() % MAX_THREAD != 0);
            std::vector<std::thread> running_threads;
            running_threads.reserve(MAX_THREAD);
            std::vector<int> nodes_assign_thread;
            for (int t = 0; t < MAX_THREAD; ++t)
            {
                int todo_in_thread = max_thread_quota;
                while (!pq.empty() && (todo_in_thread > 0))
                {
                    int current_node = pq.top();
                    nodes_assign_thread.push_back(current_node);
                    pq.pop();
                    todo_in_thread--;
                }
                std::thread tmp_thread{update_cm_dijkstra, std::ref(nodes_assign_thread), std::ref(g), std::ref(this->cost_matrix)};
                tmp_thread.join();
                nodes_assign_thread.clear();
            }
        }
        else
        {
            while (!pq.empty())
            {
                int current_node = pq.top();
                std::vector<int> current_node_v{current_node};
                update_matrix_dijkstra(cost_matrix, g, current_node_v);
                pq.pop();
            }
        }
    }
}

void MatrixContainer::update_matrix_dijkstra(std::vector<std::vector<double> > &cost_matrix, GlobalTradeGraph &gt, std::vector<int> region_ids)
{
    /* THIS IS THE OLD WAY OF CALCULATING PATHS */
    for (int region_id : region_ids)
    {
        std::string owner_TAG = gt.get_region_owner_TAG(region_id);
        //std::cout << "[running dijkstra]" << std::endl;
        GlobalGraph g = gt.return_country_graph(gt.global_graph, owner_TAG);

        std::vector<Node_Descriptor> predecessor_map(boost::num_vertices(g));
        std::vector<double> cost_from_root(boost::num_vertices(g));
        Node_Descriptor root = boost::vertex(region_id, g);

        dijkstra_shortest_paths(g, root, boost::weight_map(get(&ConnectionEdge::cost, g)).distance_map(boost::make_iterator_property_map(cost_from_root.begin(), get(boost::vertex_index, g))));

        cost_matrix[region_id] = cost_from_root;
    }
}

void MatrixContainer::summarize_matrix(bool verbose)
{
    std::cout << "dimensions of cost_matrix: (" << this->cost_matrix.size() << ", " << this->cost_matrix[0].size() << ")" << std::endl;
    if (verbose && (this->cost_matrix.size() < 10))
    {
        for (int i = 0; i < this->cost_matrix.size(); i++)
        {
            std::vector<double> tmp = this->cost_matrix[i];
            for (int j = 0; j < tmp.size(); j++)
            {
                std::cout << tmp[j] << " ";
            }
            std::cout << std::endl;
        }
    }
}