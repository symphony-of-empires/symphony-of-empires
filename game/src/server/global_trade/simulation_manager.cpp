#include <iostream>
#include <string>
#include <chrono>
#include <queue>

#include "global_trade_graph.hpp"
#include "cost_matrix.hpp"

#define ECONOMY_UPDATE_PERIOD 7

int main()
{
    std::cout << "[starting simulation]" << std::endl;

    std::cout << "[game setup]" << std::endl;
    GlobalTradeGraph graph = GlobalTradeGraph();
    int nodes = graph.init_random_graph(500, 5);
    std::cout << "\ttotal nodes: " << nodes << std::endl;
    graph.summarize_graph(true);

    MatrixContainer cost_matrix = MatrixContainer(nodes);
    //cost_matrix.update_all_paths(cost_matrix.cost_matrix, graph, 0, false); // uncomment
    cost_matrix.summarize_matrix(false);

    std::cout << "[game start]" << std::endl;
    /* MANAGER */
    double running_avg = 0;
    int long_ticks = 0;

    int daily_quota = 0;
    std::priority_queue<int> global_pq;

    auto s_start = std::chrono::high_resolution_clock::now();
    for (int tick = 0; tick < 365; tick++) // one year simulation
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::priority_queue<int> tmp_qu_daily; // reset the daily queue
        /* Every week the economy manager needs to
            1. check for new nodes
            2. assign them to a priority queue
            3. calculate how many should be updated each day
        */
        if (tick % ECONOMY_UPDATE_PERIOD == 0)
        {
            int nodes = graph.total_nodes + 1;
            global_pq = std::priority_queue<int>();
            for (int i = 0; i < nodes; i++)
            {
                if (graph.global_graph[i].region_type > 0)
                {
                    global_pq.push(i);
                }
            }
            daily_quota = global_pq.size() / ECONOMY_UPDATE_PERIOD + (global_pq.size() % ECONOMY_UPDATE_PERIOD != 0);
        }
        /* Every day the economy manager needs to
            1. look in its daily queue & update cost matrix for those nodes
        */
        int todo_today = daily_quota;
        while (!global_pq.empty() && (todo_today > 0)) // gather the daily jobs
        {
            int current_node = global_pq.top();
            tmp_qu_daily.push(current_node);
            global_pq.pop();
            todo_today--;
        }
        cost_matrix.update_subset_paths(cost_matrix.cost_matrix, graph, tmp_qu_daily, 0, true);

        auto stop = std::chrono::high_resolution_clock::now();
        auto ms_t = duration_cast<std::chrono::milliseconds>(stop - start).count();
        if (ms_t > 30)
        {
            running_avg += ms_t;
            long_ticks += 1;
        }
    };
    auto s_stop = std::chrono::high_resolution_clock::now();
    auto ms_s = duration_cast<std::chrono::milliseconds>(s_stop - s_start).count();
    std::cout << "[INFO] simulation took (" << ms_s << "ms)" << std::endl;
    std::cout << "[INFO] avg economy tick took (" << (double)running_avg / (double)long_ticks << "ms)" << std::endl;
    std::cout << "[game end]" << std::endl;

    std::cout << "example cost sanity check: " << cost_matrix.cost_matrix[265][720] << std::endl;
    std::cout << "[finished simulation]" << std::endl;
    return 0;
}

// when compiling remember inlude path for BOOST:
// g++ -std=c++14 -c simulation_manager.cpp global_trade_graph.cpp cost_matrix.cpp -I"<PATH>/boost_1_81_0"
// g++ simulation_manager.o global_trade_graph.o cost_matrix.o -o sim_run.exe
// ./sim_run.exe