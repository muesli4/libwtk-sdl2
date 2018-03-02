#ifndef LIBWTK_SDL2_LABEL_HPP
#define LIBWTK_SDL2_LABEL_HPP

// TODO tiny DSEL for markup and font size

#include "widget.hpp"

// TODO sum-of-product type would be better suited
struct paragraph
{
    paragraph(std::string text, int trailing_newlines, int font_idx);
    paragraph(std::string text, int trailing_newlines);
    paragraph(std::string text);
    paragraph() = default;

    // TODO add font, style, etc.
    std::string text;
    int trailing_newlines;
    int font_idx;
};

struct label : widget
{
    label(std::vector<paragraph> content = std::vector<paragraph>());
    label(std::string text);
    ~label() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;

    size_hint get_size_hint(int width, int height) const override;

    // label interface

    void set_text(std::string text);
    std::string get_text() const;

    void set_content(std::vector<paragraph> content);
    std::vector<paragraph> const & get_content() const;

    void set_minimum_width(int width);
    void set_maximum_width(int width);
    void set_wrap(bool wrap);

    private:

    int calculate_height_for_width(int width) const;

    std::vector<paragraph> _content;

    int _minimum_width;
    int _maximum_width;
    bool _wrap;
};

//  no wrap: constant size
//  

#endif

