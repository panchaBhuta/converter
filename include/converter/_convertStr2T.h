/*
 * _convertStr2T.h
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

#include <tuple>
#include <array>
#include <optional>
#include <type_traits>
#include <charconv>
#include <cstdlib>
#include <limits>
#include <cmath>
#include <locale>
#include <iomanip>
#include <concepts>
#include <cstddef>

#include <converter/_common.h>
#include <specializedTypes/CompTimeStr.h>

namespace converter
{

  // Enum : Conversion process for string to floating-point type conversion.
  enum struct Str2TnConversionProcess : int
              {
                FROM_CHARS = 1, // std::from_chars is locale-independent, non-allocating, and non-throwing
                // STR2N not used; as std::stoN eventually calls std::strtoN
                S2N = 2,
                // TODO test for Inf and -Inf as per example in this page   https://en.cppreference.com/w/cpp/string/byte/strtof
                STRINGSTREAM = 3,
                SPECIALIZED_IMPLEMENTATION = 4 };

  // Enum : Error Handling
  enum struct Str2TnErrorHandler : int
              {
                /*
                 * https://en.wikipedia.org/wiki/NaN
                 * In general, quiet NaNs, or qNaNs, do not raise any additional exceptions,
                 * as they propagate through most operations. But the invalid-operation exception
                 * is signaled by some operations that do not return a floating-point value,
                 * such as format conversions or certain comparison operations.
                 * The payload of a quiet NaN may be used to carry diagnostic information about
                 * the origin of the NaN value.
                */
                QUIET_NAN     = 0,    // for Floating-point
                /*
                 * https://en.wikipedia.org/wiki/NaN
                 * Signaling NaNs, or sNaNs, are special forms of a NaN that,
                 * when consumed by most operations, should raise the invalid operation exception
                 * and then, if appropriate, be "quieted" into a qNaN that may then propagate.
                 * sNaNs do not propagate. They are intended to be used for debugging purposes,
                 * to help identify uninitialized variables or other unexpected occurrences.
                */
                SIGNAL_NAN    = 1,    // for Floating-point
                INAPPLICABLE  = 2,    // for std::string
                THROW_ERROR   = 3,
                VARIANT_VAL   = 4 };  // for numeric's and std::chrono

  std::ostream& operator<<(std::ostream& os, Str2TnConversionProcess s2t)
  {
    switch (s2t) {
      case Str2TnConversionProcess::FROM_CHARS                 :  os << "FROM_CHARS";    break;
      case Str2TnConversionProcess::S2N                        :  os << "S2N";           break;
      case Str2TnConversionProcess::STRINGSTREAM               :  os << "STRINGSTREAM";  break;
      case Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION :  os << "SPECIALIZED_IMPLEMENTATION";       break;
      default                                                  :  os << "Str2TnConversionProcess::Unknown"; break;
    }
    return os;
  }

  std::ostream& operator<<(std::ostream& os, Str2TnErrorHandler err)
  {
    switch (err) {
      case Str2TnErrorHandler::QUIET_NAN                 :  os << "QUIET_NAN";    break;
      case Str2TnErrorHandler::SIGNAL_NAN                :  os << "SIGNAL_NAN";   break;
      case Str2TnErrorHandler::INAPPLICABLE              :  os << "INAPPLICABLE"; break;
      case Str2TnErrorHandler::THROW_ERROR               :  os << "THROW_ERROR";  break;
      case Str2TnErrorHandler::VARIANT_VAL               :  os << "VARIANT_VAL";  break;
      default                                            :  os << "Str2TnErrorHandler::Unknown"; break;
    }
    return os;
  }

  /**
    * @brief    This is a helper class to determine the return type of the conversion function
    *           based on the error handling process specified by 'ERR_HANDLER'.
    *           For e.g -> if 'ERR_HANDLER' is 'VARIANT_VAL', then the return type of the
    *           conversion function will be 'std::variant<T, std::string>',
    *           where 'T' is the numeric type being converted to   OR
    *           'std::string' is the error message in case of conversion failure.
    */
  template <typename T, Str2TnErrorHandler ERR_HANDLER >
  class ReturnType
  {
    using _return_types = std::tuple< T,                              //  QUIET_NAN
                                      T,                              //  SIGNAL_NAN
                                      T,                              //  INAPPLICABLE
                                      T,                              //  THROW_ERROR
                                      std::variant<T, std::string>    //  VARIANT_VAL
                                    >;
  public:
    using type = typename std::tuple_element< static_cast<size_t>(ERR_HANDLER), _return_types>::type;
  };



  // [=========[  concept :  is SPECIALIZED_IMPLEMENTATION for conversion available
  struct Error_InApplicable_True
  {
    constexpr static bool err_InApplicable = true;
    constexpr static bool err_ThrowError   = false;   // usually opposite of 'InApplicable'
    constexpr static bool err_VariantVal   = false;   // usually opposite of 'InApplicable'
  };

  struct Error_InApplicable_False
  {
    constexpr static bool err_InApplicable = false;
    constexpr static bool err_ThrowError   = true;    // usually opposite of 'InApplicable'
    constexpr static bool err_VariantVal   = true;    // usually opposite of 'InApplicable'
  };


  template<typename T>
  struct isS2TspecializedImplementationAvailable : std::false_type, Error_InApplicable_False {};

  //    "isS2TspecializedImplementationAvailable" defaulted to false, specilization is defined later.
  //    As single primary definition of "c_isS2TspecializedImplementationAvailable" introduces a cyclic dependency as below
  //
  //    ConvertFromStr<T,...>   ->
  //        DefaultStr2Tn::validConversionProcesses() ->
  //            c_isS2TspecializedImplementationAvailable   ->
  //                    ConvertFromStr::isConversionImplemented
  //
  //    template specialization of type-trait "isS2TspecializedImplementationAvailable",
  //    is used to break this cyclic dependency.
  template<typename T>
  concept c_isS2TspecializedImplementationAvailable = isS2TspecializedImplementationAvailable<T>::value;


  template<c_basic_string BSTR>
  struct isS2TspecializedImplementationAvailable<BSTR> : std::true_type, Error_InApplicable_True {};


  template<c_char T>
  struct isS2TspecializedImplementationAvailable<T> : std::true_type, Error_InApplicable_False {};
  // ]=========]  concept :  is SPECIALIZED_IMPLEMENTATION for conversion available


  // [=========[  concept :  is std::from_chars supported
  /*
    Supported Types
    The function provides overloads for the following arithmetic types:
    • Integer Types: All signed and unsigned integer types, including:
        ◦ Standard types: char, signed char, unsigned char, short, int, long, long long, and their unsigned counterparts.
        ◦ Fixed-width types: Such as int32_t or uint64_t (which are aliases for standard integer types).
        ◦ Note: bool is not supported.
    • Floating-Point Types:
        ◦ Standard types: float, double, and long double.
        ◦ Note: As of C++23, extended floating-point types (like std::float16_t or std::float128_t) are also supported where available.
  */
  template<typename T>  // broadly applicable to arithmetic-type <c_arithmetic T>
  struct is_from_chars_supported
  {
  private:
    // Helper to check for integer and floating-point overloads
    static constexpr bool _check()
    {
      if constexpr (  std::is_integral_v<T>
                      // for bool and char types, std::from_chars is not supported,
                      // but instead of compile-time error we get a run-time failure,
                      // so we exclude them here
                      && !std::is_same_v<T, bool>
                      && !converter::c_char<T>  )
      {
        return  requires(const char* first, const char* last, T& value1, int base)
                {
                  // Check for the basic (integer) version with optional base
                  { std::from_chars(first, last, value1, base) } -> std::same_as<std::from_chars_result>;
                };
      } else if constexpr (std::is_floating_point_v<T>) {
        return  requires(const char* first, const char* last, T& value1, std::chars_format fmt)
                {
                  // Check for the floating-point version which often needs a format flag
                  { std::from_chars(first, last, value1, fmt) } -> std::same_as<std::from_chars_result>;
                };
      }
      return false;
    }

  public:
    static constexpr bool value = _check();
  };

  template<typename T>
  concept c_isFromCharsSupported = is_from_chars_supported<T>::value;
  // ]=========]  concept :  is std::from_chars supported


  // [=========[  concept :  is std::sto* supported
  // Checks whether T can be obtained from std::string using any of the standard
  // std::sto* family of conversion functions.
  template<typename T>
  concept c_isS2NumSupported = c_arithmetic<T> &&
    (
      requires(const std::string& str) { { std::stoi  (str) } -> std::same_as<T>; }     ||   // int
      requires(const std::string& str) { { std::stol  (str) } -> std::same_as<T>; }     ||   // long
      requires(const std::string& str) { { std::stoll (str) } -> std::same_as<T>; }     ||   // long long
      requires(const std::string& str) { { std::stoul (str) } -> std::same_as<T>; }     ||   // unsigned long
      requires(const std::string& str) { { std::stoull(str) } -> std::same_as<T>; }     ||   // unsigned long long
      requires(const std::string& str) { { std::stof  (str) } -> std::same_as<T>; }     ||   // float
      requires(const std::string& str) { { std::stod  (str) } -> std::same_as<T>; }     ||   // double
      requires(const std::string& str) { { std::stold (str) } -> std::same_as<T>; }          // long double
    );
  // ]=========]  concept :  is std::sto* supported


  // [=========[  concept :  checks for "std::basic_istream<CharT,Traits>::operator>>"
  template< typename T,
            typename CharT = char,
            typename Traits = std::char_traits<CharT> >
  struct isExtractableFromStream
  {
    constexpr static bool value = (!c_basic_string<T>) &&
                                  //c_arithmetic<T> &&   // eliminate std::string here
                                  requires(std::basic_istream<CharT, Traits>& is, T& val)
                                  {
                                    { is >> val } -> std::same_as<std::basic_istream<CharT, Traits>&>;
                                  };
  };

  template <  typename T,
              typename CharT = char,
              typename Traits = std::char_traits<CharT> >
  concept c_isExtractableFromStream = isExtractableFromStream<T, CharT, Traits >::value;
  // ]=========]  concept :  checks for "std::basic_istream<CharT,Traits>::operator>>"



  template < c_arithmetic T, Str2TnConversionProcess CONV_PROCESS>
      requires ( CONV_PROCESS == Str2TnConversionProcess::S2N                ||
                 CONV_PROCESS == Str2TnConversionProcess::FROM_CHARS         ||
                 CONV_PROCESS == Str2TnConversionProcess::STRINGSTREAM   )
  struct isConversionS2NumCppSupported
    : std::bool_constant <
                           ( CONV_PROCESS == Str2TnConversionProcess::S2N           && c_isS2NumSupported<T>        ) ||
                           ( CONV_PROCESS == Str2TnConversionProcess::FROM_CHARS    && c_isFromCharsSupported<T>    ) ||
                           ( CONV_PROCESS == Str2TnConversionProcess::STRINGSTREAM  && c_isExtractableFromStream<T> )
                         > {};

  template < c_arithmetic T, Str2TnConversionProcess CONV_PROCESS>
      requires ( CONV_PROCESS == Str2TnConversionProcess::S2N                ||
                 CONV_PROCESS == Str2TnConversionProcess::FROM_CHARS         ||
                 CONV_PROCESS == Str2TnConversionProcess::STRINGSTREAM   )
  struct isConversionS2NumNotCppSupported
    : std::bool_constant <
                           ( CONV_PROCESS == Str2TnConversionProcess::S2N           && !c_isS2NumSupported<T>        ) ||
                           ( CONV_PROCESS == Str2TnConversionProcess::FROM_CHARS    && !c_isFromCharsSupported<T>    ) ||
                           ( CONV_PROCESS == Str2TnConversionProcess::STRINGSTREAM  && !c_isExtractableFromStream<T> )
                         > {};



  template < typename T, Str2TnConversionProcess CONV_PROCESS >
  struct isBumpedTypeS2NConversionAvailable
  {
    static constexpr bool value = false;
  };

  template < c_char T >
  struct isBumpedTypeS2NConversionAvailable<T, Str2TnConversionProcess::FROM_CHARS >
  {
    static constexpr bool value = false;
  };

  template < c_char T >
  struct isBumpedTypeS2NConversionAvailable<T, Str2TnConversionProcess::S2N >
  {
    static constexpr bool value = false;
  };

  template < c_arithmetic T, Str2TnConversionProcess CONV_PROCESS >
        requires ( isConversionS2NumCppSupported<T, CONV_PROCESS>::value )
  struct isBumpedTypeS2NConversionAvailable < T, CONV_PROCESS >
  {
    static constexpr bool value = false;
  };


  template < c_arithmetic T, Str2TnConversionProcess CONV_PROCESS >
        requires ( isConversionS2NumNotCppSupported<T, CONV_PROCESS>::value )
  struct isBumpedTypeS2NConversionAvailable < T, CONV_PROCESS >
  {
  private:
    // Return a type-identity for the nearest supported super-type (or void if none found)
    constexpr static auto _getNearestSuperTypeIdentity()
    {
      if constexpr ( c_integral<T> ) {
        if constexpr ( std::numeric_limits<T>::is_signed ) {
          if constexpr ( isConversionS2NumCppSupported<short, CONV_PROCESS>::value &&
                         std::numeric_limits<T>::max() <= std::numeric_limits<short>::max() ) {
            return std::type_identity<short>{};
          } else if constexpr ( isConversionS2NumCppSupported<int, CONV_PROCESS>::value &&
                                std::numeric_limits<T>::max() <= std::numeric_limits<int>::max() ) {
            return std::type_identity<int>{};
          } else if constexpr ( isConversionS2NumCppSupported<long, CONV_PROCESS>::value &&
                                std::numeric_limits<T>::max() <= std::numeric_limits<long>::max() ) {
            return std::type_identity<long>{};
          } else if constexpr ( isConversionS2NumCppSupported<long long, CONV_PROCESS>::value &&
                                std::numeric_limits<T>::max() <= std::numeric_limits<long long>::max() ) {
            return std::type_identity<long long>{};
          } else {
            return std::type_identity<void>{};
          }
        } else {
          if constexpr ( isConversionS2NumCppSupported<unsigned short, CONV_PROCESS>::value &&
                         std::numeric_limits<T>::max() <= std::numeric_limits<unsigned short>::max() ) {
            return std::type_identity<unsigned short>{};
          } else if constexpr ( isConversionS2NumCppSupported<unsigned int, CONV_PROCESS>::value &&
                                std::numeric_limits<T>::max() <= std::numeric_limits<unsigned int>::max() ) {
            return std::type_identity<unsigned int>{};
          } else if constexpr ( isConversionS2NumCppSupported<unsigned long, CONV_PROCESS>::value &&
                                std::numeric_limits<T>::max() <= std::numeric_limits<unsigned long>::max() ) {
            return std::type_identity<unsigned long>{};
          } else if constexpr ( isConversionS2NumCppSupported<unsigned long long, CONV_PROCESS>::value &&
                                std::numeric_limits<T>::max() <= std::numeric_limits<unsigned long long>::max() ) {
            return std::type_identity<unsigned long long>{};
          } else {
            return std::type_identity<void>{};
          }
        }
      } else if constexpr ( c_floating_point<T> ) {
        if constexpr ( isConversionS2NumCppSupported<float, CONV_PROCESS>::value &&
                       std::numeric_limits<T>::max() <= std::numeric_limits<float>::max() ) {
          return std::type_identity<float>{};
        } else if constexpr ( isConversionS2NumCppSupported<double, CONV_PROCESS>::value &&
                              std::numeric_limits<T>::max() <= std::numeric_limits<double>::max() ) {
          return std::type_identity<double>{};
        } else if constexpr ( isConversionS2NumCppSupported<long double, CONV_PROCESS>::value &&
                              std::numeric_limits<T>::max() <= std::numeric_limits<long double>::max() ) {
          return std::type_identity<long double>{};
        } else {
          return std::type_identity<void>{};
        }
      } else {
        return std::type_identity<void>{};
      }
    }

  template<bool isNearest>
  constexpr static bool _isBumped()
  {
    if constexpr (isNearest)
    {
      return ( std::numeric_limits<T>::max() <= std::numeric_limits<nearestSuperType>::max() );
    } else {
      return false;
    }
  }

  public:
    using nearestSuperType = typename decltype(_getNearestSuperTypeIdentity())::type;
    static constexpr bool hasNearest = !std::is_same_v< nearestSuperType, void >;

    static constexpr bool value = _isBumped<hasNearest>();
  };

  template <typename T>
  struct DefaultStr2Tn
  {
  private:
    static constexpr bool _isFromCharsSupported = c_isFromCharsSupported<T>
                                 || isBumpedTypeS2NConversionAvailable<T, Str2TnConversionProcess::FROM_CHARS>::value;
    static constexpr bool _isS2NumSupported = c_isS2NumSupported<T>
                                 || isBumpedTypeS2NConversionAvailable<T, Str2TnConversionProcess::S2N>::value;
    static constexpr bool _isExtractableFromStream = c_isExtractableFromStream<T>;
    static constexpr bool _isS2TspecializedImplementationAvailable = c_isS2TspecializedImplementationAvailable<T>;

    static constexpr size_t _arrSizeConversionProcesses =
                          size_t(_isFromCharsSupported)                      +
                          size_t(_isS2NumSupported)                          +
                          size_t(_isExtractableFromStream)                   +
                          size_t(_isS2TspecializedImplementationAvailable);

    static constexpr size_t _arrSizeErrorHandlers =
                                  size_t(std::numeric_limits<T>::has_quiet_NaN)       +  // has_q
                                  size_t(std::numeric_limits<T>::has_signaling_NaN)   +  // has_s
                                  size_t(isS2TspecializedImplementationAvailable<T>::err_ThrowError)  +
                                  size_t(isS2TspecializedImplementationAvailable<T>::err_VariantVal)  +
                                  size_t(isS2TspecializedImplementationAvailable<T>::err_InApplicable);

  public:

    using t_arrConversionProcesses = std::array < Str2TnConversionProcess, _arrSizeConversionProcesses >;
    using t_arrErrorHandlers       = std::array < Str2TnErrorHandler,      _arrSizeErrorHandlers >;

    // first element (index=0) of array is the default Conversion-Process for that type T
    constexpr static t_arrConversionProcesses validConversionProcesses()
    {

      static_assert( _arrSizeConversionProcesses > 0 , "Unhandled type for DefaultStr2Tn<T>::validConversionProcesses()" );

      t_arrConversionProcesses arrPossibilities{};
      size_t arrIdx = 0;

      if constexpr (_isFromCharsSupported)                    arrPossibilities[arrIdx++] = Str2TnConversionProcess::FROM_CHARS;
      if constexpr (_isS2NumSupported)                        arrPossibilities[arrIdx++] = Str2TnConversionProcess::S2N;
      if constexpr (_isExtractableFromStream)                 arrPossibilities[arrIdx++] = Str2TnConversionProcess::STRINGSTREAM;
      if constexpr (_isS2TspecializedImplementationAvailable) arrPossibilities[arrIdx++] = Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION;

      return arrPossibilities;
    }

    // first element (index=0) of array is the default ErrHandler for that type T
    constexpr static t_arrErrorHandlers validErrorHandlers()
    {
      t_arrErrorHandlers arrPossibilities{};
      size_t arrIdx = 0;

      if constexpr (std::numeric_limits<T>::has_quiet_NaN)                         arrPossibilities[arrIdx++] = Str2TnErrorHandler::QUIET_NAN;   // has_q
      if constexpr (std::numeric_limits<T>::has_signaling_NaN)                     arrPossibilities[arrIdx++] = Str2TnErrorHandler::SIGNAL_NAN;  // has_s
      if constexpr (isS2TspecializedImplementationAvailable<T>::err_ThrowError)    arrPossibilities[arrIdx++] = Str2TnErrorHandler::THROW_ERROR;
      if constexpr (isS2TspecializedImplementationAvailable<T>::err_VariantVal)    arrPossibilities[arrIdx++] = Str2TnErrorHandler::VARIANT_VAL;
      if constexpr (isS2TspecializedImplementationAvailable<T>::err_InApplicable)  arrPossibilities[arrIdx++] = Str2TnErrorHandler::INAPPLICABLE;

      return arrPossibilities;
    }
  };




  template <  typename T,
              Str2TnConversionProcess CONV_PROCESS = DefaultStr2Tn<T>::validConversionProcesses()[0],
              Str2TnErrorHandler ERR_HANDLER = DefaultStr2Tn<T>::validErrorHandlers()[0]  >
  struct ConvertFromStr
  {
    constexpr static size_t templateId = 0;
    constexpr static bool isConversionImplemented = false;
  };









  // [[============[[ S2T-converter :: concept
  // CFS -> Converter Function from String
  template <typename CFS>
  concept c_S2Tconverter =  requires (const std::string& str)
                            {
                              // 1. Check for nested type definitions
                              typename CFS::value_type;
                              typename CFS::return_type;

                              // Verifies CFS::ToVal(str) is a valid expression without an instance
                              { CFS::ToVal(str) } -> std::same_as<typename CFS::return_type>;
                            };

  template <typename CFS>
  concept c_NOT_S2Tconverter = !c_S2Tconverter<CFS>;
  // ]]============]] S2T-converter :: concept





  template < typename T, Str2TnConversionProcess CONV_PROCESS>
  struct isSupported_Str2TnConversionProcess
      // NOTE: validConversionProcesses() is inclusive of isBumpedTypeS2NConversionAvailable<>::true_type
    : std::bool_constant < check_any_of( DefaultStr2Tn<T>::validConversionProcesses(), CONV_PROCESS ) > {};


  template < typename T, Str2TnErrorHandler ERR_HANDLER>
  struct isSupported_Str2TnErrorHandler
    : std::bool_constant < check_any_of( DefaultStr2Tn<T>::validErrorHandlers(), ERR_HANDLER ) > {};



  template <typename T>
  //  requires c_arithmetic<T>
  struct Format_FromChars;

  template <c_integral T>
  struct Format_FromChars<T>
  {
    using type = int;

    constexpr static type getDefaultFormatArgs() { return 10; } // base 10 is the default for std::from_chars for integral types, but providing this function for consistency and future extensibility
  };

  template <c_floating_point T>
  struct Format_FromChars<T>
  {
    using type = std::chars_format;

    constexpr static type getDefaultFormatArgs() { return std::chars_format::general; } // std::chars_format::general is the default for std::from_chars for floating-point types, but providing this function for consistency and future extensibility
  };




  template <typename T>
  //  requires c_arithmetic<T>
  struct Format_S2N;

  struct format_s2n_integertype
  {
    std::size_t* pos; // = nullptr,
    int base; // = 10
    format_s2n_integertype(std::size_t* pos_arg = nullptr, int base_arg = 10)
      : pos(pos_arg), base(base_arg) {}

    format_s2n_integertype(const format_s2n_integertype& other) = default;
    format_s2n_integertype& operator=(const format_s2n_integertype& other) = default;
    format_s2n_integertype(format_s2n_integertype&& other) = default;
    format_s2n_integertype& operator=(format_s2n_integertype&& other) = default;
  };

  template <c_integral T>
  struct Format_S2N<T>
  {
    using type = format_s2n_integertype;

    static type getDefaultFormatArgs() { return { nullptr, 10 }; } // pos, base
  };

  template <c_floating_point T>
  struct Format_S2N<T>
  {
    using type = std::size_t*;

    static type getDefaultFormatArgs() { return nullptr; } // pos
  };








  template <typename T, Str2TnConversionProcess CONV_PROCESS >
          requires ( isSupported_Str2TnConversionProcess<T, CONV_PROCESS>::value )
  struct FormatInfo< T, CONV_PROCESS >
  {
    constexpr static auto getDefaultFormatArgs()
    {
      if constexpr (CONV_PROCESS == Str2TnConversionProcess::FROM_CHARS) {
        return Format_FromChars<T>::getDefaultFormatArgs();
      } else if constexpr (CONV_PROCESS == Str2TnConversionProcess::S2N) {
        return Format_S2N<T>::getDefaultFormatArgs();
      } else if constexpr (CONV_PROCESS == Str2TnConversionProcess::STRINGSTREAM) {
        return Format_StringStream<T>::getDefaultFormatArgs();
      } else if constexpr (CONV_PROCESS == Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION) {
        return Format_SpecializedImplementation<T>::getDefaultFormatArgs();
      }
    }

    using type = decltype(getDefaultFormatArgs());
  };



  template < typename T >
  class Str2Thelper
  {
    constexpr static DefaultStr2Tn<T>::t_arrConversionProcesses
      _ArrConversionProcesses = DefaultStr2Tn<T>::validConversionProcesses();

  public:
    // Helper to unpack the array into the template
    template <std::size_t... Is>
    static    std::tuple< typename FormatInfo<T, _ArrConversionProcesses[Is]>::type... >
    make_default_formatInfo(std::index_sequence<Is...>)
    {
      return std::tuple< typename FormatInfo<T, _ArrConversionProcesses[Is]>::type... >
                                { FormatInfo<T, _ArrConversionProcesses[Is]>::getDefaultFormatArgs()... };
    }

    using t_formatInfoTypesTuple =
        decltype(
            make_default_formatInfo (
                std::make_index_sequence<
                    std::tuple_size_v<decltype(_ArrConversionProcesses)>
                                        > {}
                                    )
                );
  };


  template < typename T, Str2TnErrorHandler ERR_HANDLER >
          requires ( ERR_HANDLER != Str2TnErrorHandler::INAPPLICABLE &&
                     isSupported_Str2TnErrorHandler<T, ERR_HANDLER>::value )
  struct ErrorHandler
  {
    using return_type = typename ReturnType<T, ERR_HANDLER>::type;
    using value_type  = T;

    template < typename E = std::invalid_argument >
    static return_type handle(const std::string& str, const std::string& errMsg)
    {
      if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
      {
        throw E(errMsg);
      } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL ) {
        return return_type{str};
      } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ) {
        if constexpr ( std::numeric_limits<T>::has_quiet_NaN )
        {
          try {
            if constexpr (std::is_same_v<T, float>) {
              return std::nanf(str.c_str());
            } else if constexpr (std::is_same_v<T, double>) {
              return std::nan(str.c_str());
            } else if constexpr (std::is_same_v<T, long double>) {
              return std::nanl(str.c_str());
            } else {
              static_assert(always_false<T>::value, "Unsupported type for FP-conversion");
            }
          } catch (...) {
            // In case if the implementation doesn't support string argument for nan functions, we can return a default quiet NaN value.
            CONVERTER_DEBUG_LOG( "nan() with string argument not supported by this implementation. Returning default quiet NaN value." );
            return std::numeric_limits<T>::quiet_NaN();
          }
        }
      } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN ) {
        if constexpr ( std::numeric_limits<T>::has_signaling_NaN )
        {
          return std::numeric_limits<T>::signaling_NaN();
        }
      } else {
        static_assert(always_false<T>::value, "Unsupported enum Str2TnErrorHandler for FP-conversion");
      }
    }
  };












  template < c_isFromCharsSupported T, Str2TnErrorHandler ERR_HANDLER >
          requires ( isSupported_Str2TnErrorHandler<T, ERR_HANDLER>::value &&
                     isSupported_Str2TnConversionProcess<T, Str2TnConversionProcess::FROM_CHARS>::value )
  struct ConvertFromStr< T, Str2TnConversionProcess::FROM_CHARS, ERR_HANDLER >
  {
    constexpr static size_t templateId = 1;
    constexpr static bool isConversionImplemented = true;
    using value_type  = T;
    using return_type = typename ReturnType<T, ERR_HANDLER>::type;
    constexpr static Str2TnErrorHandler errHandler = ERR_HANDLER;
    constexpr static Str2TnConversionProcess conversionProcess = Str2TnConversionProcess::FROM_CHARS;

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToVal(const std::string& str,
          const typename Format_FromChars<T>::type format_args =
                         Format_FromChars<T>::getDefaultFormatArgs()  )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< T = [" << specializedTypes::get_name<T>() << "], Str2TnConversionProcess::FROM_CHARS, " << ERR_HANDLER << " >::ToVal('" << str << "')");
      //  https://en.cppreference.com/w/cpp/utility/from_chars
      //  std::from_chars is locale-independent, non-allocating, and non-throwing
      T result{};

      auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result, format_args); // throws nothing

      if (ec == std::errc()) {
        return result;
      } else {
        std::stringstream errMsg;
        errMsg << std::make_error_code(ec).message() << " : 'return_type ConvertFromStr< T = [" << specializedTypes::get_name<T>()
               << "], Str2TnConversionProcess::FROM_CHARS, " << ERR_HANDLER << " >::ToVal(const std::string& str)'";

        CONVERTER_DEBUG_LOG( errMsg.str() << "  str = '" << str << "' ; result = " << result );

        if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
        {
          switch(ec)
          {
          case std::errc::invalid_argument:
            throw std::invalid_argument(errMsg.str());
          case std::errc::result_out_of_range:
            throw std::out_of_range(errMsg.str());
          default:
            throw std::logic_error(errMsg.str());
          }
        } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL ) {
          return return_type{str};
        } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN ) {
          return std::numeric_limits<T>::signaling_NaN();
        } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ) {
          return std::numeric_limits<T>::quiet_NaN();
        } else {
          static_assert(always_false<T>::value, "Unsupported Str2TnErrorHandler for FP-conversion");
        }
      }
    }
  };

  template < c_isExtractableFromStream T, Str2TnErrorHandler ERR_HANDLER >
          requires ( isSupported_Str2TnErrorHandler<T, ERR_HANDLER>::value &&
                     isSupported_Str2TnConversionProcess<T, Str2TnConversionProcess::STRINGSTREAM>::value )
  struct ConvertFromStr< T, Str2TnConversionProcess::STRINGSTREAM, ERR_HANDLER >
  {
    constexpr static size_t templateId = 2;
    constexpr static bool isConversionImplemented = true;
    using value_type  = T;
    using return_type = typename ReturnType<T, ERR_HANDLER>::type;
    constexpr static Str2TnErrorHandler errHandler = ERR_HANDLER;
    constexpr static Str2TnConversionProcess conversionProcess = Str2TnConversionProcess::STRINGSTREAM;

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToVal(const std::string& str)
    {
      // refer https://en.cppreference.com/w/cpp/io/basic_istream/operator_gtgt.html
      // std::from_chars() can be used in situations where maximum accuracy is important.

      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< T = [" << specializedTypes::get_name<T>() << "], Str2TnConversionProcess::STRINGSTREAM, " << ERR_HANDLER << " >::ToVal('" << str << "')");

      std::istringstream iss(str);
      return _ToVal(str, iss);
    }

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @param   format_args         locale for stringstream based conversion.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToVal(const std::string& str,
          const typename Format_StringStream<T>::type& format_ss) // = Format_StringStream<T>::getDefaultFormatArgs() )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< T = [" << specializedTypes::get_name<T>() << "], Str2TnConversionProcess::STRINGSTREAM, " << ERR_HANDLER << " >::ToVal('" << str << "', format_ss)");

      std::istringstream iss(str);
      format_ss.applyFormatArgs(iss); // set locale for stringstream based conversion
      return _ToVal(str, iss);
    }

  private:
    inline static bool _checkStreamFailure(const std::istringstream& iss)
    {
      if constexpr (c_char<T>)
        return ( iss.fail() || iss.bad() );  // for char-type, !iss.eof() check doesn't work as expected
      else
        return ( iss.fail() || iss.bad() || (!iss.eof()) );
    }

    inline static return_type
    _ToVal(const std::string& str,
           std::istringstream& iss)
    {
      static const std::string funcName = std::string{" : 'return_type ConvertFromStr< T = ["} + specializedTypes::get_name<T>().data
          + "], Str2TnConversionProcess::STRINGSTREAM, ERR_HANDLER >::_ToVal(const std::string& str, std::istringstream& iss)'";

      T result{};
      try
      {
        iss >> result;
      } catch(const std::exception& e) {
          const std::string errMsg = "Stream read failure. " + funcName + " :: input-string-stream-failure for str='" + str + "' errMsg=" + e.what();
          CONVERTER_DEBUG_LOG( errMsg );

          return ErrorHandler<T, ERR_HANDLER>::handle(str, errMsg);
      }


      // operator>> for built-in types (except char, char8_t, char16_t, char32_t, and bool)
      // will set the failbit if the input cannot be parsed as a valid value of the type.
      // For char, char8_t, char16_t, char32_t, and bool types, operator>> will set the failbit
      // if the input cannot be parsed as a valid value of the type or if it contains more than one character.
      if (_checkStreamFailure(iss)) {
        std::string errMsg = "Stream read failure. " + funcName + " :: input-string-stream-failure for str='" + str + "'";
        CONVERTER_DEBUG_LOG( errMsg << std::boolalpha << " iss.fail()=" << iss.fail() << "  iss.bad()="   \
                                    << iss.bad() << "  iss.eof()=" << iss.eof());
        return ErrorHandler<T, ERR_HANDLER>::handle(str, errMsg);
      } else {
        return result;
      }
    }
  };




  template< c_isS2NumSupported T, Str2TnErrorHandler ERR_HANDLER>
          requires ( c_integral<T> &&
                     isSupported_Str2TnErrorHandler<T, ERR_HANDLER>::value &&
                     isSupported_Str2TnConversionProcess<T, Str2TnConversionProcess::S2N>::value )
  struct ConvertFromStr< T, Str2TnConversionProcess::S2N, ERR_HANDLER >
  {
    constexpr static size_t templateId = 3;
    constexpr static bool isConversionImplemented = true;
    using value_type  = T;
    using return_type = typename ReturnType<T, ERR_HANDLER>::type;
    constexpr static Str2TnErrorHandler errHandler = ERR_HANDLER;
    constexpr static Str2TnConversionProcess conversionProcess = Str2TnConversionProcess::S2N;

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToVal(const std::string& str,
          typename Format_S2N<T>::type format_args =
                   Format_S2N<T>::getDefaultFormatArgs()  )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< c_integral T = [" << specializedTypes::get_name<T>() << "], Str2TnConversionProcess::S2N, " << ERR_HANDLER << " >::ToVal('" << str << "', optional_format_args)");

      try
      {
        return _ToVal(str, format_args);
      } catch (const std::invalid_argument& e) {
        const std::string errMsg = "Invalid argument: " + std::string(e.what()) + " :: str = '" + str + "'";
        CONVERTER_DEBUG_LOG( errMsg );
        if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
        {
          throw e;
        }
      } catch (const std::out_of_range& e) {
        const std::string errMsg = "Out of range: " + std::string(e.what()) + " :: str = '" + str + "'";
        CONVERTER_DEBUG_LOG( errMsg );
        if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
        {
          throw e;
        }
      }

      if constexpr ( ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL )
      {
        return return_type{str};
      }
    }

  private:
    inline static T _ToVal( const std::string& str,
                            typename Format_S2N<T>::type format_args )
        // throws  std::invalid_argument, std::out_of_range
    {
      if constexpr (std::is_same_v<T, int>) {
        return std::stoi(str, format_args.pos, format_args.base);
      } else if constexpr (std::is_same_v<T, long>) {
        return std::stol(str, format_args.pos, format_args.base);
      } else if constexpr (std::is_same_v<T, long long>) {
        return std::stoll(str, format_args.pos, format_args.base);
      } else if constexpr (std::is_same_v<T, unsigned long>) {
        return std::stoul(str, format_args.pos, format_args.base);
      } else if constexpr (std::is_same_v<T, unsigned long long>) {
        return std::stoull(str, format_args.pos, format_args.base);
      } else {
        static_assert(always_false<T>::value, "Unsupported type for conversion");
      }
    }
  };

  template < c_isS2NumSupported T, Str2TnErrorHandler ERR_HANDLER >
          requires ( c_floating_point<T> &&
                     isSupported_Str2TnErrorHandler<T, ERR_HANDLER>::value &&
                     isSupported_Str2TnConversionProcess<T, Str2TnConversionProcess::S2N>::value )
  struct ConvertFromStr< T, Str2TnConversionProcess::S2N, ERR_HANDLER >
  {
    constexpr static size_t templateId = 4;
    constexpr static bool isConversionImplemented = true;
    using value_type  = T;
    using return_type = typename ReturnType<T, ERR_HANDLER>::type;
    constexpr static Str2TnErrorHandler errHandler = ERR_HANDLER;
    constexpr static Str2TnConversionProcess conversionProcess = Str2TnConversionProcess::S2N;

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToVal(const std::string& str,
          typename Format_S2N<T>::type format_args =
                   Format_S2N<T>::getDefaultFormatArgs()  )
    {
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_floating_point T = [" << specializedTypes::get_name<T>() << "], Str2TnConversionProcess::S2N, " << ERR_HANDLER << " >::ToVal_args('" << str << "', optional_format_args)");

      try
      {
        T val = _ToVal(str, format_args);

        if (std::isnan(val))
        {
          const std::string errMsg = "Conversion resulted in nan value: " + std::to_string(val) + " :: str = '" + str + "'";
          CONVERTER_DEBUG_LOG( errMsg );
          if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR ) {
            throw errMsg;
          } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL ) {
            return return_type{str};
          } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ) {
            return val;
          } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN ) {
            //return std::numeric_limits<T>::signaling_NaN();
            return val;
          } else {
            static_assert(always_false<T>::value, "Unsupported Str2TnErrorHandler for FP-conversion");
          }
        }
        return val;

      } catch (const std::invalid_argument& e) {
        const std::string errMsg = "Invalid argument: " + std::string(e.what()) + " :: str = '" + str + "'";
        CONVERTER_DEBUG_LOG( errMsg );
        if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
        {
          throw e;
        }
      } catch (const std::out_of_range& e) {
        const std::string errMsg = "Out of range: " + std::string(e.what()) + " :: str = '" + str + "'";
        CONVERTER_DEBUG_LOG( errMsg );
        if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
        {
          throw e;
        }
      } catch (const std::string& errMsg) {
        CONVERTER_DEBUG_LOG( errMsg );
        if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
        {
          throw std::invalid_argument(errMsg);
        }
      }

      if constexpr ( ERR_HANDLER != Str2TnErrorHandler::THROW_ERROR ) //
      { // errMsg is already logged in catch block above for THROW_ERROR conditions,
        // so passing empty string here as THROW_ERROR case is already handled in catch block above.
        return ErrorHandler<T, ERR_HANDLER>::handle(str, "");
      }
    }

  private:
    inline static T _ToVal(const std::string& str, std::size_t* pos )
        // throws  std::invalid_argument, std::out_of_range
    {
      T val;
      if constexpr (std::is_same_v<T, float>) {
        val = std::stof(str, pos);  // throws  std::invalid_argument, std::out_of_range
      } else if constexpr (std::is_same_v<T, double>) {
        val = std::stod(str, pos);
      } else if constexpr (std::is_same_v<T, long double>) {
        val = std::stold(str, pos);
      } else {
        static_assert(always_false<T>::value, "Unsupported type for conversion");
      }
      return val;
    }
  };



  /**
   * @brief   If 'T' is not supported by a particular conversion family functions,
   *          then delegate the call to T's super-type.
   * @tparam  ERR_HANDLER             enum variable of type Str2TnErrorHandler.
   */
  template< c_arithmetic T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER>
          requires ( isSupported_Str2TnErrorHandler<T, ERR_HANDLER>::value    &&
                     isBumpedTypeS2NConversionAvailable<T, CONV_PROCESS>::value )
  struct ConvertFromStr<T, CONV_PROCESS, ERR_HANDLER >
  {
    using delegated_type = typename isBumpedTypeS2NConversionAvailable<T, CONV_PROCESS>::nearestSuperType;
    constexpr static size_t delegated_templateId = ConvertFromStr<delegated_type, CONV_PROCESS, ERR_HANDLER >::templateId;

    constexpr static size_t templateId = 5;
    constexpr static bool isConversionImplemented = true;
    using value_type  = T;
    using return_type = typename ReturnType<T, ERR_HANDLER>::type;
    constexpr static Str2TnErrorHandler errHandler = ERR_HANDLER;
    constexpr static Str2TnConversionProcess conversionProcess = CONV_PROCESS;

    /**
     * @brief   Converts string holding T value.
     * @param   str                 input string.
     * @returns T.
     */
    inline static return_type
    ToVal(const std::string& str,
          typename FormatInfo<T, CONV_PROCESS>::type format_args =
                   FormatInfo<T, CONV_PROCESS>::getDefaultFormatArgs()  )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< T = [" << specializedTypes::get_name<T>() << "] -> delegated_type = [" << specializedTypes::get_name<delegated_type>() << "] , " << CONV_PROCESS << ", " << ERR_HANDLER << " >::ToVal('" << str << "')");

      typename FormatInfo<delegated_type, CONV_PROCESS>::type delegate_format_args{format_args};
      // val can be variant type
      auto val = ConvertFromStr<delegated_type, CONV_PROCESS, ERR_HANDLER >::ToVal(str, delegate_format_args); // for Str2TnConversionProcess::S2N , throws std::invalid_argument, std::out_of_range
      delegated_type act_val{};

      if constexpr ( ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL )
      {
        if ( val.index() != 0 ) // i.e is error, has string value
        {
          return_type errVal{std::get<std::string>(val)}; // throws std::bad_variant_access; but we donot expect this to be thrown here
          return errVal; // return string
        }
        act_val = std::get<delegated_type>(val); // throws std::bad_variant_access; but we donot expect this to be thrown here
      } else {
        act_val = val;
      }

      if constexpr ( std::numeric_limits<T>::max() < std::numeric_limits<delegated_type>::max() )
      {
        // constexpr should evaluate to true most of the times.
        if ( act_val < std::numeric_limits<T>::min() ||
             act_val > std::numeric_limits<T>::max() )
        {
          const std::string errMsg = "out of range : ConvertFromStr<delegated_type,..>::ToVal(str) :: '" + str + "'  is outside range of std::numeric_limits<T>::[max..min]";
          CONVERTER_DEBUG_LOG( errMsg );
          return ErrorHandler<T, ERR_HANDLER>::template handle<std::out_of_range>(str, errMsg);
        }
      } else if constexpr ( std::numeric_limits<T>::max() == std::numeric_limits<delegated_type>::max() ) {
        // on extremly rare occasion, if for e.g. int and int32 byte-size is, but are two different types, instead of alias,
        // and int is supported for conversion, but int32 is not.
        // by skipping the run-time min() max() check, Squeeze out performance if it's there.
      } else {
        static_assert(always_false<T>::value, "abort!  ABORT!! How did this happen?");
      }

      const T ret_val{static_cast<T>(act_val)};
      if constexpr ( ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL )
      {
        return_type retVal{ret_val};
        return retVal;
      } else {
        return ret_val;
      }
    }
  };





  /**
   * @brief     Specialized implementation handling string to string conversion.
   */
  template<>
          //requires ( isSupported_Str2TnErrorHandler<std::string, Str2TnErrorHandler::INAPPLICABLE> &&
          //           isSupported_Str2TnConversionProcess<std::string, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION> )
  struct ConvertFromStr< std::string, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION,
                                      Str2TnErrorHandler::INAPPLICABLE >
  {
    constexpr static size_t templateId = 6;
    constexpr static bool isConversionImplemented = true;
    using value_type  = std::string;
    using return_type = std::string;
    constexpr static Str2TnErrorHandler errHandler = Str2TnErrorHandler::INAPPLICABLE;
    constexpr static Str2TnConversionProcess conversionProcess = Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION;

    /**
     * @brief   Converts string to string.
     * @param   str                 input string.
     * @returns string.
     */
    inline static std::string ToVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< std::string , Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION, Str2TnErrorHandler::INAPPLICABLE >::ToVal('" << str << "')");
      return str;
    }
  };
  template<c_basic_string BSTR>
          requires ( isSupported_Str2TnErrorHandler<BSTR, Str2TnErrorHandler::INAPPLICABLE>::value &&
                     isSupported_Str2TnConversionProcess<BSTR, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION>::value )
  struct ConvertFromStr< BSTR, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION,
                               Str2TnErrorHandler::INAPPLICABLE >
  {
    constexpr static size_t templateId = 7;
    constexpr static bool isConversionImplemented = true;
    using value_type  = BSTR;
    using return_type = BSTR;
    constexpr static Str2TnErrorHandler errHandler = Str2TnErrorHandler::INAPPLICABLE;
    constexpr static Str2TnConversionProcess conversionProcess = Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION;

    /**
     * @brief   Converts string to string.
     * @param   str                 input string.
     * @returns string.
     */
    inline static BSTR ToVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< T = [" << specializedTypes::get_name<BSTR>() << "] , Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION, Str2TnErrorHandler::INAPPLICABLE >::ToVal('" << str << "')");
      BSTR bstr = str.c_str();
      return bstr;
    }
  };

  /**
   * @brief     Specialized implementation handling string to char conversion.
   * @tparam  CH                     'char-type' converted to, from string data.
   */
  template<c_char CH, Str2TnErrorHandler ERR_HANDLER>
          requires ( isSupported_Str2TnErrorHandler<CH, ERR_HANDLER>::value &&
                     isSupported_Str2TnConversionProcess<CH, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION>::value )
  struct ConvertFromStr< CH, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION, ERR_HANDLER >
  {
    constexpr static size_t templateId = 8;
    constexpr static bool isConversionImplemented = true;
    using value_type  = CH;
    using return_type = typename ReturnType<CH, ERR_HANDLER>::type;
    constexpr static Str2TnErrorHandler errHandler = ERR_HANDLER;
    constexpr static Str2TnConversionProcess conversionProcess = Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION;

    /**
     * @brief   Converts string holding char value.
     * @param   str                 input string.
     * @returns char-type.
     */
    inline static return_type
    ToVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< T = [" << specializedTypes::get_name<CH>() << "] , Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION, " << ERR_HANDLER << " >::ToVal('" << str << "')");
      if(str.length()>1)
      {
        std::string errMsg = "String has more than one character. 'CH ConvertFromStr<c_char CH, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION, ERR_HANDLER >::ToVal(const std::string& str)' string2charT-conversion-failure for value '" + str + "'";
        CONVERTER_DEBUG_LOG( errMsg );
        return ErrorHandler<CH, ERR_HANDLER>::handle(str, errMsg);
      }

      if constexpr( std::is_same<CH, std::string::value_type>::value )
      {
        return str[0];
      } else {
        return static_cast<CH>(str[0]);
      }
    }
  };
  // ]=============================================================] ConvertFromStr




  // [=============================================================[ S2T conversion-type definition
  template< typename T,
            auto (*CONV_S2T)(const std::string&)
          >
  struct S2TwrapperFunction
  {
    using value_type  = T;
    using return_type = typename std::invoke_result_t< decltype(CONV_S2T),
                                                       const std::string& >;

    inline static return_type
    ToVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: S2TwrapperFunction< T = [" << specializedTypes::get_name<T>() << "], ... >::ToVal('" << str << "')");
      CONVERTER_DEBUG_TRY_START
        return CONV_S2T(str);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }
  };

  /**
   * @brief   If a 'type-C' satisfies concept 'c_S2Tconverter', then use that 'type-C';   else
   *          assume it's a 'type-T' and bumped up using 'ConvertFromStr< type-T >' to create
   *          a class staisfying concept 'c_S2Tconverter'.
   *          This mechanism enables 'template-converter-algorithm' to handle both 'type-C' and
   *          'type-T' using the same code base, (i.e. reduces code duplicity of Getters and Setters functions).
   * @tparam  T_C                   T can be data-type such as int, double etc; xOR
   *                                C -> Conversion class statisfying concept 'c_S2Tconverter'.
   */
  template< typename T_C >
  struct t_S2Tconv;

  template< c_NOT_S2Tconverter T >
  struct t_S2Tconv<T> {
    using conv_type = ConvertFromStr< T >;
  };

  template< c_S2Tconverter C >
  struct t_S2Tconv<C> {
    using conv_type = C;
  };



  template< typename T_C >
  using t_S2Tconv_c = typename t_S2Tconv<T_C>::conv_type;

  /**
   * @brief   convert a function with signature 'auto (*CONV_S2T)(const std::string&)'
   *          to a converter type that satisfies concept 'c_S2Tconverter'.
   * @tparam  CONV_S2T                   a function with signature 'T_V (*CONV_S2T)(const std::string&)'.
   *                                     T_V represents either numeric type 'T' or 'variant std::variant<T, std::string>'
   */
  template< auto (*CONV_S2T)(const std::string&) >
  struct f_S2Tconv
  {
  private:
    /**
     * @brief return type of function CONV_S2T
     */
    using _return_type = typename std::invoke_result_t< decltype(CONV_S2T),
                                                        const std::string& >;
    /**
     * @brief infers the underlying data-type if _return_type is an variant instance.
     *        or it's same as return type of CONV_S2T.
     */
    using _value_type  = typename InferValueType<_return_type>::value_type;
  public:
    /**
     * @brief a converter alias that satisfies concept 'c_S2Tconverter'
     */
    using conv_type = S2TwrapperFunction< _value_type, CONV_S2T >;
  };

  template< auto (*CONV_S2T)(const std::string&) >
  using f_S2Tconv_c = typename f_S2Tconv<CONV_S2T>::conv_type;

  // ]=============================================================] S2T conversion-types definition



} // namespace converter
