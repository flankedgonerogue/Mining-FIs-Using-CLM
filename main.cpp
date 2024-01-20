#include <iostream>

#include "include/Graph.hpp"

int main(int argc, char **argv)
{
    Graph graph(5);
    graph.processTransaction("BCDE");
    graph.processTransaction("ABC");
    std::cout << graph.toString();

    return 0;
}
