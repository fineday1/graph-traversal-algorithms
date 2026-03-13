#ifndef ALGORITHM_HPP
#define ALGORITHM_HPP

#include <vector>
#include <unordered_map>

#include "ISearchable.hpp"
#include "IFrontier.hpp"

void genericSearch(const ISearchable &graph, NodeID start, NodeID end, IFrontier &frontier);
std::vector<NodeID> reconstructPath(const unordered_map<NodeID, NodeID> &parents, NodeID start, NodeID end);

#endif