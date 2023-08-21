/*
 * _common.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.0
 *
 * Copyright (c) 2023-2023 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#pragma once

/*
 *  NUMERIC_LOCALE : Number locales are specific settings for the 1000 separators and decimals.
 *                   Some countries use 1.000,00. Others switch the dot and the comma for 1,000.00.
 *                   Same goes for dates as there are different conventions on how to display
 *                   the day, the month, the year.
 */
namespace converter
{
  // [=============================================================[   common helpers
  // [=========[  concept :  char-types
  //  refer https://www.codeproject.com/Articles/5348002/A-History-of-C-and-Cplusplus-Character-Data-Types
  template <typename T>
  concept c_canBsigned_char = ( std::is_same_v<T, char> ||
                                std::is_same_v<T, wchar_t> );

  template <typename T>
  struct is_signed_char { static constexpr bool value = false; };
  template <>
  struct is_signed_char<signed char> { static constexpr bool value = true; };
  template <c_canBsigned_char T>
  struct is_signed_char<T> {
    // plain chars can be either signed or unsigned as they are machine dependent, but printable characters are always positive
    static constexpr bool
    value = (static_cast<int>((std::numeric_limits<T>::min)()) < 0);
    // warning C4003: not enough arguments for function-like macro invocation 'min'
    // work-around is (std::numeric_limits<T>::min)()
  };
  template <typename T>
  concept c_signed_char = is_signed_char<T>::value;

  template <typename T, typename = void>
  struct is_unsigned_char { static constexpr bool value = false; };
  template <>
  struct is_unsigned_char<unsigned char> { static constexpr bool value = true; };
  // [==[ char8_t : corner case
       /*
        *  C++20 introduces a new character type specifically for UTF-8 encoded character data: char8_t.
        *  It has the same size and sign as unsigned char but is distinct from it.
        * 
        *  The upcoming C standard (probably C23) includes a proposal for char8_t, which is a typedef to unsigned char.
        */
  template <>
  struct is_unsigned_char<  char8_t,
                            std::enable_if_t<!std::is_same_v<unsigned char, char8_t>>
                         >
  { static constexpr bool value = true; };
  // ]==] char8_t : corner case
  template <>
  struct is_unsigned_char<char16_t> { static constexpr bool value = true; };
  template <>
  struct is_unsigned_char<char32_t> { static constexpr bool value = true; };
  template <c_canBsigned_char T>
  struct is_unsigned_char<T> {
    // plain chars can be either signed or unsigned as they are machine dependent, but printable characters are always positive
    static constexpr bool
    value = (static_cast<int>((std::numeric_limits<T>::min)()) == 0);
  };
  template <typename T>
  concept c_unsigned_char = is_unsigned_char<T>::value;

  template <typename T>
  struct is_char {
    static constexpr bool
    value = is_signed_char<T>::value || is_unsigned_char<T>::value;
  };
  template <typename T>
  concept c_char = is_char<T>::value;

/*
  ?? not used yet. but  as a place holder for future use if required. ??

  // Plain char, signed char, and unsigned char are three distinct types.
  // A char, a signed char, and an unsigned char occupy the same amount of storage and have the
  // same alignment requirements (basic.types); that is, they have the same object representation.
  template<c_char T>
  struct is_basicChar { static constexpr bool value = false; };

  template<>
  struct is_basicChar<char> { static constexpr bool value = true; };

  template<>
  struct is_basicChar<signed char> { static constexpr bool value = true; };

  template<>
  struct is_basicChar<unsigned char> { static constexpr bool value = true; };

  template <c_char T>
  concept c_basicChar = is_basicChar<T>::value;

  template <c_char T>
  concept c_NOT_basicChar = (!is_basicChar<T>::value);
*/
  // ]=========]  concept :  char-types


  // [=========[  concept : integer-types 
  // std::integral is true for 'bool' and 'char'.
  // Conversion for 'bool' and 'char' is different from other 'integral-s'.
  // Hence need for a 'concept' to segregate from 'bool' and 'char'
  template<typename T>
  struct is_integer_type
  {
    static constexpr bool value = std::is_integral_v<T> &&
                                  (!is_char<T>::value) &&
                                  (!std::is_same_v<T, bool>);
  };
  template <typename T>
  concept c_integer_type = is_integer_type<T>::value;

