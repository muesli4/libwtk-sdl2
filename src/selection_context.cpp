#include "selection_context.hpp"
#include "widget.hpp"

selection_context::selection_context(rect widget_area, widget * w)
    // Set to first selectable widget.
    : _selected_position(rect_center(widget_area))
    , _selected_widget(w == nullptr ? w : w->find_selectable(navigation_type::NEXT, _selected_position))
    , _widget_area(widget_area)
{
    select_helper();
}

// manually select a widget
void selection_context::select_widget(widget * w)
{
    unselect_helper();
    _selected_widget = w;
    select_helper();
}

void selection_context::unselect_widget()
{
    unselect_helper();
    _selected_widget = nullptr;
    // When manually unselecting the position is reset.
    _selected_position = rect_center(_widget_area);
}

void selection_context::dispatch_activation()
{
    if (_selected_widget != nullptr)
    {
        _selected_widget->on_activate();
    }
}

void selection_context::dispatch_key_event(key_event const & ke)
{
    if (_selected_widget != nullptr)
    {
        _selected_widget->on_key_event(ke);
    }
}

bool selection_context::is_selected_widget(widget const * w) const
{
    return w == _selected_widget;
}

// Performs a selection action from the currently selected widgets position.
// If no such widget exists, it will perform the action starting from the
// given widget (should be the root).
void selection_context::navigate_selection(navigation_type nt, widget * main_widget)
{
    if (_selected_widget == nullptr)
    {
        // select first selectable
        _selected_widget = main_widget->find_selectable(nt, _selected_position);
    }
    else
    {
        _selected_widget->on_unselect();
        _selected_widget = _selected_widget->navigate_selectable(nt, _selected_position);
    }
    select_helper();
}

void selection_context::change_widget_area(rect new_box)
{
    _widget_area = new_box;
    if (_selected_widget == nullptr)
        _selected_position = rect_center(_widget_area);
}

void selection_context::unselect_helper()
{
    if (_selected_widget != nullptr)
    {
        _selected_widget->on_unselect();
    }
}

void selection_context::select_helper()
{
    if (_selected_widget != nullptr)
    {
        _selected_position = rect_center(_selected_widget->get_box());
        _selected_widget->on_select();
    }
}
