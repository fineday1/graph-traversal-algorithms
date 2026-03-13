#include <vector>
#include <unordered_set>
#include <unordered_map>
#include "ISearchable.hpp"
#include "IFrontier.hpp"
#include "Algorithm.hpp"

void genericSearch(const ISearchable &graph, NodeID start, NodeID end, IFrontier &frontier)
{
    std::unordered_set<NodeID> visited;
    std::unordered_map<NodeID, NodeID> parents;

    frontier.clear();
    frontier.push(start);
    visited.insert(start);

    while(!frontier.empty())
    {
        NodeID curr = frontier.pop();
        
        if(graph.isTarget(curr, end)) break;

        std::vector<NodeID> neighbors = graph.getNeighbors(curr);
        for(const auto &n : neighbors)
        {
            if(visited.find(n) == visited.end())
            {
                visited.insert(n);
                parents[n] = curr;
                int priority = graph.getHeuristic(n, end);
                frontier.push(n, priority);
            }
        }
    }
}

std::vector<NodeID> reconstructPath(const std::unordered_map<NodeID, NodeID> &parents, NodeID start, NodeID end)
{
    std::vector<NodeID> path;
    NodeID step = end;

    while(parents.count(step)) {
        step = parents.at(step);
        if(step != start) path.push_back(step);
    }

    return path;
}