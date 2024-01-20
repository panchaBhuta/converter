/*
 * date.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.2
 *
 * Copyright (c) 2023-2023 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */


#pragma once

#include <string>
#include <iostream>

// refer https://stackoverflow.com/a/2886589/2299954

namespace converter {
  struct ci_char_traits : public std::char_traits<char>
  {
    static bool eq(char c1, char c2) { return toupper(c1) == toupper(c2); }
    static bool ne(char c1, char c2) { return toupper(c1) != toupper(c2); }
    static bool lt(char c1, char c2) { return toupper(c1) <  toupper(c2); }

    static int compare(const char* s1, const char* s2, size_t n) {
      while( n-- != 0 ) {
        if( toupper(*s1) < toupper(*s2) ) return -1;
        if( toupper(*s1) > toupper(*s2) ) return 1;
        ++s1; ++s2;
      }
      return 0;
    }

    static const char* find(const char* s, int n, char a) {
      while( n-- > 0 && toupper(*s) != toupper(a) ) {
        ++s;
      }
      return s;
    }
  };

  using ci_string  =  std::basic_string<char, ci_char_traits, std::string::allocator_type>;
}

std::ostream& operator<<(std::ostream& os, const converter::ci_string& cistr)
{
  os << cistr.c_str();
  return os;
}
std::istream& operator>>(std::istream& is, converter::ci_string& cistr)
{
  std::string read;
  is >> read;
  cistr = read.c_str();
  return is;
}


/*
template<
    class CharT,
    class Traits = std::char_traits<CharT>,
    class Allocator = std::allocator<CharT>
>
template<>
//std::string& std::string::operator=(const ci_string& cistr)
std::basic_string<CharT, Traits, Allocator>&
std::basic_string<CharT, Traits, Allocator>::operator=<ci_string>(const ci_string& cistr)
{
  (*this) = cistr.c_str();
  return (*this);
}

template<>
template<
  class CharT,
  class Traits = std::char_traits<CharT>,
  class Allocator = std::allocator<CharT>
>
ci_string& ci_string::operator=<std::basic_string<CharT, Traits, Allocator>>(const std::basic_string<CharT, Traits, Allocator>& str)
{
  (*this) = str.c_str();
  return (*this);
}
*/


std::string operator+(const std::string& str, const converter::ci_string& cistr)
{
  return str + cistr.c_str();
}
converter::ci_string operator+(const converter::ci_string& cistr, const std::string& str)
{
  converter::ci_string plus = cistr;
  plus += str.c_str();
  return plus;
}

std::string& operator+=(std::string& str, const converter::ci_string& cistr)
{
  str += cistr.c_str();
  return str;
}
converter::ci_string& operator+=(converter::ci_string& cistr, const std::string& str)
{
  cistr += str.c_str();
  return cistr;
}


