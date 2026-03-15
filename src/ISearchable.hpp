#ifndef I_SEARCHABLE_HPP
#define I_SEARCHABLE_HPP

#include <vector>
#include <functional>

#include "raylib.h"

using NodeID = int;
using ColorHook = std::function<Color(NodeID)>;

struct TEdgeState
{
    float progress;
    Color color;
};

using EdgeHook = std::function<TEdgeState(NodeID u, NodeID v)>;

class ISearchable
{
    public:
        virtual ~ISearchable() = default;
        virtual std::vector<NodeID> getNeighbors(NodeID node) const = 0;
        virtual int getHeuristic(NodeID current, NodeID target) const = 0;
        virtual bool isTarget(NodeID current, NodeID target) const { return current == target; }
        virtual void draw(float x, float y, float width, float height, ColorHook getHook, EdgeHook getEdge) const = 0;

        virtual NodeID getStart() const = 0;
        virtual NodeID getEnd() const = 0;
};

#endif