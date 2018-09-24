#include "text_button.hpp"

text_button::text_button(std::string text, std::function<void()> callback)
    : button(callback)
    , _text(text)
{
}

text_button::~text_button()
{
}

vec text_button::get_drawable_size() const
{
    return get_context_info().text_size(_text);
}

void text_button::draw_drawable(draw_context & dc, rect box) const
{
    if (!_text.empty())
    {
        dc.draw_button_text(_text, get_box());
    }
}

void text_button::set_label(std::string text)
{
    _text = text;
    mark_dirty();
}

