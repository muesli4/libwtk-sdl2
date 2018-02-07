#include "widget.hpp"

widget::widget()
    : _dirty(true)
    , _parent(nullptr)
{
}

void widget::on_mouse_down_event(mouse_down_event const & e)
{
    // Default implementation: Events are ignored.
}

void widget::on_mouse_up_event(mouse_up_event const & e)
{
}

void widget::on_key_event(key_event const & e)
{
}

void widget::on_activate()
{
}

void widget::draw_when_dirty(draw_context & dc, selection_context const & sc)
{
    if (_dirty)
    {
        on_draw(dc, sc);
        _dirty = false;
    }
}

void widget::mark_dirty()
{
    _dirty = true;
    // TODO add handler for parent to ignore (e.g. when widget is not visible)
    if (_parent != nullptr)
        _parent->mark_dirty();
}

SDL_Rect const & widget::get_box() const
{
    return _box;
}

void widget::set_layout_info(layout_info const & li)
{
    _layout_info = &li;
}

layout_info const & widget::get_layout_info() const
{
    return *_layout_info;
}

void widget::set_parent(widget * parent)
{
    _parent = parent;
}

std::vector<widget *> widget::get_children()
{
    return {};
}

std::vector<widget const *> widget::get_children() const
{
    return {};
}

void widget::apply_layout(SDL_Rect box)
{
    _box = box;
    apply_layout_to_children();
}

void widget::apply_layout_to_children()
{
}

widget * widget::find_selectable(navigation_type nt)
{
    // Default implementation: Widgets are not selectable by default.
    return nullptr;
}

widget * widget::navigate_selectable(navigation_type nt)
{
    // Default implementation: Not navigateable by default.
    return nullptr;
}

widget * widget::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    // Default implementation: Has no child widgets, therefore this will never
    // be called from a child.
    return nullptr;
}

widget::~widget()
{
}

widget * widget::navigate_selectable_parent(navigation_type nt, point center)
{
    if (_parent == nullptr)
        return nullptr;
    else
        return _parent->navigate_selectable_from_children(nt, this, center);
}

void widget::on_select()
{
}

void widget::on_unselect()
{
}

