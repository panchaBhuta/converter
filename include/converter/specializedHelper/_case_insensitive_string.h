/*
 * case_insensitive_string.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.4
 *
 * Copyright (c) 2023-2025 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */


#pragma once

#include <string>
#include <iostream>

// refer https://stackoverflow.com/a/2886589/2299954
//       https://cplusplus.com/reference/string/char_traits/compare/
namespace converter {
  struct ci_char_traits : public std::char_traits<char>
  {
    static bool eq(char c1, char c2) { return toupper(c1) == toupper(c2); }
    static bool ne(char c1, char c2) { return toupper(c1) != toupper(c2); }
    static bool lt(char c1, char c2) { return toupper(c1) <  toupper(c2); }

    static int compare(const char* s1, const char* s2, size_t n) {
      while( n-- ) {
        char us1 = static_cast<char>(std::toupper(*s1));
        char us2 = static_cast<char>(std::toupper(*s2));
        if (!std::char_traits<char>::eq(us1,us2)) return std::char_traits<char>::lt(us1,us2)?-1:1;
        ++s1; ++s2;
      }
      return 0;
    }

    static const char* find(const char* s, int n, char a) {
      char ua = static_cast<char>(std::toupper(a));
      while( n-- > 0 && static_cast<char>(std::toupper(*s)) != ua ) {
        ++s;
      }
      return s;
    }
  };
}
