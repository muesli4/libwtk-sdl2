// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "byte_array_slice.hpp"

byte_array_slice::byte_array_slice(std::byte * ptr, size_t size)
    : _ptr(ptr)
    , _size(size)
{
}

byte_array_slice byte_array_slice::slice(int offset, int new_size)
{
    return byte_array_slice(_ptr + offset, new_size);
}

byte_array_slice byte_array_slice::resize(int new_size)
{
    return slice(0, new_size);
}

byte_array_slice byte_array_slice::advance(int amount)
{
    return slice(amount, _size - amount);
}

std::byte * byte_array_slice::data()
{
    return _ptr;
}

std::byte const * byte_array_slice::data() const
{
    return _ptr;
}

size_t byte_array_slice::size() const
{
    return _size;
}

