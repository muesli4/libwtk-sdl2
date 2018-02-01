#include "selection_context.hpp"

selection_context::selection_context(widget * w)
    // Set to first selectable widget.
    : _selected_widget(w == nullptr ? w : w->find_selectable())
{
}

// manually select a widget
void selection_context::select_widget(widget * w)
{
    _selected_widget = w;
}

void selection_context::activate_selected_widget()
{
    if (_selected_widget != nullptr)
    {
        _selected_widget->on_activate();
    }
}

bool selection_context::is_selected_widget(widget * w)
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
        _selected_widget = main_widget->find_selectable();
    }
    else
    {
        _selected_widget = _selected_widget->navigate_selectable(nt);
    }
}

