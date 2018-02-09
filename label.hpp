#ifndef LABEL_HPP
#define LABEL_HPP

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
    vec min_size_hint() const override;

    // label interface

    void set_text(std::string text);
    std::string get_text() const;

    void set_content(std::vector<paragraph> content);
    std::vector<paragraph> const & get_content() const;

    private:

    std::vector<paragraph> _content;
};

#endif

