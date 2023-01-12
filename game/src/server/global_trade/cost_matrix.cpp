#include <iostream>
#include <limits>
#include <queue>

#include "boost/graph/dijkstra_shortest_paths.hpp"

#include "cost_matrix.hpp"

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
    std::cout << "[ updateding all paths ";
    int nodes = (int)g.total_nodes;
    std::priority_queue<int> pq; // in the future this will be a queue of node descriptors
    for (int i = 0; i < nodes; i++)
    {
        pq.push(i);
    }
    if (option == 0)
    {
        std::cout << "using dijkstra "; //dijkstra
        if (parallel)
        {
            std::cout << "in parallel... ]" << std::endl;
            // not implemented
        }
        else
        {
            std::cout << "not in parallel... ]" << std::endl;
            while (!pq.empty())
            {
                int current_node = pq.top();
                update_matrix_dijkstra(cost_matrix, g.global_graph, current_node, g.get_region_owner_TAG(current_node));
                pq.pop();
            }
        }
    }
}

void MatrixContainer::update_matrix_dijkstra(std::vector<std::vector<double> > &cost_matrix, GlobalGraph g, int region_id, std::string owner_TAG)
{
    std::cout << "[running dijkstra]" << std::endl;
    // GlobalGraph = return_country_graph(g, owner_TAG) In the future manipulate a copy of the graph based on region_owner_TAG

    std::vector<Node_Descriptor> predecessor_map(boost::num_vertices(g));
    std::vector<double> cost_from_root(boost::num_vertices(g));
    Node_Descriptor root = boost::vertex(region_id, g);

    dijkstra_shortest_paths(g, root, boost::weight_map(get(&ConnectionEdge::cost, g)).distance_map(boost::make_iterator_property_map(cost_from_root.begin(), get(boost::vertex_index, g))));

    std::cout << "costs for region " << region_id << ": ";
    for (auto cit = cost_from_root.begin(); cit != cost_from_root.end(); cit++)
    {
        std::cout << *cit << " ";
    }
    std::cout << std::endl;

    cost_matrix[region_id] = cost_from_root;
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