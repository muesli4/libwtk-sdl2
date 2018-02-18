#include "mouse_tracker.hpp"

void mouse_tracker::mouse_down(point p)
{
    _opt_down_pos = p;
}

mouse_up_event mouse_tracker::mouse_up(point p)
{
    if (_opt_down_pos.has_value())
    {
        auto down_p = _opt_down_pos.value();
        auto length = p - down_p;

        return mouse_up_event { p, std::make_optional(mouse_movement { down_p, length }) };

        _opt_down_pos.reset();
    }
    else
    {
        return mouse_up_event { p, std::nullopt };
    }
}
