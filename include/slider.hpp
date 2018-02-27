#ifndef LIBWTK_SDL2_SLIDER_HPP
#define LIBWTK_SDL2_SLIDER_HPP

#include "selectable.hpp"

struct slider : selectable
{
    /**
     * Create a slider that selects an integer interval with length \ref step
     * starting with intervals from \ref start up or down to \ref end.
     *
     * For a \ref step of 1 the intervals become equal to integers.
     */
    slider(int start, int end, int step = 1);
    ~slider() override;

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_mouse_up_event(mouse_up_event const & e) override;
    void on_mouse_down_event(mouse_down_event const & e) override;
    void on_mouse_move_event(mouse_move_event const & e) override;
    void on_key_event(key_event const & e) override;

    void apply_layout_to_children() override;

    vec min_size_hint() const override;
    vec nat_size_inc_hint() const override;

    // slider interface

    /**
     * Set the slider to the interval with index \ref n.
     */
    void set_step_interval(int n);
    int get_step_interval() const;

    /**
     * Returns the starting value of the currently selected interval.
     */
    int get_value() const;

    private:

    void refresh_knob_box();
    int get_step_with_abs_pos(int abs_pos);
    void set_step(int step);

    int _start;
    int _end;
    int _step;

    int _num_steps;

    int _current_step;

    bool _pressed;

    SDL_Rect _knob_box;
    int _step_width;
    int _step_width_rems;
};

#endif

