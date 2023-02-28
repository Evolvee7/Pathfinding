#include <iomanip>
#include <iostream>
#include <SDL2/SDL.h>
#include "PathfindingGrid.hpp"



// TODO: Prevent looping infinitely if grid is not solvable
// TODO: Draw different color values on cells depending on counter values in cells positions
//       OR draw cells counter values instead

void PrintGrid(const Grid<int>& grid);

int main()
{
    // Choose grid size
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


    Vec2i cell_size(50, 50);
    // Init SDL2
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window* window;
    SDL_Renderer* renderer;
    SDL_CreateWindowAndRenderer(cell_size.x * grid_size.x, cell_size.y * grid_size.y, 0, &window, &renderer);



    const Vec2i start(1,1);
    const Vec2i finish(grid_size.x-2, grid_size.y-2);

    PathfindingGrid grid(grid_size, start, finish);

    //grid.Set(Vec2i(2,2), Mark::wall);   // Add some obstacles
    //grid.Set(Vec2i(3,3), Mark::wall);
    //grid.Set(Vec2i(4,4), Mark::wall);
    //grid.Set(Vec2i(5,5), Mark::wall);
    //grid.Set(Vec2i(6,6), Mark::wall);
    //grid.Set(Vec2i(6,7), Mark::wall);
    //grid.Set(Vec2i(6,8), Mark::wall);

    grid.Solve();
    grid.Draw(renderer, cell_size);

    
    SDL_Event e;
    bool quit = false;
    while(quit == false)
    {
        SDL_WaitEvent(&e);
        switch(e.type)
        {
            case SDL_KEYUP:
                if(e.key.keysym.sym == SDLK_s)
                {
                    Vec2i mouse_pos(0,0);
                    SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
                    Vec2i grid_pos(mouse_pos/cell_size.x);

                    if(grid_pos.x == 0 || grid_pos.x == grid_size.x-1)
                        break;
                    if(grid_pos.y == 0 || grid_pos.y == grid_size.y-1)
                        break; 

                    grid.ResetStart(grid_pos);
                    grid.Solve();
                    grid.Draw(renderer, cell_size);
                }
                else if(e.key.keysym.sym == SDLK_f)
                {
                    Vec2i mouse_pos(0,0);
                    SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
                    Vec2i grid_pos(mouse_pos/cell_size.x);

                    if(grid_pos.x == 0 || grid_pos.x == grid_size.x-1)
                        break;
                    if(grid_pos.y == 0 || grid_pos.y == grid_size.y-1)
                        break; 

                    grid.ResetFinish(grid_pos);
                    grid.Solve();
                    grid.Draw(renderer, cell_size);
                }
                else
                {
                    quit = true;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                Vec2i mouse_pos(0,0);
                SDL_GetMouseState(&mouse_pos.x, &mouse_pos.y);
                Vec2i grid_pos(mouse_pos/cell_size.x);

                if(grid_pos.x == 0 || grid_pos.x == grid_size.x-1)
                    break;
                if(grid_pos.y == 0 || grid_pos.y == grid_size.y-1)
                    break;    

                char grid_mark = grid.Get(grid_pos);
                switch(grid_mark)
                {
                    case Mark::empty:
                        grid_mark = Mark::wall;
                        break;
                    
                    case Mark::wall:
                        grid_mark = Mark::empty;
                        break;
                    
                    case Mark::path:
                        grid_mark = Mark::wall;
                        break;
                }
                grid.Set(grid_pos, grid_mark);
                grid.Solve();
                grid.Draw(renderer, cell_size);

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