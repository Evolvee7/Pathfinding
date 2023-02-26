#include <iomanip>
#include <iostream>
#include <queue>
#include "Grid.hpp"



void PrintGrid(const Grid<int>& grid);
void PrintGrid(const Grid<char>& grid);

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
    char empty_mark = '.';
    char wall_mark = 'X';
    char finish_mark = 'F';
    char start_mark = 'S';
    char path_mark= 'o';

    Vec2i start(1,2);
    Vec2i finish(8,8);

    Vec2i grid_size(10,10);

    // Keeps track of all created Cells
    std::vector<Cell> main_list;

    // Temporary stores outside Cells
    std::queue<Cell> queue;


    // Grid to hold inital state
    Grid<char> inital_grid(grid_size);
    inital_grid.Fill(empty_mark);
    inital_grid.Outline(wall_mark);
    inital_grid.Set(start, start_mark);
    inital_grid.Set(finish, finish_mark);

    inital_grid.Set(Vec2i(2,2), wall_mark);   // Some obstacles
    inital_grid.Set(Vec2i(3,3), wall_mark);
    inital_grid.Set(Vec2i(4,4), wall_mark);
    inital_grid.Set(Vec2i(5,5), wall_mark);
    inital_grid.Set(Vec2i(6,6), wall_mark);
    inital_grid.Set(Vec2i(6,7), wall_mark);
    inital_grid.Set(Vec2i(6,8), wall_mark);


    // Make inital Cell at finish
    Cell inital_cell(finish, 0);
    main_list.emplace_back(inital_cell);
    queue.emplace(inital_cell);


    bool solved = false;
    while(solved == false) 
    {
        const Cell& cell = queue.front();
        queue.pop();
        for(const Vec2i& pos: inital_grid.GetAdjecentTo(cell.pos))
        {
            // Skip wall
            if(inital_grid.Get(pos) == wall_mark)
                continue;

            // Skip if cell position already exists in main_list
            bool exists = false;
            for(const Cell& cell: main_list)
            {
                if(cell.pos == pos)
                {
                    exists = true;
                    break;
                }
            }
            if(exists == true)
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

    // Grid to mark counter values
    Grid<int> counter_grid(grid_size);
    counter_grid.Fill(9999);   // Fill with high enough number
    // Mark counters onto grid
    for(const Cell& cell: main_list)
    {
        counter_grid.Set(cell.pos, cell.counter);
    }
    PrintGrid(counter_grid);


    // Seek the shortest path starting from S
    std::vector<Vec2i> path;
    path.emplace_back(start);
    while(!(path.back() == finish))
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


    // Grid with the shortest path
    Grid<char> final_grid(inital_grid);
    for(const Vec2i& pos: path)
    {
        final_grid.Set(pos, path_mark);
    }
    PrintGrid(final_grid);

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