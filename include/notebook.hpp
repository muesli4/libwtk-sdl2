#ifndef LIBWTK_SDL2_NOTEBOOK_HPP
#define LIBWTK_SDL2_NOTEBOOK_HPP

#include <vector>

#include "widget.hpp"

// A container that has a widget on every page but only one page is visible at a
// time.
struct notebook : widget
{
    notebook(std::vector<widget_ptr> pages);
    ~notebook() override;

    void on_child_dirty(widget * w) override;
    std::vector<widget *> get_visible_children() override;
    std::vector<widget const *> get_visible_children() const override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_up_event(mouse_up_event const & e) override;
    void on_mouse_down_event(mouse_down_event const & e) override;

    void apply_layout_to_children() override;

    widget * find_selectable(navigation_type nt, point center) override;
    widget * navigate_selectable(navigation_type nt, point center) override;
    widget * navigate_selectable_from_children(navigation_type nt, widget * w, point center) override;

    std::vector<widget *> get_children() override;

    size_hint get_size_hint(int width, int height) const override;

    // notebook interface
    std::size_t get_page() const;
    void set_page(std::size_t index);

    private:

    widget * get_shown_widget();
    widget const * get_shown_widget() const;


    std::vector<widget_ptr> _pages;
    std::size_t _current_page_index;
};

#endif