  //template <typename T>
  //concept c_std_integral = is_integral_v<T>;
  // ]=========]  concept : integer-types


  // [=========[  concept : floating_point-types 
  // Going with the flow, and just wrapping up floating_point
  template<typename T>
  concept c_floating_point = std::is_floating_point_v<T>;
  // ]=========]  concept : floating_point-types


  /*
   * Refer : https://stackoverflow.com/questions/18118408/what-is-the-difference-between-quiet-nan-and-signaling-nan
   * 
   * Generally, the purpose of a signaling NaN (sNaN) is for debugging. For example, floating-point objects might be
   * initialized to sNaN. Then, if the program fails to one of them a value before using it, an exception will occur
   * when the program uses the sNaN in an arithmetic operation. A program will not produce an sNaN inadvertently;
   * no normal operations produce sNaNs. They are only created specifically for the purpose of having a signaling NaN,
   * not as the result of any arithmetic.
   */
  template<typename T>
  concept c_signaling_NaN = std::numeric_limits<T>::has_signaling_NaN;

  /*
   * Refer : https://stackoverflow.com/questions/18118408/what-is-the-difference-between-quiet-nan-and-signaling-nan
   * 
   * In contrast, quiet NaN are for more normal programming. They can be produced by normal operations when there is no
   * numerical result (e.g., taking the square root of a negative number when the result must be real). Their purpose
   * is generally to allow arithmetic to proceed somewhat normally. E.g., you might have a huge array of numbers,
   * some of which represent special cases that cannot be handled normally. You can call a complicated function to
   * process this array, and it could operate on the array with usual arithmetic, ignoring NaNs. After it ends,
   * you would separate the special cases for more work.
   */
  template<typename T>
  concept c_quiet_NaN = std::numeric_limits<T>::has_quiet_NaN;

  template<typename T>
  concept c_number_type = std::is_arithmetic_v<T> && (!is_char<T>::value);

  template<typename T>
  concept c_NOT_string = (!std::is_same_v<T, std::string>);


  template<typename T> struct is_variant : std::false_type {};
  template<typename ...Args>
  struct is_variant<std::variant<Args...>> : std::true_type {};
  template<typename ...Args>
  concept c_variant = is_variant<Args...>::value;
  template<typename ...Args>
  concept c_NOT_variant = !is_variant<Args...>::value;

  template <typename T>
  struct InferValueType {
    using value_type = T;
  };

  template <typename T>
  struct InferValueType<std::variant<T, std::string>> {
    using value_type = T;
  };

  /*
  template <typename ...Args>
  struct InferValueType<std::variant<Args...>> {
    using value_type = typename std::tuple_element_t< 0,
                                                      std::tuple<Args...>  // 
                                                    >;
  };
  */

  // ]=============================================================]   common helpers



  // [=============================================================[ COMMON_FORMAT
  template <typename IOSS>
  struct is_iostream {
    static constexpr bool
    value = is_char<typename IOSS::char_type>::value &&
                         (std::is_same_v<std::basic_istringstream<typename IOSS::char_type>,IOSS> ||
                          std::is_same_v<std::basic_ostringstream<typename IOSS::char_type>,IOSS>);
  };
  template <typename IOSS>
  concept c_iostream = is_iostream<IOSS>::value;


  template<c_iostream IOSS>
  struct Format_StreamAsIs
  {
    using stream_type = IOSS;

    constexpr static inline void streamUpdate(IOSS&) {}
  };

  template<c_iostream IOSS>
  struct Format_StreamUseClassicLocale
  {
    using stream_type = IOSS;
    static inline void streamUpdate(IOSS& ss)
    {
      const std::locale& ulocale = std::locale::classic();
      ss.imbue(ulocale);
    }
  };

  template<c_iostream IOSS, const char* usrLoc>
  struct Format_StreamUserLocale
  {
    using stream_type = IOSS;
    static inline void streamUpdate(IOSS& ss)
    {
      const std::locale userlocale(usrLoc);
      ss.imbue(userlocale);
    }
  };



