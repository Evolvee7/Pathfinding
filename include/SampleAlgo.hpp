#pragma once

#include <algorithm>
#include <queue>
#include "IPathfindingAlgo.hpp"





class SampleAlgo: IPathfindingAlgo
{
public:
    std::vector<Vec2i> GetPath(const PathfindingGrid& grid) const override;
    void Visualize(SDL_Renderer* renderer, const PathfindingGrid& grid, const Vec2i& cell_size) const override;

private:
    std::vector<Cell> CreateCellsAround(const PathfindingGrid& grid) const;
};

std::vector<Vec2i> SampleAlgo::GetPath(const PathfindingGrid& grid) const
{
    std::vector<Cell> cells = CreateCellsAround(grid);

    if(cells.empty())
        return std::vector<Vec2i>();

    Grid<int> counter_grid(grid.Size());
    counter_grid.Fill(9999);
    for(const Cell& cell: cells)
    {
        counter_grid.Set(cell.pos, cell.counter);
    }

    std::vector<Vec2i> path;
    path.emplace_back(grid.Start());
    while(path.back() != grid.Goal())
    {
        const Vec2i& point = path.back();
        Cell lowest_value_cell(point, 9999);

        for(const Vec2i& pos: counter_grid.GetAdjecentTo(point))
        {
            if(counter_grid.Get(pos) < lowest_value_cell.counter)
            {
                Cell new_lowest_value_cell(pos, counter_grid.Get(pos));
                lowest_value_cell = new_lowest_value_cell;
            }
        }
        path.emplace_back(lowest_value_cell.pos);
    }

    return path;
}

std::vector<Cell> SampleAlgo::CreateCellsAround(const PathfindingGrid& grid) const
{
    std::vector<Cell> cells;
    std::queue<Cell> queue;

    Cell inital_cell(grid.Goal(), 0);
    cells.emplace_back(inital_cell);
    queue.emplace(inital_cell);

    while(queue.empty() == false) 
    {
        const Cell& cell = queue.front();
        
        for(const Vec2i& pos: grid.GetAdjecentTo(cell.pos))
        {
            // Skip if position is occupied by wall
            if(grid.Get(pos) == Mark::wall)
                continue;
            
            // Skip if position is already stored
            if(std::find_if(cells.begin(), cells.end(), [&](const Cell& cell){ return cell.pos == pos; }) != cells.end())
                continue;
            
            Cell new_cell(pos, cell.counter + 1);
            cells.emplace_back(new_cell);

            if(pos == grid.Start())
            {
                return cells;
            }

            queue.emplace(new_cell);
        }
        queue.pop();
    }

    return std::vector<Cell>();
}

void SampleAlgo::Visualize(SDL_Renderer* renderer, const PathfindingGrid& grid, const Vec2i& cell_size) const
{
    grid.Draw(renderer, cell_size);

    SDL_Rect rect{0, 0, cell_size.x, cell_size.y};
    // Draw cells
    for(const Cell& cell: CreateCellsAround(grid))
    {
        if(cell.pos == grid.Start() || cell.pos == grid.Goal())
            continue;
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 50, SDL_ALPHA_OPAQUE);
        rect.x = cell.pos.x*cell_size.x;
        rect.y = cell.pos.y*cell_size.y;
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);
    }
    // Draw path
    for(const Vec2i& pos: GetPath(grid))
    {
        if(pos == grid.Start() || pos == grid.Goal())
            continue;
        
        SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
        rect.x = pos.x*cell_size.x;
        rect.y = pos.y*cell_size.y;
        SDL_RenderFillRect(renderer, &rect);

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
        SDL_RenderDrawRect(renderer, &rect);
    }

    SDL_RenderPresent(renderer);
}