/*
 * case_insensitive_string.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.2
 *
 * Copyright (c) 2023-2024 Gautam Dhar
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

  using ci_string  =  std::basic_string<char, ci_char_traits, std::string::allocator_type>;

  template<typename Traits>
  concept c_NOT_ci_char_traits = !std::is_same_v<ci_char_traits, Traits>;


  /*
    *  https://stackoverflow.com/questions/5195512/namespaces-and-operator-resolution
    *  refer URL^  for   "Namespaces and operator resolution"  for eg: 'operator<<'
  */
  template<typename CharT, // = char,
           converter::c_NOT_ci_char_traits Traits, // = std::char_traits<CharT>,
           typename Allocator> // = std::allocator<CharT> >
  inline std::basic_ostream<CharT, Traits>&
  operator<<(       std::basic_ostream<CharT, Traits>& os,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    return (os << cistr.c_str());
  }
  template<typename CharT, converter::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_istream<CharT, Traits>&
  operator>>(       std::basic_istream<CharT, Traits>& is,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    std::string read;
    is >> read;
    cistr = read.c_str();
    return is;
  }


  template<typename CharT, converter::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_string<CharT, Traits, Allocator>
  operator+ ( const std::basic_string<CharT, Traits,                    Allocator>& str,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    return str + cistr.c_str();
  }
  template<typename CharT, converter::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_string<CharT, converter::ci_char_traits, Allocator>
  operator+ ( const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr,
              const std::basic_string<CharT, Traits,                    Allocator>& str)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> plus = cistr;
    plus += str.c_str();
    return plus;
  }


  template<typename CharT, converter::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_string<CharT, Traits, Allocator>&
  operator+=(       std::basic_string<CharT, Traits,                    Allocator>& str,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    str += cistr.c_str();
    return str;
  }
  template<typename CharT, converter::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_string<CharT, converter::ci_char_traits, Allocator>&
  operator+=(       std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr,
              const std::basic_string<CharT, Traits,                    Allocator>& str)
  {
    cistr += str.c_str();
    return cistr;
  }


  template<typename CharT, converter::c_NOT_ci_char_traits Traits, typename Allocator>
  inline bool
  operator==( const std::basic_string<CharT, Traits,                    Allocator>& str,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> chk = str;
    return chk == cistr;
  }
  template<typename CharT, converter::c_NOT_ci_char_traits Traits, typename Allocator>
  inline bool
  operator==( const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr,
              const std::basic_string<CharT, Traits,                    Allocator>& str)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> chk = str;
    return cistr == chk;
  }


  template<typename CharT, converter::c_NOT_ci_char_traits Traits, typename Allocator>
  inline auto
  operator<=>( const std::basic_string<CharT, Traits,                     Allocator>& str,
                const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> chk = str;
    return chk <=> cistr;
  }
  template<typename CharT, converter::c_NOT_ci_char_traits Traits, typename Allocator>
  inline auto
  operator<=>( const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr,
                const std::basic_string<CharT, Traits,                   Allocator>& str)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> chk = str;
    return cistr <=> chk;
  }

/* this gives compilation error
  using ::operator<<;
  using ::operator>>;
  using ::operator+;
  using ::operator+=;
  using ::operator==;
  using ::operator<=>;
*/
}
