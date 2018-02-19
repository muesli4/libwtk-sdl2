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

void widget::on_mouse_move_event(mouse_move_event const & e)
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

void widget::set_context_info(context_info const & ci)
{
    _context_info = &ci;
}

context_info const & widget::get_context_info() const
{
    return *_context_info;
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

    // If we can't assing enough space for a widget make at least a sane box.
    _box.w = std::max(0, _box.w);
    _box.h = std::max(0, _box.h);

    apply_layout_to_children();
}

void widget::apply_layout_to_children()
{
}

widget * widget::find_selectable(navigation_type nt, point center)
{
    // Default implementation: Widgets are not selectable by default.
    return nullptr;
}

widget * widget::navigate_selectable(navigation_type nt, point center)
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

vec widget::nat_size_inc_hint() const
{
    // Default implemtation: The widgets minimal size is its natural size.
    return { 0, 0 };
}

int widget::height_for_width_hint(int width) const
{
    // Default implementation: Not supported.
    return -1;
}

widget::~widget()
{
}

std::optional<swipe_info> widget::get_swipe_info_with_context_info(mouse_up_event const & e)
{
    if (e.opt_movement.has_value())
    {
        auto const & m = e.opt_movement.value();
        if (within_rect(m.origin, _box))
            return get_swipe_info(m, _context_info->swipe_cfg);
    }

    return std::nullopt;
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

