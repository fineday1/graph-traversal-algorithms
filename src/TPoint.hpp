#ifndef POINT_HPP
#define POINT_HPP

struct TPoint
{
    int x, y;
    bool operator==(const TPoint &other) const { return x == other.x && y == other.y; }
    bool operator!=(const TPoint &other) const { return x != other.x || y != other.y; }
    bool operator<(const TPoint &other) const
    {
        return (y != other.y) ? (y < other.y) : (x < other.x);
    }
};

#endif