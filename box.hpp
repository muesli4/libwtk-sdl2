#ifndef BOX_HPP
#define BOX_HPP

#include "container.hpp"

struct box : container
{
    enum class orientation { VERTICAL, HORIZONTAL };

    box(orientation o, std::initializer_list<widget_ptr> ws);

    void apply_layout_to_children() override;

    widget * find_selectable(navigation_type nt) override;

    static bool is_orthogonal(navigation_type nt, orientation o);
    static bool is_forward(navigation_type nt);

    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override;

    ~box() override;

    private:

    orientation _o;

};


#endif
