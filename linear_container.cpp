#include "linear_container.hpp"

linear_container::linear_container(std::initializer_list<widget_ptr> ws)
    : _children(ws)
{
    for (auto cptr : _children)
        cptr->set_parent(this);

    mark_dirty();
}

void linear_container::on_draw(draw_context & dc, selection_context const & sc) const
{
    for (auto cptr : _children)
        cptr->on_draw(dc, sc);
}

void linear_container::on_mouse_event(mouse_event const & me)
{
    // TODO need logic to capture mouse moves outside of a widget
    for (auto cptr : _children)
    {
        cptr->on_mouse_event(me);
    }
    mark_dirty();
}

linear_container::~linear_container()
{
}

