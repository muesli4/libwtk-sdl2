#include "swipe_area.hpp"

swipe_area::swipe_area(std::function<void(swipe_direction)> swipe_callback, std::function<void()> press_callback)
    : _swipe_callback(swipe_callback)
    , _press_callback(press_callback)
{
}

swipe_area::~swipe_area()
{
}

void swipe_area::on_draw(draw_context & dc, selection_context const & sc) const
{
}

/*
void swipe_area::on_mouse_down_event(mouse_down_event const & e)
{
}
*/

void swipe_area::on_mouse_up_event(mouse_up_event const & e)
{
    auto opt_dir = get_swipe_direction_with_context_info(e);
    if (opt_dir.has_value())
    {
        _swipe_callback(opt_dir.value());
    }
    else if (within_rect(e.position, get_box()))
    {
        _press_callback();
    }
}

vec swipe_area::min_size_hint() const
{
    // TODO how to dimension ?
    int v = get_context_info().font_line_skip();
    return { v, v };
}

