#ifndef C_QUEUE_FRONTIER_HPP
#define C_QUEUE_FRONTIER_HPP

#include <queue>

#include "IFrontier.hpp"

class CQueueFrontier : public IFrontier {
public:
    void push(NodeID node, int /*priority*/) override
    {
        m_queue.push(node);
    }
    NodeID pop() override
    {
        NodeID n = m_queue.front();
        m_queue.pop();
        return n;
    }

    bool empty() const override
    {
        return m_queue.empty();
    }
    void clear() override
    {
        while (!m_queue.empty()) m_queue.pop();
    }
private:
    std::queue<NodeID> m_queue;
};

#endif
