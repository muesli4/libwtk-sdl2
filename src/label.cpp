#include "label.hpp"
#include "util.hpp"

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

paragraph::paragraph(std::string text, int trailing_newlines, int font_idx)
    : text(text)
    , trailing_newlines(trailing_newlines)
    , font_idx(font_idx)
{
}

paragraph::paragraph(std::string text, int trailing_newlines)
    : paragraph(text, trailing_newlines, 0)
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
    , _minimum_width(-1)
    , _maximum_width(-1)
    , _wrap(false)
{
}

label::~label()
{
}

void label::on_draw(draw_context & dc, selection_context const & sc) const
{
    auto real_box = get_box();
    int yoffset = real_box.y;
    int remaining_height = real_box.h;

    // clear background
    dc.draw_background(get_box());

    for (auto const & tf : _content)
    {
        rect fbox { real_box.x, yoffset, opt_min(_maximum_width, real_box.w), remaining_height};

        // TODO which newline height?
        int used_height
            = dc.draw_label_text(fbox, tf.text, _wrap, tf.font_idx)
            + tf.trailing_newlines * get_context_info().font_line_skip(tf.font_idx);
        if (used_height > remaining_height)
            break;
        yoffset += used_height;
        remaining_height -= used_height;
    }
#ifdef DEBUG_LAYOUT
    dc.set_color({255, 0, 0});
    dc.draw_rect(get_box());
#endif
}

size_hint label::get_size_hint(int width, int height) const
{
    int wrap_param;

    if (_wrap)
    {
        // Assuming the average character is more than twice as high as it is
        // wide, this will allow about 45 characters on the line, which should
        // be a reasonable minimum for the width.
        //
        // Does not account for different font sizes.
        int const wrap_constant = 13 * get_context_info().font_line_skip();

        // If there is no target width use the wrap constant.
        int const targeted_width = opt_or_value(width, wrap_constant);

        // Limit the width by user given value.
        wrap_param = opt_min(_maximum_width, targeted_width);
    }
    else
    {
        wrap_param = -1;
    }

    vec minimal { 0, 0 };
    for (auto const & tf : _content)
    {
        vec psize = get_context_info().text_size(tf.text, wrap_param, tf.font_idx);

        minimal.w = std::max(psize.w, minimal.w);
        minimal.h += psize.h + tf.trailing_newlines * get_context_info().font_line_skip(tf.font_idx);
    }

    minimal.w = std::max(_minimum_width, minimal.w);

    return size_hint(minimal);
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
            result += NEWLINE_C_STR;
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

void label::set_minimum_width(int width)
{
    _minimum_width = width;
}

void label::set_maximum_width(int width)
{
    _maximum_width = width;
}

void label::set_wrap(bool wrap)
{
    _wrap = wrap;
}

