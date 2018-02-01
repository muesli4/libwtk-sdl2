#ifndef LINEAR_CONTAINER_HPP
#define LINEAR_CONTAINER_HPP

#include <vector>

#include "widget.hpp"

struct linear_container : widget
{
    linear_container(std::initializer_list<widget_ptr> ws);
    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_event(mouse_event const & me) override;
    ~linear_container() override;

    protected:

    std::vector<widget_ptr> _children;
};

#endif

