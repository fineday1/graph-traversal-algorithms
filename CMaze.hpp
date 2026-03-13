#ifndef MAZE_HPP
#define MAZE_HPP

#include <string>
#include <vector>

#include "ISearchable.hpp"
#include "TPoint.hpp"

class CMaze : public ISearchable
{
    public:
        CMaze(const std::string &fileName);

        std::vector<NodeID> getNeighbors(NodeID node) const override;
        int getHeuristic(NodeID current, NodeID target) const override;
        
        NodeID pointToID(const TPoint &point) const;
        TPoint IDToPoint(NodeID node) const;

        const std::vector<std::string> &getGrid() const;
        NodeID getStart() const;
        NodeID getEnd() const;
    private:
        std::vector<std::string> m_grid;
        int m_width;
        int m_height;
        NodeID m_start;
        NodeID m_end;

        bool isValidMove(int x, int y) const;
};

#endif