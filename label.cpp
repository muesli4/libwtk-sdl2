#include "label.hpp"
#include "sdl_util.hpp"

#include <sstream>

std::vector<paragraph> parse_text_fragments(std::string text)
{
    std::vector<paragraph> result;
    std::istringstream iss;
    iss.str(text);

    std::string prev_paragraph;
    int trailing_newlines = 0;

    for (std::string line; std::getline(iss, line);)
    {
        if (line.empty())
        {
            trailing_newlines++;
        }
        else
        {
            if (trailing_newlines != 0 || !prev_paragraph.empty())
            {
                result.emplace_back(prev_paragraph, trailing_newlines);
            }

            prev_paragraph.swap(line);
            trailing_newlines = 0;
        }
    }
    // TODO ignore last trailing newlines?
    result.emplace_back(prev_paragraph, trailing_newlines);

    return result;
}

paragraph::paragraph(std::string text, int trailing_newlines)
    : text(text)
    , trailing_newlines(trailing_newlines)
{
}

paragraph::paragraph(std::string text)
    : paragraph(text, 0)
{
}

label::label(std::string text)
    : _content(parse_text_fragments(text))
{
}

label::label(std::vector<paragraph> content)
    : _content(content)
{
}

label::~label()
{
}

void label::on_draw(draw_context & dc, selection_context const & sc) const
{
    int const font_height = get_layout_info().font_line_skip();

    auto real_box = pad_rect(get_box(), font_height / 2);
    int yoffset = real_box.y;
    int remaining_height = real_box.h;

    // clear background
    dc.draw_background(get_box());

    for (auto const & tf : _content)
    {
        int used_height = dc.draw_label_text({ real_box.x, yoffset, real_box.w, remaining_height}, tf.text) + tf.trailing_newlines * font_height;
        if (used_height > remaining_height)
            break;
        yoffset += used_height;
        remaining_height -= used_height;
    }
}

vec label::min_size_hint() const
{
    int font_height = get_layout_info().font_line_skip();
    vec size { 0, 0 };
    for (auto const & tf : _content)
    {
        // TODO this is not working properly
        size.w = std::max(get_layout_info().text_minimum_width(tf.text) * 5, size.w);
        size.h += font_height * 5 + tf.trailing_newlines * font_height;
    }
    return vec{ font_height, font_height } + size;
}

// label interface

void label::set_text(std::string text)
{
    _content = parse_text_fragments(text);

    mark_dirty();
}

std::string label::get_text() const
{
    std::string result;
    for (auto const & tf : _content)
    {
        result += tf.text;
        for (int i = 0; i < tf.trailing_newlines; i++)
        {
            // TODO windows compatibility
            result += "\n";
        }
    }
    return result;
}

void label::set_content(std::vector<paragraph> content)
{
    _content = content;

    mark_dirty();
}

std::vector<paragraph> const & label::get_content() const
{
    return _content;
}

