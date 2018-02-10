#ifndef TABLE_HPP
#define TABLE_HPP

#include "container.hpp"

struct table : container
{
    struct entry
    {
        SDL_Rect placement;
        widget_ptr wptr;
    };

    table(vec size, std::vector<entry> entries);
    ~table() override;

    std::vector<widget *> get_children() override;
    std::vector<widget const *> get_children() const override;
    void apply_layout_to_children() override;
    widget * find_selectable(navigation_type nt, point center) override;
    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override;

    vec min_size_hint() const override;

    private:

    int length_with_spacing(int * lengths, int n) const;
    void min_cell_dimensions(int * min_widths, int * min_heights) const;

    std::vector<entry> _entries;
    vec _size;
    std::vector<std::vector<int>> _grid;
    int _spacing;

};

#endif

