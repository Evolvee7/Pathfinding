#include <iomanip>
#include <iostream>
#include <SDL2/SDL.h>
#include "SampleAlgo.hpp"



// TODO: Draw different color values on cells depending on counter values in cells positions
//       OR draw cells counter values instead
// TODO: Add hold-n-drag wall putting

void PrintGrid(const Grid<int>& grid);

int main()
{
    Vec2i grid_size(0,0);
    const int min_grid_length = 5;
    const int max_grid_length = 20;

    while(true)
    {
        std::cout << "Choose grid size(" << min_grid_length << '-' << max_grid_length << "): ";
        std::cin >> grid_size.x;

        if(std::cin.fail())
        {
            std::cout << "ERROR: Input must be integer value." << std::endl;
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }
        if(grid_size.x < min_grid_length)
        {
            std::cout << "ERROR: Input must be greater or equal to " << min_grid_length << std::endl;
            continue;
        }
        if(grid_size.x > max_grid_length)
        {
            std::cout << "ERROR: Input must be less or equal to " << max_grid_length << std::endl;
            continue;
        }

        break;
    }
    grid_size.y = grid_size.x;


    Vec2i cell_size(50, 50);
    // Init SDL2
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(cell_size.x * grid_size.x, cell_size.y * grid_size.y, 0, &window, &renderer);



    const Vec2i start(1,1);
    const Vec2i goal(grid_size.x-2, grid_size.y-2);

    PathfindingGrid grid(grid_size, start, goal);
    SampleAlgo algo;


    algo.Visualize(renderer, grid, cell_size);
    
    
    SDL_Event e;
    bool quit = false;
    while(quit == false)
    {
        SDL_WaitEvent(&e);
        if(e.type == SDL_KEYUP)
        {
            if(e.key.keysym.sym == SDLK_s)
            {
                Vec2i mouse_pos(0,0);
                SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
                Vec2i grid_pos(mouse_pos/cell_size.x);

                grid.SetStart(grid_pos);
                algo.Visualize(renderer, grid, cell_size);
            }
            else if(e.key.keysym.sym == SDLK_f)
            {
                Vec2i mouse_pos(0,0);
                SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
                Vec2i grid_pos(mouse_pos/cell_size.x);

                grid.SetFinish(grid_pos);
                algo.Visualize(renderer, grid, cell_size);
            }
            else
            {
                quit = true;
            }
        }
        else if(e.type == SDL_MOUSEBUTTONUP)
        {
            Vec2i mouse_pos(0,0);
            SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
            Vec2i grid_pos(mouse_pos/cell_size.x);

            char grid_mark = grid.Get(grid_pos);
            switch(grid_mark)
            {
                case Mark::empty:
                    grid_mark = Mark::wall;
                    break;
                
                case Mark::wall:
                    grid_mark = Mark::empty;
                    break;
            }
            grid.Set(grid_pos, grid_mark);
            algo.Visualize(renderer, grid, cell_size);
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void PrintGrid(const Grid<int>& grid)
{
    for(int y = 0; y < grid.Size().y; ++y)
    {
        for(int x = 0; x < grid.Size().x; ++x)
        {
            std::cout << std::setw(4) << grid.Get(Vec2i(x,y)) << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}