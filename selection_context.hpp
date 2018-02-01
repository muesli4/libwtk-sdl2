#ifndef SELECTION_CONTEXT_HPP
#define SELECTION_CONTEXT_HPP

#include "widget.hpp"

// TODO work with shared_ptr (in case a selected widget gets removed)
//
// This class provides an optional mechanism to interact with the widgets. Think
// of it as traversing a 2-dimensional grid in which the widgets are positioned.
// The primary purpose is the use of a remote control.
//
// In principle this represents tabulator based widget focus.
struct selection_context
{
    selection_context(widget * w = nullptr);

    // Manually select a widget.
    void select_widget(widget * w);

    // Runs the selected widget's activation event (this may happen when
    // pressing the return key).
    void activate_selected_widget();

    bool is_selected_widget(widget * w);

    // Performs a selection action from the currently selected widgets position.
    // If no such widget exists, it will perform the action starting from the
    // given widget.
    //
    // This is the entry point of all navigation events.
    void navigate_selection(navigation_type nt, widget * main_widget);

    private:

    widget * _selected_widget;
};

#endif

