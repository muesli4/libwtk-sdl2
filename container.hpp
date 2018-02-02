#ifndef LINEAR_CONTAINER_HPP
#define LINEAR_CONTAINER_HPP

#include <vector>

#include "widget.hpp"

struct container : widget
{
    container(std::initializer_list<widget_ptr> ws);
    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_up_event(mouse_up_event const & me) override;
    void on_mouse_down_event(mouse_down_event const & me) override;
    ~container() override;


    void apply_layout_to_children() override = 0;

    widget * find_selectable(navigation_type nt) override = 0;

    protected:

    std::vector<widget_ptr> _children;
};

#endif

