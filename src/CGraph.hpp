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
        void update(float deltaTime);

        NodeID getStart() const override;
        NodeID getEnd() const;
    private:
        std::unordered_map<NodeID, std::vector<NodeID>> m_adjList;
        std::unordered_map<NodeID, Vector2> m_nodePositions;
        std::unordered_map<NodeID, Vector2> m_velocities;
        std::vector<NodeID> m_nodeIDs;

        NodeID m_start;
        NodeID m_end;

        const float RepulsionStrength = 10.0f;
        const float SpringStrength = 5.0f;
        const float DesiredLength = 0.5f;
        const float GravityStrength = 0.75f;
};

#endif