// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include <algorithm>
#include <iterator>

#include "box.hpp"
#include "util.hpp"

// TODO default setting singleton
box::box(orientation o, box::children_type children)
    : box(o, children, 5, false)
{
}

box::box(orientation o, box::children_type children, int children_spacing)
    : box(o, children, children_spacing, false)
{
}

box::box(orientation o, box::children_type children, bool children_homogeneous)
    : box(o, children, 5, children_homogeneous)
{
}

box::box(orientation o, box::children_type children, int children_spacing, bool children_homogeneous)
    : _children(children)
    , _children_spacing(children_spacing)
    , _children_homogeneous(children_homogeneous)
    , _o(o)
{
    init_children();
}

void box::on_box_allocated()
{
    std::size_t const n = _children.size();

    if (n > 0)
    {

        // TODO properly handle orthogonal size (e.g., center, align, etc.)
        //      currently it is filled

        using namespace std;

        int const spacing_length = (n - 1) * _children_spacing;

        int min_sum = 0;
        int nat_sum = 0;
        vector<size_hint> size_hints;
        size_hints.reserve(n);


        if (_children_homogeneous)
        {
            for (auto const & c : _children)
            {
                auto & wptr = c.wptr;
                if (_o == orientation::VERTICAL)
                {
                    size_hints.push_back(wptr->get_size_hint(get_box().w, -1));
                    auto const & sh = size_hints.back();
                    min_sum += sh.minimal.h;
                    nat_sum += sh.natural.h;
                }
                else
                {
                    size_hints.push_back(wptr->get_size_hint(-1, get_box().h));
                    auto const & sh = size_hints.back();
                    min_sum += sh.minimal.w;
                    nat_sum += sh.natural.w;
                }
            }

            if (_o == orientation::HORIZONTAL)
            {
                int const avail_width = get_box().w - spacing_length;

                bool const use_natural_width = avail_width >= nat_sum;
                int const used_width = use_natural_width
                                     ? avail_width
                                     : std::max(min_sum, avail_width);

                length_distributor ld(used_width, n);

                int xoffset = get_box().x;

                for (std::size_t k = 0; k < n; ++k)
                {
                    int const child_width = ld.dist_end(k);

                    _children[k].wptr->apply_layout({ xoffset, get_box().y, child_width, get_box().h });
                    xoffset += child_width + _children_spacing;
                }
            }
            else
            {
                int const avail_height = get_box().h - spacing_length;

                bool const use_natural_height = avail_height >= nat_sum;
                int const used_height = use_natural_height
                                      ? avail_height
                                      : std::max(min_sum, avail_height);

                length_distributor ld(used_height, n);

                int yoffset = get_box().y;

                for (std::size_t k = 0; k < n; ++k)
                {
                    int const child_height = ld.dist_end(k);

                    _children[k].wptr->apply_layout({ get_box().x, yoffset, get_box().w, child_height });
                    yoffset += child_height + _children_spacing;
                }
            }
        }
        else
        {
            // Algorithm outline:
            // 1. Ask for minimum and natural size of each widget
            // 2. Calculate difference with available space for both minimal and
            //    natural size.
            // 3. If natural size will fit for every widget use natural size,
            //    otherwise use minimum size.
            // 4. Distribute left-over space to widgets with expand property.
            int num_expand = 0;

            for (auto const & c : _children)
            {
                auto const & wptr = c.wptr;

                if (_o == orientation::VERTICAL)
                {
                    size_hints.push_back(wptr->get_size_hint(get_box().w, -1));
                    min_sum += size_hints.back().minimal.h;
                    nat_sum += size_hints.back().natural.h;
                }
                else
                {
                    size_hints.push_back(wptr->get_size_hint(-1, get_box().h));
                    min_sum += size_hints.back().minimal.w;
                    nat_sum += size_hints.back().natural.w;
                }

                if (c.expand)
                    num_expand += 1;
            }

            int const max_length = _o == orientation::HORIZONTAL ? get_box().w : get_box().h;
            int const avail_after_spacing = max(0, max_length - spacing_length);

            int const rem_space_min_length = avail_after_spacing - min_sum;

            bool const use_natural_size = nat_sum <= avail_after_spacing;
            bool const fill_to_natural = !use_natural_size && rem_space_min_length > 0;

            // The amounts that are used to partially fill the children to their
            // natural size.
            vector<int> partial_nat_size_incs;
            length_distributor ld(use_natural_size ? avail_after_spacing - nat_sum : 0, num_expand);

            if (fill_to_natural)
            {
                // 1. Sort differences between natural size (or use min heap).
                // 2. While there is the minimum difference times the remaining
                //    widgets space left:
                //    Fill all remaining widgets equally by the minimum
                //    difference (only for widgets that support intermediate
                //    sizes). Subtract that from every remaining difference.
                //
                //    TODO add option to fill all widgets to a percentage

                int rem_space = rem_space_min_length;

                // This is used to avoid subtracting from all elements of the heap.
                int allocated = 0;

                // Differences and the corresponding index.
                auto cmp_first = [](auto a, auto b){ return get<0>(a) > get<0>(b); };
                vector<pair<int, size_t>> diff_heap;
                diff_heap.reserve(n);
                for (size_t k = 0; k < n; ++k)
                {
                    if (_children[k].wptr->can_use_intermediate_size())
                    {
                        auto const & sh = size_hints[k];
                        // TODO limit to 0 ?
                        int const nat_inc = _o == orientation::HORIZONTAL ? sh.natural.w - sh.minimal.w : sh.natural.h - sh.minimal.h;
                        diff_heap.emplace_back(nat_inc, k);
                    }
                }

                make_heap(begin(diff_heap), end(diff_heap), cmp_first);

                vector<int> tmp_partial_nat_size_incs(n, 0);
                do
                {
                    pop_heap(begin(diff_heap), end(diff_heap), cmp_first);
                    int min_idx;
                    int min_diff;
                    tie(min_diff, min_idx) = diff_heap.back();
                    {
                        int const new_allocated = min_diff;
                        min_diff -= allocated;
                        allocated = new_allocated;
                    }

                    int const next_allocation = min_diff * diff_heap.size();
                    // can we allocate the current size for each?
                    if (next_allocation <= rem_space)
                    {
                        // allocate min diff for every widget (including the popped one)
                        for (auto const & p : diff_heap)
                        {
                            tmp_partial_nat_size_incs[get<1>(p)] += min_diff;
                        }

                        diff_heap.pop_back();
                    }
                    else
                    {
                        length_distributor ld(rem_space, diff_heap.size());

                        for (std::size_t k = 0; k < diff_heap.size(); ++k)
                        {
                            tmp_partial_nat_size_incs[get<1>(diff_heap[k])] += ld.dist_end(k);
                        }
                        break;
                    }

                    rem_space -= next_allocation;
                }
                // Protect against violated pre-conditions. E.g., when there is
                // not enough space for the widgets.
                // TODO figure out when this happens
                while (!diff_heap.empty());
                partial_nat_size_incs.swap(tmp_partial_nat_size_incs);
            }

            int offset = _o == orientation::HORIZONTAL ? get_box().x : get_box().y;

            for (size_t k = 0; k < n; k++)
            {
                auto & c = _children[k];

                // Distribute remaining length or fill to natural size.
                int const extra_length = fill_to_natural
                                       ? partial_nat_size_incs[k]
                                       : (c.expand ? ld.dist_end(k) : 0);

                auto const & sh = size_hints[k];
                vec const & size = use_natural_size ? sh.natural : sh.minimal;

                if (_o == orientation::HORIZONTAL)
                {
                    // TODO should both lengths be limited?
                    int w = min(size.w + extra_length, get_box().w);
                    c.wptr->apply_layout({ offset, get_box().y, w, get_box().h });
                    offset += w + _children_spacing;
                }
                else
                {
                    int h = min(size.h + extra_length, get_box().h);
                    c.wptr->apply_layout({ get_box().x, offset, get_box().w, h });
                    offset += h + _children_spacing;
                }
            }
        }
    }
}

