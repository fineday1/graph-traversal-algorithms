#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>
#include <sstream>

#include "CGraph.hpp"
#include "raymath.h"

CGraph::CGraph(const std::string &fileName)
    : m_start(0), m_end(0)
{
    std::ifstream file(fileName);
    if(!file.is_open())
    {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return;
    }

    std::string line;
    std::set<int> allNodes;
    NodeID startID = 0, endID = 0;
    
    std::vector<std::pair<NodeID, NodeID>> tempEdges;

    while(std::getline(file, line))
    {
        if(line.empty() || line[0] == '#') continue;
        else if(line.find("start") != std::string::npos)
        {
            std::sscanf(line.c_str(), "start %d", &startID);
        }
        else if(line.find("end") != std::string::npos)
        {
            std::sscanf(line.c_str(), "end %d", &endID);
        }
        else
        {
            size_t colonPos = line.find(':');
            if(colonPos == std::string::npos) continue;

            NodeID source = std::stoi(line.substr(0, colonPos));
            std::stringstream ss(line.substr(colonPos + 1));
            NodeID neighbor;
            allNodes.insert(source);
            while(ss >> neighbor)
            {
                m_adjList[source].push_back(neighbor);
                tempEdges.push_back({source, neighbor});
                allNodes.insert(neighbor);
            }
        }
    }

    m_start = startID;
    m_end = endID;

    int index = 0;
    int totalNodes = allNodes.size();
    for(NodeID id : allNodes)
    {
        m_nodeIDs.push_back(id);
        m_idToIndex[id] = index;
        
        float angle = (float)index * (2.0f * PI / (float)totalNodes);
        m_positions.push_back({ cosf(angle), sinf(angle) });
        m_velocities.push_back({ 0, 0 });
        
        index++;
    }

    for(const auto& edge : tempEdges)
    {
        if(edge.first < edge.second)
        {
            m_edges.push_back({ m_idToIndex[edge.first], m_idToIndex[edge.second] });
        }
    }
}

std::vector<NodeID> CGraph::getNeighbors(NodeID node) const
{
    if(m_adjList.count(node))
    {
        return m_adjList.at(node);
    }
    return {};
}

int CGraph::getHeuristic(NodeID current, NodeID target) const
{
    if(m_idToIndex.count(current) && m_idToIndex.count(target))
    {
        Vector2 p1 = m_positions[m_idToIndex.at(current)];
        Vector2 p2 = m_positions[m_idToIndex.at(target)];
        return (int)(Vector2Distance(p1, p2) * 100.0f);
    }
    return 0;
}

void CGraph::draw(float x, float y, float width, float height, ColorHook getHook, EdgeHook getEdge) const
{
    Vector2 center = { x + width / 2.0f, y + height / 2.0f };

    float maxDist = 0.01f;
    for(const auto& pos : m_positions)
    {
        float d = Vector2Length(pos);
        if(d > maxDist) maxDist = d;
    }

    float minDim = (width < height ? width : height);
    float scale = (minDim / 2.0f) / maxDist * 0.85f;

    auto toScreen = [center, scale](Vector2 norm) -> Vector2
    {
        return { center.x + norm.x * scale, center.y + norm.y * scale };
    };

    for(const auto& edge : m_edges)
    {
        Vector2 p1 = toScreen(m_positions[edge.u]);
        Vector2 p2 = toScreen(m_positions[edge.v]);

        DrawLineEx(p1, p2, 1.0f, BLACK);

        TEdgeState state = getEdge(m_nodeIDs[edge.u], m_nodeIDs[edge.v]);
        if(state.progress > 0.0f) 
        {
            Vector2 pAnimated = Vector2Lerp(p1, p2, state.progress);
            DrawLineEx(p1, pAnimated, 3.0f, state.color);
        }
    }

    for(size_t i = 0; i < m_positions.size(); ++i)
    {
        Vector2 pos = toScreen(m_positions[i]);
        NodeID id = m_nodeIDs[i];

        Color nodeColor = getHook(id);

        DrawCircleV(pos, 15.0f, nodeColor);
        DrawCircleLinesV(pos, 15.0f, BLACK);

        std::string label = std::to_string(id);
        int textW = MeasureText(label.c_str(), 10);
        DrawText(label.c_str(), (int)pos.x - textW / 2, (int)pos.y - 5, 10, DARKGRAY);
    }
}

void CGraph::update(float deltaTime)
{
    int n = (int)m_positions.size();

    for(int i = 0; i < n; ++i)
    {
        for(int j = i + 1; j < n; ++j)
        {
            Vector2 diff = Vector2Subtract(m_positions[i], m_positions[j]);
            float distSq = Vector2LengthSqr(diff);
            if(distSq < 0.0001f) continue;

            float forceMag = (RepulsionStrength / distSq) * deltaTime;
            Vector2 forceVec = Vector2Scale(Vector2Normalize(diff), forceMag);

            m_velocities[i] = Vector2Add(m_velocities[i], forceVec);
            m_velocities[j] = Vector2Subtract(m_velocities[j], forceVec);
        }
    }

    for(const auto& edge : m_edges)
    {
        Vector2 diff = Vector2Subtract(m_positions[edge.v], m_positions[edge.u]);
        float dist = Vector2Length(diff);
        if(dist < 0.01f) continue;

        float springF = (dist - DesiredLength) * SpringStrength;
        Vector2 forceVec = Vector2Scale(diff, (springF / dist) * deltaTime);

        m_velocities[edge.u] = Vector2Add(m_velocities[edge.u], forceVec);
        m_velocities[edge.v] = Vector2Subtract(m_velocities[edge.v], forceVec);
    }

    for(int i = 0; i < n; ++i)
    {
        m_velocities[i] = Vector2Subtract(m_velocities[i], Vector2Scale(m_positions[i], GravityStrength * deltaTime));
        m_positions[i] = Vector2Add(m_positions[i], Vector2Scale(m_velocities[i], deltaTime));
        m_velocities[i] = Vector2Scale(m_velocities[i], 0.9f);
    }
}

NodeID CGraph::getStart() const { return m_start; }
NodeID CGraph::getEnd() const { return m_end; }