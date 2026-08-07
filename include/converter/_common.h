/*
 * _common.h
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

#include <charconv>
#include <type_traits>
#include <concepts>
#include <algorithm>


/*
 *  NUMERIC_LOCALE : Number locales are specific settings for the 1000 separators and decimals.
 *                   Some countries use 1.000,00. Others switch the dot and the comma for 1,000.00.
 *                   Same goes for dates as there are different conventions on how to display
 *                   the day, the month, the year.
 */
namespace converter
{
  template <typename T>
  struct always_true : std::true_type {};

  template <typename T>
  struct always_false : std::false_type {};  // usually as default for type-traits or in static_assert()

  // [=============================================================[   common helpers
  // [=========[  concept :  types
  //  refer https://www.codeproject.com/Articles/5348002/A-History-of-C-and-Cplusplus-Character-Data-Types
  template <typename T>
  concept c_char = std::is_same_v<T, char>          ||
                   std::is_same_v<T, signed char>   ||
                   std::is_same_v<T, unsigned char> ||
                   std::is_same_v<T, wchar_t>       ||
                   std::is_same_v<T, char8_t>       ||   // C++20
                   std::is_same_v<T, char16_t>      ||   // C++11
                   std::is_same_v<T, char32_t>;          // C++11

  template<typename T>
  concept c_arithmetic = std::is_arithmetic_v<T>;  // std::is_integral<T>::value || std::is_floating_point<T>::value

  template <typename T>
  concept c_integral = std::is_integral<T>::value;  // includes bool and char's signed and unsigned

  template<typename T>
  concept c_floating_point = std::is_floating_point<T>::value;
  // ]=========]  concept : types


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

  // refer: https://stackoverflow.com/a/72451771/2299954
  namespace _prv_impl
  {
    // decay_t will remove const, & and volatile from the type
    template<typename T>
    inline constexpr bool is_string_class_decayed = false;

    template<typename... T>
    inline constexpr bool is_string_class_decayed<std::basic_string<T...>> = true;
  } // namespace impl

  template<typename T>
  inline constexpr bool is_string = _prv_impl::is_string_class_decayed<std::decay_t<T>>;

  template<typename T>
  concept c_basic_string = is_string<T>;


  template <typename ENUM, std::size_t N>
  constexpr bool check_any_of(std::array<ENUM, N> arr, ENUM enumVal)
  {
    return std::any_of( arr.begin(), arr.end(),
                        [enumVal](ENUM val) { return val == enumVal; }
                      );
    return false;
  }





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





  template< typename T >
  struct Format_StringStream;

  class Format_StringStream_Base
  {
  protected:
    /*
      * Default constructor. Constructs a copy of the global C++ locale,
      * which is the locale most recently used as the argument to std::locale::global
      * or a copy of std::locale::classic() if no call to std::locale::global has been made.
    */
    Format_StringStream_Base( const std::locale& loc ) // = std::locale{}
                        //std::ios_base::fmtflags flags = std::ios_base::fmtflags(0)  -> use later if needed
        : _loc(loc), _hasLocParam(true) //, _flags(flags)
    {}

    Format_StringStream_Base()
        : _loc(std::locale{}), _hasLocParam(false) //, _flags(flags)
    {}

    Format_StringStream_Base(const Format_StringStream_Base&  other) = default;
    Format_StringStream_Base(      Format_StringStream_Base&& other) = default;


    Format_StringStream_Base( const std::locale& loc, const bool hasLocParam ) // = std::locale{}
                        //std::ios_base::fmtflags flags = std::ios_base::fmtflags(0)  -> use later if needed
        : _loc(loc), _hasLocParam(hasLocParam) //, _flags(flags)
    {}



    const std::locale _loc;
    //const std::ios_base::fmtflags _flags;
    const bool _hasLocParam;

    template<typename OT>
    friend struct Format_StringStream;

  public:
    bool hasLocParam() const { return _hasLocParam; }
    const std::locale& getLoc() const { return _loc; }

    template<
              class CharT,
              class Traits = std::char_traits<CharT>
            >
    void applyFormatArgs(std::basic_ios<CharT, Traits>& ios) const
    {
      if (!_hasLocParam) return;

      ios.imbue(_loc);
      // ios.setf(_flags); // set format flags if needed in future
    }
  };


  template < c_integral T >
  struct Format_StringStream<T> : public Format_StringStream_Base
  {
    using type = Format_StringStream<T>;

    static type getDefaultFormatArgs()
    {
      return type();
    }

    // Safely inherit all constructors from the Base class
    using Format_StringStream_Base::Format_StringStream_Base;

