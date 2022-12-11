// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_UTIL_HPP
#define LIBWTK_SDL2_UTIL_HPP

#ifdef _WIN32
#define NEWLINE_C_STR "\r\n"
#else
#define NEWLINE_C_STR "\n"
#endif

// check overflow and ensure at most upper_bound
unsigned int inc_ensure_upper(unsigned int new_pos, unsigned int old_pos, unsigned int upper_bound);

// check underflow and ensure at least lower_bound
unsigned int dec_ensure_lower(unsigned int new_pos, unsigned int old_pos, unsigned int lower_bound);

/**
 * A helper to properly distribute space via integer division. \ref dist_end
 * should be preferred over \ref dist_start.
 */
struct length_distributor
{
    length_distributor(int const length, int const n);

    int dist_end(int const k) const;
    int dist_start(int const k) const;

    private:

    int const _n;
    int const _extra_length_div;
    int const _extra_length_rem;
};

#endif

