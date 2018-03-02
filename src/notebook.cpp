#include "notebook.hpp"

notebook::notebook(std::vector<widget_ptr> pages)
    : _pages(pages)
    , _current_page_index(0)
{
    // TODO enforce invariant that _current_page_index is always valid? zero elements ok?
    for (auto p : _pages)
        p->set_parent(this);
}

notebook::~notebook()
{
}

void notebook::on_child_dirty(widget * w)
{
    if (w == get_shown_widget())
        mark_child_dirty(w);
}

std::vector<widget *> notebook::get_visible_children()
{
    return { get_shown_widget() };
}

std::vector<widget const *> notebook::get_visible_children() const
{
    return { get_shown_widget() };
}

void notebook::on_draw(draw_context & dc, selection_context const & sc) const
{
    dc.draw_background(get_box());
    get_shown_widget()->on_draw(dc, sc);
}

void notebook::on_mouse_up_event(mouse_up_event const & e)
{
    get_shown_widget()->on_mouse_up_event(e);
}

void notebook::on_mouse_down_event(mouse_down_event const & e)
{
    get_shown_widget()->on_mouse_down_event(e);
}

void notebook::on_box_allocated()
{
    for (auto p : _pages)
        p->apply_layout(get_box());
}

widget * notebook::find_selectable(navigation_type nt, point center)
{
    return get_shown_widget()->find_selectable(nt, center);
}

widget * notebook::navigate_selectable(navigation_type nt, point center)
{
    return get_shown_widget()->navigate_selectable(nt, center);
}

widget * notebook::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    return navigate_selectable_parent(nt, center);
}

widget * notebook::get_shown_widget()
{
    return _pages[_current_page_index].get();
}

widget const * notebook::get_shown_widget() const
{
    return _pages[_current_page_index].get();
}

std::size_t notebook::get_page() const
{
    return _current_page_index;
}

void notebook::set_page(std::size_t index)
{
    if (index < _pages.size())
        _current_page_index = index;
    mark_dirty();
}

std::vector<widget *> notebook::get_children()
{
    std::vector<widget *> res;
    for (auto & ptr : _pages)
    {
        res.push_back(ptr.get());
    }
    return res;
}

size_hint notebook::get_size_hint(int width, int height) const
{
    vec minimal = { 0, 0 };
    vec natural = { 0, 0 };
    for (auto p : _pages)
    {
        auto sh = p->get_size_hint(width, height);
        minimal.w = std::max(minimal.w, sh.minimal.w);
        minimal.h = std::max(minimal.h, sh.minimal.h);
        natural.w = std::max(natural.w, sh.natural.w);
        natural.h = std::max(natural.h, sh.natural.h);
    }
    return size_hint(minimal, natural);
}

