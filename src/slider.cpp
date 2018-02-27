#include <algorithm>

#include "slider.hpp"

int calculate_step(int start, int end, int num_steps)
{
    int size = std::abs(start - end) / (num_steps - 1);
    return start < end ? size : -size;
}

slider::slider(int start, int end, int num_steps, std::function<void(int)> value_callback)
    : _start(start)
    , _end(end)
    , _step(calculate_step(start, end, num_steps))
    , _num_steps(num_steps)
    , _current_step(0)
    , _pressed_step(-1)
    , _value_callback(value_callback)
{
}

slider::slider(int start, int end, std::function<void(int)> value_callback)
    : slider(start, end, std::abs(start - end), value_callback)
{
}

slider::~slider()
{
}

void slider::on_draw(draw_context & dc, selection_context const & sc) const
{
    dc.draw_background(get_box());
    bool const selected = sc.is_selected_widget(this);

    // draw rut
    SDL_Color rut_border { 100, 100, 100 };
    SDL_Color rut_fill { 50, 50, 50 };
    SDL_Color rut_used_fill { 70, 70, 70 };

    int rut_y_offset = get_box().y + _knob_box.h / 2 - 2;
    SDL_Rect left_rut { get_box().x, rut_y_offset, _knob_box.x - get_box().x + 1, 4 };

    // last point within the know
    int knob_end = _knob_box.x + _knob_box.w - 1;
    SDL_Rect right_rut { knob_end, rut_y_offset, get_box().w - (knob_end - get_box().x), 4 };

    dc.set_color(rut_border);
    dc.draw_rect(left_rut);
    dc.draw_rect(right_rut);

    dc.set_color(rut_used_fill);
    dc.draw_rect_filled(pad_rect(left_rut, 1));

    dc.set_color(rut_fill);
    dc.draw_rect_filled(pad_rect(right_rut, 1));

    // draw knob
    dc.draw_button_box(_knob_box, _pressed_step != -1, selected);
}

void slider::on_mouse_up_event(mouse_up_event const & e)
{
    if (_pressed_step != -1)
    {
        if (_pressed_step != _current_step)
            emit_value_callback(_pressed_step);
        _pressed_step = -1;
        mark_dirty();
    }
}

void slider::on_mouse_down_event(mouse_down_event const & e)
{
    if (within_rect(e.position, get_box()))
    {
        // start knob pull
        if (within_rect(e.position, _knob_box))
        {
            _pressed_step = _current_step;
        }
        // move directly to position
        else
        {
            int next_step = get_step_with_abs_pos(e.position.x);
            int last_step = _current_step;
            set_step(next_step);

            // TODO signal on slide already?
            if (next_step != last_step)
                emit_value_callback(next_step);
        }
        mark_dirty();
    }
}

void slider::on_mouse_move_event(mouse_move_event const & e)
{
    // If the button has been pressed, react to move events from all positions
    // (even if not in box).
    if (_pressed_step != -1)
    {
        int next_step = get_step_with_abs_pos(e.position.x);
        set_step(next_step);
        mark_dirty();
    }
}

void slider::on_key_event(key_event const & e)
{
    // TODO implement keys
}

void slider::apply_layout_to_children()
{
    _step_width = get_box().w / _num_steps;
    _step_width_rems = get_box().w % _num_steps;
    refresh_knob_box();
}

vec slider::min_size_hint() const
{
    // TODO use styling engine

    // knob size:
    //     4 pixel width (2 border, 2 fill)
    //     font_height (2 border, rest fill)
    // width of line:
    //     4 pixel per step + 2 pixel border width
    //     4 pixel (2 border, 2 fill)
    return { 2 + 4 * _num_steps, get_context_info().font_line_skip() * 2 };
}

vec slider::nat_size_inc_hint() const
{
    // TODO
    return { 0, 0 };
}

void slider::set_step_interval(int n)
{
    set_step(std::clamp(n, 0, _num_steps - 1));
    mark_dirty();
}

int slider::get_step_interval() const
{
    return _current_step;
}

int slider::get_value() const
{
    return _start + _current_step * _step;
}

void slider::refresh_knob_box()
{
    // Distribute remaining width to first segements.
    _knob_box.x = get_box().x + _current_step * _step_width
                + std::min(_current_step, _step_width_rems);
    _knob_box.y = get_box().y;
    _knob_box.w = _step_width + (_current_step < _step_width_rems ? 1 : 0);
    _knob_box.h = get_context_info().font_line_skip() * 2;
}

int slider::get_step_with_abs_pos(int abs_pos)
{
    // currently only x-dimension
    int offset = abs_pos - (get_box().x + 1);

    // Take remainder distribution into account when calculating the index.
    int rem_point = _step_width_rems * (1 + _step_width);

    int index;
    if (offset < rem_point)
    {

        index = offset / (_step_width + 1);
    }
    else
    {
        index = _step_width_rems + (offset - rem_point) / _step_width;
    }

    return std::clamp(index, 0, _num_steps - 1);
}

void slider::set_step(int step)
{
    _current_step = step;
    refresh_knob_box();
}

void slider::emit_value_callback(int step)
{
    // TODO cache value?
    _value_callback(_start + _current_step * _step);
}

