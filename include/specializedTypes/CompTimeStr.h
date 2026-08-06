/*
 * CompTimeStr.h
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


/*

In C++, passing a character string as a template parameter requires the use of Non-Type Template Parameters (NTTPs).
Prior to C++20, this was primarily achieved by passing a pointer to a global or static const char[] or const char* literal,
or by defining a custom type that could hold the string data and satisfy the requirements for NTTPs.

With C++20, the capabilities of NTTPs were expanded, allowing for more direct use of
string literals as template arguments by leveraging custom literal types and consteval functions.

Methods for using char strings as template parameters:
------------------------------------------------------
*  Using a custom constexpr or consteval string type (C++20 and later):

This is the most robust and modern approach. A struct or class can be defined that holds
the character array and provides consteval constructors and comparison operators.
This type can then be used as a non-type template parameter.


Key Considerations:
-------------------
*  Compile-time vs. Run-time:
   --------------------------
   Template parameters are resolved at compile time.
   Therefore, any string passed as a template parameter must be known at compile time.

*  Structural Types (for C++20 NTTPs):
   -----------------------------------
   When creating custom types for NTTPs in C++20, ensure they are "structural types,"
   meaning they can be compared for equality and do not contain non-static data members
   that are not structural types themselves.

*  String Literals:
   ----------------
   Direct use of string literals as template arguments for custom types is
   significantly simplified in C++20.



Also refer "Expression Templates"   ::    https://www.youtube.com/watch?v=IiVl5oSU5B8

*/


#pragma once

#include <algorithm>
#include <cstddef> // For std::size_t




namespace specializedTypes
{
  template<std::size_t N>
  struct CompTimeStr
  {
    std::size_t size;
    char data[N] {};

    consteval CompTimeStr(const char (&str)[N])  // only Compile time
        : size(N)
    {
      std::copy_n(str, N, data);
    }

    constexpr CompTimeStr(const char (&str)[N], bool)  // Compile time, can be run-time
        : size(N)
    {
      std::copy_n(str, N, data);
    }

    // Add comparison operators if needed
    consteval bool operator==(const CompTimeStr<N>& other) const
    {
      return std::equal(data, data + N, other.data);
    }

    // Provide a helper to expose data safely as a string_view
    constexpr std::string_view view() const {
      return std::string_view(data, N - 1); // exclude null terminator
    }

  };

  // Deduction guide to allow omitting <N> when initializing manually
  template<std::size_t N>
  CompTimeStr(const char (&str)[N]) -> CompTimeStr<N>;
/*
  template<CompTimeStr S>
  struct MyTemplateClass {
    void print_string() const {
      // Access S.data
      // Example: std::cout << S.data << std::endl;
    }
  };

  // Usage:
  // MyTemplateClass<"Hello World"> instance;
*/




  // Source - https://stackoverflow.com/a/35943472
  // Posted by Jamboree, modified by community. See post 'Timeline' for change history
  // Retrieved 2026-07-13, License - CC BY-SA 3.0

  struct string_view
  {
    char const* data;
    std::streamsize size;
  };

  inline std::ostream& operator<<(std::ostream& o, string_view const& s)
  {
    return o.write(s.data, s.size);
  }

  template<class T>
  constexpr
  string_view get_name()  // use this instead of typeid(T).name(){as it results in cryptic name}
  {
    //return {__PRETTY_FUNCTION__, sizeof(__PRETTY_FUNCTION__)};
    // __PRETTY_FUNCTION__ = "constexpr specializedTypes::string_view specializedTypes::get_name() [with T = short int]"

    char const* p = __PRETTY_FUNCTION__;
    char const* const pEnd = p + sizeof(__PRETTY_FUNCTION__);

    while (*p++ != '=' && p < pEnd);
    for (; *p == ' ' && p < pEnd; ++p);
    char const* p2 = p;
    int count = 1;
    for (; p2 < pEnd;++p2)
    {
      switch (*p2)
      {
      case '[':
        ++count;
        break;
      case ']':
        --count;
        if (!count)
          return {p, static_cast<std::streamsize>(p2 - p)};
      default :
        break;
      }
    }
    return {};
  }

  template <typename T>
  constexpr auto type_name_length = get_name<T>().size;

}

