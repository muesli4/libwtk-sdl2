#include "list_view.hpp"
#include "util.hpp"
#include <iostream>

// TODO do bounds checking on positions
list_view::list_view(std::vector<std::string> const & values, std::size_t position, std::function<void(std::size_t)> activate_callback)
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
    dc.draw_entry_box(get_box(), sc.is_selected_widget(this));

    // TODO what was the difference between font height and font line skip?
    int const entry_height = static_cast<int>(get_context_info().font_height());

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

        rect abs_rect { x_offset, y_offset, entry_width, entry_height_with_overlap};

        // favor pressed over selected
        if (_opt_pressed_point.has_value() && within_rect(_opt_pressed_point.value(), abs_rect))
            dc.draw_entry_pressed_background(abs_rect);
        // favor pressed over active
        else if (sc.is_selected_widget(this) && _selected_position == n)
            dc.draw_entry_active_background(abs_rect);
        else if (_highlight_position == n)
            dc.draw_entry_hightlighted_background(abs_rect);


        dc.draw_entry_text(_values.get()[n], { x_offset, y_offset, entry_width, entry_height_with_overlap}, -_x_shift);

        y_offset += _row_height;
        n++;
    }

    // draw position indicator if it doesn't fit on one page
    if (_values.get().size() > static_cast<std::size_t>(_visible_entries))
    {
        int const ind_len = std::max(INDICATOR_MIN_HEIGHT, static_cast<int>(((get_box().h - 2) * _visible_entries) / _values.get().size()));
        int const ind_w = INDICATOR_MIN_WIDTH;

        int const ind_y = get_box().y + 1 + ((get_box().h - 2 - ind_len) * _position) / (_values.get().size() - _visible_entries);
        rect ind_rect { get_box().x + get_box().w - ind_w - 1, ind_y, ind_w, ind_len};
        dc.draw_entry_position_indicator(ind_rect);
    }
}

int list_view::hit_entry(int y) const
{
    return (y - get_box().y - 2) / _row_height;
}

void list_view::on_mouse_up_event(mouse_up_event const & e)
{

    auto opt_info = get_swipe_info_with_context_info(e);
    // scroll on swipe
    if (opt_info.has_value())
    {
        auto const & info = opt_info.value();

        if (info.type == swipe_info::type::DIRECTION)
        {
            auto const & movement = e.opt_movement.value();
            int font_height = get_context_info().font_line_skip();
            if (info.dir == swipe_direction::LEFT)
            {
                // movement is negative here
                std::size_t next_x_shift = _x_shift + (movement.length.w * font_height) / 15;
                _x_shift = dec_ensure_lower(next_x_shift, _x_shift, 0);
                mark_dirty();
            }
            else if (info.dir == swipe_direction::RIGHT)
            {
                // movement is positive here
                std::size_t next_x_shift = _x_shift + (movement.length.w * font_height) / 15;
                // TODO maximum? max of all entry widths
                _x_shift = inc_ensure_upper(next_x_shift, _x_shift, 5000);
                mark_dirty();
            }
            else
            {
                int const distance = scroll_distance(movement.length.h);

                if (distance < 0)
                    scroll_up(-distance);
                else
                    scroll_down(distance);
            }
        }
        else
        {
            int down_entry = _opt_pressed_point.has_value() ? hit_entry(_opt_pressed_point.value().y) : -1;

            // Only activate when down and up where on the same entry.
            if (down_entry >= 0 && down_entry == hit_entry(e.position.y))
            {
                std::size_t const pressed_position = _position + down_entry;

                // Ensure clicked entry is within bounds.
                if (pressed_position < _values.get().size())
                {
                    _activate_callback(pressed_position);
                }
            }
            mark_dirty();
        }
    }

    _opt_pressed_point.reset();
}

int list_view::scroll_distance(int movement_height) const
{
    // intention: 10 big swipes will get you through the whole list
    //            1  small swipe will move the view by half of the visible entries
    //            a quadratic function is used to scale the range in between

    int const sign = movement_height < 0 ? -1 : 1;
    int const abs_h = abs(movement_height);

    int const y1 = _visible_entries / 2;
    int const x1 = 3 * _row_height;

    if (abs_h <= x1)
    {
        return sign * y1;
    }
    else
    {
        // move (size / 11) positions at most
        int const y2 = _values.get().size() / 11;
        int const x2 = get_box().h;

        double const slope = 0.0d;

        // compute the coefficients for f(h) = a2 * h^2 + a1 * h + a0 with:
        //     f(x1) = y1
        //     f(x2) = y2
        //     f'(x1) = slope
        double const a2 = static_cast<double>(y2 - slope - y1) / (square(x2) - 2 * x1 * x2 + square(x1));
        double const a1 = slope - 2 * a2 * x1;
        double const a0 = y1 + a2 * square(x1) - slope * x1;

        // std::cout << a2 << "x^2 + " << a1 << "x + " << a0 << " -> f(" << movement_height << ") = " << sign * (a2 * square(abs_h) + a1 * abs_h + a0) << std::endl;
        return sign * (a2 * square(abs_h) + a1 * abs_h + a0);
    }
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
    if (_selected_position < _values.get().size())
        _activate_callback(_selected_position);
}

void list_view::on_box_allocated()
{
    // TODO code smell ?
    _row_height = get_context_info().font_line_skip();
    _visible_entries = (get_box().h - 2) / _row_height;
}


size_hint list_view::get_size_hint(int width, int height) const
{
    int const line_height = get_context_info().font_line_skip();

    int const preset_min_width = INDICATOR_MIN_WIDTH + 4 + 2 * line_height;
    int const preset_min_height = std::max(2 * INDICATOR_MIN_HEIGHT, 2 * line_height);

    int const preset_nat_width = preset_min_width + 200;
    int const preset_nat_height = preset_min_height + 2 * line_height;

    // Use all provided width, if any.
    vec minimal { opt_or_value(width, preset_min_width)
                , opt_or_value(height, preset_min_height)
                };
    vec natural { opt_or_value(width, preset_nat_width)
                , opt_or_value(height, preset_nat_height)
                };

    return size_hint(minimal, natural);
}

void list_view::set_position(std::size_t position)
{
    auto size = _values.get().size();
    _position = std::min(position, size == 0 ? 0 : size - 1);
    mark_dirty();
}

void list_view::set_selected_position(std::size_t position)
{
    auto size = _values.get().size();
    _selected_position = std::min(position, size == 0 ? 0 : size - 1);

    if (_selected_position < _position + get_visible_entries()
        && _selected_position >= _position)
        mark_dirty();
}

void list_view::set_highlight_position(std::size_t position)
{
    auto size = _values.get().size();
    _highlight_position = std::min(position, size == 0 ? 0 : size - 1);
    if (_highlight_position < _position + get_visible_entries()
        && _highlight_position >= _position)
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