widget * box::find_selectable(navigation_type nt, point center)
{
    // When the navigation request is orthogonal, pick the widget with the
    // closest distance on that particular dimension.
    if (is_orthogonal(nt))
    {

        widget * min_w;
        int min_distance;

        std::size_t k = 0;

        // Find a selectable widget for a start.
        for (; k < _children.size(); ++k)
        {
            min_w = _children[k].wptr->find_selectable(nt, center);

            if (min_w != nullptr)
            {
                auto found_widget_center = rect_center(min_w->get_box());
                min_distance = relevant_distance(center, found_widget_center);
                break;
            }
        }

        if (min_w == nullptr)
            return nullptr;
        else
        {
            // Refine by finding a selectable widget with a smaller distance.
            for (++k; k < _children.size(); ++k)
            {
                auto new_w = _children[k].wptr->find_selectable(nt, center);

                if (new_w != nullptr)
                {
                    auto new_center = rect_center(new_w->get_box());
                    int new_distance = relevant_distance(center, new_center);

                    if (new_distance < min_distance)
                    {
                        min_w = new_w;
                        min_distance = new_distance;
                    }
                    else
                    {
                        // Assumption: Parallel widget dimension increases with
                        // order in container.
                        break;
                    }
                }
            }

            return min_w;
        }
    }
    // Otherwise just pick the first selectable one in the direction of the
    // request.
    else
    {
        if (is_forward(nt))
        {
            for (auto c : _children)
            {
                auto w = c.wptr->find_selectable(nt, center);
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
                auto w = it->wptr->find_selectable(nt, center);
                if (w != nullptr)
                {
                    return w;
                }
            }
        }
    }
    return nullptr;
}

