#ifndef LIBWTK_SDL2_UTIL
#define LIBWTK_SDL2_UTIL
#include <memory>

#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>

#include "geometry.hpp"

struct surface_deleter
{
    void operator()(SDL_Surface * s);
};

typedef std::unique_ptr<SDL_Surface, surface_deleter> unique_surface_ptr;

struct texture_destroyer
{
    void operator()(SDL_Texture * t);
};

typedef std::unique_ptr<SDL_Texture, texture_destroyer> unique_texture_ptr;

SDL_Texture * load_raw_texture_from_image(SDL_Renderer * r, std::string filename);

unique_texture_ptr load_texture_from_image(SDL_Renderer * r, std::string filename);

typedef std::shared_ptr<SDL_Texture> shared_texture_ptr;

shared_texture_ptr load_shared_texture_from_image(SDL_Renderer * r, std::string filename);

SDL_Renderer * renderer_from_window(SDL_Window * window);
SDL_Rect box_from_renderer(SDL_Renderer * renderer);

void print_rect(SDL_Rect const & r);

SDL_Rect pad_rect(SDL_Rect box, int padding);

// create a surface with similar properties
SDL_Surface * create_surface(uint32_t pfe, int width, int height);
SDL_Surface * create_surface(SDL_PixelFormat const * pf, int width, int height);

// Scale a source rectangle that it fits into the target while preserving aspect
// ratio. Also returns the uncovered rectangles.
std::tuple<SDL_Rect, SDL_Rect, SDL_Rect> scale_preserve_ar(vec size, SDL_Rect target);

// blit a surface to another surface while preserving aspect ratio
void blit_preserve_ar(SDL_Surface * source, SDL_Surface * dest, SDL_Rect const * destrect);

vec texture_dim(SDL_Texture const * tex);

#endif

