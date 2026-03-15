#ifndef C_PRIORITY_FRONTIER_HPP
#define C_PRIORITY_FRONTIER_HPP

#include <queue>
#include <vector>
#include <functional>

#include "IFrontier.hpp"

class CPriorityFrontier : public IFrontier {
public:
    void push(NodeID node, int priority) override
    {
        m_priorityQ.push({node, priority});
    }
    NodeID pop() override
    {
        NodeID n = m_priorityQ.top().id;
        m_priorityQ.pop();
        return n;
    }

    bool empty() const override
    {
        return m_priorityQ.empty();
    }
    void clear() override
    {
        while (!m_priorityQ.empty()) m_priorityQ.pop();
    }
private:
    struct TNode
    {
        NodeID id;
        int priority;
        bool operator>(const TNode& other) const {
            return priority > other.priority;
        }
    };
    std::priority_queue<TNode, std::vector<TNode>, std::greater<TNode>> m_priorityQ;
};

#endif
