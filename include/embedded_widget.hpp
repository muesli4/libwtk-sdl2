#ifndef LIBWTK_SDL2_EMBEDDED_WIDGET_HPP
#define LIBWTK_SDL2_EMBEDDED_WIDGET_HPP

#include <utility>

#include "widget.hpp"

/**
 * Embeds a widget without sharing details of the concrete type to the outside
 * while forwarding its widget interface.
 */
template <typename BaseWidget>
struct embedded_widget : widget
{
    /**
     * Construct the embedded widget by forwarding the given argument to it.
     */
    template <typename... Args>
    embedded_widget(Args &&... args)
        : _embedded_widget(std::forward<Args>(args)...)
    {
        _embedded_widget.set_parent(this);
    }

    ~embedded_widget() override {}

    void on_draw(draw_context & dc, selection_context const & sc) const override { _embedded_widget.on_draw(dc, sc); }
    void on_mouse_up_event(mouse_up_event const & e) override { _embedded_widget.on_mouse_up_event(e); }
    void on_mouse_down_event(mouse_down_event const & e) override { _embedded_widget.on_mouse_down_event(e); }
    void on_mouse_move_event(mouse_move_event const & e) override { _embedded_widget.on_mouse_move_event(e); }
    void on_key_event(key_event const & e) override { _embedded_widget.on_key_event(e); }
    void on_activate() override { _embedded_widget.on_activate(); }
    std::vector<widget *> get_children() override { return { &_embedded_widget }; }
    std::vector<widget const *> get_children() const override { return { &_embedded_widget }; }
    void on_box_allocated() override { _embedded_widget.apply_layout(get_box()); }
    widget * find_selectable(navigation_type nt, point center) override { return _embedded_widget.find_selectable(nt, center); }
    widget * navigate_selectable(navigation_type nt, point center) override { return _embedded_widget.navigate_selectable(nt, center); }
    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override { return navigate_selectable_parent(nt, center); }
    size_hint get_size_hint(int width, int height) const override { return _embedded_widget.get_size_hint(width, height); }
    virtual bool can_use_intermediate_size() const { return _embedded_widget.can_use_intermediate_size(); }

    protected:

    BaseWidget _embedded_widget;
};

#endif

