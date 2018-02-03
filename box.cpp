#include <algorithm>

#include "box.hpp"

box::box(orientation o, std::initializer_list<widget_ptr> ws)
    : container(ws)
    , _o(o)
{
}

void box::apply_layout_to_children()
{
    std::size_t const n = _children.size();

    if (n > 0)
    {
        if (_o == orientation::HORIZONTAL)
        {
            // evenly split box
            int const child_width = _box.w / n;
            int xoffset = _box.x;

            for (auto cptr : _children)
            {
                cptr->apply_layout({ xoffset, _box.y, child_width, _box.h });
                xoffset += child_width;
            }
        }
        else
        {
            int const child_height = _box.h / n;
            int yoffset = _box.y;

            for (auto cptr : _children)
            {
                cptr->apply_layout({ _box.x, yoffset, _box.w, child_height });
                yoffset += child_height;
            }
        }

    }
}

widget * box::find_selectable(navigation_type nt)
{
    // TODO add point
    if (is_orthogonal(nt, _o))
    {
        // get middle
        if (!_children.empty())
            return _children[_children.size() / 2]->find_selectable(nt);
    }
    else
    {

        if (is_forward(nt))
        {
            for (auto cptr : _children)
            {
                auto w = cptr->find_selectable(nt);
                if (w != nullptr)
                {
                    return w;
                }
            }
        }
        else
        {
            for (auto it = _children.rbegin(); it != _children.rend(); ++it)
            {
                auto w = (*it)->find_selectable(nt);
                if (w != nullptr)
                {
                    return w;
                }
            }
        }
    }
    return nullptr;
}

bool box::is_orthogonal(navigation_type nt, orientation o)
{
    return
        ((nt == navigation_type::NEXT_X || nt == navigation_type::PREV_X)
            && o == orientation::VERTICAL
        )
        ||
        ((nt == navigation_type::NEXT_Y || nt == navigation_type::PREV_Y)
            && o == orientation::HORIZONTAL
        );
}

bool box::is_forward(navigation_type nt)
{
    return
        nt == navigation_type::NEXT
        || nt == navigation_type::NEXT_X
        || nt == navigation_type::NEXT_Y;
}

widget * box::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    if (is_orthogonal(nt, _o))//nt == navigation_type::NEXT_Y || nt == navigation_type::PREV_Y)
    {
        return navigate_selectable_parent(nt, center);
    }
    else
    {
        auto const begin = _children.begin();
        auto const end = _children.end();

        // locate widget in container
        auto it = std::find_if(begin, end, [w](widget_ptr const & wptr){ return wptr.get() == w; });

        // find first selectable widget of all navigationally consecutive widgets
        while (true)
        {
            if (is_forward(nt))
            {
                it++;
            }
            else
            {
                it--;
            }

            if (it == end || it < begin)
            {
                return navigate_selectable_parent(nt, rect_center(_box));
            }
            else
            {
                auto result = (*it)->find_selectable(nt);
                if (result != nullptr)
                    return result;
            }
        }
    }
}

box::~box()
{
}
