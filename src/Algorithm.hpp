#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <vector>
#include <unordered_map>
#include <functional>

#include "ISearchable.hpp"
#include "IFrontier.hpp"

using onVisit = std::function<void(NodeID)>;

std::unordered_map<NodeID, NodeID> genericSearch(const ISearchable &graph,
                                             NodeID start, 
                                             NodeID end, 
                                             IFrontier &frontier, 
                                             onVisit callback = nullptr);
std::vector<NodeID> reconstructPath(const std::unordered_map<NodeID, NodeID> &parents, NodeID start, NodeID end);

#endif