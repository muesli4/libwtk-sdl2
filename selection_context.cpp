#include "selection_context.hpp"

#include "widget.hpp"


selection_context::selection_context(widget * w)
    // Set to first selectable widget.
    : _selected_widget(w == nullptr ? w : w->find_selectable(navigation_type::NEXT))
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
        _selected_widget = main_widget->find_selectable(nt);
    }
    else
    {
        // TODO inform the old widget it has been unselected instead
        _selected_widget->mark_dirty();
        _selected_widget = _selected_widget->navigate_selectable(nt);
    }
    select_helper();
}

void selection_context::unselect_helper()
{
    if (_selected_widget != nullptr)
    {
        // TODO inform the new widget it has been unselected instead
        _selected_widget->mark_dirty();
    }
}

void selection_context::select_helper()
{
    if (_selected_widget != nullptr)
    {
        // TODO inform the new widget it has been selected instead
        _selected_widget->mark_dirty();
    }
}
