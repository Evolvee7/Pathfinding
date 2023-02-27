#include <algorithm>
#include <iomanip>
#include <iostream>
#include <queue>
#include <SDL2/SDL.h>
#include "PathfindingGrid.hpp"



// TODO: Prevent looping infinitely if grid is not solvable
// TODO: Choose start and finish position, after grid size is set


std::vector<Cell> GetCellsAround(const Grid<char>& grid, const Vec2i& begin, const Vec2i& end);
void MarkCellCounters(Grid<int>& grid, const std::vector<Cell>& cells);
std::vector<Vec2i> GetPath(const Grid<int>& grid, const Vec2i& start, const Vec2i& finish);

void PrintGrid(const Grid<int>& grid);


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

    const Vec2i start(1,2);
    const Vec2i finish(8,8);

// 1) Make solvable grid

    PathfindingGrid inital_grid(grid_size, start, finish);
    inital_grid.Fill(Mark::empty);
    inital_grid.Outline(Mark::wall);
    inital_grid.Set(start, Mark::start);
    inital_grid.Set(finish, Mark::finish);

    inital_grid.Set(Vec2i(2,2), Mark::wall);   // Add some obstacles
    inital_grid.Set(Vec2i(3,3), Mark::wall);
    inital_grid.Set(Vec2i(4,4), Mark::wall);
    inital_grid.Set(Vec2i(5,5), Mark::wall);
    inital_grid.Set(Vec2i(6,6), Mark::wall);
    inital_grid.Set(Vec2i(6,7), Mark::wall);
    inital_grid.Set(Vec2i(6,8), Mark::wall);


// 2) Make cells phase

    
    std::vector<Cell> main_list = GetCellsAround(inital_grid, finish, start);


// 3) Mark cells counter values

    // Grid to mark counter values
    Grid<int> counter_grid(grid_size);
    counter_grid.Fill(9999);   // Fill with high enough number

    MarkCellCounters(counter_grid, main_list);

    PrintGrid(counter_grid);


// 4) Path creation phase


    std::vector<Vec2i> path = GetPath(counter_grid, start, finish);


// 5) Show solved grid

    char path_mark = 'o';
    PathfindingGrid solved_grid(inital_grid);

    for(const Vec2i& pos: path)
    {
        if(pos != start && pos != finish)
            solved_grid.Set(pos, path_mark);
    }
    solved_grid.Print();

    solved_grid.Draw(renderer);
    
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
                // 1) Check if grid is solvable?
                // 2) Update* grid only if:
                // - only 1 start and finish will exist
                //
                // *Update = change . -> X -> S -> F -> .   ,etc...
        }
    }


    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}



std::vector<Cell> GetCellsAround(const Grid<char>& grid, const Vec2i& begin, const Vec2i& end)
{
    std::vector<Cell> cells_around;

    // Temporary stores Cells
    std::queue<Cell> queue;

    // Start from finish
    Cell inital_cell(begin, 0);
    cells_around.emplace_back(inital_cell);
    queue.emplace(inital_cell);

    bool solved = false;
    while(solved == false) 
    {
        //
        // IF QUEUE IS EMPTY HERE -> GRID IS NOT SOLVABLE?
        //

        const Cell cell = queue.front();
        queue.pop();
        
        for(const Vec2i& pos: grid.GetAdjecentTo(cell.pos))
        {
            // Skip if position is occupied by wall
            if(grid.Get(pos) == Mark::wall)
                continue;
            
            // Skip if position is already stored
            if(std::find_if(cells_around.begin(), cells_around.end(), [&](const Cell& cell){ return cell.pos == pos; }) != cells_around.end())
                continue;
            
            Cell new_cell(pos, cell.counter + 1);
            cells_around.emplace_back(new_cell);

            if(pos == end)
            {
                solved = true;
                break;
            }

            queue.emplace(new_cell);
        }
    }

    return cells_around;
}

void MarkCellCounters(Grid<int>& grid, const std::vector<Cell>& cells)
{
    for(const Cell& cell: cells)
    {
        grid.Set(cell.pos, cell.counter);
    }
}

std::vector<Vec2i> GetPath(const Grid<int>& grid, const Vec2i& start, const Vec2i& finish)
{
    std::vector<Vec2i> path;
    path.emplace_back(start);

    while(path.back() != finish)
    {
        const Vec2i& point = path.back();
        Cell lowest_value_cell(point, 9999);

        for(const Vec2i& pos: grid.GetAdjecentTo(point))
        {
            if(grid.Get(pos) < lowest_value_cell.counter)
            {
                Cell new_lowest_value_cell(pos, grid.Get(pos));
                lowest_value_cell = new_lowest_value_cell;
            }
        }
        path.emplace_back(lowest_value_cell.pos);
    }

    return path;
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