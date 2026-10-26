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

#include <string>
#include <variant>
#include <sstream>
#include <ostream>

#include <system_error>
#include <stdexcept>

#include <converter/_common.h>
#include <specializedTypes/CompTimeStr.h>
#include <specializedTypes/case_insensitive_string.h>

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
  // for more details refer comments of template class ErrorHandler
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
                QUIET_NAN     = 0,    // for Floating-point only
                /*
                 * https://en.wikipedia.org/wiki/NaN
                 * Signaling NaNs, or sNaNs, are special forms of a NaN that,
                 * when consumed by most operations, should raise the invalid operation exception
                 * and then, if appropriate, be "quieted" into a qNaN that may then propagate.
                 * sNaNs do not propagate. They are intended to be used for debugging purposes,
                 * to help identify uninitialized variables or other unexpected occurrences.
                */
                SIGNAL_NAN    = 1,    // for Floating-point only
                INAPPLICABLE  = 2,    // for std::string only
                THROW_ERROR   = 3,    // if Floating-Point then treat NAN's as exception
                VARIANT_VAL   = 4 };  // Preserve the input in a typed variant alternative for NaN or conversion failure.

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
    *           conversion function will be 'variantReturnType<T>',
    *           where 'T' is the target type, Str2TNaNInput preserves the input
    *           when conversion produces NaN, and Str2TConversionFailure preserves
    *           the input when conversion fails.
    */
  template <typename T, Str2TnErrorHandler ERR_HANDLER >
  class ReturnType
  {
    using _return_types = std::tuple< T,                              //  QUIET_NAN
                                      T,                              //  SIGNAL_NAN
                                      T,                              //  INAPPLICABLE
                                      T,                              //  THROW_ERROR
                                      variantReturnType<T>            //  VARIANT_VAL
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
  struct isConversionS2NumCppSupported
    : std::bool_constant <
                           ( CONV_PROCESS == Str2TnConversionProcess::S2N           && c_isS2NumSupported<T>        ) ||
                           ( CONV_PROCESS == Str2TnConversionProcess::FROM_CHARS    && c_isFromCharsSupported<T>    ) ||
                           ( CONV_PROCESS == Str2TnConversionProcess::STRINGSTREAM  && c_isExtractableFromStream<T> )
                         > {};

  template < c_arithmetic T, Str2TnConversionProcess CONV_PROCESS>
  struct isConversionS2NumNotCppSupported
    : std::bool_constant <
                           ( CONV_PROCESS == Str2TnConversionProcess::S2N           && !c_isS2NumSupported<T>        ) ||
                           ( CONV_PROCESS == Str2TnConversionProcess::FROM_CHARS    && !c_isFromCharsSupported<T>    ) ||
                           ( CONV_PROCESS == Str2TnConversionProcess::STRINGSTREAM  && !c_isExtractableFromStream<T> )
                         > {};


  // Primary template:
  // Bumped conversion is not applicable.
  template < typename T, Str2TnConversionProcess CONV_PROCESS >
  struct isBumpedTypeS2NConversionAvailable
  {
    // void means this template is not applicable to a
    // numeric bump conversion (primary template).
    using nearestSuperType = void;

    static constexpr bool value = false;
  };


  template <c_numeric T, Str2TnConversionProcess CONV_PROCESS>
      requires (
          isConversionS2NumNotCppSupported<T, CONV_PROCESS>::value
      )
  struct isBumpedTypeS2NConversionAvailable<T, CONV_PROCESS>
      : isBumpedTypeConversionAvailable<
            T,
            CONV_PROCESS,
            isConversionS2NumCppSupported>
  {
  public:
    // std::nullptr_t       -> bumping applicable, but no supported super-type.                      value = false
    // numeric ST           -> bumping available for the type T. type T bumped to super-type ST.     value = true

    using nearestSuperType =
        typename decltype(
            isBumpedTypeConversionAvailable<
                  T,
                  CONV_PROCESS,
                  isConversionS2NumCppSupported>::
            template _getNearestSuperTypeIdentity<true>()
        )::type;

    static constexpr bool value =
        !std::is_same_v<
            nearestSuperType,
            std::nullptr_t>;
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

    using e_ConversionProcess = Str2TnConversionProcess;
    using e_ErrorHandler      = Str2TnErrorHandler;

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
    : std::bool_constant < check_any_of( DefaultStr2Tn<T>::validConversionProcesses(),
                                         static_cast<Str2TnConversionProcess>(CONV_PROCESS)   //  static_cast needed for windows-*:msvc  combination
                                       ) > {};


  template < typename T, Str2TnErrorHandler ERR_HANDLER>
  struct isSupported_Str2TnErrorHandler
    : std::bool_constant < check_any_of( DefaultStr2Tn<T>::validErrorHandlers(),
                                         static_cast<Str2TnErrorHandler>(ERR_HANDLER)   //  static_cast needed for windows-*:msvc  combination
                                       ) > {};



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
      } else {
        static_assert(sizeof(T) == 0,
              "For [FormatInfo< T, Str2TnConversionProcess CONV_PROCESS >] , no 'FormatInfo' for CONV_PROCESS");
      }
    }

    using type = decltype(getDefaultFormatArgs());
  };



  template < typename T >
  class Str2Thelper
  {
    constexpr static typename DefaultStr2Tn<T>::t_arrConversionProcesses
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


  /*
   * -----------------------------------------------------------------------------
   * String-to-number error-handling policy
   *
   * The selected handler determines the result when conversion fails or, for a
   * floating-point target, when conversion produces NaN.
   *
   * Error handler | Result type                      | Behavior
   * --------------|----------------------------------|------------------------------
   * QUIET_NAN     | T                                | Return a quiet NaN. It can
   *               |                                  | propagate through floating-
   *               |                                  | point calculations without
   *               |                                  | normally raising an invalid-
   *               |                                  | operation exception.
   * SIGNAL_NAN    | T                                | Return a signaling NaN. Its
   *               |                                  | intended purpose is to help
   *               |                                  | detect unexpected values.
   *               |                                  | Using it in an operation may
   *               |                                  | signal an invalid operation,
   *               |                                  | depending on the platform and
   *               |                                  | floating-point environment.
   * THROW_ERROR   | T                                | Throw an exception to report
   *               |                                  | the conversion problem.
   * VARIANT_VAL   | variantReturnType<T>             | Preserve the original input
   *               |                                  | string when conversion
   *               |                                  | fails or produces NaN.
   *
   * For VARIANT_VAL, the alternatives have these meanings:
   *
   *   index 0: T                      -- successful conversion to a non-NaN value
   *   index 1: Str2TNaNInput          -- conversion produced NaN; input is preserved
   *   index 2: Str2TConversionFailure -- conversion failed; input is preserved
   *
   * Example outcomes for floating-point conversion:
   *
   *   Input       Alternative / result
   *   ----------  -----------------------------------------------------------
   *   "12.5"      index 0: the converted numeric value
   *   "nan"       index 1: Str2TNaNInput{"nan"}, when the mechanism parses NaN
   *   "garbage"   index 2: Str2TConversionFailure{"garbage"}
   *   "inf"       index 0: infinity, if the selected mechanism accepts it
   *
   * The application should inspect the active alternative, not infer the
   * outcome from the stored text. In particular, a string preserved in
   * Str2TNaNInput has already been classified by the converter as producing
   * NaN; a string in Str2TConversionFailure represents a failed conversion.
   * An application may then choose its own policy for either outcome.
   *
   * QUIET_NAN and SIGNAL_NAN require the target type to support the corresponding
   * NaN representation. A signaling NaN does not guarantee that a hardware
   * floating-point exception will be raised merely by creating or returning it;
   * behavior depends on the implementation and floating-point environment.
   * INAPPLICABLE is used for conversions such as string-to-string conversion,
   * where a numeric error-handling policy does not apply.
   *
   * This design lets the converter distinguish NaN outcomes from conversion
   * failures without imposing application-specific interpretation rules. The
   * wrapper types are non-template types because they store only the original
   * input string; only variantReturnType<T> depends on the target type.
   * -----------------------------------------------------------------------------
   */
  template < typename T, Str2TnErrorHandler ERR_HANDLER >
          requires ( ERR_HANDLER != Str2TnErrorHandler::INAPPLICABLE &&
                     isSupported_Str2TnErrorHandler<T, ERR_HANDLER>::value )
  struct ErrorHandler
  {
    using return_type = typename ReturnType<T, ERR_HANDLER>::type;
    using value_type  = T;

    template < typename E = std::invalid_argument >
    static return_type handle(const std::string& strVal,
                              const std::string& errMsg)
    {
      return _handle<E>( strVal, errMsg, 2);
    }

    template < typename E = std::invalid_argument >
    static return_type handleVariant(const std::string& strVal,
                                     const std::string& errMsg)
    {
      // CheckNAN calls this when the parsed numeric value is NaN: alternative 1.
      return _handle<E>(strVal, errMsg, 1);
    }

    template < typename E = std::invalid_argument >
        requires std::is_base_of_v<std::exception, E>
    static return_type handle(const std::string& strVal, const E& err)
    {
      if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
      {
        throw err;
      } else {
        return handle<E>(strVal, err.what());
      }
    }

    static return_type handle(const std::string& strVal, const std::errc errCode)
    {
      std::error_code objErrCode = std::make_error_code(errCode);
      return handle(strVal, errCode, objErrCode.message());
    }

    static return_type handle(const std::string& strVal, const std::errc errCode, const std::string& message)
    {
      if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
      {
        throw_appropriate_exception(errCode, message);
      } else {
        return handle(strVal, message);
      }
    }

    [[noreturn]] static void throw_appropriate_exception(std::errc errCode, const std::string& message)
    {
      switch (errCode)
      {
        // Logic / Argument Errors
        case std::errc::invalid_argument:
          throw std::invalid_argument(message);
        case std::errc::result_out_of_range:
        case std::errc::value_too_large:
          throw std::out_of_range(message);
            
        // Memory / Resource Errors
        case std::errc::not_enough_memory:
          throw std::bad_alloc(); // Note: bad_alloc doesn't take a message string
            
        // Runtime / Domain Errors
        case std::errc::argument_out_of_domain:
          throw std::domain_error(message);
        case std::errc::operation_not_permitted:
        case std::errc::permission_denied:
          throw std::runtime_error(message + ": Permission denied");
            
        // Fallback for everything else (I/O, networking, hardware faults)
        default:
          throw std::system_error(std::make_error_code(errCode), message);
      }
    }

  private:
    template < typename E >
    static return_type _handle(const std::string& strVal,
                               const std::string& errMsg,
                               const size_t variantIndex) // 1 -> Str2TNaNInput; 2 -> Str2TConversionFailure
    {
      if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR )
      {
        throw E(errMsg);
      } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL ) {
        if(variantIndex == 1)
          return return_type{
                std::in_place_index<1>,
                Str2TNaNInput{strVal}
            };
        else
          return return_type{
                std::in_place_index<2>,
                Str2TConversionFailure{strVal}
            };
      } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ) {
        if constexpr ( std::numeric_limits<T>::has_quiet_NaN )
        {
          try {
            if constexpr (std::is_same_v<T, float>) {
              return std::nanf(strVal.c_str());
            } else if constexpr (std::is_same_v<T, double>) {
              return std::nan(strVal.c_str());
            } else if constexpr (std::is_same_v<T, long double>) {
              return std::nanl(strVal.c_str());
            } else {
              static_assert(always_false<T>::value, "Unsupported type for FP-conversion");
            }
          } catch (...) {
            // In case if the implementation doesn't support string argument for nan functions, we can return a default quiet NaN value.
            CONVERTER_DEBUG_LOG( "nan() with string argument not supported by this implementation. Returning default quiet NaN value." );
            return std::numeric_limits<T>::quiet_NaN();
          }
        } else {
          static_assert(always_false<T>::value, "Config-conflict for Str2TnErrorHandler::QUIET_NAN as std::numeric_limits<T>::has_quiet_NaN is false :: for FP-conversion");
        }
      } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN ) {
        if constexpr ( std::numeric_limits<T>::has_signaling_NaN )
        {
          return std::numeric_limits<T>::signaling_NaN();
        } else {
          static_assert(always_false<T>::value, "Config-conflict for Str2TnErrorHandler::SIGNAL_NAN as std::numeric_limits<T>::has_signaling_NaN is false :: for FP-conversion");
        }
      } else {
        static_assert(always_false<T>::value, "Unsupported enum Str2TnErrorHandler for FP-conversion");
      }
    }
  };


  template < typename T, Str2TnErrorHandler ERR_HANDLER >
  struct CheckNAN
  {
    using return_type = typename ErrorHandler<T, ERR_HANDLER>::return_type;
    using value_type  = T;

    static return_type handle(const std::string& strVal, const T val)
    {
      if constexpr (c_floating_point<T>)
      {
        if (std::isnan(val))
        {
          const std::string errMsg =
              "Conversion resulted in nan value: " + std::to_string(val) + " :: strVal = '" + strVal + "'";

          CONVERTER_DEBUG_LOG(errMsg);

          return ErrorHandler<T, ERR_HANDLER>::handleVariant(strVal, errMsg);
        }
      }

      return val;
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

      if (ec == std::errc())
      {
        const char* const end = str.data() + str.size();

        if (ptr != end)
        {
          const std::string errMsg =
              "Unconsumed characters in input: '" + str + "'";

          CONVERTER_DEBUG_LOG(errMsg);

          return ErrorHandler<T, ERR_HANDLER>::handle(str, errMsg);
        }

        return CheckNAN<T, ERR_HANDLER>::handle( str, result);
      } else {
        std::stringstream errMsg;
        errMsg << std::make_error_code(ec).message() << " : 'return_type ConvertFromStr< T = [" << specializedTypes::get_name<T>()
               << "], Str2TnConversionProcess::FROM_CHARS, " << ERR_HANDLER << " >::ToVal(const std::string& str)'";

        CONVERTER_DEBUG_LOG( errMsg.str() << "  str = '" << str << "' ; result = " << result );

        return ErrorHandler<T, ERR_HANDLER>::handle(str, ec, errMsg.str());
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
      if (_checkStreamFailure(iss))
      {
        // The branching below is applicable across all compilers tested on github
        if constexpr (c_floating_point<T> &&
                      std::numeric_limits<T>::has_quiet_NaN)
        {
          specializedTypes::ci_string    ciNaN{"nan"};
          if ( ciNaN == str )
          {
            // workaround for stream as it does not convert string "nan"
            // to floating-type NaN in other conversion mechanisms
            T pseudoResult = (std::isnan(result)) ?
                              result              :                  // compilers mostly don't hit this branch path
                              std::numeric_limits<T>::quiet_NaN() ;  // this is the branch path taken by most compilers
            return CheckNAN<T, ERR_HANDLER>::handle( str, pseudoResult);
          }
        }
        std::string errMsg = "Stream read failure. " + funcName + " :: input-string-stream-failure for str='" + str + "'";
        CONVERTER_DEBUG_LOG( errMsg << std::boolalpha << " iss.fail()=" << iss.fail() << "  iss.bad()="   \
                                    << iss.bad() << "  iss.eof()=" << iss.eof());
        return ErrorHandler<T, ERR_HANDLER>::handle(str, errMsg);
      } else {
        return CheckNAN<T, ERR_HANDLER>::handle( str, result);
      }
    }
  };




  template< c_isS2NumSupported T, Str2TnErrorHandler ERR_HANDLER >
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
        return return_type{
              std::in_place_index<2>,
              Str2TConversionFailure{str}
          };
      } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR ) {
        // scenario handled before, no need to do anything here
      } else {
        static_assert(always_false<T>::value, "for c_integral<T> : Unsupported enum Str2TnErrorHandler for FP-conversion");
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
        T result = _ToVal(str, format_args);

        return CheckNAN<T, ERR_HANDLER>::handle( str, result);

      } catch (const std::invalid_argument& err) {
        const std::string errMsg = "Invalid argument: " + std::string(err.what()) + " :: str = '" + str + "'";
        CONVERTER_DEBUG_LOG( errMsg );
        return ErrorHandler<T, ERR_HANDLER>::handle(str, err);
      } catch (const std::out_of_range& err) {
        const std::string errMsg = "Out of range: " + std::string(err.what()) + " :: str = '" + str + "'";
        CONVERTER_DEBUG_LOG( errMsg );
        return ErrorHandler<T, ERR_HANDLER>::handle(str, err);
      }
    }

  private:
    inline static T _ToVal(const std::string& str, std::size_t* pos )
        // throws  std::invalid_argument, std::out_of_range
    {
      T val;
      std::size_t  posSub = 0;
      std::size_t* posSubPtr = &posSub;
      if (pos != nullptr)
      {
        posSubPtr = pos;
      }
      if constexpr (std::is_same_v<T, float>) {
        val = std::stof(str, posSubPtr);  // throws  std::invalid_argument, std::out_of_range
      } else if constexpr (std::is_same_v<T, double>) {
        val = std::stod(str, posSubPtr);
      } else if constexpr (std::is_same_v<T, long double>) {
        val = std::stold(str, posSubPtr);
      } else {
        static_assert(always_false<T>::value, "Unsupported type for conversion");
      }

      if ( (*posSubPtr) != str.size() )
      {
          throw std::invalid_argument(
              "Unconsumed characters in input: '" + str + "'");
      }

      return val;
    }
  };



  /**
   * @brief   If 'T' is not supported by a particular conversion family functions,
   *          then delegate the call to T's super-type.
   * @tparam  ERR_HANDLER             enum variable of type Str2TnErrorHandler.
   */
  template< c_arithmetic T,
            Str2TnConversionProcess CONV_PROCESS,
            Str2TnErrorHandler ERR_HANDLER >
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
        if ( val.index() == 0 )
        {
          act_val = std::get<delegated_type>(val); // throws std::bad_variant_access; but we donot expect this to be thrown here
        } else if(val.index() == 1) {
          return return_type{
                std::in_place_index<1>,
                std::get<Str2TNaNInput>(val)
            };
        } else {
          return return_type{
                std::in_place_index<2>,
                std::get<Str2TConversionFailure>(val)
            };
        }
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
        // on extremly rare occasion, if for e.g. int and int32 byte-size is equal,
        // but are two different types, instead of alias,
        // and int is supported for conversion, but int32 is not.
        // by skipping the run-time min() max() check,
        // Squeeze out performance if it's there.
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
  template<c_char CH, Str2TnErrorHandler ERR_HANDLER >
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
      if(str.length() != 1)
      {
        std::string errMsg =
            "String must contain exactly one character. "
            "Character conversion failed for input '" + str + "'";
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
   *                                     T_V represents either T or variantReturnType<T> (with distinct NaN/failure alternatives).
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
