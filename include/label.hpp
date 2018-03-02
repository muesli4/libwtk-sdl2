#ifndef LIBWTK_SDL2_LABEL_HPP
#define LIBWTK_SDL2_LABEL_HPP

// TODO tiny DSEL for markup and font size
// TODO sum-of-product type would be better suited for paragraph

#include "widget.hpp"

/**
 * A type to pass a paragraph with trailing newlines to a label. A paragraph
 * can use a different font and trailing newlines will use that fonts height.
 */
struct paragraph
{
    paragraph(std::string text, int trailing_newlines, int font_idx);
    paragraph(std::string text, int trailing_newlines);
    paragraph(std::string text);
    paragraph() = default;

    // TODO add font style, etc.
    std::string text;
    int trailing_newlines;
    int font_idx;
};

/**
 * A widget to display (wrapped) text. 
 */
struct label : widget
{
    label(std::vector<paragraph> content = std::vector<paragraph>());
    label(std::string text);
    ~label() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;

    size_hint get_size_hint(int width, int height) const override;

    /**
     * @name Label Interface
     * @{
     */

    /**
     * Set text by parsing a string for paragraphs.
     */
    void set_text(std::string text);

    std::string get_text() const;

    /**
     * Set the content of a label by giving the paragraphs directly.
     */
    void set_content(std::vector<paragraph> content);

    std::vector<paragraph> const & get_content() const;

    /**
     * Set the maximum text width in pixels.
     */
    void set_minimum_width(int width);
    void set_maximum_width(int width);

    /**
     * Set whether a label should use multiline text. This works especially well
     * in containers that know their width (e.g., a vertical box).
     */
    void set_wrap(bool wrap);

    /** @} */

    private:

    std::vector<paragraph> _content;

    int _minimum_width;
    int _maximum_width;
    bool _wrap;
};

#endif

