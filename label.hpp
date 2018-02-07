#ifndef LABEL_HPP
#define LABEL_HPP

// TODO different text sizes (will require changes to the font system)
// TODO tiny DSEL for markup and font size

#include "widget.hpp"

struct label : widget
{
    label(std::string text = std::string());
    ~label() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    vec min_size_hint() const override;

    // label interface

    void set_text(std::string text);
    std::string const & get_text();

    private:

    std::string _text;
};

#endif

