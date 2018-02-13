#include <iostream>
#include <SDL2/SDL_image.h>

#include "sdl_util.hpp"

void surface_deleter::operator()(SDL_Surface * s)
{
    SDL_FreeSurface(s);
}

void texture_destroyer::operator()(SDL_Texture * t)
{
    SDL_DestroyTexture(t);
}

unique_texture_ptr load_texture_from_image(SDL_Renderer * r, std::string filename)
{
    SDL_Surface * s = IMG_Load(filename.c_str());

    if (s == nullptr)
        throw std::runtime_error(std::string("could not load image: ") + IMG_GetError());

    // TODO how to get to the renderer
    SDL_Texture * t = SDL_CreateTextureFromSurface(r, s);

    if (t == nullptr)
        throw std::runtime_error(std::string("could not convert image to texture: ") + SDL_GetError());

    return unique_texture_ptr(t);
}

SDL_Renderer * renderer_from_window(SDL_Window * window)
{
    // sometimes a renderer is already associated with the window
    SDL_Renderer * renderer = SDL_GetRenderer(window);

    if (renderer == nullptr)
        renderer = SDL_CreateRenderer(window, -1, 0);
    if (renderer == nullptr)
        throw std::runtime_error("Failed to create renderer: " + std::string(SDL_GetError()));
    return renderer;
}

SDL_Rect box_from_renderer(SDL_Renderer * renderer)
{
    SDL_Rect box { 0, 0 };
    if (SDL_GetRendererOutputSize(renderer, &box.w, &box.h) < 0)
        throw std::runtime_error("Failed to get output size from renderer: " + std::string(SDL_GetError()));
    return box;
}

void print_rect(SDL_Rect const & r)
{
    std::cout << r.x << " " << r.y << " on " << r.w << "x" << r.h << std::endl;
}

// create a surface with a pixel format enum and dimensions
SDL_Surface * create_surface(uint32_t pfe, int width, int height)
{
    return SDL_CreateRGBSurface(0, width, height, SDL_BITSPERPIXEL(pfe), 0, 0, 0, 0);
}

SDL_Surface * create_surface(SDL_PixelFormat const * pf, int width, int height)
{
    return SDL_CreateRGBSurface(0, width, height, pf->BitsPerPixel, pf->Rmask, pf->Gmask, pf->Bmask, pf->Amask);
}

std::tuple<SDL_Rect, SDL_Rect, SDL_Rect> scale_preserve_ar(vec size, SDL_Rect target)
{
    int const w = target.w;
    int const h = target.h;

    double const dest_ratio = static_cast<double>(w) / static_cast<double>(h);
    double const source_ratio = static_cast<double>(size.w) / static_cast<double>(size.h);

    bool const dest_wider = dest_ratio >= source_ratio;

    // remaining width and height
    int const rw = dest_wider ? source_ratio * h : w;
    int const rh = dest_wider ? h : (w / source_ratio);

    int const pad_x = (w - rw) / 2;
    int const pad_y = (h - rh) / 2;

    SDL_Rect const fitted_target = { target.x + pad_x, target.y + pad_y, rw, rh };
    if (dest_wider)
    {
        SDL_Rect const left = { target.x, target.y, pad_x, h };
        SDL_Rect const right = { target.x + w - pad_x, target.y, pad_x, h };
        return { fitted_target, left, right };
    }
    else
    {
        SDL_Rect const top = { target.x, target.y, w, pad_y };
        SDL_Rect const bottom = { target.x, target.y + h - pad_y, w, pad_y };
        return { fitted_target, top, bottom };
    }
}

void blit_preserve_ar(SDL_Surface * source, SDL_Surface * dest, SDL_Rect const * destrect)
{
    SDL_Rect target, rem1, rem2;
    std::tie(target, rem1, rem2) = scale_preserve_ar({ source->w, source->h }, *destrect);

    SDL_BlitScaled(source, nullptr, dest, &target);
    SDL_FillRect(dest, &rem1, SDL_MapRGB(dest->format, 0, 0, 0));
    SDL_FillRect(dest, &rem2, SDL_MapRGB(dest->format, 0, 0, 0));
}

SDL_Rect pad_rect(SDL_Rect box, int padding)
{
    return { box.x + padding, box.y + padding, box.w - 2 * padding, box.h - 2 * padding };
}

vec texture_dim(SDL_Texture * tex)
{
    vec size;
    SDL_QueryTexture(tex, nullptr, nullptr, &size.w, &size.h);
    return size;
}

