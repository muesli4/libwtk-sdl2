#ifndef LIBWTK_SDL2_TABLE_HPP
#define LIBWTK_SDL2_TABLE_HPP

#include "container.hpp"

#include <vector>

/**
 * Use a grid to attach several widgets with a position to it. \ref grid::entry
 * is used for that purpose and allows the widget to span over a rectangular
 * area.
 *
 * Note that unused rows and columns will still use the given spacing.
 */
struct grid : container
{
    struct entry
    {
        SDL_Rect placement;
        widget_ptr wptr;
    };

    grid(vec size, std::vector<entry> entries, int padding);
    ~grid() override;

    std::vector<widget *> get_children() override;
    std::vector<widget const *> get_children() const override;
    void on_box_allocated() override;
    widget * find_selectable(navigation_type nt, point center) override;
    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override;

    size_hint get_size_hint(int width, int height) const override;

    private:

    int length_with_spacing(std::vector<int> const & lengths) const;
    void min_cell_dimensions(int * min_widths, int * min_heights) const;
    void compute_offsets(std::vector<int> & lengths, std::vector<int> & offsets, int n, int box_length, int box_start);

    std::vector<entry> _entries;
    vec _size;
    std::vector<std::vector<int>> _grid;
    int _spacing;

    std::vector<int> _x_offsets;
    std::vector<int> _y_offsets;

};

#endif

