#include "swipe_detector.hpp"

swipe_detector::swipe_detector(int press_threshold, double dir_unambig_factor)
    : _press_threshold(press_threshold)
    , _dir_unambig_factor(dir_unambig_factor)
{
}

void swipe_detector::mouse_down(point p)
{
    _opt_down_pos = p;
}

mouse_up_event swipe_detector::mouse_up(point p)
{
    if (_opt_down_pos.has_value())
    {
        auto down_p = _opt_down_pos.value();
        _opt_down_pos.reset();
        auto swipe_vec = down_p - p;
        auto abs_swipe_vec = abs(swipe_vec);

        swipe_action act;

        // vertical swipe
        if (abs_swipe_vec.h * _dir_unambig_factor >= abs_swipe_vec.w)
        {
            if (swipe_vec.h < 0)
            {
                act = swipe_action::UP;
            }
            else
            {
                act = swipe_action::DOWN;
            }
        }
        // horizontal swipe
        else if (abs_swipe_vec.w * _dir_unambig_factor >= abs_swipe_vec.h)
        {
            if (swipe_vec.w > 0)
            {
                act = swipe_action::RIGHT;
            }
            else
            {
                act = swipe_action::LEFT;
            }
        }
        else
        {
            goto return_no_swipe;
        }

        if (_press_threshold < abs_swipe_vec.w || _press_threshold < abs_swipe_vec.h)
        {
            swipe_event se { .position = down_p, .length = swipe_vec, .action = act };
            return mouse_up_event { p, std::optional<swipe_event>(se) };
        }
    }

return_no_swipe:
    return { p, std::nullopt };
}

