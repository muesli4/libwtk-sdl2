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
    , _extra_length_div(length / n)
    , _extra_length_rem(length % n)
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

