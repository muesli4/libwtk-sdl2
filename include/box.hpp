#ifndef LIBWTK_SDL2_BOX_HPP
#define LIBWTK_SDL2_BOX_HPP

#include <vector>

#include "container.hpp"

struct box : container
{
    struct child
    {
        // Determines whether this child gets additional space when available.
        // Additional space will be distributed when every widget has reached
        // its natural size, otherwise natural size is distributed equally among
        // all widgets.
        bool expand;
        widget_ptr wptr;
    };

    typedef std::vector<child> children_type;

    enum class orientation { VERTICAL, HORIZONTAL };

    box(orientation o, children_type children);
    box(orientation o, children_type children, int children_spacing);
    box(orientation o, children_type children, bool children_homogeneous);
    box(orientation o, children_type children, int children_spacing, bool children_homogeneous);

    void apply_layout_to_children() override;

    widget * find_selectable(navigation_type nt, point center) override;

    static bool is_orthogonal(navigation_type nt, orientation o);
    static bool is_forward(navigation_type nt);

    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override;

    vec min_size_hint() const override;
    vec nat_size_inc_hint() const override;

    ~box() override;

    // box interface

    std::vector<widget *> get_children() override;
    std::vector<widget const *> get_children() const override;

    private:

    // Finds the relevant distance based on the containers orientation.
    int relevant_distance(point old_center, point new_center) const;

    // Check for orthogonality to container orientation.
    bool is_orthogonal(navigation_type nt) const;


    children_type _children;
    int _children_spacing;
    bool _children_homogeneous;

    orientation _o;

};

widget_ptr hbox(box::children_type children);
widget_ptr vbox(box::children_type children);

widget_ptr hbox(box::children_type children, int children_spacing);
widget_ptr vbox(box::children_type children, int children_spacing);

widget_ptr hbox(box::children_type children, bool children_homogeneous);
widget_ptr vbox(box::children_type children, bool children_homogeneous);

widget_ptr hbox(box::children_type children, int children_spacing, bool children_homogeneous);
widget_ptr vbox(box::children_type children, int children_spacing, bool children_homogeneous);

#endif
