#pragma once

#include <SDL2/SDL_render.h>
#include "PathfindingGrid.hpp"



class IPathfindingAlgo
{
public:
    virtual std::vector<Vec2i> GetPath(const PathfindingGrid& grid) const = 0;
    virtual void Visualize(SDL_Renderer* renderer, const PathfindingGrid& grid, const Vec2i& cell_size) const = 0;
};