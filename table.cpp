#include "table.hpp"

#include <algorithm>
#include <numeric>

table::table(vec size, std::vector<entry> entries)
    : _entries(entries)
    , _size(size)
    , _grid(size.w, std::vector<int>(size.h, -1))
    , _spacing(20) // TODO
    , _x_offsets(size.h + 1, 0)
    , _y_offsets(size.w + 1, 0)
{
    init_children();

    for (std::size_t k = 0; k < _entries.size(); ++k)
    {
        // TODO sanity check
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
    int widths[_size.w];
    int heights[_size.h];
    min_cell_dimensions(widths, heights);

    // For the moment: Equally distribute remaining space on all rows and columns.
    // TODO consider height-for-width (how?)
    // TODO distribute to empty cells?
    int const min_width = length_with_spacing(widths, _size.w);
    int const min_height = length_with_spacing(heights, _size.h);

    int const bonus_width = (get_box().w - min_width) / _size.w;
    int const bonus_height = (get_box().h - min_height) / _size.h;

    for (auto & w : widths)
    {
        w += bonus_width;
    }

    for (auto & h : heights)
    {
        h += bonus_height;
    }

    // Compute offsets, store them for reuse and apply to children.
    _x_offsets[0] = get_box().x;
    _y_offsets[0] = get_box().y;

    for (int k = 0; k < _size.w; ++k)
    {
        _x_offsets[k + 1] = widths[k] + _x_offsets[k] + _spacing;
    }

    for (int k = 0; k < _size.h; ++k)
    {
        _y_offsets[k + 1] = heights[k] + _y_offsets[k] + _spacing;
    }

    for (auto & e : _entries)
    {
        int const x = _x_offsets[e.placement.x];
        int const y = _y_offsets[e.placement.y];

        int const w = _x_offsets[e.placement.x + e.placement.w] - x - _spacing;
        int const h = _y_offsets[e.placement.y + e.placement.h] - y - _spacing;

        e.wptr->apply_layout({ x, y, w, h });
    }
}

template <typename It>
int find_y_index(It first, It last, point center, vec size)
{
    auto it_y = std::find_if(first, last, [=](int bound){ return bound > center.y; });
    return std::min(size.h - 1, static_cast<int>(std::distance(first, it_y)) - 1);
}

template <typename It>
int find_x_index(It first, It last, point center, vec size)
{
    auto it_x = std::find_if(first, last, [=](int bound){ return bound > center.x; });

    return std::min(size.w - 1, static_cast<int>(std::distance(first, it_x)) - 1);
}

widget * table::find_selectable(navigation_type nt, point center)
{
    if (_entries.empty())
        return nullptr;

    if (nt == navigation_type::NEXT)
    {
        return _entries[0].wptr.get();
    }
    else if (nt == navigation_type::PREV)
    {
        return _entries.back().wptr.get();
    }
    else if (nt == navigation_type::NEXT_X)
    {
        int const y = find_y_index(std::begin(_y_offsets), std::end(_y_offsets), center, _size);
        for (int x = 0; x < _size.w; ++x)
        {
            int eidx = _grid[x][y];

            if (eidx != -1)
                return _entries[eidx].wptr.get();
        }
    }
    else if (nt == navigation_type::PREV_X)
    {
        int const y = find_y_index(std::begin(_y_offsets), std::end(_y_offsets), center, _size);
        for (int x = _size.w - 1; x >= 0; --x)
        {
            int eidx = _grid[x][y];

            if (eidx != -1)
                return _entries[eidx].wptr.get();
        }
    }
    else if (nt == navigation_type::NEXT_Y)
    {
        int const x = find_x_index(std::begin(_x_offsets), std::end(_x_offsets), center, _size);

        for (int y = 0; y < _size.h; ++y)
        {
            int eidx = _grid[x][y];

            if (eidx != -1)
                return _entries[eidx].wptr.get();
        }
    }
    else //if (nt == navigation_type::PREV_Y)
    {
        int const x = find_x_index(std::begin(_x_offsets), std::end(_x_offsets), center, _size);

        for (int y = _size.h - 1; y >= 0; --y)
        {
            int eidx = _grid[x][y];

            if (eidx != -1)
                return _entries[eidx].wptr.get();
        }
    }

    return nullptr;
}

template <typename It>
widget * navigate_natural_order(It first, It last, navigation_type nt, widget * w, point center)
{

    auto it = std::find_if(first, last, [=](auto const & e){ return e.wptr.get() == w; });

    if (it != last)
    {

        while (true)
        {
            ++it;

            if (it == last)
            {
                break;
            }
            else
            {
                auto w = it->wptr->find_selectable(nt, center);

                if (w != nullptr)
                    return w;
            }
        }

    }

    return nullptr;
}

widget * table::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    // TODO consistent behavior with box

    if (nt == navigation_type::NEXT)
    {
        auto res = navigate_natural_order(std::begin(_entries), std::end(_entries), nt, w, center);
        if (res != nullptr)
            return res;
    }
    else if (nt == navigation_type::PREV)
    {
        auto res = navigate_natural_order(std::rbegin(_entries), std::rend(_entries), nt, w, center);
        if (res != nullptr)
            return res;
    }
    else
    {
        int const y = find_y_index(std::begin(_y_offsets), std::end(_y_offsets), center, _size);
        int const x = find_x_index(std::begin(_x_offsets), std::end(_x_offsets), center, _size);

        if (nt == navigation_type::NEXT_X)
        {
            for (int p = x; p < _size.w; ++p)
            {
                int eidx = _grid[p][y];
                if (eidx != -1)
                {
                    auto & e = _entries[eidx];
                    widget * found_widget = e.wptr.get();

                    if (found_widget != w)
                    {
                        widget * selectable_widget = found_widget->find_selectable(nt, center);
                        if (selectable_widget != nullptr)
                            return selectable_widget;
                    }
                }
            }
        }
        else if (nt == navigation_type::PREV_X)
        {
            for (int p = x; p >= 0; --p)
            {
                int eidx = _grid[p][y];
                if (eidx != -1)
                {
                    auto & e = _entries[eidx];
                    widget * found_widget = e.wptr.get();

                    if (found_widget != w)
                    {
                        widget * selectable_widget = found_widget->find_selectable(nt, center);
                        if (selectable_widget != nullptr)
                            return selectable_widget;
                    }
                }
            }
        }
        else if (nt == navigation_type::NEXT_Y)
        {
            for (int p = y; p < _size.h; ++p)
            {
                int eidx = _grid[x][p];
                if (eidx != -1)
                {
                    auto & e = _entries[eidx];
                    widget * found_widget = e.wptr.get();

                    if (found_widget != w)
                    {
                        widget * selectable_widget = found_widget->find_selectable(nt, center);
                        if (selectable_widget != nullptr)
                            return selectable_widget;
                    }
                }
            }
        }
        else //if (nt == navigation_type::PREV_Y)
        {
            for (int p = y; p >= 0; --p)
            {
                int eidx = _grid[x][p];
                if (eidx != -1)
                {
                    auto & e = _entries[eidx];
                    widget * found_widget = e.wptr.get();

                    if (found_widget != w)
                    {
                        widget * selectable_widget = found_widget->find_selectable(nt, center);
                        if (selectable_widget != nullptr)
                            return selectable_widget;
                    }
                }
            }
        }
    }

    return navigate_selectable_parent(nt, center);
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

