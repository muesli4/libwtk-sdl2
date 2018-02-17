#ifndef LIBWTK_SDL2_BUTTON_HPP
#define LIBWTK_SDL2_BUTTON_HPP

#include <functional>

#include "selectable.hpp"

struct button : selectable
{
    button(std::string text, std::function<void()> callback);

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_up_event(mouse_up_event const & e) override;
    void on_mouse_down_event(mouse_down_event const & e) override;
    void on_key_event(key_event const & e) override;
    void on_activate() override;
    vec min_size_hint() const override;
    vec nat_size_inc_hint() const override;

    ~button() override;

    // button interface

    void set_label(std::string text);

    private:

    bool _pressed;
    std::string _text;
    std::function<void()> _callback;
};


#endif

