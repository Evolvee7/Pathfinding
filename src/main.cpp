#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <SDL2/SDL.h>
#include "PathfindingGrid.hpp"



// TODO: Prevent looping infinitely if grid is not solvable
// TODO: Choose start and finish position, after grid size is set

void PrintGrid(const Grid<int>& grid);


int main()
{
// 0) Choose grid size
    const int min_grid_length = 5;
    const int max_grid_length = 20;
    Vec2i grid_size(0,0);

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


// 0.5) Initalize SDL2
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(50 * grid_size.x, 50 * grid_size.y, 0, &window, &renderer);


    // Stages of pathfinding:
    // 1) make solvable grid
    // 2) starting at finish:
    //    a) add finish cell with counter = 0 to queue and main_list
    //    b) for each element in queue seek for first element which:
    //        - isnt wall
    //        - isnt already in main_list
    //    c) add it to queue and main_list
    //    d) if not reached start return to b)
    // 3) mark cells counter values onto other grid
    // 4) path goes from start and follows lowest counter values around adjecent fields

    const Vec2i start(1,2);
    const Vec2i finish(8,8);

    PathfindingGrid grid(grid_size, start, finish);

    grid.Set(Vec2i(2,2), Mark::wall);   // Add some obstacles
    grid.Set(Vec2i(3,3), Mark::wall);
    grid.Set(Vec2i(4,4), Mark::wall);
    grid.Set(Vec2i(5,5), Mark::wall);
    grid.Set(Vec2i(6,6), Mark::wall);
    grid.Set(Vec2i(6,7), Mark::wall);
    grid.Set(Vec2i(6,8), Mark::wall);

    grid.Solve();

    grid.Draw(renderer);

    
    SDL_Event e;
    bool quit = false;
    while(quit == false)
    {
        SDL_WaitEvent(&e);
        switch(e.type)
        {
            case SDL_KEYUP:
                quit = true;
                break;
            case SDL_MOUSEBUTTONUP:
                Vec2i mouse_pos(0,0);
                SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);

                Vec2i grid_pos(mouse_pos/50);
                char grid_mark = grid.Get(grid_pos);
                switch(grid_mark)
                {
                    case Mark::empty:
                        grid_mark = Mark::wall;
                        break;
                    
                    case Mark::wall:
                        grid_mark = Mark::empty;
                        break;
                    
                    case Mark::start:
                        //grid_mark = Mark::finish;
                        break;
                    
                    case Mark::finish:
                        //grid_mark = Mark::empty;
                        break;
                    
                    case Mark::path:
                        grid_mark = Mark::wall;
                        break;
                    
                }
                grid.Set(grid_pos, grid_mark);
                grid.Solve();
                grid.Draw(renderer);

                // 1) Check if grid is solvable?
                // 2) Update* grid only if:
                // - only 1 start and finish will exist
                //
                // *Update = change . -> X -> S -> F -> .   ,etc...
                break;
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