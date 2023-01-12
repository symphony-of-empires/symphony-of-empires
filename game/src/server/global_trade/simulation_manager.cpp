#include <iostream>
#include <string>

#include "global_trade_graph.hpp"
#include "cost_matrix.hpp"

// this file will eventually become the simulation manager
int main()
{
    std::cout << "[starting simulation]" << std::endl;

    std::cout << "[game setup]" << std::endl;
    GlobalTradeGraph graph = GlobalTradeGraph();
    int nodes = graph.init_graph("dummy path");
    std::cout << "\ttotal nodes: " << nodes << std::endl;
    graph.summarize_graph(true);

    MatrixContainer cost_matrix = MatrixContainer(nodes);
    cost_matrix.summarize_matrix(true);
    cost_matrix.update_all_paths(cost_matrix.cost_matrix, graph, 0, false);

    std::cout << "[game start]" << std::endl;
    // dummy simulation
    for (int tick = 0; tick < 10; tick++)
    {
        std::cout << "do tick" << std::endl;
        // calculate paths and update cost matrix
        // update edges
    };
    std::cout << "[game end]" << std::endl;
    cost_matrix.summarize_matrix(true);

    std::cout << "[finished simulation]" << std::endl;
    return 0;
}

// when compiling remember inlude path for BOOST:
// g++ -std=c++11 -c simulation_manager.cpp global_trade_graph.cpp cost_matrix.cpp -I"<PATH>/boost_1_81_0"
// g++ simulation_manager.o global_trade_graph.o cost_matrix.o -o sim_run.exe
// ./sim_run.exe