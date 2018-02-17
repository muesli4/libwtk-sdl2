#ifndef LIBWTK_SDL2_SWIPE_AREA_HPP
#define LIBWTK_SDL2_SWIPE_AREA_HPP

#include <functional>

#include "widget.hpp"

struct swipe_area : widget
{
    swipe_area(std::function<void(swipe_action)> swipe_callback, std::function<void()> press_callback);
    ~swipe_area() override;

    // TODO remove
    void on_draw(draw_context & dc, selection_context const & sc) const override {}

    //void on_mouse_down_event(mouse_down_event const & e) override;
    void on_mouse_up_event(mouse_up_event const & e) override;

    vec min_size_hint() const override;

    private:

    bool _pressed;

    std::function<void(swipe_action)> _swipe_callback;
    std::function<void()> _press_callback;
};

#endif

