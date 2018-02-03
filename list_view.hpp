#ifndef LIST_VIEW_HPP
#define LIST_VIEW_HPP

#include <vector>
#include <functional>
#include <optional>

#include "selectable.hpp"

struct list_view : selectable
{
    list_view(draw_context const & dc, std::size_t position, std::size_t selected_position, std::size_t highlight_position, std::vector<std::string> const & values, std::function<void()> activate_callback);
    ~list_view() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_up_event(mouse_up_event const & e) override;
    void on_mouse_down_event(mouse_down_event const & e) override;
    void on_key_event(key_event const & e) override;
    void on_activate() override;

    private:

    std::optional<point> _opt_pressed_point;

    std::size_t _position;

    std::size_t _selected_position;

    std::size_t _highlight_position;

    // TODO more columns and other types
    std::vector<std::string> const & _values;

    std::function<void()> _activate_callback;

    // TODO this is a hack, since the draw_context is only available when drawing, need to fix later by changing the design somehow
    draw_context const & _dc;
};

#endif

