#include "container.hpp"

container::container(std::initializer_list<widget_ptr> ws)
    : _children(ws)
{
    for (auto cptr : _children)
        cptr->set_parent(this);

    mark_dirty();
}

void container::on_draw(draw_context & dc, selection_context const & sc) const
{
    for (auto cptr : _children)
        cptr->on_draw(dc, sc);
}

void container::on_mouse_event(mouse_event const & me)
{
    // TODO need logic to capture mouse moves outside of a widget
    for (auto cptr : _children)
    {
        cptr->on_mouse_event(me);
    }
    mark_dirty();
}

container::~container()
{
}

