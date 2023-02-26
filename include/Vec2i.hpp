#pragma once



struct Vec2i
{
    int x;
    int y;

    Vec2i(int x, int y): x(x), y(y)
    {
    }

    bool operator==(const Vec2i& other_pos) const
    {
        return x == other_pos.x && y == other_pos.y;
    }
};