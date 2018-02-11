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
    // Trailing newlines are not ignored.
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
    : label(parse_text_fragments(text))
{
}

label::label(std::vector<paragraph> content)
    : _content(content)
    , _cached_height(-1)
{
}

label::~label()
{
}

void label::on_draw(draw_context & dc, selection_context const & sc) const
{
    int const font_height = get_layout_info().font_line_skip();

    auto real_box = pad_rect(get_box(), font_height / 2);
    // TODO refactor centered property
    // If the real box is bigger now, the text should be centered.
    int yoffset = real_box.y + (real_box.h - (height_for_width_hint(get_box().w) - font_height)) / 2;
    int remaining_height = real_box.h;

    // clear background
    dc.draw_background(get_box());
#ifdef DEBUG_LAYOUT
    dc.set_color({255, 0, 0});
    dc.draw_rect(get_box());
#endif

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
        // Assuming the average character is more than twice as high as it is
        // wide, this will allow about 45 characters on the line, which should
        // be a reasonable minimum for the width.
        vec psize = get_layout_info().text_size(tf.text, 13 * get_layout_info().font_height());

        size.w = std::max(psize.w, size.w);
        size.h += psize.h;
    }
    return vec{ font_height, font_height } + size;
}

int label::height_for_width_hint(int width) const
{
    if (_cached_height == -1)
        _cached_height = calculate_height_for_width(width);
    return _cached_height;
}

// label interface

void label::set_text(std::string text)
{
    set_content(parse_text_fragments(text));
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
    _cached_height = -1;

    mark_dirty();
}

std::vector<paragraph> const & label::get_content() const
{
    return _content;
}

int label::calculate_height_for_width(int width) const
{
    int const font_height = get_layout_info().font_line_skip();
    int const real_width = width - font_height;

    int height = 0;
    for (auto const & tf : _content)
    {
        height += get_layout_info().text_size(tf.text, real_width).h + tf.trailing_newlines * font_height;
    }
    return height + font_height;
}

