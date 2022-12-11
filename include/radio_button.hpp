// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_RADIO_BUTTON_HPP
#define LIBWTK_SDL2_RADIO_BUTTON_HPP

#include <vector> 
#include <string> 
#include <functional> 

#include "selectable.hpp"

struct radio_button : selectable
{
    typedef std::function<void(int)> callback_type;

    /**
     * Construct a radio button with a new group.
     */
    radio_button(std::string label, callback_type callback);

    /**
     * Construct a radio button from an existing group of another radio button.
     */
    radio_button(std::string label, radio_button & other);

    ~radio_button();

    void on_draw(draw_context & dc, selection_context const & sc) const override;
    void on_activate() override;
    void on_mouse_up_event(mouse_up_event const & e) override;
    void on_key_event(key_event const & e) override;
    size_hint get_size_hint(int width, int height) const override;

    /**
     * @name Radio Button Interface
     * @{
     */

    void select();

    /** @} */

    private:

    // A group is never accessed directly and is kept alive with a shared
    // pointer from the individual buttons.
    struct group
    {
        group(callback_type && callback);

        void add(radio_button & rb);
        void remove(radio_button const & rb);
        void set_active(radio_button const & rb);
        bool is_active(radio_button const & rb);

        private:

        callback_type _callback;
        radio_button * _active_radio_button;
        std::vector<radio_button *> _radio_buttons;
    };

    radio_button(std::string label, std::shared_ptr<group> g);

    std::string _label;
    std::shared_ptr<group> _group;
};

// TODO lacks functionality to remove an option, solution: embedded_widget<box>
// with methods to add and remove radio buttons (or ids of those)
widget_ptr radio_box_from_labels(std::vector<std::string> labels, radio_button::callback_type callback);

#endif

