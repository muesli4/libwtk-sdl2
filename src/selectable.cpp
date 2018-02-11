#include "selectable.hpp"

widget * selectable::find_selectable(navigation_type nt, point center)
{
    return this;
}

widget * selectable::navigate_selectable(navigation_type nt, point p)
{
    return navigate_selectable_parent(nt, p);
}

selectable::~selectable()
{
}

void selectable::on_select()
{
    mark_dirty();
}

void selectable::on_unselect()
{
    mark_dirty();
}
