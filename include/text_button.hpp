// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_TEXT_BUTTON_HPP
#define LIBWTK_SDL2_TEXT_BUTTON_HPP

#include <string>

#include "button.hpp"

struct button;

struct text_button : button
{
    text_button(std::string text, std::function<void()> callback);

    ~text_button() override;

    /**
     * @name Text Button Interface
     * @{
     */

    void set_label(std::string text);

    /** @} */

    private:

    void draw_drawable(draw_context & dc, rect box) const override;
    vec get_drawable_size() const override;

    std::string _text;
};

#endif

