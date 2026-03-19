#ifndef CGRAPH_HPP
#define CGRAPH_HPP

#include <string>
#include <vector>
#include <unordered_map>

#include "ISearchable.hpp"
#include "raylib.h"

class CGraph : public ISearchable
{
    public:
        CGraph(const std::string &fileName);

        std::vector<NodeID> getNeighbors(NodeID node) const override;
        int getHeuristic(NodeID current, NodeID target) const override;

        void draw(float x, float y, float width, float height, ColorHook getHook, EdgeHook getEdge) const override;
        void update(float deltaTime) override;

        NodeID getStart() const override;
        NodeID getEnd() const override;
    private:
        std::unordered_map<NodeID, std::vector<NodeID>> m_adjList;

        std::vector<Vector2> m_positions;
        std::vector<Vector2> m_velocities;
        std::vector<NodeID> m_nodeIDs;
        std::unordered_map<NodeID, int> m_idToIndex;

        struct TEdgeIdx { int u, v; };
        std::vector<TEdgeIdx> m_edges;

        NodeID m_start;
        NodeID m_end;

        const float RepulsionStrength = 10.0f;
        const float SpringStrength = 15.0f;
        const float DesiredLength = 0.5f;
        const float GravityStrength = 1.5f;
};

#endif