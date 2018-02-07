#include "label.hpp"
#include "sdl_util.hpp"

label::label(std::string text)
    : _text(text)
{
}

label::~label()
{
}

void label::on_draw(draw_context & dc, selection_context const & sc) const
{
    dc.draw_label_text(pad_rect(get_box(), get_layout_info().font_line_skip() / 2), _text);
}

vec label::min_size_hint() const
{
    int v = get_layout_info().font_line_skip();
    // TODO this is not working properly
    vec size { get_layout_info().text_minimum_width(_text) * 5, v * 5 };
    return vec{ v, v } + size;
}


// label interface

void label::set_text(std::string text)
{
    _text = text;
    mark_dirty();
}

std::string const & label::get_text()
{
    return _text;
}

