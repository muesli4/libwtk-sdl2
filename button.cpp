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
    dc.draw_button_box(_box, _pressed, sc.is_selected_widget(this));
    if (!_text.empty())
    {
        dc.draw_button_text(_box, _text);
    }
}

void button::on_mouse_down_event(mouse_down_event const & e)
{
    bool hit = within_rect(e.position, _box);
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
        bool hit = within_rect(e.position, _box);
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

button::~button()
{
}
