#pragma once

#include <iostream>
#include <iomanip>
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

    void Solve();

    bool IsSolved() const { return m_solved; }
    const std::vector<Vec2i>& GetPath() const { return m_path; }
    const Grid<int>& GetCounterGrid() const { return m_counter_grid; }

    void Print() const;
    void Draw(SDL_Renderer* renderer) const;

private:
    Vec2i m_start;
    Vec2i m_finish;

    Grid<int> m_counter_grid;

    std::vector<Cell> m_cells;
    std::vector<Vec2i> m_path;

    bool m_solved;
};

PathfindingGrid::PathfindingGrid(const Vec2i& size, const Vec2i& start, const Vec2i& finish): Grid<char>(size), m_counter_grid(size), m_start(start), m_finish(finish)
{
    m_solved = false;

    Fill(Mark::empty);
    Outline(Mark::wall);
    Set(start, Mark::start);
    Set(finish, Mark::finish);
}

void PathfindingGrid::Solve()
{
    m_cells.clear();
    m_path.clear();
}

void PathfindingGrid::Print() const
{
    for(int y = 0; y < m_size.y; ++y)
    {
        for(int x = 0; x < m_size.x; ++x)
        {
            std::cout << std::setw(2) << Get(Vec2i(x,y)) << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void PathfindingGrid::Draw(SDL_Renderer* renderer) const
{
    for(int y = 0; y < m_size.y; ++y)
    {
        for(int x = 0; x < m_size.x; ++x)
        {
            if(Get(Vec2i(x,y)) == '.')
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            else if(Get(Vec2i(x,y)) == 'X')
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            else if(Get(Vec2i(x,y)) == 'o')
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
            else if(Get(Vec2i(x,y)) == 'S')
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            else if(Get(Vec2i(x,y)) == 'F')
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_Rect rect{x*50, y*50, 50, 50};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}