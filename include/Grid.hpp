#pragma once

#include <vector>
#include "Vec2i.hpp"



template <typename T>
class Grid
{
public:
    Grid(const Vec2i& size);

    const Vec2i& Size() const;

    const T& Get(const Vec2i& pos) const;
    void Set(const Vec2i& pos, const T& value);

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
const Vec2i& Grid<T>::Size() const {
    return m_size;
}

template <typename T>
const T& Grid<T>::Get(const Vec2i& pos) const {
    return m_cells[pos.y*m_size.x+pos.x];
}

template <typename T>
void Grid<T>::Set(const Vec2i& pos, const T& value) {
    m_cells[pos.y*m_size.x+pos.x] = value;
}