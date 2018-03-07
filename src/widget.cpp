#include "widget.hpp"

dirty_type combine(dirty_type a, dirty_type b)
{
    return std::max(a, b);
}

size_hint::size_hint(vec min, vec nat)
    : minimal(min)
    , natural(nat)
{
}

size_hint::size_hint(vec min)
    : size_hint(min, min)
{
}

widget::widget()
    : _dirty(dirty_type::DIRTY)
    , _parent(nullptr)
{
}

widget::~widget()
{
}

void widget::on_child_dirty(widget * child)
{
    mark_child_dirty(child);
}

std::vector<widget *> widget::get_visible_children()
{
    return get_children();
}

std::vector<widget const *> widget::get_visible_children() const
{
    return get_children();
}

void widget::clear_dirty()
{
    // Assumption: If the parent is not marked as dirty, neither is any child.
    if (_dirty != dirty_type::CLEAN)
    {
        _dirty = dirty_type::CLEAN;

        // Invisible widgets are not considered.
        for (widget * w : get_visible_children())
        {
            w->clear_dirty();
        }
    }
}

void widget::mark_dirty()
{
    _dirty = dirty_type::DIRTY;
    notify_parent_child_dirty();
}

void widget::mark_child_dirty(widget * child)
{
    _dirty = combine(_dirty, dirty_type::CHILD_DIRTY);
    notify_parent_child_dirty();
}

void widget::draw(draw_context & dc, selection_context const & sc) const
{
    on_draw(dc, sc);
    auto const & vcs = get_visible_children();
    // Draw in reversed Z-order.
    for (auto it = std::crbegin(vcs); it != std::crend(vcs); ++it)
    {
        (*it)->draw(dc, sc);
    }
}

// Draws only dirty widgets.
void widget::draw_dirty(draw_context & dc, selection_context const & sc) const
{
    // If dirty do a complete redraw of the subtree.
    if (_dirty == dirty_type::DIRTY)
        draw(dc, sc);
    else if (_dirty == dirty_type::CHILD_DIRTY)
    {
        // TODO Current assumption: child box hasn't changed so redrawing
        // background is not necessary. This may change in the future and then
        // the dirty areas from children are necessary.
        auto const & vcs = get_visible_children();
        for (auto it = std::crbegin(vcs); it != std::crend(vcs); ++it)
        {
            (*it)->draw_dirty(dc, sc);
        }
    }
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

std::optional<swipe_info> widget::get_swipe_info_with_context_info(mouse_up_event const & e)
{
    if (e.opt_movement.has_value())
    {
        auto const & m = e.opt_movement.value();
        if (within_rect(m.origin, get_box()))
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

void widget::notify_parent_child_dirty()
{
    if (_parent != nullptr)
        _parent->on_child_dirty(this);
}

int opt_min(int opt_length, int length)
{
    return opt_length < 0 ? length : std::min(opt_length, length);
}

int opt_max(int opt_length, int length)
{
    return opt_length < 0 ? length : std::max(opt_length, length);
}

int opt_or_value(int opt_length, int length)
{
    return opt_length < 0 ? length : opt_length;
}

int opt_change(int opt_length, int length)
{
    return opt_length < 0 ? opt_length : length;
}

