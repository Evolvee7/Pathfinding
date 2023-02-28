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
    const char path = 'o';
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

    bool Solve();

    bool IsSolved() const { return m_solved; }
    const std::vector<Vec2i>& GetPath() const { return m_path; }
    const Grid<int>& GetCounterGrid() const { return m_counter_grid; }

    void Print() const;
    void Draw(SDL_Renderer* renderer, const Vec2i& cell_size) const;

private:
    Vec2i m_start;
    Vec2i m_finish;

    Grid<int> m_counter_grid;

    std::vector<Cell> m_cells;
    std::vector<Vec2i> m_path;

    bool m_solved;

    const int m_unused = 9999;

private:
    void CreateCellsAround();
    void MarkCellCounters();
    void CreatePath();
    void MarkPath();
};

PathfindingGrid::PathfindingGrid(const Vec2i& size, const Vec2i& start, const Vec2i& finish): Grid<char>(size), m_counter_grid(size), m_start(start), m_finish(finish)
{
    m_solved = false;

    Fill(Mark::empty);
    Outline(Mark::wall);
    Set(start, Mark::start);
    Set(finish, Mark::finish);
}

bool PathfindingGrid::Solve()
{
    m_solved = false;
    for(const Vec2i& pos: m_path)
    {
        if(Get(pos) == Mark::path)
            Set(pos, Mark::empty);
    }
    m_cells.clear();
    m_path.clear();

    CreateCellsAround();
    m_counter_grid.Fill(m_unused);
    MarkCellCounters();
    CreatePath();
    MarkPath();

    m_solved = true;
    return m_solved;
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

void PathfindingGrid::Draw(SDL_Renderer* renderer, const Vec2i& cell_size) const
{
    SDL_Rect rect{0, 0, cell_size.x, cell_size.y};
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
            
            rect.x = x*cell_size.x;
            rect.y = y*cell_size.y;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}



void PathfindingGrid::CreateCellsAround()
{
    // Temporary stores Cells
    std::queue<Cell> queue;

    // Start from finish
    Cell inital_cell(m_finish, 0);
    m_cells.emplace_back(inital_cell);
    queue.emplace(inital_cell);

    bool solved = false;
    while(solved == false) 
    {
        //
        // IF QUEUE IS EMPTY HERE -> GRID IS NOT SOLVABLE?
        //

        const Cell cell = queue.front();
        queue.pop();
        
        for(const Vec2i& pos: GetAdjecentTo(cell.pos))
        {
            // Skip if position is occupied by wall
            if(Get(pos) == Mark::wall)
                continue;
            
            // Skip if position is already stored
            if(std::find_if(m_cells.begin(), m_cells.end(), [&](const Cell& cell){ return cell.pos == pos; }) != m_cells.end())
                continue;
            
            Cell new_cell(pos, cell.counter + 1);
            m_cells.emplace_back(new_cell);

            if(pos == m_start)
            {
                solved = true;
                break;
            }

            queue.emplace(new_cell);
        }
    }
}

void PathfindingGrid::MarkCellCounters()
{
    for(const Cell& cell: m_cells)
    {
        m_counter_grid.Set(cell.pos, cell.counter);
    }
}

void PathfindingGrid::CreatePath()
{
    m_path.emplace_back(m_start);

    while(m_path.back() != m_finish)
    {
        const Vec2i& point = m_path.back();
        Cell lowest_value_cell(point, m_unused);

        for(const Vec2i& pos: m_counter_grid.GetAdjecentTo(point))
        {
            if(m_counter_grid.Get(pos) < lowest_value_cell.counter)
            {
                Cell new_lowest_value_cell(pos, m_counter_grid.Get(pos));
                lowest_value_cell = new_lowest_value_cell;
            }
        }
        m_path.emplace_back(lowest_value_cell.pos);
    }
}

void PathfindingGrid::MarkPath()
{
    for(const Vec2i& pos: m_path)
    {
        if(pos != m_start && pos != m_finish)
            Set(pos, Mark::path);
    }
}