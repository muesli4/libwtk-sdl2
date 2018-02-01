#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <functional>

#include "widget.hpp"

struct button : widget
{
    button(std::string text, std::function<void()> callback);

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_event(mouse_event const & e) override;
    void on_key_event(key_event const & e) override;
    void on_activate() override;
    widget * find_selectable() override;
    widget * navigate_selectable(navigation_type nt) override;
    ~button() override;

    private:

    bool _pressed;
    std::string _text;
    std::function<void()> _callback;
};


#endif

