// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_BYTE_ARRAY_SLICE_HPP
#define LIBWTK_SDL2_BYTE_ARRAY_SLICE_HPP

#include <cstddef>

// This is just a pointer and a size.
struct byte_array_slice
{
    byte_array_slice(std::byte * ptr, size_t size);

    template <int N>
    byte_array_slice(std::byte ptr[N])
        : _ptr(ptr)
        , _size(N)
    {
    }

    byte_array_slice slice(int offset, int new_size);

    byte_array_slice resize(int new_size);

    byte_array_slice advance(int amount);

    std::byte * data();

    std::byte const * data() const;

    size_t size() const;

    private:

    std::byte * _ptr;
    size_t _size;
};


#endif

