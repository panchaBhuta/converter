/*
 * _compilerWorkAround.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v2.4
 *
 * Copyright (c) 2023-2026 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#pragma once


/*
 * Compiler-specific workarounds
 * 
 * Observed on Github runners :
 *    Failed : GCC12.3   (ubuntu-22.04, g++-12) and (ubuntu-22.04-arm, g++-12)
 *    Passed : GCC12.4   (ubuntu-24.04, g++-12) and (ubuntu-24.04-arm, g++-12)
 *
 * GCC 12.3 has an internal compiler error (Segmentation fault) when
 * matching the constrained template-template parameter used by
 * isBumpedTypeConversionAvailable.
 *
 * GCC 12.4 and later handle the same construct correctly.
 *
 * Keep the stronger c_numeric constraint for all unaffected compilers /
 * compiler versions. On the affected GCC versions, use 'typename' only
 * for the template-template parameter so that the capability trait can
 * still be instantiated without triggering the compiler bug.
 *
 * This macro is intentionally defined here as a preprocessor token.
 * It does not require the c_numeric concept to be declared at this point;
 * the C++ parser sees either 'typename' or 'c_numeric' after preprocessing.
 */
#if defined(__GNUC__) && !defined(__clang__) && ((__GNUC__ < 12) || (__GNUC__ == 12 && __GNUC_MINOR__ < 4))

  #define CONVERTER_TEMPLATE_TYPE_CONSTRAINT typename

#else

  #define CONVERTER_TEMPLATE_TYPE_CONSTRAINT c_numeric

#endif


#include <string>
#include <concepts>
#include <limits>
#include <map>


#include <specializedTypes/case_insensitive_string.h>

#if defined(_MSC_VER)

#include <cmath>

namespace converter
{
  template <std::floating_point T>
  inline T msvc_preserveNaNSign_workaround(const std::string& strVal, const T& val)
  {
    /*
     * WORKAROUND for MSVC + (FROM_CHARS || S2N):
     *
     * MSVC's std::from_chars() and std::sto*() converts
     * "nan(ind)" and "+nan(ind)" to a negative NaN.
     * Correct the sign of the successfully parsed NaN to match
     * the sign in the input string.
     */
    const auto pos = strVal.find_first_not_of(" \t\n\r\f\v");


    const bool isNegative =
        pos != std::string::npos && strVal[pos] == '-';

    return std::copysign(val, isNegative ? T{-1} : T{1});;
  }

} // namespace converter

#endif



namespace converter
{
  inline auto fromchars_gooble_leading_whiteSpace_workaround(const std::string& str)
  {
    auto pos = str.find_first_not_of(" \t\n\r\f\v");    // gooble up leading white spaces

    if (pos == std::string::npos)
      return pos;

    if ( ((pos + 1) < str.size()) &&
         str[pos]   == '+'        &&                    // gooble up first char if it has +ve sign
         str[pos+1] != '-'                              // use case NaN : "+-1.0"
       )
    ++pos;
    return pos;
  }


  template <std::floating_point T>
      requires ( std::numeric_limits<T>::has_infinity )
  inline bool stringstream_infinity_workaround(const std::string& strVal, T& val)
  {
    if(std::isinf(val))
      return true;

    if (std::isnan(val))
      return false;

    CONVERTER_DEBUG_LOG("stringstream_infinity_workaround() :: checking strVal='" << strVal << "'");

    // stringstream does not recognize string inputs "inf", "+inf", "-inf" as valid parameter.
    // Instead of treating it as infinity, it treats it as an error.
    // To the keep the conversion behavior consistent across various
    // conversion mechanism's (FROM_CHARS, S2N, "STRINGSTREAM"),
    // the below code has been added.
    // TODO: add the above observation to README

    static const std::map<specializedTypes::ci_string, T> infinitySpellings
      {
        {  "inf",  std::numeric_limits<T>::infinity() },
        { "+inf",  std::numeric_limits<T>::infinity() },
        { "-inf", -std::numeric_limits<T>::infinity() },
        {  "infinity",  std::numeric_limits<T>::infinity() },
        { "+infinity",  std::numeric_limits<T>::infinity() },
        { "-infinity", -std::numeric_limits<T>::infinity() }
      };
    if (auto search = infinitySpellings.find(specializedTypes::ci_string{strVal.c_str()});
        search != infinitySpellings.end())
    {
      val = search->second;
      return true;
    }

    return false;
  }




        /*
         *  The string 1.40129846432481707092372958328991613e-45 is the exact decimal representation of
         *  std::numeric_limits<float>::denorm_min() (the smallest positive subnormal float, equal to 2⁻¹⁴⁹),
         *  which triggers an underflow ERANGE error in std::strtof on major C++ standard libraries
         *  like GCC (libstdc++) and Clang (libc++).
         *
         *  Why This Exact Value Throws
         *
         *  • Subnormal Underflow Rules: While an IEEE 754 32-bit float can store 2⁻¹⁴⁹ in memory,
         *    its exponent bits are all zeros (0x00000001 in hex), making it a subnormal (denormalized) number
         *    rather than a normal number (which stops at FLT_MIN ≈ 1.17549e-38).
         *  • C Standard Library Behavior: Under the hood, std::stof calls C's strtof().
         *    According to the C/C++ standards, strtof is permitted to set errno = ERANGE
         *    whenever a non-zero result underflows below FLT_MIN—even if the value is
         *    still representable as a subnormal float.
         *  • Library Differences: On Linux/macOS (glibc / Apple libc), strtof sets ERANGE for subnormals,
         *    so std::stof throws std::out_of_range. On Windows (MSVC), strtof does not set ERANGE
         *    for representable subnormals, so the exact same code succeeds there.
         *
         *  How to Parse It Properly
         *
         *  Depending on your compiler and needs, use one of these approaches:
         *  1. Use std::from_chars (Recommended, C++17+)
         *     std::from_chars accurately parses all valid subnormals down to denorm_min()
         *     without setting ERANGE or throwing exceptions
         */






} // namespace converter
