#ifndef LIBWTK_SDL2_TABLE_HPP
#define LIBWTK_SDL2_TABLE_HPP

#include "container.hpp"

#include <vector>

struct table : container
{
    struct entry
    {
        SDL_Rect placement;
        widget_ptr wptr;
    };

    table(vec size, std::vector<entry> entries, int padding);
    ~table() override;

    std::vector<widget *> get_children() override;
    std::vector<widget const *> get_children() const override;
    void apply_layout_to_children() override;
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

