#include "button.hpp"

#include "geometry.hpp"

button::button(std::string text, std::function<void()> callback)
    : _pressed(false)
    , _text(text)
    , _callback(callback)
{
}

void button::on_draw(draw_context & dc, selection_context const & sc) const
{
    dc.draw_button_box(get_box(), _pressed, sc.is_selected_widget(this));
    if (!_text.empty())
    {
        dc.draw_button_text(_text, get_box());
    }
}

void button::on_mouse_down_event(mouse_down_event const & e)
{
    bool hit = within_rect(e.position, get_box());
    if (hit != _pressed)
    {
        _pressed = hit;
        mark_dirty();
    }
}

void button::on_mouse_up_event(mouse_up_event const & e)
{
    if (_pressed)
    {
        bool hit = within_rect(e.position, get_box());
        if (hit)
            _callback();
        _pressed = false;
        mark_dirty();
    }
}

void button::on_key_event(key_event const & e)
{
    // TODO enter?
}

void button::on_activate()
{
    _callback();
}

vec button::min_size_hint() const
{
    int h_spacing = get_layout_info().font_line_skip();
    vec size = get_layout_info().text_size(_text);
    return { size.w + h_spacing, size.h + h_spacing };
}

button::~button()
{
}
