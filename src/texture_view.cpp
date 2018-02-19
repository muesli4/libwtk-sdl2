#include <SDL2/SDL_image.h>

#include "texture_view.hpp"

texture_view::texture_view()
    : _p()
    , _size { 1, 1 }
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

void texture_view::apply_layout_to_children()
{
    if (_p)
        refresh_target();
}

vec texture_view::min_size_hint() const
{
    return fit_to_width(_min_width);
}

vec texture_view::nat_size_inc_hint() const
{
    return fit_to_width(_nat_width);
}

int texture_view::height_for_width_hint(int width) const
{
    // Fit upwards to available width as long as it is below natural width.
    return fit_to_width(std::min(_nat_width, width)).h;
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
}

vec texture_view::fit_to_width(int width) const
{
    double ratio = static_cast<double>(width) / _size.w;
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

