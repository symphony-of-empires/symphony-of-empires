#include <iostream>
#include <string>
#include <chrono>

#include "global_trade_graph.hpp"
#include "cost_matrix.hpp"

// this file will eventually become the simulation manager
void old_sim()
{
    // std::cout << "[starting simulation]" << std::endl;

    // std::cout << "[game setup]" << std::endl;
    // GlobalTradeGraph graph = GlobalTradeGraph();
    // int nodes = graph.init_random_graph("dummy path");
    // std::cout << "\ttotal nodes: " << nodes << std::endl;
    // graph.summarize_graph(true);

    // MatrixContainer cost_matrix = MatrixContainer(nodes);
    // cost_matrix.summarize_matrix(true);
    // //cost_matrix.update_all_paths(cost_matrix.cost_matrix, graph, 0, false);
    // //cost_matrix.summarize_matrix(true);

    // std::cout << "[game start]" << std::endl;
    // // right now this is only a dummy simulation

    // /* MANAGER
    // every tick the manager should
    //     1. check for edge updates
    //     2. check for node updates
    // every X ticks (sticky-ness of economics) the manager should:
    //     1. recalculate costs for all paths & update matrix
    // */
    // for (int tick = 0; tick < 10; tick++) // a tick shouldn't be every day
    // {
    //     std::cout << "do tick" << std::endl;
    //     if (tick == 2)
    //     { // at tick 2 a railroad lv1 is built betwen CAN and USA
    //         graph.update_edge(std::make_pair(1, 2), 9);
    //     }
    //     if (tick == 5)
    //     { // at tick 5 calculate paths, path calcs and edge updates also really don't have to happen at the same time
    //         cost_matrix.update_all_paths(cost_matrix.cost_matrix, graph, 0, false);
    //         cost_matrix.summarize_matrix(true);
    //     }
    // };
    // std::cout << "[game end]" << std::endl;

    // std::cout << "[finished simulation]" << std::endl;
}

int main()
{
    std::cout << "[starting simulation]" << std::endl;

    std::cout << "[game setup]" << std::endl;
    GlobalTradeGraph graph = GlobalTradeGraph();
    int nodes = graph.init_random_graph(500, 5);
    std::cout << "\ttotal nodes: " << nodes << std::endl;
    graph.summarize_graph(true);

    MatrixContainer cost_matrix = MatrixContainer(nodes);
    cost_matrix.update_all_paths(cost_matrix.cost_matrix, graph, 0, false);
    cost_matrix.summarize_matrix(true);

    std::cout << "[game start]" << std::endl;
    /* MANAGER
    every tick the manager should
        1. check for edge updates
        2. check for node updates
    every X ticks (sticky-ness of economics) the manager should:
        1. recalculate costs for all paths & update matrix
    */
    double running_avg = 0;
    int long_ticks = 0;
    auto s_start = std::chrono::high_resolution_clock::now();
    for (int tick = 0; tick < 365; tick++) // one year simulation
    {
        auto start = std::chrono::high_resolution_clock::now();
        if (tick % 7 == 0) // every week update path costs
        {
            cost_matrix.update_all_paths(cost_matrix.cost_matrix, graph, 0, false);
        }
        auto stop = std::chrono::high_resolution_clock::now();
        auto ms_t = duration_cast<std::chrono::milliseconds>(stop - start).count();
        if (ms_t > 50)
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

    std::cout << "[finished simulation]" << std::endl;
    // sanity check
    return 0;
}

// when compiling remember inlude path for BOOST:
// g++ -std=c++11 -c simulation_manager.cpp global_trade_graph.cpp cost_matrix.cpp -I"<PATH>/boost_1_81_0"
// g++ simulation_manager.o global_trade_graph.o cost_matrix.o -o sim_run.exe
// ./sim_run.exe