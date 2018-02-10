#include "table.hpp"

#include <algorithm>
#include <numeric>

table::table(vec size, std::vector<entry> entries)
    : _entries(entries)
    , _size(size)
    , _grid(size.w, std::vector<int>(size.h, -1))
    , _spacing(20) // TODO
{
    init_children();

    for (std::size_t k = 0; k < _entries.size(); ++k)
    {
        auto const & e = _entries[k];
        for (int x = e.placement.x; x < e.placement.x + e.placement.w; ++x)
        {
            for (int y = e.placement.y; y < e.placement.y + e.placement.h; ++y)
            {
                _grid[x][y] = k;
            }
        }
    }
}

table::~table()
{
}

std::vector<widget *> table::get_children()
{
    std::vector<widget *> result;
    for (auto & e : _entries)
        result.push_back(e.wptr.get());
    return result;
}

std::vector<widget const *> table::get_children() const
{
    std::vector<widget const *> result;
    for (auto const & e : _entries)
        result.push_back(e.wptr.get());
    return result;
}

void table::apply_layout_to_children()
{

    int min_widths[_size.w];
    int min_heights[_size.h];
    min_cell_dimensions(min_widths, min_heights);

    // For the moment: Equally distribute remaining space on all rows and columns.
    // TODO consider height-for-width (how?)
    // TODO distribute to empty cells?
    int const min_width = length_with_spacing(min_widths, _size.w);
    int const min_height = length_with_spacing(min_heights, _size.h);

    int const bonus_width = (get_box().w - min_width) / _size.w;
    int const bonus_height = (get_box().h - min_height) / _size.h;

    for (auto & w : min_widths)
    {
        w += bonus_width;
    }

    for (auto & h : min_heights)
    {
        h += bonus_height;
    }

    // Compute offsets and sizes.
    int x_offsets[_size.w];
    int y_offsets[_size.h];
    x_offsets[0] = 0;
    y_offsets[0] = 0;
    std::partial_sum(min_widths, min_widths + _size.w - 1, x_offsets + 1);
    std::partial_sum(min_heights, min_heights + _size.h - 1, y_offsets + 1);

    for (auto & e : _entries)
    {
        int const x = get_box().x + x_offsets[e.placement.x] + e.placement.x * _spacing;
        int const y = get_box().y + y_offsets[e.placement.y] + e.placement.y * _spacing;

        int const w = std::accumulate(min_widths + e.placement.x, min_widths + e.placement.x + e.placement.w, 0) + std::max(0, e.placement.w - 1) * _spacing;
        int const h = std::accumulate(min_heights + e.placement.y, min_heights + e.placement.y + e.placement.h, 0) + std::max(0, e.placement.h - 1) * _spacing;

        e.wptr->apply_layout({ x, y, w, h });
    }
}

widget * table::find_selectable(navigation_type nt, point center)
{
    // TODO Create a an array with row and column coordinates to find the grid position.
    return nullptr;
}

widget * table::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    // TODO Navigate with _grid.
    return nullptr;
}

vec table::min_size_hint() const
{
    int min_widths[_size.w];
    int min_heights[_size.h];
    min_cell_dimensions(min_widths, min_heights);

    return { length_with_spacing(min_widths, _size.w), length_with_spacing(min_heights, _size.h) };
}

int table::length_with_spacing(int * lengths, int n) const
{
    return std::accumulate(lengths, lengths + n, std::max(0, n - 1) * _spacing);
}

void table::min_cell_dimensions(int * min_widths, int * min_heights) const
{
    vec min_sizes[_entries.size()];
    for (std::size_t k = 0; k < _entries.size(); ++k)
    {
        min_sizes[k] = _entries[k].wptr->min_size_hint();
    }

    // Determine the minimum width for each column and row in one go.
    std::uninitialized_fill_n(min_widths, _size.w, 0);
    for (int y = 0; y < _size.h; ++y)
    {
        min_heights[y] = 0;

        for (int x = 0; x < _size.w; ++x)
        {
            int const entry_index = _grid[x][y];
            if (entry_index != -1)
            {
                auto const & e = _entries[entry_index];

                vec const min_size = min_sizes[entry_index];
                min_heights[y] = std::max(min_heights[y], min_size.h / e.placement.h);
                min_widths[x] = std::max(min_widths[x], min_size.w / e.placement.w);
            }
        }
    }
}

