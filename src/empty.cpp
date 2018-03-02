#include "empty.hpp"

empty::empty()
{
}

empty::~empty()
{
}

void empty::on_draw(draw_context & dc, selection_context const & sc) const
{
}

size_hint empty::get_size_hint(int width, int height) const
{
    return size_hint({ 0, 0 });
}
