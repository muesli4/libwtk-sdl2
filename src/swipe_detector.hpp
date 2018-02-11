#ifndef SWIPE_DETECTOR_HPP
#define SWIPE_DETECTOR_HPP

#include <optional>

#include "geometry.hpp"
#include "mouse_event.hpp"

struct swipe_detector
{
    swipe_detector(int press_threshold, double dir_unambig_factor);

    void mouse_down(point p);
    mouse_up_event mouse_up(point p);

    private:

    int _press_threshold;
    double _dir_unambig_factor;
    std::optional<point> _opt_down_pos;
};

#endif

