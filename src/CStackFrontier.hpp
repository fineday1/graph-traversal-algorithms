#ifndef C_STACK_FRONTIER_HPP
#define C_STACK_FRONTIER_HPP

#include <stack>

#include "IFrontier.hpp"

class CStackFrontier : public IFrontier {
public:
    void push(NodeID node, int /*priority*/) override
    {
        m_stack.push(node);
    }
    NodeID pop() override
    {
        NodeID n = m_stack.top();
        m_stack.pop();
        return n;
    }

    bool empty() const override
    {
        return m_stack.empty();
    }
    void clear() override
    {
        while (!m_stack.empty()) m_stack.pop();
    }
private:
    std::stack<NodeID> m_stack;
};

#endif
