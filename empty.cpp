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

vec empty::min_size_hint() const
{
    return { 0, 0 };
}

