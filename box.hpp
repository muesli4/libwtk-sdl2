#ifndef BOX_HPP
#define BOX_HPP

#include <vector>

#include "container.hpp"

struct box : container
{
    struct child
    {
        bool expand;
        widget_ptr wptr;
    };

    enum class orientation { VERTICAL, HORIZONTAL };

    box(orientation o, std::vector<child> children, int children_spacing = 0, bool children_homogeneous = true);

    void apply_layout_to_children() override;

    widget * find_selectable(navigation_type nt, point center) override;

    static bool is_orthogonal(navigation_type nt, orientation o);
    static bool is_forward(navigation_type nt);

    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override;

    vec min_size_hint() const override;

    ~box() override;

    // box interface

    std::vector<widget *> get_children() override;
    std::vector<widget const *> get_children() const override;

    private:


    std::vector<child> _children;
    int _children_spacing;
    bool _children_homogeneous;

    orientation _o;

};

widget_ptr hbox(std::vector<box::child> children, int children_spacing = 0, bool children_homogeneous = true);
widget_ptr vbox(std::vector<box::child> children, int children_spacing = 0, bool children_homogeneous = true);

#endif
