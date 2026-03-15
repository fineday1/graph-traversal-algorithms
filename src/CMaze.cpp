#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <cstdio>
#include <map>
#include <set>
#include <cmath>
#include <algorithm>

#include "CMaze.hpp"
#include "TPoint.hpp"

CMaze::CMaze(const std::string &fileName)
    : m_width(0), m_height(0), m_start(0), m_end(0)
{
    std::ifstream file(fileName);
    if(!file.is_open())
    {
        std::cerr << "Error: Could not open file " << fileName << std::endl;
        return;
    }

    std::string line;
    TPoint start = {};
    TPoint end = {};

    while(std::getline(file, line))
    {
        if(line.empty()) continue;
        if(line.find("start") != std::string::npos)
        {
            std::sscanf(line.c_str(), "start %d, %d", &start.x, &start.y);
        }
        else if(line.find("end") != std::string::npos)
        {
            std::sscanf(line.c_str(), "end %d, %d", &end.x, &end.y);
        } 
        else
        {
            m_grid.push_back(line);
            if(m_width == 0) m_width = (int)line.length();
            m_height++;
        }
    }

    m_start = pointToID(start);
    m_end = pointToID(end);
}

std::vector<NodeID> CMaze::getNeighbors(NodeID node) const
{
    std::vector<NodeID> neighbors;
    const std::vector<TPoint> moves = { {1, 0}, {0, 1}, {-1, 0}, {0, -1} };
    TPoint p = IDToPoint(node);

    for(const auto &move : moves)
    {
        int nx = p.x + move.x;
        int ny = p.y + move.y;

        if(isValidMove(nx, ny))
        {
            neighbors.push_back(pointToID({nx, ny}));
        }
    }

    return neighbors;
}

int CMaze::getHeuristic(NodeID current, NodeID target) const
{
    TPoint p1 = IDToPoint(current);
    TPoint p2 = IDToPoint(target);
    return std::abs(p1.x - p2.x) + std::abs(p1.y - p2.y);
}

void CMaze::draw(float x, float y, float width, float height, ColorHook getHook) const
{
    float cellW = width / (float)m_width;
    float cellH = height / (float)m_height;

    for(int iy = 0; iy < m_height; ++iy)
    {
        for(int ix = 0; ix < m_width; ++ix)
        {
            NodeID id = pointToID({ix, iy});

            Color color;
            if(m_grid[iy][ix] == 'X')
            {
                color = BLACK;
            }
            else
            {
                color = getHook(id);
            }

            Rectangle rect = { x + (ix * cellW), y + (iy * cellH), cellW, cellH };
            DrawRectangleRec(rect, color);
            DrawRectangleLinesEx(rect, 1, GRAY);
        }
    }
}

NodeID CMaze::pointToID(const TPoint &point) const
{
    return point.x + (point.y * m_width);
}

TPoint CMaze::IDToPoint(NodeID node) const
{
    return { (node % m_width), (node / m_width) };
}

const std::vector<std::string> &CMaze::getGrid() const { return m_grid; }
NodeID CMaze::getStart() const { return m_start; }
NodeID CMaze::getEnd() const { return m_end; }

bool CMaze::isValidMove(int x, int y) const
{
    if(x < 0 || x >= m_width || y < 0 || y >= m_height) return false;
    return m_grid[y][x] != 'X';
}