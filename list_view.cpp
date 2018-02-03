#include "list_view.hpp"
#include "util.hpp"
#include <iostream>

// TODO do bounds checking on positions
list_view::list_view(draw_context const & dc, std::size_t position, std::size_t selected_position, std::size_t highlight_position, std::vector<std::string> const & values, std::function<void()> activate_callback)
    : _opt_pressed_point{}
    , _position(position)
    , _selected_position(selected_position)
    , _highlight_position(highlight_position)
    , _values(values)
    , _activate_callback(activate_callback)
    , _dc(dc)
{
}

list_view::~list_view()
{
}

void list_view::on_draw(draw_context & dc, selection_context const & sc) const
{
    dc.draw_entry_box(_box);

    int const entry_height = static_cast<int>(dc.font_height());

    // the entry box has a border of 1 on each side TODO make more flexible
    int const x_offset = _box.x + 1;
    int const entry_width = _box.w - 2;
    int y_offset = _box.y + 1;
    std::size_t n = _position;
    int const y_end = _box.y + _box.h;
    while (n < _values.size() && y_offset < y_end)
    {
        int const overlap = (y_offset + entry_height) - y_end;
        int const entry_height_with_overlap = entry_height - /*(overlap < 0 ? 0 : overlap + 1);*/std::max(0, overlap + 1);

        SDL_Rect abs_rect { x_offset, y_offset, entry_width, entry_height_with_overlap};

        // favor pressed over active
        if (_opt_pressed_point.has_value() && within_rect(_opt_pressed_point.value(), abs_rect))
            dc.draw_entry_pressed_background(abs_rect);
        else if (_selected_position == n)
            dc.draw_entry_active_background(abs_rect);

        dc.draw_entry_text(_values[n], abs_rect);

        y_offset += dc.font_line_skip();
        n++;
    }

    int const visible_entries = (_box.h - 2) / dc.font_line_skip();
    // draw position indicator if it doesn't fit on one page
    if (_values.size() > static_cast<std::size_t>(visible_entries))
    {
        int const ind_len = std::max(15, static_cast<int>(((_box.h - 2) * visible_entries) / _values.size()));
        int const ind_w = 7;

        int const ind_y = _box.y + 1 + ((_box.h - 2 - ind_len) * _position) / (_values.size() - visible_entries);
        SDL_Rect ind_rect { _box.x + _box.w - ind_w - 1, ind_y, ind_w, ind_len};
        dc.draw_entry_position_indicator(ind_rect);
    }
}

void list_view::on_mouse_up_event(mouse_up_event const & e)
{
    // scroll on swipe
    if (e.opt_swipe_event.has_value())
    {
        // TODO swipe in x-axis may show the entries with another x_offset
        swipe_event const & se = e.opt_swipe_event.value();
        if (within_rect(se.position, _box))
        {
            int const visible_entries = (_box.h - 2) / _dc.font_line_skip();
            int const distance = static_cast<int>(visible_entries) * se.length.h / (_box.w / 2);
            unsigned int const next_selected_position = _position + distance;
            if (se.action == swipe_action::UP)
                _position = dec_ensure_lower(next_selected_position, _position, 0);
            else if (se.action == swipe_action::DOWN)
                _position = inc_ensure_upper(next_selected_position, _position, _values.size() < static_cast<std::size_t>(visible_entries) ? 0 : _values.size() - visible_entries);
            mark_dirty();
        }
    }
    else if (within_rect(e.position, _box))
    {
        // TODO may be detected more precise
        std::size_t const visible_index = (e.position.y - _box.y) / _dc.font_line_skip();

        // set focus on hit entry
        // TODO focus seperate from mouse ?
        // TODO allow unfocused state ?
        std::size_t const new_selected_position = _position + visible_index;

        if (new_selected_position < _values.size())
        {
            std::cout << "new selected position = " << new_selected_position << ", visible index = " << visible_index << std::endl;
            _selected_position = new_selected_position;
            on_activate();
            mark_dirty();
        }
    }

    _opt_pressed_point.reset();
}

void list_view::on_mouse_down_event(mouse_down_event const & e)
{
    _opt_pressed_point = e.position;
    mark_dirty();
}

void list_view::on_key_event(key_event const & e)
{
    // implement scroll with keys?
}

void list_view::on_activate()
{
    _activate_callback();
}

/*
// for the moment only one highlight is supported, maybe more make sense later on
int list_view(SDL_Rect box, std::vector<std::string> const & entries, unsigned int & pos, int highlight, font_atlas & fa, gui_context & gc)
{
    unsigned int const visible_items = list_view_visible_items(box, fa);

    int selection = -1;
    gc.draw_entry_box(box);

    SDL_Rect text_box { box.x + 1, box.y + 1, box.w - 2, static_cast<int>(fa.height()) };

    gui_event_info const & gei = gc.gei;

    bool const swipe = gei.valid_swipe && gei.mouse_event && within_rect(gei.down_x, gei.down_y, box) && !gei.pressed;
    // hacky update before drawing...
    if (swipe && gei.abs_ydiff * gc.dir_unambig_factor_threshold >= gei.abs_xdiff)
    {
        int const distance = static_cast<int>(visible_items) * 10 * gei.ydiff / (box.w / 2);
        unsigned int const next_pos = pos + distance;
        if (gei.ydiff < 0)
            pos = dec_ensure_lower(next_pos, pos, 0);
        else
            pos = inc_ensure_upper(next_pos, pos, entries.size() < visible_items ? 0 : entries.size() - visible_items);
    }


    return swipe ? -1 : selection;
}
*/

