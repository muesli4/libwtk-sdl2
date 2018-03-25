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
    int x_offset;
    int y_offset;
};

#endif

