#ifndef FRONTIER_HPP
#define FRONTIER_HPP

#include "ISearchable.hpp"

class IFrontier
{
    public:
        virtual ~IFrontier() = default;

        virtual void push(NodeID node, int priority = 0) = 0;
        virtual NodeID pop() = 0;
        
        virtual bool empty() const = 0;
        virtual void clear() = 0;
};

#endif