    /// Rule of 3 / 5 Rule Compliance
    Format_StringStream(const Format_StringStream&  other) = default;
    Format_StringStream(      Format_StringStream&& other) = default;

    template<c_integral OT>
        requires ( !std::is_same_v<T, OT> )
    Format_StringStream(const Format_StringStream<OT>& other)
        : Format_StringStream_Base(other._loc, other._hasLocParam)
    {}

    template<
              class CharT,
              class Traits = std::char_traits<CharT>
            >
    void applyFormatArgs(std::basic_ios<CharT, Traits>& ios) const
    {

      Format_StringStream_Base::applyFormatArgs(ios);
    }
  };

  template < c_floating_point T >
  struct Format_StringStream<T> : public Format_StringStream_Base
  {
    using type = Format_StringStream<T>;

    static type getDefaultFormatArgs()
    {
      return type();
    }

    Format_StringStream( const std::locale& loc, // = std::locale{},
                         const int precision )    // = std::cout.precision() // default_precision
                        //std::ios_base::fmtflags flags = std::ios_base::fmtflags(0)  -> use later if needed
        : Format_StringStream_Base(loc), _precision(precision), _hasPrecisionParam(true) //, _flags(flags)
    {}

    Format_StringStream()
        : Format_StringStream_Base(), _precision(std::numeric_limits<T>::max_digits10), _hasPrecisionParam(false) //, _flags(flags)
    {}

    Format_StringStream( const std::locale& loc )    // = std::numeric_limits<T>::max_digits10 // default_precision
                        //std::ios_base::fmtflags flags = std::ios_base::fmtflags(0)  -> use later if needed
        : Format_StringStream_Base(loc), _precision(std::numeric_limits<T>::max_digits10), _hasPrecisionParam(false) //, _flags(flags)
    {}

    Format_StringStream( const int precision )    // = std::numeric_limits<T>::max_digits10 // default_precision
                        //std::ios_base::fmtflags flags = std::ios_base::fmtflags(0)  -> use later if needed
        : Format_StringStream_Base(), _precision(precision), _hasPrecisionParam(true) //, _flags(flags)
    {}

    /// Rule of 3 / 5 Rule Compliance
    Format_StringStream(const Format_StringStream&  other) = default;
    Format_StringStream(      Format_StringStream&& other) = default;

    template<c_floating_point OT>
        requires ( !std::is_same_v<T, OT> )
    Format_StringStream(const Format_StringStream<OT>& other)
        : Format_StringStream_Base(static_cast< Format_StringStream_Base& >(other)),
          _precision(std::numeric_limits<T>::max_digits10), _hasPrecisionParam(other._hasPrecisionParam)
    {}

    template<
              class CharT,
              class Traits = std::char_traits<CharT>
            >
    void applyFormatArgs(std::basic_ios<CharT, Traits>& ios) const
    {

      Format_StringStream_Base::applyFormatArgs(ios);

      if (!_hasPrecisionParam) return;

      /*
       * refer document PROJECT_DIR/doc/T2text2T_conversion.odt
       *
       * using digits10 instead of max_digits10 to set precision for stringstream-based conversion,
       * as the precision for stringstream-based conversion should be set to the maximum number of
       * decimal digits that can be represented without losing precision, which is given by digits10.
       * Setting it to max_digits10 would be unnecessarily high and could lead to performance issues
       * without providing any additional benefit in terms of precision for typical use cases.
      */
      //if constexpr ( std::numeric_limits<T>::digits10 > 0 )  // Input safety (Text ➡️ Float)
      // we donot use "digits10" as for any round-trip conersions
      //   "Text ➡️ Float ➡️ Text"    OR   "Float ➡️ Text ➡️ Float"
      // "max_digits10" has prominance even for : Input safety (Text ➡️ Float)
      if constexpr ( c_floating_point<T> )  // Output safety (Float ➡️ Text)
      {
        if (_precision > 0)
        {
          // https://en.cppreference.com/w/cpp/io/manip/setprecision.html
          ios.precision(_precision);
        }
      }
      // ios.setf(_flags); // set format flags if needed in future
    }

  private:
    const std::streamsize _precision;
    //const std::ios_base::fmtflags _flags;
    const bool _hasPrecisionParam;
  };



  template< typename T, auto ... ARGS >
  struct Format_SpecializedImplementation
  {
    static_assert(sizeof(T) == 0,
              "No Format_SpecializedImplementation specialization found");
  };


  template <typename T, auto CONV_PROCESS, auto ... ARGS >
  struct FormatInfo;

  // ]=============================================================]   common helpers


} // namespace converter

