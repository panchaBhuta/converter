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

#include <converter/specializedTypes/_case_insensitive_string.h>

namespace specializedTypes {
  using ci_string  =  std::basic_string<char, converter::ci_char_traits, std::string::allocator_type>;

  template<typename Traits>
  concept c_NOT_ci_char_traits = !std::is_same_v<converter::ci_char_traits, Traits>;
}

  /*
    *  https://stackoverflow.com/questions/5195512/namespaces-and-operator-resolution
    *  refer URL^  for   "Namespaces and operator resolution"  for eg: 'operator<<'
  */
  template<typename CharT, // = char,
           specializedTypes::c_NOT_ci_char_traits Traits, // = std::char_traits<CharT>,
           typename Allocator> // = std::allocator<CharT> >
  inline std::basic_ostream<CharT, Traits>&
  operator<<(       std::basic_ostream<CharT, Traits>& os,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    return (os << cistr.c_str());
  }
  template<typename CharT, specializedTypes::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_istream<CharT, Traits>&
  operator>>(       std::basic_istream<CharT, Traits>& is,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    std::string read;
    is >> read;
    cistr = read.c_str();
    return is;
  }


  template<typename CharT, specializedTypes::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_string<CharT, Traits, Allocator>
  operator+ ( const std::basic_string<CharT, Traits,                    Allocator>& str,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    return str + cistr.c_str();
  }
  template<typename CharT, specializedTypes::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_string<CharT, converter::ci_char_traits, Allocator>
  operator+ ( const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr,
              const std::basic_string<CharT, Traits,                    Allocator>& str)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> plus = cistr;
    plus += str.c_str();
    return plus;
  }


  template<typename CharT, specializedTypes::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_string<CharT, Traits, Allocator>&
  operator+=(       std::basic_string<CharT, Traits,                    Allocator>& str,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    str += cistr.c_str();
    return str;
  }
  template<typename CharT, specializedTypes::c_NOT_ci_char_traits Traits, typename Allocator>
  inline std::basic_string<CharT, converter::ci_char_traits, Allocator>&
  operator+=(       std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr,
              const std::basic_string<CharT, Traits,                    Allocator>& str)
  {
    cistr += str.c_str();
    return cistr;
  }


  template<typename CharT, specializedTypes::c_NOT_ci_char_traits Traits, typename Allocator>
  inline bool
  operator==( const std::basic_string<CharT, Traits,                    Allocator>& str,
              const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> chk = str;
    return chk == cistr;
  }
  template<typename CharT, specializedTypes::c_NOT_ci_char_traits Traits, typename Allocator>
  inline bool
  operator==( const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr,
              const std::basic_string<CharT, Traits,                    Allocator>& str)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> chk = str;
    return cistr == chk;
  }


  template<typename CharT, specializedTypes::c_NOT_ci_char_traits Traits, typename Allocator>
  inline auto
  operator<=>( const std::basic_string<CharT, Traits,                     Allocator>& str,
                const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> chk = str;
    return chk <=> cistr;
  }
  template<typename CharT, specializedTypes::c_NOT_ci_char_traits Traits, typename Allocator>
  inline auto
  operator<=>( const std::basic_string<CharT, converter::ci_char_traits, Allocator>& cistr,
                const std::basic_string<CharT, Traits,                   Allocator>& str)
  {
    std::basic_string<CharT, converter::ci_char_traits, Allocator> chk = str;
    return cistr <=> chk;
  }

/* this gives compilation error when declared in "namespace" specializedTypes"
  using ::operator<<;
  using ::operator>>;
  using ::operator+;
  using ::operator+=;
  using ::operator==;
  using ::operator<=>;
*/
//}


namespace specializedTypes {
  using ::operator<<;
  using ::operator>>;
  using ::operator+;
  using ::operator+=;
  using ::operator==;
  using ::operator<=>;
}