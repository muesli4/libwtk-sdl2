#include "bin.hpp"

bin::bin(widget_ptr child)
    : _child(child)
{
    _child->set_parent(this);
}

bin::~bin()
{
}

void bin::on_draw(draw_context & dc, selection_context const & sc) const
{
    // we don't have to draw the background because either the parent widget
    // will or someone else is responsible when we're the root widget
}

void bin::on_mouse_down_event(mouse_down_event const & me)
{
    _child->on_mouse_down_event(me);
}

void bin::on_mouse_up_event(mouse_up_event const & me)
{
    _child->on_mouse_up_event(me);
}

void bin::on_mouse_move_event(mouse_move_event const & e)
{
    _child->on_mouse_move_event(e);
}

widget * bin::find_selectable(navigation_type nt, point center)
{
    return _child->find_selectable(nt, center);
}

widget * bin::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    return navigate_selectable_parent(nt, center);
}

size_hint bin::get_size_hint(int width, int height) const
{
    return _child->get_size_hint(width, height);
}

std::vector<widget *> bin::get_children()
{
    return { _child.get() };
}

std::vector<widget const *> bin::get_children() const
{
    return { _child.get() };
}

