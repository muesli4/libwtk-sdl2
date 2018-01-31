#include "button.hpp"

#include "geometry.hpp"

button::button(std::string text, std::function<void()> callback)
    : _pressed(false)
    , _text(text)
    , _callback(callback)
{
}

void button::on_draw(draw_context & dc) const
{
    //std::cout << _pressed << std::endl;
    dc.draw_button_box(_box, _pressed);
    if (!_text.empty())
    {
        dc.draw_button_text(_box, _text);
    }
}

void button::on_mouse_event(mouse_event const & e)
{
    //std::cout << "mouse event " << within_rect(e.x, e.y, _box) << ' ' << _box.x << ' ' << _box.y << std::endl;
    bool hit = within_rect(e.x, e.y, _box);
    bool old_pressed = _pressed;
    _pressed = hit && e.type == mouse_event::button_type::DOWN;
    if (hit && old_pressed == true && !_pressed)
        _callback();

    // TODO
    mark_dirty();
}

void button::on_key_event(key_event const & e)
{
    // TODO enter?
}
