#include "widget.hpp"

void widget::draw_when_dirty(draw_context & dc)
{
    if (_dirty)
    {
        on_draw(dc);
        _dirty = false;
    }
}

void widget::mark_dirty()
{
    _dirty = true;
}

SDL_Rect const & widget::box() const
{
    return _box;
}

void widget::apply_layout(SDL_Rect box)
{
    _box = box;
}
