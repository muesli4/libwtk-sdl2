#include "container.hpp"

container::~container()
{
}

void container::on_draw(draw_context & dc, selection_context const & sc) const
{
    for (auto c : get_children())
    {
        c->on_draw(dc, sc);
    }
}

void container::on_mouse_down_event(mouse_down_event const & me)
{
    // TODO need logic to capture mouse moves outside of a widget
    for (auto c : get_children())
    {
        c->on_mouse_down_event(me);
    }
}

void container::on_mouse_up_event(mouse_up_event const & me)
{
    // TODO need logic to capture mouse moves outside of a widget
    for (auto c : get_children())
    {
        c->on_mouse_up_event(me);
    }
}

void container::init_children()
{
    // can't provide this as constructor
    for (auto c : get_children())
    {
        c->set_parent(this);
    }
}

