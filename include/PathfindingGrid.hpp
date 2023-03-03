#pragma once

#include <iostream>
#include <SDL2/SDL_render.h>
#include "Grid.hpp"



namespace Mark
{
    const char empty = '.';
    const char wall = 'X';
    const char start = 'S';
    const char finish = 'F';
};

struct Cell
{
    Vec2i pos;
    int counter;

    Cell(const Vec2i& pos, int counter): pos(pos), counter(counter)
    {
    }
};

class PathfindingGrid: public Grid<char>
{
public:
    PathfindingGrid(const Vec2i& size, const Vec2i& start, const Vec2i& finish);

    void SetStart(const Vec2i& new_pos);
    void SetFinish(const Vec2i& new_pos);

    const Vec2i& Start() const { return m_start; }
    const Vec2i& Goal() const { return m_goal; }

    void Print() const;
    void Draw(SDL_Renderer* renderer, const Vec2i& cell_size) const;

private:
    Vec2i m_start;
    Vec2i m_goal;
};

PathfindingGrid::PathfindingGrid(const Vec2i& size, const Vec2i& start, const Vec2i& goal): Grid<char>(size), m_start(start), m_goal(goal)
{
    Fill(Mark::empty);
    Set(start, Mark::start);
    Set(goal, Mark::finish);
}

void PathfindingGrid::SetStart(const Vec2i& new_pos)
{
    if(Get(new_pos) == Mark::finish)
        return;
    
    Set(m_start, Mark::empty);
    m_start = new_pos;
    Set(m_start, Mark::start);
}

void PathfindingGrid::SetFinish(const Vec2i& new_pos)
{
    if(Get(new_pos) == Mark::start)
        return;
    
    Set(m_goal, Mark::empty);
    m_goal = new_pos;
    Set(m_goal, Mark::finish);
}



void PathfindingGrid::Print() const
{
    for(int y = 0; y < m_size.y; ++y)
    {
        for(int x = 0; x < m_size.x; ++x)
        {
            std::cout << Get(Vec2i{x,y}) << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PathfindingGrid::Draw(SDL_Renderer* renderer, const Vec2i& cell_size) const
{
    SDL_Rect rect{0, 0, cell_size.x, cell_size.y};
    for(int y = 0; y < m_size.y; ++y)
    {
        for(int x = 0; x < m_size.x; ++x)
        {
            const char mark = Get(Vec2i{x,y});
            if(mark == Mark::empty)
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            else if(mark == Mark::wall)
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            else if(mark == Mark::start)
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            else if(mark == Mark::finish)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            else
            {
                std::cerr << "ERROR: Undefined marker " << mark << " at pos (" << x << ',' << y << ")." << std::endl;
                return;
            }

            rect.x = x*cell_size.x;
            rect.y = y*cell_size.y;
            SDL_RenderFillRect(renderer, &rect);

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
}