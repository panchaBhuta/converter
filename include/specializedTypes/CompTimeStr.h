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


/*
 * on WINDOWS os, MSVC compiler; doesn't support :
 *   1.     constexpr std::copy_n(...);
 *   2.     consteval bool operator==(const CompTimeStr<N>& other);
 *
 * _workaroundConfig.h provides the workaround
 */
#if USE_CONSTEXPR_STRING_COPYN == e_DISABLE_FEATURE
#include <converter/_workaroundConfig.h>
#endif

namespace specializedTypes
{
  template<std::size_t N>
  struct CompTimeStr
  {
    char data[N] {};

    consteval CompTimeStr(const char (&str)[N])
    {
#if USE_CONSTEXPR_STRING_COPYN == e_ENABLE_FEATURE
      std::copy_n(str, N, data);
#else
      workaround::copy_n(str, N, data);
#endif
    }

    // Add comparison operators if needed
    consteval bool operator==(const CompTimeStr<N>& other) const
    {
#if USE_CONSTEXPR_STRING_COPYN == e_ENABLE_FEATURE
      return std::equal(data, data + N, other.data);
#else
      return workaround::equal(data, data + N, other.data);
#endif
    }

  };
}

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
