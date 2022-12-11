// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "utf8.hpp"

int utf8_byte_count(uint8_t start_byte)
{
    if (start_byte & 0b10000000)
    {
        int count = 0;
        while (start_byte & 0b10000000)
        {
            count++;
            start_byte <<= 1;
        }
        return count;
    }
    else
    {
        return 1;
    }
}

// extracts a utf8 character writes it to buff and return the length
int fetch_utf8(char * buff, char const * ptr)
{
    if (*ptr == 0)
    {
        return 0;
    }
    else
    {
        int num_bytes = utf8_byte_count(*ptr);

        for (int pos = 0; pos < num_bytes; pos++)
        {
            buff[pos] = *ptr;
            ptr++;
        }
        buff[num_bytes] = 0;

        return num_bytes;
    }
}

bool is_utf8_following_byte(uint8_t b)
{
    // following bytes have most significant bit set and 2nd most significant
    // bit cleared
    return (b & 0b10000000) && !(b & 0b01000000);
}

int count_utf8_backwards(char const * ptr)
{
    int count = 1;
    while (is_utf8_following_byte(*ptr))
    {
        count++;
        ptr--;
    }
    return count;
}

