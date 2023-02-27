#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <SDL2/SDL.h>
#include "Grid.hpp"



// TODO: Prevent looping infinitely if grid is not solvable
// TODO: Choose start and finish position, after grid size is set

void PrintGrid(const Grid<int>& grid);
void PrintGrid(const Grid<char>& grid);
void DrawGrid(const Grid<char>& grid, SDL_Renderer* renderer);

struct Cell
{
    Vec2i pos;
    int counter;

    Cell(const Vec2i& pos, int counter): pos(pos), counter(counter)
    {
    }
};

int main()
{
// 0) Choose grid size
    const int min_grid_quadrant_size = 5;
    const int max_grid_quadrant_size = 20;
    Vec2i grid_size(0,0);

    while(true)
    {
        std::cout << "Choose grid size(" << min_grid_quadrant_size << '-' << max_grid_quadrant_size << "): ";
        std::cin >> grid_size.x;

        if(std::cin.fail())
        {
            std::cout << "ERROR: Input must be integer value." << std::endl;
            std::cin.clear();
            std::cin.ignore(1000, '\n');
            continue;
        }
        if(grid_size.x < min_grid_quadrant_size)
        {
            std::cout << "ERROR: Input must be greater or equal to " << min_grid_quadrant_size << std::endl;
            continue;
        }
        if(grid_size.x > max_grid_quadrant_size)
        {
            std::cout << "ERROR: Input must be less or equal to " << max_grid_quadrant_size << std::endl;
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

    char empty_mark = '.';
    char wall_mark = 'X';
    char finish_mark = 'F';
    char start_mark = 'S';

    Vec2i start(1,2);
    Vec2i finish(8,8);

// 1) Make solvable grid

    Grid<char> inital_grid(grid_size);
    inital_grid.Fill(empty_mark);
    inital_grid.Outline(wall_mark);
    inital_grid.Set(start, start_mark);
    inital_grid.Set(finish, finish_mark);

    inital_grid.Set(Vec2i(2,2), wall_mark);   // Add some obstacles
    inital_grid.Set(Vec2i(3,3), wall_mark);
    inital_grid.Set(Vec2i(4,4), wall_mark);
    inital_grid.Set(Vec2i(5,5), wall_mark);
    inital_grid.Set(Vec2i(6,6), wall_mark);
    inital_grid.Set(Vec2i(6,7), wall_mark);
    inital_grid.Set(Vec2i(6,8), wall_mark);


// 2) Make cells phase

    // Keeps track of all created Cells
    std::vector<Cell> main_list;

    // Temporary stores Cells
    std::queue<Cell> queue;

    // Start from finish
    Cell inital_cell(finish, 0);
    main_list.emplace_back(inital_cell);
    queue.emplace(inital_cell);

    bool solved = false;
    while(solved == false) 
    {
        const Cell cell = queue.front();
        queue.pop();
        
        for(const Vec2i& pos: inital_grid.GetAdjecentTo(cell.pos))
        {
            // Skip if position is occupied by wall
            if(inital_grid.Get(pos) == wall_mark)
                continue;
            
            // Skip if position already exists in main_list
            if(std::find_if(main_list.begin(), main_list.end(), [&](const Cell& cell){ return cell.pos == pos; }) != main_list.end())
                continue;
            
            Cell new_cell(pos, cell.counter + 1);
            main_list.emplace_back(new_cell);

            if(inital_grid.Get(pos) == start_mark)
            {
                solved = true;
                break;
            }

            queue.emplace(new_cell);
        }
    }


// 3) Mark cells counter values

    // Grid to mark counter values
    Grid<int> counter_grid(grid_size);
    counter_grid.Fill(9999);   // Fill with high enough number

    // Mark counters onto grid
    for(const Cell& cell: main_list)
    {
        counter_grid.Set(cell.pos, cell.counter);
    }
    PrintGrid(counter_grid);


// 4) Path creation phase

    std::vector<Vec2i> path;
    path.emplace_back(start);

    while(path.back() != finish)
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


// 5) Show solved grid

    char path_mark = 'o';
    Grid<char> solved_grid(inital_grid);

    for(const Vec2i& pos: path)
    {
        if(pos != start && pos != finish)
            solved_grid.Set(pos, path_mark);
    }
    PrintGrid(solved_grid);


    DrawGrid(solved_grid, renderer);
    
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

void PrintGrid(const Grid<char>& grid)
{
    for(int y = 0; y < grid.Size().y; ++y)
    {
        for(int x = 0; x < grid.Size().x; ++x)
        {
            std::cout << std::setw(2) << grid.Get(Vec2i(x,y)) << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;
}

void DrawGrid(const Grid<char>& grid, SDL_Renderer* renderer)
{
    for(int y = 0; y < grid.Size().y; ++y)
    {
        for(int x = 0; x < grid.Size().x; ++x)
        {
            if(grid.Get(Vec2i(x,y)) == '.')
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);
            else if(grid.Get(Vec2i(x,y)) == 'X')
                SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
            else if(grid.Get(Vec2i(x,y)) == 'o')
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, SDL_ALPHA_OPAQUE);
            else if(grid.Get(Vec2i(x,y)) == 'S')
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, SDL_ALPHA_OPAQUE);
            else if(grid.Get(Vec2i(x,y)) == 'F')
                SDL_SetRenderDrawColor(renderer, 255, 0, 0, SDL_ALPHA_OPAQUE);
            SDL_Rect rect{x*50, y*50, 50, 50};
            SDL_RenderFillRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}