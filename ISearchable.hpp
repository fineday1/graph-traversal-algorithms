#ifndef I_SEARCHABLE_HPP
#define I_SEARCHABLE_HPP

#include <vector>

using NodeID = int;

class ISearchable
{
    public:
        virtual ~ISearchable() = default;
        virtual std::vector<NodeID> getNeighbors(NodeID node) const = 0;
        virtual int getHeuristic(NodeID current, NodeID target) const = 0;
        virtual bool isTarget(NodeID current, NodeID target) const { return current == target; }
};

#endif