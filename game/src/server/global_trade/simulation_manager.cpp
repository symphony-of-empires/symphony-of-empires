#include <algorithm>
#include <chrono>
#include <iostream>
#include <numeric>
#include <queue>
#include <set>
#include <string>

#include "global_trade_graph.hpp"
#include "cost_matrix.hpp"

#define WEEK 7
#define TWO_WEEKS 14
#define EIGHT_WEEKS 56

std::vector<int> get_first_n_jobs(std::vector<int> &v, int start_index, int num)
{
    return std::vector<int>(v.begin() + start_index, v.begin() + std::min(start_index + num, static_cast<int>(v.size())));
}

std::vector<int> remove_jobs(std::vector<int> &s_jobs, std::vector<int> &to_remove)
{
    std::unordered_set<int> to_remove_set(to_remove.begin(), to_remove.end());
    std::vector<int> schedulable_jobs;
    for (size_t i = 0; i < s_jobs.size(); i++)
    {
        if (to_remove_set.find(s_jobs[i]) == to_remove_set.end())
        {
            schedulable_jobs.push_back(s_jobs[i]);
        }
    }
    return schedulable_jobs;
}

int main()
{
    std::cout << "[game setup]" << std::endl;
    /* before game start */
    auto start_setup = std::chrono::high_resolution_clock::now();
    GlobalTradeGraph graph = GlobalTradeGraph();
    int nodes = 1000;
    int maximum_node_index = graph.init_random_graph(nodes);
    graph.summarize_graph(true);
    MatrixContainer matrix = MatrixContainer(nodes);
    std::cout << "[INFO] init all costs at game start" << std::endl;
    matrix.summarize_matrix(false);
    matrix.update_all_regions(graph, matrix.cost_matrix, graph.currently_used_nodes, false);
    matrix.summarize_matrix(false);
    auto stop_setup = std::chrono::high_resolution_clock::now();
    auto setup_t = std::chrono::duration_cast<std::chrono::milliseconds>(stop_setup - start_setup).count();
    std::cout << "[INFO] setup took (" << setup_t / 1000.0 << "s)" << std::endl;

    std::cout << "[game start]" << std::endl;
    /* update game start */
    /* SIMULATION ALGORITHM */
    auto sim_start = std::chrono::high_resolution_clock::now();
    auto num_ticks = 365;
    auto all_ticks = 0;
    for (int tick = 0; tick < num_ticks; tick++) // one year simulation
    {
        std::cout << "[tick no." << tick << "]";
        auto tick_start = std::chrono::high_resolution_clock::now();

        /* path calculations */
        std::vector<int> daily_hub_jobs;
        std::vector<int> daily_astar_jobs;
        std::vector<int> daily_node_jobs;
        std::vector<int> schedulable_jobs = remove_jobs(graph.currently_used_nodes, graph.hub_nodes);
        int hub_jobs_per_day = (graph.hub_nodes.size() + WEEK - 1) / WEEK;
        int hubs_jobs_index = hub_jobs_per_day * (tick % WEEK);
        if (hubs_jobs_index < graph.hub_nodes.size()) // update optimal hub costs every week
        {
            int hub_jobs_per_day = (graph.hub_nodes.size() + WEEK - 1) / WEEK;
            daily_hub_jobs = get_first_n_jobs(graph.hub_nodes,
                                              hubs_jobs_index,
                                              hub_jobs_per_day);
            matrix.update_all_regions(graph, matrix.cost_matrix, daily_hub_jobs, false);
        }
        int astar_jobs_per_day = (schedulable_jobs.size() + TWO_WEEKS - 1) / TWO_WEEKS;
        int astar_jobs_index = astar_jobs_per_day * (tick % TWO_WEEKS);
        if (astar_jobs_index < schedulable_jobs.size()) // update node with astar over a short period to ensure 'good' paths
        {
            daily_astar_jobs = get_first_n_jobs(schedulable_jobs,
                                                astar_jobs_index,
                                                astar_jobs_per_day);
            matrix.update_non_hub_regions(graph, matrix.cost_matrix, daily_astar_jobs, false);
        }
        int node_jobs_per_day = (schedulable_jobs.size() + EIGHT_WEEKS - 1) / EIGHT_WEEKS;
        int node_jobs_index = node_jobs_per_day * (tick % EIGHT_WEEKS);
        if (node_jobs_index < schedulable_jobs.size()) // update node with dijkstra over a longer period to ensure optimal paths
        {
            daily_node_jobs = get_first_n_jobs(schedulable_jobs,
                                               node_jobs_index,
                                               node_jobs_per_day);
            matrix.update_all_regions(graph, matrix.cost_matrix, daily_node_jobs, false);
        }

        std::cout << std::endl;

        auto tick_stop = std::chrono::high_resolution_clock::now();
        auto ms_tick = std::chrono::duration_cast<std::chrono::milliseconds>(tick_stop - tick_start).count();
        all_ticks += ms_tick;
    }
    auto sim_stop = std::chrono::high_resolution_clock::now();
    auto ms_sim = std::chrono::duration_cast<std::chrono::milliseconds>(sim_stop - sim_start).count();
    std::cout << "[INFO] simulation took (" << ms_sim / 1000.0 << "s)" << std::endl;
    std::cout << "[INFO] average tick took (" << (all_ticks / num_ticks) / 1000.0 << "s)" << std::endl;

    std::cout << "[game end]" << std::endl;
    matrix.summarize_matrix(false);

    std::cout << "[finished simulation]" << std::endl;
    return 0;
};

// when compiling remember inlude path for BOOST:
// g++ -std=c++14 -c simulation_manager.cpp global_trade_graph.cpp cost_matrix.cpp -I"/User/boost_1_81_0"
// g++ simulation_manager.o global_trade_graph.o cost_matrix.o -o sim_run.exe
// ./sim_run.exe