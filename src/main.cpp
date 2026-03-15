#include "CVisualizerApp.hpp"
#include <iostream>

int main()
{
    CVisualizerApp app;

    if(!app.init(800, 600, "Graph Traversal Visualizer"))
    {
        std::cerr << "Failed to initialize application" << std::endl;
        return -1;
    }

    if(!app.loadGraph("maze.txt"))
    {
        std::cerr << "Failed to load initial graph" << std::endl;
    }

    app.run();

    return 0;
}