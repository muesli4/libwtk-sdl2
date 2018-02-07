#include "swipe_area.hpp"

swipe_area::swipe_area(std::function<void(swipe_action)> swipe_callback, std::function<void()> press_callback)
    : _swipe_callback(swipe_callback)
    , _press_callback(press_callback)
{
}

swipe_area::~swipe_area()
{
}

/*
void swipe_area::on_mouse_down_event(mouse_down_event const & e)
{
}
*/

void swipe_area::on_mouse_up_event(mouse_up_event const & e)
{
    if (e.opt_swipe_event.has_value())
    {
        auto se = e.opt_swipe_event.value();

        if (within_rect(se.position, get_box()))
            _swipe_callback(se.action);
    }
    else if (within_rect(e.position, get_box()))
    {
        _press_callback();
    }
}

vec swipe_area::min_size_hint() const
{
    // TODO how to dimension ?
    int v = get_layout_info().font_line_skip();
    return { v, v };
}

