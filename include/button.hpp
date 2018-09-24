#ifndef LIBWTK_SDL2_BUTTON_HPP
#define LIBWTK_SDL2_BUTTON_HPP

#include <functional>

#include "selectable.hpp"

struct button : selectable
{
    button(std::function<void()> callback);

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_up_event(mouse_up_event const & e) override;
    void on_mouse_down_event(mouse_down_event const & e) override;
    void on_key_event(key_event const & e) override;
    void on_activate() override;
    size_hint get_size_hint(int width, int height) const override;

    ~button() override;

    protected:

    virtual void draw_drawable(draw_context & dc, rect box) const = 0;
    virtual vec get_drawable_size() const = 0;

    private:

    bool _pressed;
    std::function<void()> _callback;
};


#endif

