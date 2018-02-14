#include "list_view.hpp"
#include "util.hpp"
#include <iostream>

// TODO do bounds checking on positions
list_view::list_view(std::vector<std::string> const & values, std::size_t position, std::function<void()> activate_callback)
    : _opt_pressed_point{}
    , _position(position)
    , _selected_position(values.size())
    , _highlight_position(values.size())
    , _x_shift(0)
    , _values(values)
    , _activate_callback(activate_callback)
    , _row_height(1)
{
}

list_view::~list_view()
{
}

int const INDICATOR_MIN_WIDTH = 7;
int const INDICATOR_MIN_HEIGHT = 15;

void list_view::on_draw(draw_context & dc, selection_context const & sc) const
{
    dc.draw_entry_box(get_box());

    // TODO what was the difference between font height and font line skip?
    int const entry_height = static_cast<int>(get_layout_info().font_height());

    // the entry box has a border of 1 on each side TODO make more flexible
    int const x_offset = get_box().x + 1;
    int const entry_width = get_box().w - 2;
    int y_offset = get_box().y + 1;
    std::size_t n = _position;
    int const y_end = get_box().y + get_box().h;
    while (n < _values.get().size() && y_offset < y_end)
    {
        int const overlap = (y_offset + entry_height) - y_end;
        int const entry_height_with_overlap = entry_height - /*(overlap < 0 ? 0 : overlap + 1);*/std::max(0, overlap + 1);

        SDL_Rect abs_rect { x_offset, y_offset, entry_width, entry_height_with_overlap};

        // favor pressed over active
        if (_opt_pressed_point.has_value() && within_rect(_opt_pressed_point.value(), abs_rect))
            dc.draw_entry_pressed_background(abs_rect);
        else if (_highlight_position == n)
            dc.draw_entry_hightlighted_background(abs_rect);
        else if (sc.is_selected_widget(this) && _selected_position == n)
            dc.draw_entry_active_background(abs_rect);


        dc.draw_entry_text(_values.get()[n], { x_offset - _x_shift, y_offset, 900, entry_height_with_overlap} /*abs_rect*/, _x_shift /*std::max(0, entry_width - static_cast<int>(_x_shift))*/);

        y_offset += _row_height;
        n++;
    }

    // draw position indicator if it doesn't fit on one page
    if (_values.get().size() > static_cast<std::size_t>(_visible_entries))
    {
        int const ind_len = std::max(INDICATOR_MIN_HEIGHT, static_cast<int>(((get_box().h - 2) * _visible_entries) / _values.get().size()));
        int const ind_w = INDICATOR_MIN_WIDTH;

        int const ind_y = get_box().y + 1 + ((get_box().h - 2 - ind_len) * _position) / (_values.get().size() - _visible_entries);
        SDL_Rect ind_rect { get_box().x + get_box().w - ind_w - 1, ind_y, ind_w, ind_len};
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
        if (within_rect(se.position, get_box()))
        {
            if (se.action == swipe_action::LEFT)
            {
                std::size_t old = _x_shift;
                _x_shift -= 10;
                if (old < _x_shift)
                    _x_shift = 0;
                mark_dirty();
            }
            else if (se.action == swipe_action::RIGHT)
            {
                _x_shift += 10;
                mark_dirty();
            }
            else
            {
                int const distance = static_cast<int>(_visible_entries) * se.length.h / (get_box().w / 2);

                if (distance < 0)
                    scroll_up(distance);
                else
                    scroll_down(distance);
            }
        }
    }
    else if (within_rect(e.position, get_box()))
    {
        // TODO may be detected more precise
        std::size_t const visible_index = (e.position.y - get_box().y) / _row_height;

        // set focus on hit entry
        // TODO focus seperate from mouse ?
        // TODO allow unfocused state ?
        std::size_t const new_selected_position = _position + visible_index;

        if (new_selected_position < _values.get().size())
        {
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

void list_view::apply_layout_to_children()
{
    // TODO code smell ?
    _row_height = get_layout_info().font_line_skip();
    _visible_entries = (get_box().h - 2) / _row_height;
}

vec list_view::min_size_hint() const
{
    int const line_height = get_layout_info().font_line_skip();
    int const min_width = INDICATOR_MIN_WIDTH + 4 + 2 * line_height;
    int const min_height = std::max(2 * INDICATOR_MIN_HEIGHT, 2 * line_height);
    return { min_width, min_height };
}

void list_view::set_position(std::size_t position)
{
    auto size = _values.get().size();
    _position = std::min(position, size == 0 ? 0 : size - 1);
    mark_dirty();
}

void list_view::set_list(std::vector<std::string> const & values, std::size_t position)
{
    _values = values;
    _position = position;
    _selected_position = values.size();
    _highlight_position = values.size();
    mark_dirty();
}

std::size_t list_view::get_visible_entries() const
{
    return _visible_entries;
}

void list_view::scroll_up(std::size_t amount)
{
    unsigned int const next_selected_position = _position - amount;
    _position = dec_ensure_lower(next_selected_position, _position, 0);
    mark_dirty();
}

void list_view::scroll_down(std::size_t amount)
{
    unsigned int const next_selected_position = _position + amount;
    _position = inc_ensure_upper(next_selected_position, _position, _values.get().size() < static_cast<std::size_t>(_visible_entries) ? 0 : _values.get().size() - _visible_entries);
    mark_dirty();
}

void list_view::scroll_up()
{
    scroll_up(_visible_entries);
}

void list_view::scroll_down()
{
    scroll_down(_visible_entries);
}

