#pragma once



struct Vec2i
{
    int x;
    int y;

    Vec2i(int x, int y): x(x), y(y)
    {
    }

    bool operator==(const Vec2i& other) const
    {
        return x == other.x && y == other.y;
    }

    bool operator!=(const Vec2i& other) const
    {
        return !operator==(other);
    }

    Vec2i operator/(int num) const
    {
        return Vec2i(x/num, y/num);
    }
};