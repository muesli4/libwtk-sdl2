// SPDX-FileCopyrightText: Moritz Bruder <muesli4 at gmail dot com>
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef LIBWTK_SDL2_UTF8_HPP
#define LIBWTK_SDL2_UTF8_HPP

#include <cstdint>

int utf8_byte_count(uint8_t start_byte);

// get the current utf8 character, write it into the buffer and return its length
int fetch_utf8(char * buff, char const * ptr);

bool is_utf8_following_byte(uint8_t b);

// assumes a non-empty and well-formed utf8 string
int count_utf8_backwards(char const * ptr);

#endif

