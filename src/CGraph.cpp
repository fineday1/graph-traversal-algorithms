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
    NodeID start = 0, end = 0;
    while(std::getline(file, line))
    {
        if(line.empty() || line[0] == '#') continue;
        else if(line.find("start") != std::string::npos)
        {
            std::sscanf(line.c_str(), "start %d", &start);
        }
        else if(line.find("end") != std::string::npos)
        {
            std::sscanf(line.c_str(), "end %d", &end);
        }

        size_t colonPos = line.find(':');
        if(colonPos == std::string::npos) continue;

        int source = std::stoi(line.substr(0, colonPos));
        std::stringstream ss(line.substr(colonPos + 1));
        int neighbor;
        while(ss >> neighbor)
        {
            m_adjList[source].push_back(neighbor);
            allNodes.insert(source);
            allNodes.insert(neighbor);
        }
    }

    m_start = start;
    m_end = end;

    int i = 0;
    int n = allNodes.size();
    for(int node : allNodes)
    {
        float angle = (float)i * (2.0f * PI / (float)n);
        m_nodePositions[node] = { cosf(angle), sinf(angle) };
        i++;
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
    if (m_nodePositions.count(current) && m_nodePositions.count(target))
    {
        Vector2 p1 = m_nodePositions.at(current);
        Vector2 p2 = m_nodePositions.at(target);
        return (int)(Vector2Distance(p1, p2) * 100.0f);
    }
    return 0;
}

void CGraph::draw(float x, float y, float width, float height, ColorHook getHook, EdgeHook getEdge) const
{
    Vector2 center = { x + width / 2.0f, y + height / 2.0f };
    float scale = (width < height ? width : height) * 0.4f;

    auto toScreen = [center, scale](Vector2 norm) -> Vector2
    {
        return { center.x + norm.x * scale, center.y + norm.y * scale };
    };

    // Draw lines
    for(const auto &[source, neighbors] : m_adjList)
    {
        if (m_nodePositions.count(source))
        {
            Vector2 p1 = toScreen(m_nodePositions.at(source));
            for(NodeID target : neighbors)
            {
                if (m_nodePositions.count(target))
                {
                    Vector2 p2 = toScreen(m_nodePositions.at(target));

                    // Draw base line
                    DrawLineEx(p1, p2, 1.0f, BLACK);

                    TEdgeState state = getEdge(source, target);
                    if(state.progress > 0.0f) 
                    {
                        Vector2 pAnimated = Vector2Lerp(p1, p2, state.progress);
                        DrawLineEx(p1, pAnimated, 3.0f, state.color);
                    }
                }
            }
        }
    }

    // Draw nodes
    for(const auto &[id, normPos] : m_nodePositions)
    {
        Vector2 pos = toScreen(normPos);

        Color nodeColor = getHook(id);

        DrawCircleV(pos, 15.0f, nodeColor);
        DrawCircleLinesV(pos, 15.0f, BLACK);

        std::string label = std::to_string(id);
        int textW = MeasureText(label.c_str(), 10);
        DrawText(label.c_str(), (int)pos.x - textW / 2, (int)pos.y - 5, 10, DARKGRAY);
    }
}

NodeID CGraph::getStart() const { return m_start; }
NodeID CGraph::getEnd() const { return m_end; }