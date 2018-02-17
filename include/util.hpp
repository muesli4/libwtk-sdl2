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

#endif

