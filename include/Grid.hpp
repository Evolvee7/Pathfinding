#pragma once

#include <vector>
#include "Vec2i.hpp"



template <typename T>
class Grid
{
public:
    Grid(const Vec2i& size);
    Grid(const Grid<T>& other);

    const Vec2i& Size() const;
    const std::vector<T>& GetCells() const;

    const T& Get(const Vec2i& pos) const;
    void Set(const Vec2i& pos, const T& value);

    void Fill(const T& value);

    std::vector<Vec2i> GetAdjecentTo(const Vec2i& pos) const;

    void Outline(const T& value);

protected:
    Vec2i m_size;
    std::vector<T> m_cells;
};

template <typename T>
Grid<T>::Grid(const Vec2i& size): m_size(size)
{
    m_cells.reserve(size.x*size.y);
}

template <typename T>
Grid<T>::Grid(const Grid<T>& other): m_size(other.Size())
{
    m_cells.reserve(m_size.x*m_size.y);
    
    const std::vector<T>& other_cells = other.GetCells();
    for(int i = 0; i < m_size.x*m_size.y; ++i)
    {
        m_cells[i] = other_cells[i];
    }
}

template <typename T>
const Vec2i& Grid<T>::Size() const
{
    return m_size;
}

template <typename T>
const std::vector<T>& Grid<T>::GetCells() const
{
    return m_cells;
}

template <typename T>
const T& Grid<T>::Get(const Vec2i& pos) const
{
    return m_cells[pos.y*m_size.x+pos.x];
}

template <typename T>
void Grid<T>::Set(const Vec2i& pos, const T& value)
{
    m_cells[pos.y*m_size.x+pos.x] = value;
}

template <typename T>
void Grid<T>::Fill(const T& value)
{
    for(int y = 0; y < m_size.y; ++y)
    {
        for(int x = 0; x < m_size.x; ++x)
        {
            Set(Vec2i{x,y}, value);
        }
    }
}

template <typename T>
std::vector<Vec2i> Grid<T>::GetAdjecentTo(const Vec2i& pos) const
{
    std::vector<Vec2i> adjecent;
    adjecent.reserve(4);
    if(pos.x > 0)
        adjecent.emplace_back(Vec2i{pos.x-1,pos.y});
    if(pos.x < m_size.x-1)
        adjecent.emplace_back(Vec2i{pos.x+1,pos.y});
    if(pos.y > 0)
        adjecent.emplace_back(Vec2i{pos.x,pos.y-1});
    if(pos.y < m_size.y-1)
        adjecent.emplace_back(Vec2i{pos.x,pos.y+1});
    return adjecent;
}

template <typename T>
void Grid<T>::Outline(const T& value)
{
    for(int x = 0; x < m_size.x; ++x)
    {
        Set(Vec2i{x, 0}, value);
        Set(Vec2i{x, m_size.y-1}, value);
    }
    for(int y = 1; y < m_size.y - 1; ++y)   // Skip first row and last row (already filled)
    {
        Set(Vec2i{0, y}, value);
        Set(Vec2i{m_size.x-1, y}, value);
    }
}