widget * box::navigate_selectable_from_children(navigation_type nt, widget * w, point center)
{
    // When navigating in a linear container like box, we don't have to consider
    // the center hint, as we're either orthogonal with the navigation request,
    // then the parent will handle it, or we're parallel to it, then we just
    // need to pick a consecutive widget.

    if (is_orthogonal(nt))
    {
        return navigate_selectable_parent(nt, center);
    }
    else
    {
        auto const begin = _children.begin();
        auto const end = _children.end();

        // locate widget in container
        auto it = std::find_if(begin, end, [w](child const & c){ return c.wptr.get() == w; });

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
                return navigate_selectable_parent(nt, center);
            }
            else
            {
                auto result = it->wptr->find_selectable(nt, center);
                if (result != nullptr)
                    return result;
            }
        }
    }
}

size_hint box::get_size_hint(int width, int height) const
{
    int const spacing = _children.empty() ? 0 : (_children.size() - 1) * _children_spacing;
    vec minimal = { 0, 0 };
    vec natural = { 0, 0 };

    int hom_minimal_max = 0;
    int hom_natural_max = 0;
    for (auto & c : _children)
    {
        if (_o == orientation::HORIZONTAL)
        {
            auto sh = c.wptr->get_size_hint(-1, height);

            minimal.h = std::max(sh.minimal.h, minimal.h);
            natural.h = std::max(sh.natural.h, natural.h);
            if (_children_homogeneous)
            {
                hom_minimal_max = std::max(sh.minimal.w, hom_minimal_max);
                hom_natural_max = std::max(sh.natural.w, hom_natural_max);
            }
            else
            {
                minimal.w += sh.minimal.w;
                natural.w += sh.natural.w;
            }
        }
        else
        {
            auto sh = c.wptr->get_size_hint(width, -1);

            minimal.w = std::max(sh.minimal.w, minimal.w);
            natural.w = std::max(sh.natural.w, natural.w);
            if (_children_homogeneous)
            {
                hom_minimal_max = std::max(sh.minimal.h, hom_minimal_max);
                hom_natural_max = std::max(sh.natural.h, hom_natural_max);
            }
            else
            {
                minimal.h += sh.minimal.h;
                natural.h += sh.natural.h;
            }
        }
    }

    if (_children_homogeneous)
    {
        if (_o == orientation::HORIZONTAL)
        {
            minimal.w += hom_minimal_max * _children.size() + spacing;
            natural.w += hom_natural_max * _children.size() + spacing;
        }
        else
        {
            minimal.h += hom_minimal_max * _children.size() + spacing;
            natural.h += hom_natural_max * _children.size() + spacing;
        }
    }

    return size_hint(minimal, natural);
}

box::~box()
{
}

int box::relevant_distance(point old_center, point new_center) const
{
    return std::abs(_o == orientation::HORIZONTAL ? old_center.x - new_center.x : old_center.y - new_center.y);
}

bool box::is_orthogonal(navigation_type nt) const
{
    return
        ((nt == navigation_type::NEXT_X || nt == navigation_type::PREV_X)
            && _o == orientation::VERTICAL
        )
        ||
        ((nt == navigation_type::NEXT_Y || nt == navigation_type::PREV_Y)
            && _o == orientation::HORIZONTAL
        );
}

std::vector<widget *> box::get_children()
{
    std::vector<widget *> result;
    std::for_each(std::begin(_children), std::end(_children), [&](auto c){ result.push_back(c.wptr.get()); });
    return result;
}

std::vector<widget const *> box::get_children() const
{
    std::vector<widget const *> result;
    std::for_each(std::begin(_children), std::end(_children), [&](auto c){ result.push_back(c.wptr.get()); });
    return result;
}

widget_ptr hbox(box::children_type children)
{
    return std::make_shared<box>(box::orientation::HORIZONTAL, children);
}

widget_ptr vbox(box::children_type children)
{
    return std::make_shared<box>(box::orientation::VERTICAL, children);
}


widget_ptr hbox(box::children_type children, int children_spacing)
{
    return std::make_shared<box>(box::orientation::HORIZONTAL, children, children_spacing);
}

widget_ptr vbox(box::children_type children, int children_spacing)
{
    return std::make_shared<box>(box::orientation::VERTICAL, children, children_spacing);
}


widget_ptr hbox(box::children_type children, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::HORIZONTAL, children, children_homogeneous);
}

widget_ptr vbox(box::children_type children, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::VERTICAL, children, children_homogeneous);
}

widget_ptr hbox(box::children_type children, int children_spacing, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::HORIZONTAL, children, children_spacing, children_homogeneous);
}

widget_ptr vbox(box::children_type children, int children_spacing, bool children_homogeneous)
{
    return std::make_shared<box>(box::orientation::VERTICAL, children, children_spacing, children_homogeneous);
}

