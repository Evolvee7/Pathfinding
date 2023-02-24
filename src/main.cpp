#include "Grid.hpp"



int main()
{
    Grid<char> grid(Vec2i(5,5));
    grid.Fill('.');
    grid.Set(Vec2i(0,2), 'S');
    grid.Set(Vec2i(4,4), 'E');
    grid.Print();
    
    return 0;
}