#pragma once

#include <algorithm>
#include <iostream>
#include <iomanip>
#include <queue>
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

    void ResetStart(const Vec2i& new_pos);
    void ResetFinish(const Vec2i& new_pos);

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
    bool CreateCellsAround(const Vec2i& begin, const Vec2i& end);
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


    if(CreateCellsAround(m_finish, m_start) == false)
        return m_solved;

    m_counter_grid.Fill(m_unused);
    MarkCellCounters();
    CreatePath();
    MarkPath();

    m_solved = true;
    return m_solved;
}

void PathfindingGrid::ResetStart(const Vec2i& new_pos)
{
    if(new_pos.x < 0 || new_pos.x > m_size.x - 1)
        return;
    if(new_pos.y < 0 || new_pos.y > m_size.y - 1)
        return;
    
    Set(m_start, Mark::empty);
    m_start = new_pos;
    Set(m_start, Mark::start);
}

void PathfindingGrid::ResetFinish(const Vec2i& new_pos)
{
    if(new_pos.x < 0 || new_pos.x > m_size.x - 1)
        return;
    if(new_pos.y < 0 || new_pos.y > m_size.y - 1)
        return;
    
    Set(m_finish, Mark::empty);
    m_finish = new_pos;
    Set(m_finish, Mark::finish);
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
            const char mark = Get(Vec2i(x,y));
            if(mark == Mark::empty)
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            else if(mark == Mark::wall)
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            else if(mark == Mark::start)
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            else if(mark == Mark::finish)
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            else if(mark == Mark::path)
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
            else
            {
                std::cerr << "ERROR: Undefined marker " << mark << " at pos (" << x << ',' << y << ")." << std::endl;
                return;
            }

            rect.x = x*cell_size.x;
            rect.y = y*cell_size.y;
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}



bool PathfindingGrid::CreateCellsAround(const Vec2i& begin, const Vec2i& end)
{
    // Temporary stores Cells
    std::queue<Cell> queue;

    // Start from finish
    Cell inital_cell(begin, 0);
    m_cells.emplace_back(inital_cell);
    queue.emplace(inital_cell);

    while(queue.empty() == false) 
    {
        //
        // IF QUEUE IS EMPTY HERE -> GRID IS NOT SOLVABLE?
        //

        const Cell& cell = queue.front();
        
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

            if(pos == end)
            {
                return true;
            }

            queue.emplace(new_cell);
        }
        queue.pop();
    }

    return false;
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