  /*
      https://stackoverflow.com/questions/52299062/template-specialization-for-sfinae
      That is simply how SFINAE works ;)

      As you know, you have to "create a failure" within the template declaration and not inside the template definition. As this:

      template < typename X, typename = ... here is the code which may generate an error     during instantiation >
      void Bla() {}

      The only chance to put some "code" in the declaration is to define something in the template parameter list or inside the template function declaration itself like:

      template < typename X>
      void Bla( ... something which may generates an error )   <<== also works on return type
      {}
  */
  // also refer  ::  https://stackoverflow.com/questions/11055923/stdenable-if-parameter-vs-template-parameter/11056146#11056146
  template <typename, typename = void>
  struct has_streamUpdate : std::false_type {};

  template <typename FMT>
  struct has_streamUpdate<FMT, std::void_t< decltype(FMT::streamUpdate),  // check for the presence of static member finction   FMT::streamUpdate
                                            typename FMT::stream_type     // check for the presence of type-def FMT::stream_type
                                          >
                         >
           : std::is_same<decltype(&FMT::streamUpdate), void(*)(typename FMT::stream_type&)>  // member is function
  {};

  template <typename, typename = void >
  struct is_formatSS : std::false_type {};

  template <typename FMT>
  struct is_formatSS< FMT,
                      typename std::enable_if_t<   has_streamUpdate<FMT>::value &&
                                                   is_iostream<typename FMT::stream_type>::value
                                               >
                    >
           : std::true_type
  {};
  template <typename FMT>
  concept c_formatSS = is_formatSS<FMT>::value;

  template <typename, typename = void>
  struct is_formatYMDss : std::false_type {};

  template <typename FMT>
  struct is_formatYMDss< FMT,
                         std::void_t<decltype(FMT::ymdFormat)>  // check for the presence of FMT::ymdFormat
                       >
           : is_formatSS<FMT>
  {};
  template <typename FMT>
  concept c_formatYMDss = is_formatYMDss<FMT>::value;

  template<c_iostream IOSS,
           c_formatSS FORMAT_1,
           c_formatSS FORMAT_2 >
      requires std::is_same_v<IOSS,typename FORMAT_1::stream_type> &&
               std::is_same_v<IOSS,typename FORMAT_2::stream_type>
  struct Format_StreamCombineFormat
  {
    using stream_type = IOSS;

    static inline void streamUpdate(IOSS& ss)
    {
      FORMAT_2::streamUpdate(ss);
      FORMAT_1::streamUpdate(ss);
    }
  };

  template<c_floating_point T>
  constexpr int getDecimalPrecision()
  {
    /*
       Q: why +3 in the return ?
       A: There is loss of data-precision at several places, for e.g:
          float pi_val = 3.1415926535f;  // (1)
          std::ostringstream oss;
          oss << pi_val; std::string pi_str = oss.str();  // (2)
          std::istringstream iss(pi_str); float pi_read;
          iss >> pi_read; // (3)

          The data-precision loss is happening at steps (1), (2) and (3)
          The precision-loss at (1), where in rvalue-raw is not captured exactly
          in lvalue-variable. This is system dependent.
          The value in lvalue-variable(pi_val) should be written to text at a
          higher precision digits for float(hence +3). As we need to eliminate the
          precision loss happening at steps (2) and (3).
          The repeatability or read-write accuracy can only be achieved by using
          higher precision of that specified by precision accuracy for float.

          Refer test091, which fails when +3 is removed here.
    */
    //  https://www.learncpp.com/cpp-tutorial/floating-point-numbers/
    //  https://en.cppreference.com/w/cpp/types/numeric_limits/digits10
    if constexpr (std::is_same_v<T, float>)
      return FLT_DIG+3;  // (4 bytes) -> 6 significant digits, typically 7
      //  (6 for IEEE float)
    else
    if constexpr (std::is_same_v<T, double>)
      return DBL_DIG+3;  // (8 bytes) -> 15 significant digits, typically 16
      //  (15 for IEEE double)
    else
    if constexpr (std::is_same_v<T, long double>)
      return LDBL_DIG+3;  // (if 16 bytes) -> 33-36 significant digits
      //  (18 for 80-bit Intel long double; 33 for IEEE quadruple)
  }
  // ]=============================================================] COMMON_FORMAT
}
