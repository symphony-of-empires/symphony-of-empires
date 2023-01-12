#include <iostream>
#include <string>

#include "global_trade_graph.hpp"

int main()
{
    std::cout << "[starting simulation]" << std::endl;

    GlobalTradeGraph graph = GlobalTradeGraph();
    graph.init_graph("empty path");

    graph.summarize_graph(true);

    std::cout << "[finished simulation]" << std::endl;
    return 0;
}

// when compiling remember inlude path:
// g++ -c test_simulation.cpp global_trade_graph.cpp -I"/boost_1_81_0"
// g++ test_simulation.o global_trade_graph.o -o sim_run.exe