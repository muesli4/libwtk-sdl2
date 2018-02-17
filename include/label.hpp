#ifndef LIBWTK_SDL2_LABEL_HPP
#define LIBWTK_SDL2_LABEL_HPP

// TODO different text sizes (will require changes to the font system)
// TODO tiny DSEL for markup and font size

#include "widget.hpp"

// TODO sum-of-product type would be better suited
struct paragraph
{
    paragraph(std::string text, int trailing_newlines);
    paragraph(std::string text);
    paragraph() = default;

    // TODO add font, style, etc.
    std::string text;
    int trailing_newlines;
};

struct label : widget
{
    label(std::vector<paragraph> content = std::vector<paragraph>());
    label(std::string text);
    ~label() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void apply_layout_to_children() override;
    vec min_size_hint() const override;
    int height_for_width_hint(int width) const override;

    // label interface

    void set_text(std::string text);
    std::string get_text() const;

    void set_content(std::vector<paragraph> content);
    std::vector<paragraph> const & get_content() const;

    private:

    int calculate_height_for_width(int width) const;

    std::vector<paragraph> _content;
    mutable int _cached_height;
};

#endif

