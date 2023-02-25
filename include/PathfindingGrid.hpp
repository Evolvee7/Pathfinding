#pragma once

#include <iostream>
#include "Grid.hpp"




class PathfindingGrid: public Grid<char>
{
public:
    PathfindingGrid(const Vec2i& size, const Vec2i& start, const Vec2i& finish);

    void Print() const;

private:
    Vec2i m_start;
    Vec2i m_finish;

    const char m_start_mark = 'S';
    const char m_finish_mark = 'F';
    const char m_empty_mark = '.';
    const char m_wall_mark = 'X';

private:
    void Fill(char value);
    void Outline(char value);
};

PathfindingGrid::PathfindingGrid(const Vec2i& size, const Vec2i& start, const Vec2i& finish): Grid<char>(size), m_start(start), m_finish(finish)
{
    Fill(m_empty_mark);
    Outline(m_wall_mark);
    Set(start, m_start_mark);
    Set(finish, m_finish_mark);
}

void PathfindingGrid::Print() const {
    for(int y = 0; y < m_size.y; ++y)
    {
        for(int x = 0; x < m_size.x; ++x)
        {
            std::cout << Get(Vec2i(x,y));
        }
        std::cout << std::endl;
    }
}

void PathfindingGrid::Fill(char value) {
    for(int y = 0; y < m_size.y; ++y)
    {
        for(int x = 0; x < m_size.x; ++x)
        {
            Set(Vec2i(x,y), value);
        }
    }
}

void PathfindingGrid::Outline(char value) {
    for(int x = 0; x < m_size.x; ++x)
    {
        Set(Vec2i(x, 0), value);
        Set(Vec2i(x, m_size.y-1), value);
    }
    for(int y = 0; y < m_size.y; ++y)
    {
        Set(Vec2i(0, y), value);
        Set(Vec2i(m_size.x-1, y), value);
    }
}
