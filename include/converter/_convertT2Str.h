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
  enum struct Tn2StrConversionProcess : int
              {
                TO_CHARS = 1, // std::to_chars is locale-independent, non-allocating, and non-throwing
                TO_STRING = 2,
                STRINGSTREAM = 3,
                SPECIALIZED_IMPLEMENTATION = 4 };


  std::ostream& operator<<(std::ostream& os, Tn2StrConversionProcess t2s)
  {
    switch (t2s) {
      case Tn2StrConversionProcess::TO_CHARS                   :  os << "TO_CHARS";    break;
      case Tn2StrConversionProcess::TO_STRING                  :  os << "TO_STRING";           break;
      case Tn2StrConversionProcess::STRINGSTREAM               :  os << "STRINGSTREAM";  break;
      case Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION :  os << "SPECIALIZED_IMPLEMENTATION";       break;
      default                                                  :  os << "Tn2StrConversionProcess::Unknown"; break;
    }
    return os;
  }


  template<typename T>
  struct isT2SspecializedImplementationAvailable : std::false_type {};

  //    "isT2SspecializedImplementationAvailable" defaulted to false, specilization is defined later.
  //    As single primary definition of "c_isT2SspecializedImplementationAvailable" introduces a cyclic dependency as below
  //
  //    ConvertFromVal<T,...>   ->
  //        DefaultTn2Str::validConversionProcesses() ->
  //            c_isT2SspecializedImplementationAvailable   ->
  //                    ConvertFromVal::isConversionImplemented
  //
  //    template specialization of type-trait "isT2SspecializedImplementationAvailable",
  //    is used to break this cyclic dependency.
  template<typename T>
  concept c_isT2SspecializedImplementationAvailable = isT2SspecializedImplementationAvailable<T>::value;


  template<c_basic_string BSTR>
  struct isT2SspecializedImplementationAvailable<BSTR> : std::true_type {};


  template<c_char T>
  struct isT2SspecializedImplementationAvailable<T> : std::true_type {};
  // ]=========]  concept :  is SPECIALIZED_IMPLEMENTATION for conversion available


  // [=========[  concept :  is std::to_chars supported
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
  struct is_to_chars_supported
  {
  private:
    // Helper to check for integer and floating-point overloads
    static constexpr bool _check()
    {
      if constexpr (  std::is_integral_v<T>
                      // for bool, std::to_chars is not supported,
                      // for char types, std::to_chars is not supported,
                      // but instead of compile-time error we get a run-time failure,
                      // so we exclude them here
                      && !converter::c_char<T>  )
      {
        return  requires(char* first, char* last, T value1, int base)
                {
                  // Check for the basic (integer) version with optional base
                  { std::to_chars(first, last, value1, base) } -> std::same_as<std::to_chars_result>;
                };
      } else if constexpr (std::is_floating_point_v<T>) {
        return  requires(char* first, char* last, T value1, std::chars_format fmt, int precision)
                {
                  // Check for the floating-point version which often needs a format flag
                  { std::to_chars(first, last, value1, fmt, precision) } -> std::same_as<std::to_chars_result>;
                };
      }
      return false;
    }

  public:
    static constexpr bool value = _check();
  };

  template<typename T>
  concept c_isToCharsSupported = is_to_chars_supported<T>::value;
  // ]=========]  concept :  is std::to_chars supported


  // [=========[  concept :  is std::to_string()
  // Checks whether T can be converted to std::string using any of the standard
  // std::to_string() family of conversion functions.
  /*
     WARNING :: With floating point types std::to_string may yield unexpected results as the number
                of significant digits in the returned string can be zero, for e.g: pVal = 1e-09

                The return value may differ significantly from what std::cout prints by default, see the example.

     std::cout: 23.43
     to_string: 23.430000

     std::cout: 1e-09
     to_string: 0.000000

     std::cout: 1e+40
     to_string: 10000000000000000303786028427003666890752.000000

     std::cout: 1e-40
     to_string: 0.000000

     std::cout: 1.23457e+08
     to_string: 123456789.000000

      That's why for c_floating_point<T> is enabled with macro ENABLE_FLOATINGPOINT_TO_STRING
   */
  template<typename T>
  concept c_isNum2SSupported = ( ( c_integral<T> && !std::same_as<T, bool> && !c_char<T> ) ||
                                 ( c_floating_point<T> && bool(ENABLE_FLOATINGPOINT_TO_STRING) )
                               )  &&
                               requires(T val) { { std::to_string(val) } -> std::same_as<std::string>; };
  // ]=========]  concept :  is std::to_string() supported


  // [=========[  concept :  checks for "std::basic_istream<CharT,Traits>::operator>>"
  template< typename T,
            typename CharT = char,
            typename Traits = std::char_traits<CharT> >
  struct isWritableToStream
  {
    constexpr static bool value = (!c_basic_string<T>) &&
                                  //c_arithmetic<T> &&   // eliminate std::string here
                                  requires(std::basic_ostream<CharT, Traits>& is, T& val)
                                  {
                                    { is << val } -> std::same_as<std::basic_ostream<CharT, Traits>&>;
                                  };
  };

  template <  typename T,
              typename CharT = char,
              typename Traits = std::char_traits<CharT> >
  concept c_isWritableToStream = isWritableToStream<T, CharT, Traits >::value;
  // ]=========]  concept :  checks for "std::basic_istream<CharT,Traits>::operator>>"



  template < c_arithmetic T, Tn2StrConversionProcess CONV_PROCESS>
      requires ( CONV_PROCESS == Tn2StrConversionProcess::TO_STRING           ||
                 CONV_PROCESS == Tn2StrConversionProcess::TO_CHARS            ||
                 CONV_PROCESS == Tn2StrConversionProcess::STRINGSTREAM   )
  struct isConversionNum2SCppSupported
    : std::bool_constant <
                           ( CONV_PROCESS == Tn2StrConversionProcess::TO_STRING           && c_isNum2SSupported<T>     ) ||
                           ( CONV_PROCESS == Tn2StrConversionProcess::TO_CHARS            && c_isToCharsSupported<T>   ) ||
                           ( CONV_PROCESS == Tn2StrConversionProcess::STRINGSTREAM        && c_isWritableToStream<T>   )
                         > {};

  template < c_arithmetic T, Tn2StrConversionProcess CONV_PROCESS>
      requires ( CONV_PROCESS == Tn2StrConversionProcess::TO_STRING           ||
                 CONV_PROCESS == Tn2StrConversionProcess::TO_CHARS            ||
                 CONV_PROCESS == Tn2StrConversionProcess::STRINGSTREAM   )
  struct isConversionNum2SNotCppSupported
    : std::bool_constant <
                           ( CONV_PROCESS == Tn2StrConversionProcess::TO_STRING           && !c_isNum2SSupported<T>   ) ||
                           ( CONV_PROCESS == Tn2StrConversionProcess::TO_CHARS            && !c_isToCharsSupported<T> ) ||
                           ( CONV_PROCESS == Tn2StrConversionProcess::STRINGSTREAM        && !c_isWritableToStream<T> )
                         > {};


  // Primary template:
  // Bumped conversion is not applicable.
  template < typename T, Tn2StrConversionProcess CONV_PROCESS >
  struct isBumpedTypeN2SConversionAvailable
  {
    // void means this template is not applicable to a
    // numeric bump conversion (primary template).
    using nearestSuperType = void;

    static constexpr bool value = false;
  };


  template <c_numeric T, Tn2StrConversionProcess CONV_PROCESS>
      requires (
          isConversionNum2SNotCppSupported<T, CONV_PROCESS>::value
      )
  struct isBumpedTypeN2SConversionAvailable<T, CONV_PROCESS>
      : isBumpedTypeConversionAvailable<
            T,
            CONV_PROCESS,
            isConversionNum2SCppSupported>
  {
  public:
    // std::nullptr_t       -> bumping applicable, but no supported super-type.                      value = false
    // numeric ST           -> bumping available for the type T. type T bumped to super-type ST.     value = true

    using nearestSuperType =
        typename decltype(
            isBumpedTypeConversionAvailable<
                  T,
                  CONV_PROCESS,
                  isConversionNum2SCppSupported>::template
            _getNearestSuperTypeIdentity<
                CONV_PROCESS != Tn2StrConversionProcess::TO_STRING ||
                bool(ENABLE_FLOATINGPOINT_TO_STRING)
            >()
        )::type;

    static constexpr bool value =
        !std::is_same_v<
            nearestSuperType,
            std::nullptr_t>;
  };




  template <typename T>
  struct DefaultTn2Str
  {
  private:
    static constexpr bool _isToCharsSupported = c_isToCharsSupported<T>
                                 || isBumpedTypeN2SConversionAvailable<T, Tn2StrConversionProcess::TO_CHARS>::value;
    static constexpr bool _isNum2SSupported = c_isNum2SSupported<T>
                                 || isBumpedTypeN2SConversionAvailable<T, Tn2StrConversionProcess::TO_STRING>::value;
    static constexpr bool _isWritableToStream = c_isWritableToStream<T>;
    static constexpr bool _isT2SspecializedImplementationAvailable = c_isT2SspecializedImplementationAvailable<T>;

    static constexpr size_t _arrSizeConversionProcesses =
                          size_t(_isToCharsSupported)                        +
                          size_t(_isNum2SSupported)                          +
                          size_t(_isWritableToStream)                        +
                          size_t(_isT2SspecializedImplementationAvailable);

  public:

    using t_arrConversionProcesses = std::array < Tn2StrConversionProcess, _arrSizeConversionProcesses >;

    // first element (index=0) of array is the default Conversion-Process for that type T
    constexpr static t_arrConversionProcesses validConversionProcesses()
    {

      static_assert( _arrSizeConversionProcesses > 0 , "Unhandled type for DefaultTn2Str<T>::validConversionProcesses()" );

      t_arrConversionProcesses arrPossibilities{};
      size_t arrIdx = 0;

      if constexpr (_isToCharsSupported)                      arrPossibilities[arrIdx++] = Tn2StrConversionProcess::TO_CHARS;
      if constexpr (_isNum2SSupported)                        arrPossibilities[arrIdx++] = Tn2StrConversionProcess::TO_STRING;
      if constexpr (_isWritableToStream)                      arrPossibilities[arrIdx++] = Tn2StrConversionProcess::STRINGSTREAM;
      if constexpr (_isT2SspecializedImplementationAvailable) arrPossibilities[arrIdx++] = Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION;

      return arrPossibilities;
    }
  };




  template <  typename T,
              Tn2StrConversionProcess CONV_PROCESS = DefaultTn2Str<T>::validConversionProcesses()[0]  >
  struct ConvertFromVal
  {
    constexpr static size_t templateId = 0;
    constexpr static bool isConversionImplemented = false;
  };









  // [[============[[ T2S-converter :: concept
  // CFV -> Converter Function from Value
  template <typename CFV>
  concept c_T2Sconverter =  requires (const std::string& str)
                            {
                              // 1. Check for nested type definitions
                              typename CFV::value_type;
                              typename CFV::return_type;

                              // Verifies CFV::ToStr(str) is a valid expression without an instance
                              { CFV::ToStr(str) } -> std::same_as<typename CFV::return_type>;
                            };

  template <typename CFV>
  concept c_NOT_T2Sconverter = !c_T2Sconverter<CFV>;
  // ]]============]] T2S-converter :: concept





  template < typename T, Tn2StrConversionProcess CONV_PROCESS>
  struct isSupported_Tn2StrConversionProcess
      // NOTE: validConversionProcesses() is inclusive of isBumpedTypeN2SConversionAvailable<>::true_type
    : std::bool_constant < check_any_of( DefaultTn2Str<T>::validConversionProcesses(), CONV_PROCESS ) > {};



  template <typename T>
  //  requires c_arithmetic<T>
  struct Format_ToChars;

  template <c_integral T>
  struct Format_ToChars<T>
  {
    using type = int;

    constexpr static int charArraySize = std::numeric_limits<T>::digits10 + 10;

    constexpr static type getDefaultFormatArgs() { return 10; } // base 10 is the default for std::to_chars for integral types, but providing this function for consistency and future extensibility
  };

  template <c_floating_point T>
  struct Format_ToChars<T>
  {
    using type = Format_ToChars<T>;

    constexpr static int charArraySize = std::numeric_limits<T>::max_digits10 + 10;

    const std::chars_format format;    // = std::chars_format::general,
    const               int precision; // = std::numeric_limits<T>::max_digits10
    const              bool isDefault;

    constexpr Format_ToChars()
      : format(std::chars_format::general),
        precision(std::numeric_limits<T>::max_digits10),
        isDefault(true)
    {}

    constexpr Format_ToChars( std::chars_format pFormat,
                              int pPrecision )
      : format(pFormat),
        precision(pPrecision),
        isDefault(false)
    {}

    Format_ToChars(const Format_ToChars&  other) = default;
    Format_ToChars(      Format_ToChars&& other) = default;

    template<c_floating_point OT>
        requires ( !std::is_same_v<T, OT> )
    Format_ToChars(const Format_StringStream<OT>& other)
        : format(other.format),
          precision(std::numeric_limits<OT>::max_digits10),
          isDefault(other.isDefault)
    {}

    constexpr static type getDefaultFormatArgs() { return Format_ToChars<T>(); } // std::chars_format::general is the default for std::to_chars for floating-point types, but providing this function for consistency and future extensibility

  };




  struct Format_N2S
  {
    using type = std::nullptr_t;

    constexpr static type getDefaultFormatArgs() { return nullptr; } // pos
  };








  template <typename T, Tn2StrConversionProcess CONV_PROCESS >
          requires ( isSupported_Tn2StrConversionProcess<T, CONV_PROCESS>::value )
  struct FormatInfo< T, CONV_PROCESS >
  {
    constexpr static auto getDefaultFormatArgs()
    {
      if constexpr (CONV_PROCESS == Tn2StrConversionProcess::TO_CHARS) {
        return Format_ToChars<T>::getDefaultFormatArgs();
      } else if constexpr (CONV_PROCESS == Tn2StrConversionProcess::TO_STRING) {
        return Format_N2S::getDefaultFormatArgs();
      } else if constexpr (CONV_PROCESS == Tn2StrConversionProcess::STRINGSTREAM) {
        return Format_StringStream<T>::getDefaultFormatArgs();
      } else if constexpr (CONV_PROCESS == Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION) {
        return Format_SpecializedImplementation<T>::getDefaultFormatArgs();
      } else {
        static_assert(sizeof(T) == 0,
              "For [FormatInfo< T, Tn2StrConversionProcess CONV_PROCESS >] , no 'FormatInfo' for CONV_PROCESS");
      }
    }

    using type = decltype(getDefaultFormatArgs());
  };



  template < typename T >
  class T2Strhelper
  {
    constexpr static typename DefaultTn2Str<T>::t_arrConversionProcesses
      _ArrConversionProcesses = DefaultTn2Str<T>::validConversionProcesses();

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











  template < c_isToCharsSupported T >
          requires ( isSupported_Tn2StrConversionProcess<T, Tn2StrConversionProcess::TO_CHARS>::value )
  struct ConvertFromVal< T, Tn2StrConversionProcess::TO_CHARS >
  {
    constexpr static size_t templateId = 1;
    constexpr static bool isConversionImplemented = true;
    using value_type  = T;
    using return_type = std::string;
    constexpr static Tn2StrConversionProcess conversionProcess = Tn2StrConversionProcess::TO_CHARS;

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToStr(const T& val,
          const typename Format_ToChars<T>::type& format_args =
                         Format_ToChars<T>::getDefaultFormatArgs()  )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< T = [" << specializedTypes::get_name<T>() << "], Tn2StrConversionProcess::TO_CHARS >::ToStr('" << val << "')");
      /*
      size_t strSize = Format_ToChars<T>::charArraySize;
      if constexpr (c_floating_point<T>)
      {
        if( !format_args.isDefault && format_args.precision > 0 )
          strSize = static_cast<size_t>(format_args.precision);
      }
      std::string str( strSize + 10, '\0');  // +10 just oversize to be safe
      */

      std::array<char, Format_ToChars<T>::charArraySize +20 > str;  // +20 just to be on the safe side :)
      std::to_chars_result result;

      //  https://en.cppreference.com/w/cpp/utility/to_chars
      //  std::to_chars is locale-independent, non-allocating, and non-throwing
      if constexpr (c_integral<T>)
      {
        result = std::to_chars( str.data(), str.data() + str.size(), val, format_args );
      } else if constexpr (c_floating_point<T>) {
        result = std::to_chars( str.data(), str.data() + str.size(), val, format_args.format, format_args.precision );
      } else {
        static_assert(always_false<T>::value, "Unsupported TYPE for numeric-conversion");
      }

      if (result.ec == std::errc()) {
        return std::string(str.data(), result.ptr);
      } else {
        std::stringstream errMsg;
        errMsg << std::make_error_code(result.ec).message() << " : 'std::string ConvertFromVal< T = [" << specializedTypes::get_name<T>()
               << "], Tn2StrConversionProcess::TO_CHARS >::ToStr(const T& val, auto... format_args)'";
        CONVERTER_DEBUG_LOG( errMsg.str() << "  val = '" << val << "'");

        switch(result.ec)
        {
        case std::errc::invalid_argument:
          throw std::invalid_argument(errMsg.str());
        case std::errc::result_out_of_range:
          throw std::out_of_range(errMsg.str());
        default:
          throw std::logic_error(errMsg.str());
        }
      }
    }
  };

  template < c_isWritableToStream T >
          requires ( isSupported_Tn2StrConversionProcess<T, Tn2StrConversionProcess::STRINGSTREAM>::value )
  struct ConvertFromVal< T, Tn2StrConversionProcess::STRINGSTREAM >
  {
    constexpr static size_t templateId = 2;
    constexpr static bool isConversionImplemented = true;
    using value_type  = T;
    using return_type = std::string;
    constexpr static Tn2StrConversionProcess conversionProcess = Tn2StrConversionProcess::STRINGSTREAM;

    /*
      When T = std::chrono::year_month_day,
      the date default-format is short_format = "%F" (equivalent long_format = "%Y-%m-%d");
      date-format is fixed and cannot be changed.

      For passing user-defined date-format refer, template specialization
      ConvertFromVal<std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION>
      in _convertDate.h
    */

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToStr(const T& val)
    {
      // refer https://en.cppreference.com/w/cpp/io/basic_istream/operator_gtgt.html
      // std::to_chars() can be used in situations where maximum accuracy is important.

      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< T = [" << specializedTypes::get_name<T>() << "], Tn2StrConversionProcess::STRINGSTREAM >::ToStr('" << val << "')");

      std::ostringstream oss;
      return _ToStr(val, oss);
    }

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @param   format_args         locale for stringstream based conversion.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToStr(const T& val,
          const typename Format_StringStream<T>::type& format_ss) // = Format_StringStream<T>::getDefaultFormatArgs() )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< T = [" << specializedTypes::get_name<T>() << "], Tn2StrConversionProcess::STRINGSTREAM >::ToStr('" << val << "', format_ss)");

      std::ostringstream oss;
      format_ss.applyFormatArgs(oss); // set locale for stringstream based conversion
      return _ToStr(val, oss);
    }

  private:
    inline static bool _checkStreamFailure(const std::ostringstream& oss)
    {
      //if constexpr (c_char<T>)
        return ( oss.fail() || oss.bad() );
      //else
      //  return ( oss.fail() || oss.bad() || (!oss.eof()) ); // not applicable for std:ostream
    }

    inline static return_type
    _ToStr(const T& val,
           std::ostringstream& oss)
    {
      static const std::string funcName = std::string{" : 'return_type ConvertFromVal< T = ["}
                             + std::string{specializedTypes::get_name<T>().data, static_cast<size_t>(specializedTypes::get_name<T>().size)}
                             + "], Tn2StrConversionProcess::STRINGSTREAM >::_ToStr(const T& val, std::ostringstream& oss)'";

      try
      {
        oss << val;
      } catch(const std::exception& e) {
          const std::string errMsg = "Stream write failure. " + funcName + " :: failure errMsg=" + e.what();
          CONVERTER_DEBUG_LOG( errMsg );

          throw std::invalid_argument(errMsg);
      }

      if (_checkStreamFailure(oss)) {
        std::string errMsg = "Stream write failure. " + funcName + " :: output-string-stream-failure.";
        CONVERTER_DEBUG_LOG( errMsg << std::boolalpha << " oss.fail()=" << oss.fail() << "  oss.bad()="   \
                                    << oss.bad() << "  oss.eof()=" << oss.eof());
        throw std::invalid_argument(errMsg);
      } else {
        return oss.str();
      }
    }
  };




  template< c_isNum2SSupported T>
          requires ( isSupported_Tn2StrConversionProcess<T, Tn2StrConversionProcess::TO_STRING>::value )
  struct ConvertFromVal< T, Tn2StrConversionProcess::TO_STRING >
  {
    constexpr static size_t templateId = 3;
    constexpr static bool isConversionImplemented = true;
    using value_type  = T;
    using return_type = std::string;

    constexpr static Tn2StrConversionProcess conversionProcess = Tn2StrConversionProcess::TO_STRING;

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToStr(const T& val,
          [[maybe_unused]]
          const typename Format_N2S::type& format_args =
                         Format_N2S::getDefaultFormatArgs() )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< T = [" << specializedTypes::get_name<T>() << "], Tn2StrConversionProcess::TO_STRING >::ToStr('" << val << "')");

      try
      {
        return std::to_string(val);
      } catch (const std::bad_alloc& e) {
        const std::string errMsg = std::string{"error in ConvertFromVal< T = ["} + specializedTypes::get_name<T>().data +
                                    "], Tn2StrConversionProcess::TO_STRING >::ToStr(val) -> std::bad_alloc: " + std::string(e.what());
        CONVERTER_DEBUG_LOG( errMsg );
        throw e;
      }
    }
  };



  /**
   * @brief   If 'T' is not supported by a particular conversion family functions,
   *          then delegate the call to T's super-type.
   */
  template< c_arithmetic T, Tn2StrConversionProcess CONV_PROCESS>
          requires ( isBumpedTypeN2SConversionAvailable<T, CONV_PROCESS>::value )
  struct ConvertFromVal<T, CONV_PROCESS>
  {
    using delegated_type = typename isBumpedTypeN2SConversionAvailable<T, CONV_PROCESS>::nearestSuperType;
    constexpr static size_t delegated_templateId = ConvertFromVal<delegated_type, CONV_PROCESS >::templateId;

    constexpr static size_t templateId = 5;
    constexpr static bool isConversionImplemented = true;
    using value_type  = T;
    using return_type = std::string;
    constexpr static Tn2StrConversionProcess conversionProcess = CONV_PROCESS;

    /**
     * @brief   Converts string holding T value.
     * @param   str                 input string.
     * @returns T.
     */
    inline static return_type
    ToStr(const T& val,
          const typename FormatInfo<T, CONV_PROCESS>::type& format_args =
                         FormatInfo<T, CONV_PROCESS>::getDefaultFormatArgs()  )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< T = [" << specializedTypes::get_name<T>() << "] -> delegated_type = [" << specializedTypes::get_name<delegated_type>() << "] , " << CONV_PROCESS << " >::ToStr('" << val << "')");

      typename FormatInfo<delegated_type, CONV_PROCESS>::type delegate_format_args{format_args};
      return ConvertFromVal<delegated_type, CONV_PROCESS >::ToStr(val, delegate_format_args);
    }
  };





  /**
   * @brief     Specialized implementation handling string to string conversion.
   */
  template<>
          //requires ( isSupported_Tn2StrConversionProcess<std::string, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION> )
  struct ConvertFromVal< std::string, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >
  {
    constexpr static size_t templateId = 6;
    constexpr static bool isConversionImplemented = true;
    using value_type  = std::string;
    using return_type = std::string;
    constexpr static Tn2StrConversionProcess conversionProcess = Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION;

    /**
     * @brief   Converts string to string.
     * @param   str                 input string.
     * @returns string.
     */
    inline static std::string ToStr(const std::string& val)
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::string , Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >::ToStr('" << val << "')");
      return val;
    }
  };
  template<c_basic_string BSTR>
          requires ( isSupported_Tn2StrConversionProcess<BSTR, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION>::value )
  struct ConvertFromVal< BSTR, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >
  {
    constexpr static size_t templateId = 7;
    constexpr static bool isConversionImplemented = true;
    using value_type  = BSTR;
    using return_type = std::string;
    constexpr static Tn2StrConversionProcess conversionProcess = Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION;

    /**
     * @brief   Converts string to string.
     * @param   str                 input string.
     * @returns string.
     */
    inline static return_type ToStr(const BSTR& val)
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< T = [" << specializedTypes::get_name<BSTR>() << "] , Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >::ToStr(val)");
      return std::string{val.c_str()};
    }
  };

  /**
   * @brief     Specialized implementation handling string to char conversion.
   * @tparam  CH                     'char-type' converted to, from string data.
   */
  template<c_char CH >
          requires ( isSupported_Tn2StrConversionProcess<CH, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION>::value )
  struct ConvertFromVal< CH, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >
  {
    constexpr static size_t templateId = 8;
    constexpr static bool isConversionImplemented = true;
    using value_type  = CH;
    using return_type = std::string;
    constexpr static Tn2StrConversionProcess conversionProcess = Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION;

    /**
     * @brief   Converts string holding char value.
     * @param   str                 input string.
     * @returns char-type.
     */
    inline static return_type
    ToStr(const CH& val)
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< T = [" << specializedTypes::get_name<CH>() << "] , Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >::ToStr(val)");
      std::string ssVal("_");
      ssVal.at(0) = static_cast<char>(val);
      return ssVal;
    }
  };

  // ]=============================================================] ConvertFromVal





  // [=============================================================[ T2S conversion-type definition
  template< typename T, typename TI,
            std::string (*CONV_T2S)(const TI&)
          >
  struct T2SwrapperFunction
  {
    using value_type = T;
    using input_type = TI;

    //inline static std::string
    inline static typename std::enable_if_t< (!std::is_same_v<std::string, TI>),std::string>
    ToStr(const input_type& val)
    {
      CONVERTER_DEBUG_LOG("trace :: T2SwrapperFunction< T = [" << specializedTypes::get_name<T>() << "], ... >::ToStr('" << val << "')");
      CONVERTER_DEBUG_TRY_START
        return CONV_T2S(val);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }
  };

  // refer : https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0127r1.html




  /**
   * @brief   If a 'type-C' satisfies concept 'c_T2Sconverter', then use that 'type-C';   else
   *          assume it's a 'type-T' and bumped up using 'ConvertFromVal< type-T >' to create
   *          a class staisfying concept 'c_T2Sconverter'.
   *          This mechanism enables 'template-converter-algorithm' to handle both 'type-C' and
   *          'type-T' using the same code base, (i.e. reduces code duplicity).
   * @tparam  T_C                   T can be data-type such as int, double etc ;
   *                                xOR  C -> Conversion class statisfying concept 'c_T2Sconverter'.
   */
  template< typename T_C >
  struct t_T2Sconv;
  /*
  template< typename T_C, auto ... CONV_T2S >
  struct t_T2Sconv;

  template< auto CONV_T2S >
  struct t_T2Sconv< void, CONV_T2S > {
    using conv_type = f_T2Sconv_c<CONV_T2S>;
  };
  */

  template< c_NOT_T2Sconverter T >
  struct t_T2Sconv<T> {
    using conv_type = ConvertFromVal< T >;
  };

  template< c_T2Sconverter C >
  struct t_T2Sconv<C> {
    using conv_type = C;
  };



  template< typename T_C >
  using t_T2Sconv_c = typename t_T2Sconv< T_C >::conv_type;

  /**
   * @brief   convert a function with signature 'std::string (*CONV_T2S)(const T_V&)'
   *          to a converter type that satisfies concept 'c_T2Sconverter'.
   * @tparam  CONV_T2S                   a function with signature 'std::string (*CONV_T2S)(const T_V&)'.
   *                                     T_V represents either numeric type 'T' or 'variant std::variant<T, std::string>'
   */
  template<auto CONV_T2S >
  struct f_T2Sconv;

  template< typename T_V, std::string (*CONV_T2S)(const T_V&) >
  struct f_T2Sconv<CONV_T2S>
  {
  private:
    /**
     * @brief input type of function CONV_T2S
     */
    using _input_type = T_V;
    /**
     * @brief infers the underlying data-type if _input_type is an variant instance.
     *        or it's same as input type of CONV_T2S.
     */
    using _value_type  = typename InferValueType<_input_type>::value_type;
  public:
    /**
     * @brief a converter alias that satisfies concept 'c_T2Sconverter'
     */
    using conv_type = T2SwrapperFunction< _value_type, _input_type, CONV_T2S >;
  };

  //template< typename T_V, std::string (*CONV_T2S)(const T_V&) >
  template<auto CONV_T2S >
  using f_T2Sconv_c = typename f_T2Sconv<CONV_T2S>::conv_type;

  // ]=============================================================] T2S conversion-type definition





} // namespace converter
