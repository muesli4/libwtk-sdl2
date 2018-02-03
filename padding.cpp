#include "padding.hpp"

padding::padding(int pad, widget_ptr wptr) : padding(pad, pad, wptr)
{
}

padding::padding(int pad_x, int pad_y, widget_ptr wptr)
    : _pad_x(pad_x)
    , _pad_y(pad_y)
    , _wptr(wptr)
{
    wptr->set_parent(this);
}

void padding::on_draw(draw_context & dc, selection_context const & sc) const
{
    // we don't have to draw the background because either the parent widget
    // will or someone else is responsible when we're the root widget
    _wptr->on_draw(dc, sc);
}

void padding::on_mouse_down_event(mouse_down_event const & me)
{
    _wptr->on_mouse_down_event(me);
}

void padding::on_mouse_up_event(mouse_up_event const & me)
{
    _wptr->on_mouse_up_event(me);
}

void padding::on_key_event(key_event const & ke)
{
    _wptr->on_key_event(ke);
}

void padding::apply_layout_to_children()
{
    _wptr->apply_layout({ _box.x + _pad_x, _box.y + _pad_y, _box.w - 2 * _pad_x, _box.h - 2 * _pad_y });
}

widget * padding::find_selectable(navigation_type nt)
{
    return _wptr->find_selectable(nt);
}

widget * padding::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    return navigate_selectable_parent(nt, center);
}

std::shared_ptr<padding> pad(int pad_x, int pad_y, widget_ptr wptr)
{
    return std::make_shared<padding>(pad_x, pad_y, wptr);
}

std::shared_ptr<padding> pad(int pad, widget_ptr wptr)
{
    return std::make_shared<padding>(pad, wptr);
}

padding::~padding()
{
}
