#include "notebook.hpp"

notebook::notebook(std::initializer_list<widget_ptr> pages)
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

void notebook::on_draw(draw_context & dc, selection_context const & sc) const
{
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

void notebook::apply_layout_to_children()
{
    for (auto p : _pages)
        p->apply_layout(get_box());
}

widget * notebook::find_selectable(navigation_type nt)
{
    return get_shown_widget()->find_selectable(nt);
}

widget * notebook::navigate_selectable(navigation_type nt)
{
    return get_shown_widget()->navigate_selectable(nt);
}

widget * notebook::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    return navigate_selectable_parent(nt, center);
}

// notebook interface
widget_ptr notebook::get_shown_widget()
{
    return _pages[_current_page_index];
}

widget_ptr const notebook::get_shown_widget() const
{
    return _pages[_current_page_index];
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

vec notebook::min_size_hint() const
{
    vec max_vec;
    for (auto p : _pages)
    {
        vec v = p->min_size_hint();
        max_vec.w = std::max(max_vec.w, v.w);
        max_vec.h = std::max(max_vec.h, v.h);

    }
    return max_vec;
}
