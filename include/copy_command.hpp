// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_COPY_COMMAND_HPP
#define LIBWTK_SDL2_COPY_COMMAND_HPP

#include "geometry.hpp"

struct SDL_Texture;

// copy textures to the screen in a batch
struct copy_command
{
    SDL_Texture * texture;

    // The part of the texture to copy.
    rect source;

    // The offset of the target coordinates.
    vec offset;
};

#endif

