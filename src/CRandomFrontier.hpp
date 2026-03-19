#ifndef C_RANDOM_FRONTIER_HPP
#define C_RANDOM_FRONTIER_HPP

#include <vector>
#include <random>
#include <algorithm>

#include "IFrontier.hpp"

class CRandomFrontier : public IFrontier {
public:
    void push(NodeID node, int /*priority*/) override
    {
        m_nodes.push_back(node);
    }
    NodeID pop() override
    {
        if (m_nodes.empty()) return -1;

        static std::mt19937 gen(std::random_device{}());

        std::uniform_int_distribution<size_t> dist(0, m_nodes.size() - 1);
        size_t randomIdx = dist(gen);

        NodeID n = m_nodes[randomIdx];
        std::swap(m_nodes[randomIdx], m_nodes.back());
        m_nodes.pop_back();

        return n;
    }

    bool empty() const override
    {
        return m_nodes.empty();
    }
    void clear() override
    {
        m_nodes.clear();
    }
private:
    std::vector<NodeID> m_nodes;
};

#endif