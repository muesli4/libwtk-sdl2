// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <SDL2/SDL_image.h>

#include "texture_view.hpp"

texture_view::texture_view()
    : _p()
    , _size { 0, 0 }
    , _min_width(0)
    , _nat_width(0)
    , _target { 0, 0, 0, 0 }
{
}

texture_view::texture_view(unique_texture_ptr p, int min_width, int nat_width)
    : _p(std::move(p))
    , _size(texture_dim(_p.get()))
    , _min_width(decode_min_width_param(min_width))
    , _nat_width(decode_nat_width_param(nat_width))
    , _target { 0, 0, 0, 0 }
{
}

texture_view::~texture_view()
{
}


void texture_view::on_draw(draw_context & dc, selection_context const & sc) const
{
    if (_p)
        dc.copy_texture(_p.get(), _target);
    else
        dc.draw_background(get_box());
}

void texture_view::on_box_allocated()
{
    if (_p)
    {
        refresh_target();
    }
}

size_hint texture_view::get_size_hint(int width, int height) const
{
    int min_width = opt_min(width, _min_width);
    int nat_width = opt_min(width, _nat_width);

    return size_hint(fit_to_width(min_width), fit_to_width(nat_width));
}

void texture_view::set_texture(unique_texture_ptr p, int min_width, int nat_width)
{
    bool was_nullptr = _p.operator bool();

    _p = std::move(p);
    if (_p)
    {
        _size = texture_dim(_p.get());
        refresh_target();
        _min_width = decode_min_width_param(min_width);
        _nat_width = decode_nat_width_param(nat_width);
        mark_dirty();
    }
    else
    {
        _min_width = 0;
        _nat_width = 0;
        if (!was_nullptr)
        {
            mark_dirty();
        }
    }
}

void texture_view::refresh_target()
{
    std::tie(_target, std::ignore, std::ignore) = scale_preserve_ar(_size, get_box());
    vec min_size = get_size_hint(-1, -1).minimal;

    // TODO find out why the box of a widget might scale down below its minimum
    // size (probably related to box), then we can remove this as a ugly
    // workaround
    _target.w = std::max(_target.w, min_size.w);
    _target.h = std::max(_target.h, min_size.h);
}

vec texture_view::fit_to_width(int width) const
{
    double ratio = static_cast<double>(width) / std::max(1, _size.w);
    return { width, static_cast<int>(_size.h * ratio) };
}

int texture_view::decode_min_width_param(int min_width) const
{
    return min_width < 0 ? _size.w / 2 : std::min(min_width, _size.w);
}

int texture_view::decode_nat_width_param(int nat_width) const
{
    return nat_width < 0 ? _size.w : std::max(_min_width, nat_width);
}

