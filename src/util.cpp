// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "util.hpp"

#include <algorithm>

unsigned int inc_ensure_upper(unsigned int new_pos, unsigned int old_pos, unsigned int upper_bound)
{
    return new_pos < old_pos ? upper_bound : std::min(new_pos, upper_bound);
}

unsigned int dec_ensure_lower(unsigned int new_pos, unsigned int old_pos, unsigned int lower_bound)
{
    return new_pos > old_pos ? lower_bound : std::max(new_pos, lower_bound);
}

length_distributor::length_distributor(int const length, int const n)
    : _n(n)
    // temporary fix TODO should we check for n = 0? it won't be used most likely (but then the caller side has to check)
    , _extra_length_div(n > 0 ? length / n : 0)
    , _extra_length_rem(n > 0 ? length % n : 0)
{
}

int length_distributor::dist_end(int const k) const
{
    return _extra_length_div + (k >= (_n - _extra_length_rem) ? 1 : 0);
}

int length_distributor::dist_start(int const k) const
{
    return _extra_length_div + (k < _extra_length_rem ? 1 : 0);
}

