/*
 * _convertDate.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.4
 *
 * Copyright (c) 2023-2026 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#pragma once

#include <chrono>
//#include <string_view>
#include <cstring>
#include <string>
#include <array>
#include <map>
#include <locale>
#include <algorithm>
#include <cctype>

#include <converter/_common.h>

#include <converter/_workaroundConfig.h>
#include <specializedTypes/CompTimeStr.h>


#if    USE_CHRONO_TOSTREAM_1 == e_ENABLE_FEATURE
  #include <format>
#endif

#if  USE_DATE_FROMSTREAM_2 == e_ENABLE_FEATURE  ||  USE_DATE_TOSTREAM_2 == e_ENABLE_FEATURE
  #include <date/date.h>
#endif

#if    USE_CHRONO_FROMSTREAM_1 == e_ENABLE_FEATURE
  namespace datelibFrom = std::chrono;
#elif  USE_DATE_FROMSTREAM_2 == e_ENABLE_FEATURE
  namespace datelibFrom = date;
#endif

#if    USE_CHRONO_TOSTREAM_1 == e_ENABLE_FEATURE
  namespace datelibTo = std::chrono;
  namespace formatLibTo = std;
#elif  USE_DATE_TOSTREAM_2 == e_ENABLE_FEATURE
  namespace datelibTo = date;
  namespace formatLibTo = date;
#endif


namespace converter
{



  // [=============================================================[ COMMON_FORMAT

  template <>
  struct Format_SpecializedImplementation<std::chrono::year_month_day>
    : public Format_StringStream_Base
  {
    using type = Format_SpecializedImplementation<std::chrono::year_month_day>;

    /*
      * Default constructor. Constructs a copy of the global C++ locale,
      * which is the locale most recently used as the argument to std::locale::global
      * or a copy of std::locale::classic() if no call to std::locale::global has been made.
    */
    Format_SpecializedImplementation(
                        const std::string& fmt, // "%Y-%m-%d"  without curly braces
                        const std::locale& loc) // = std::locale{},
                        //std::ios_base::fmtflags flags = std::ios_base::fmtflags(0)  -> use later if needed
        : Format_StringStream_Base(loc), _dateFormat_runTime(fmt)
    {}

    Format_SpecializedImplementation(const std::string& fmt) // "%Y-%m-%d"  without curly braces
        : Format_StringStream_Base(), _dateFormat_runTime(fmt)
    {}

    // Rule of 3 / 5 Rule Compliance
    Format_SpecializedImplementation(const Format_SpecializedImplementation& other) = default;
    Format_SpecializedImplementation(Format_SpecializedImplementation&& other) = default;

    // 2. Returns a default instance using the fallback literal string format
    static type getDefaultFormatArgs()
    {
      return type("%F"); // "%F" standard equivalent to "%Y-%m-%d"
    }

    template<
              class CharT,
              class Traits = std::char_traits<CharT>
            >
    void applyFormatArgs(std::basic_ios<CharT, Traits>& ios) const
    {
      Format_StringStream_Base::applyFormatArgs(ios);
    }

    const std::string&   getDateFormat() const
    {
      return _dateFormat_runTime;  //"%Y-%m-%d"
    }

  private:
    const std::string _dateFormat_runTime;  // "%Y-%m-%d"
  };


  /*
   * FmtStr must appear in the specialization argument list.In your current snippet,
   * CompTimeStr<N> is provided as the second argument, but the template parameter FmtStr
   * is completely unreferenced in the signature <std::chrono::year_month_day, CompTimeStr<N>>.
   * Because FmtStr cannot be deduced by the compiler, compilation will fail.

  template <std::size_t N, specializedTypes::CompTimeStr<N> FmtStr>
  struct Format_SpecializedImplementation<std::chrono::year_month_day, specializedTypes::CompTimeStr<N> >
  */
  template <std::size_t N, specializedTypes::CompTimeStr<N> FmtStr>   // FmtStr = "{:%d-%m-%Y}" with curly braces
  struct Format_SpecializedImplementation<std::chrono::year_month_day, FmtStr>
    : public Format_StringStream_Base
  {
    // Evaluates to this specific instantiated class type
    using type = Format_SpecializedImplementation<std::chrono::year_month_day, FmtStr>;

    inline static constexpr specializedTypes::CompTimeStr<N> dateFormat_compileTime{FmtStr};

    // Returns a default-constructed instance of this specialized formatter
    static type getDefaultFormatArgs()
    {
      return type();
    }

    // Safely inherit all constructors from the Base class
    using Format_StringStream_Base::Format_StringStream_Base;

    /// Rule of 3 / 5 Rule Compliance
    Format_SpecializedImplementation(const Format_SpecializedImplementation&  other) = default;
    Format_SpecializedImplementation(      Format_SpecializedImplementation&& other) = default;

    template<
              class CharT,
              class Traits = std::char_traits<CharT>
            >
    void applyFormatArgs(std::basic_ios<CharT, Traits>& ios) const
    {
      // Forward the locale configuration up to the stream base
      Format_StringStream_Base::applyFormatArgs(ios);
    }
  };

  template <>
  struct Format_StringStream<std::chrono::year_month_day>
    : public Format_StringStream_Base
  {
    using type = Format_StringStream<std::chrono::year_month_day>;

    static type getDefaultFormatArgs()
    {
      return type();
    }

    // Safely inherit all constructors from the Base class
    using Format_StringStream_Base::Format_StringStream_Base;

    /// Rule of 3 / 5 Rule Compliance
    Format_StringStream(const Format_StringStream&  other) = default;
    Format_StringStream(      Format_StringStream&& other) = default;

    template<
              class CharT,
              class Traits = std::char_traits<CharT>
            >
    void applyFormatArgs(std::basic_ios<CharT, Traits>& ios) const
    {
      Format_StringStream_Base::applyFormatArgs(ios);
    }
  };

  // ]=============================================================] COMMON_FORMAT


  // [=============================================================[ ConvertFromStr

  template<>
  struct isS2TspecializedImplementationAvailable<std::chrono::year_month_day> : std::true_type, Error_InApplicable_False {};

  /**
   * @brief     Specialized implementation handling string to 'std::chrono::year_month_day' conversion.
   */
  template<Str2TnErrorHandler ERR_HANDLER>
          requires ( isSupported_Str2TnErrorHandler<std::chrono::year_month_day, ERR_HANDLER>::value )
  struct ConvertFromStr<std::chrono::year_month_day, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION,
                                                     ERR_HANDLER >
  {
  private:
    inline static std::chrono::year_month_day _dateLib2chrono(const datelibFrom::year_month_day& val)
    {
#if    USE_CHRONO_FROMSTREAM_1 == 1
      return val;
#else // if  USE_DATELIB_FROMSTREAM_2 == 1
      return std::chrono::year_month_day {
          std::chrono::year{static_cast<int>(val.year())},
          std::chrono::month{static_cast<unsigned>(val.month())},
          std::chrono::day{static_cast<unsigned>(val.day())}
      };
#endif
    }

    /**
     * @brief   Converts string holding 'year_month_day' value. The string has the format "%F" -> "%Y-%m-%d"
     * @param   str                 input string representing date.
     * @param   fmt                 a format string of date-string.
     * @returns 'year_month_day'.
     */
    inline static
    typename ReturnType<std::chrono::year_month_day, ERR_HANDLER>::type
    _ToVal( const std::string& str,
            std::istringstream& iss,
            const std::string::value_type* fmt )
    {
      datelibFrom::year_month_day ymd;

      // Ensure the stream is strictly configured NOT to throw(e.g std::ios_base::failure) under any flag changes
      iss.exceptions(std::ios_base::goodbit);  // IMPORTANT flag

#if    USE_CHRONO_FROMSTREAM_1 == 1
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< std::chrono::year_month_day, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION, " << ERR_HANDLER << ">_ToVal()->  calling std::chrono::parse()");
      // Parse string into chrono::year_month_day object (C++20)
      iss >> std::chrono::parse(fmt, ymd);
#else
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< std::chrono::year_month_day, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION, " << ERR_HANDLER << ">_ToVal()->  calling date::from_stream()");
      date::from_stream(iss, fmt, ymd);
#endif

      const bool is_failed = iss.fail();
      const bool is_bad = iss.bad();
      //const bool is_eof = iss.eof();

      // Validate stringstream is parsed as expected
      if (is_failed || is_bad)
      {
        std::ostringstream ess;

        ess << ((USE_CHRONO_FROMSTREAM_1 == e_ENABLE_FEATURE)? "std::chrono" : "(lib->)date" );
        ess << " ::: strYMD='" << str << "' , format='" << fmt << "' stream-parse failed.";
        ess << " iss.fail()=" << is_failed << " iss.bad()="  << is_bad;

        return ErrorHandler<value_type, ERR_HANDLER>::template handle<std::invalid_argument>(str, ess.str());
      }

      // Validate structural boundaries for final object safety
      if (!ymd.ok())
      {
        std::ostringstream ess{};

        ess << ((USE_CHRONO_FROMSTREAM_1 == e_ENABLE_FEATURE)? "std::chrono" : "(lib->)date" );
        ess << " ::: strYMD='" << str << "' , format='" << fmt << "' invalid-date, conversion failed." << std::endl;

        return ErrorHandler<value_type, ERR_HANDLER>::template handle<std::invalid_argument>(str, ess.str());
      }


      return _dateLib2chrono(ymd);
    }

  public:
    constexpr static size_t templateId = 10000;
    constexpr static bool isConversionImplemented = true;
    using value_type  = std::chrono::year_month_day;
    using return_type = typename ReturnType<std::chrono::year_month_day, ERR_HANDLER>::type;
    constexpr static Str2TnErrorHandler errHandler = ERR_HANDLER;
    constexpr static Str2TnConversionProcess conversionProcess = Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION;

    /**
     * @brief   Converts string holding 'year_month_day' value.
     * @param   str                 input string representing date.
     * @param   fmt                 a format string of date-string. format like "%F" -> "%Y-%m-%d"
     * @returns 'year_month_day'.
     */
    inline static return_type
    ToVal(  const std::string& str,
            const std::string::value_type* fmt = "%F" )
    {
      std::istringstream iss(str);
      return _ToVal(str, iss, fmt);
    }

    /**
     * @brief   Converts string holding 'year_month_day' value. The string has the format "%F" -> "%Y-%m-%d"
     * @param   str                 input string representing date.
     * @param   fmt                 a format string of date-string.
     * @param   loc                 to set locale of the stream.
     * @returns 'year_month_day'.
     */
    inline static return_type
    ToVal(  const std::string& str,
            const typename Format_SpecializedImplementation<std::chrono::year_month_day>::type& format_ss)
            // = Format_SpecializedImplementation<std::chrono::year_month_day>::getDefaultFormatArgs() )
    {
      std::istringstream iss(str);
      format_ss.applyFormatArgs(iss);
      return _ToVal(str, iss, format_ss.getDateFormat());
    }
  };
  // ]=============================================================] ConvertFromStr



  // [=============================================================[ ConvertFromVal

  template<>
  struct isT2SspecializedImplementationAvailable<std::chrono::year_month_day> : std::true_type {};

  /**
   * @brief     Specialized implementation handling 'year_month_day' to string conversion.
   */
  template<>
  struct ConvertFromVal<std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION>
  {
    constexpr static size_t templateId = 10000;
    constexpr static bool isConversionImplemented = true;
    using value_type  = std::chrono::year_month_day;
    using return_type = std::string;
    constexpr static Tn2StrConversionProcess conversionProcess = Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION;

    // Create static compile-time fallback constant token
#if    USE_CHRONO_TOSTREAM_1 == 1
    inline static constexpr specializedTypes::CompTimeStr defaultFmt{"{:%F}"};   // "%F" -> "%Y-%m-%d"
#else
    inline static constexpr specializedTypes::CompTimeStr defaultFmt{"%F"};   // "%F" -> "%Y-%m-%d"
#endif

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @param   format_ss           locale for stringstream based conversion.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToStr(const std::chrono::year_month_day& val,
          const typename Format_SpecializedImplementation<std::chrono::year_month_day>::type& format_ss) // "%F" -> "%Y-%m-%d"
          // = Format_SpecializedImplementation<std::chrono::year_month_day>::getDefaultFormatArgs() )
    {
#if    USE_CHRONO_TOSTREAM_1 == 1
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >::ToStr(val='" << val << "', format_ss)");
#else
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >::ToStr(dateStr, format_ss)");
#endif

      std::ostringstream oss;
      format_ss.applyFormatArgs(oss); // set locale for stringstream based conversion

#if    USE_CHRONO_TOSTREAM_1 == 1
      //std::chrono::to_stream(oss, fmt, val);

      std::string fmtV("{:");
      fmtV += format_ss.getDateFormat();  // format_ss.getDateFormat() = "%F" -> "%Y-%m-%d"
      fmtV += "}";
      oss << std::vformat(fmtV, std::make_format_args(val));
#else // if  USE_DATELIB_TOSTREAM_2 == 1
      //oss << date::format(fmt, val);
      date::to_stream(oss, format_ss.getDateFormat().c_str(), _chrono2dateLib(val));
#endif

      return oss.str();
    }

    template< specializedTypes::CompTimeStr FS = defaultFmt > // "%F" -> "%Y-%m-%d"
    inline static return_type
    ToStr(const std::chrono::year_month_day& val,
          const typename Format_SpecializedImplementation<std::chrono::year_month_day, FS>::type& format_ss
           = Format_SpecializedImplementation<std::chrono::year_month_day, FS>::getDefaultFormatArgs() )
    {
#if    USE_CHRONO_TOSTREAM_1 == 1
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >::ToStr<" << FS.data << ">(val='" << val << "', format_ss)");
#else
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >::ToStr<" << FS.data << ">(dateStr, format_ss)");
#endif

      std::ostringstream oss;
      format_ss.applyFormatArgs(oss); // set locale for stringstream based conversion

      // Convert your compile-time data safely into a format string wrapper
      // This eliminates the need for expensive 'std::ostringstream' heap overhead!
#if    USE_CHRONO_TOSTREAM_1 == 1
      constexpr std::format_string<std::chrono::year_month_day> fmtStr{ FS.data };
#endif


      if (format_ss.hasLocParam())
      {
        oss << formatLibTo::format( format_ss.getLoc(),
#if    USE_CHRONO_TOSTREAM_1 == 1
          fmtStr
#else
          FS.data
#endif
        , _chrono2dateLib(val) );
      } else {
        oss << formatLibTo::format(
#if    USE_CHRONO_TOSTREAM_1 == 1
          fmtStr
#else
          FS.data
#endif
        , _chrono2dateLib(val) );
      }

      return oss.str();
    }

    /**
     * @brief   Converts 'year_month_day' using a user-provided string literal.
     * @note    The template parameter ensures compile-time format validation.
     * @param   val     Input 'year_month_day'.
     * @param   fmt     String literal format wrapper (e.g., "%Y-%m-%d").
     * @returns Formatted std::string.
     */
    template<std::size_t N>
    inline static std::string
    ToStr( const std::chrono::year_month_day& val,
           const char (&fmt)[N])   // "%Y-%m-%d"
    {
#if    USE_CHRONO_TOSTREAM_1 == 1
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, SPECIALIZED_IMPLEMENTATION >::ToStr<N>(val='" << val << "', '" << fmt << "')");
#else
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, SPECIALIZED_IMPLEMENTATION >::ToStr<N>(dateStr, '" << fmt << "')");
#endif

      return formatLibTo::format( fmt, _chrono2dateLib(val) ); // as std::format() accepts literal string as compile-time constant
      //return formatLibTo::format("%Y-%m-%d", val);
    }

    template<std::size_t N>
    inline static std::string
    ToStr( const std::chrono::year_month_day& val,
           const char (&fmt)[N],   // "%Y-%m-%d"
           const std::locale& loc) // Passed as a traditional runtime reference
    {
#if    USE_CHRONO_TOSTREAM_1 == 1
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, SPECIALIZED_IMPLEMENTATION >::ToStr<N>(val='" << val << "', '" << fmt << "', loc)");
#else
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, SPECIALIZED_IMPLEMENTATION >::ToStr<N>(dateStr, '" << fmt << "', loc)");
#endif

      // std::format accepts a std::locale as its first argument
      return formatLibTo::format(loc, fmt, _chrono2dateLib(val) ); // as std::format() accepts literal string as compile-time constant
      //return formatLibTo::format(loc, "%Y-%m-%d", val);
    }
/*
int main() {
    using namespace std::chrono;
    year_month_day date = 2026y / July / 9d;

    // Imbue the stream with the user's native system environment environment locale
    std::cout.imbue(std::locale(""));

    // The 'L' flag forces format to use the stream's current locale settings
    std::cout << std::format("{:L%x}", date) << '\n';
}
*/

  private:
    inline static datelibTo::year_month_day _chrono2dateLib(const std::chrono::year_month_day& val)
    {
#if    USE_CHRONO_TOSTREAM_1 == 1
      return val;
#else // if  USE_DATELIB_TOSTREAM_2 == 1
      return date::year_month_day {
          date::year{static_cast<int>(val.year())},
          date::month{static_cast<unsigned>(val.month())},
          date::day{static_cast<unsigned>(val.day())}
      };
#endif
    }
  };

  // ]=============================================================] ConvertFromVal


  // [=============================================================[ COMMON_FORMAT

  template <>
  struct FormatInfo< std::chrono::year_month_day, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION >
  {
    static auto getDefaultFormatArgs()
    {
      return Format_SpecializedImplementation<std::chrono::year_month_day>::getDefaultFormatArgs();
    }

    using type = Format_StringStream<std::chrono::year_month_day>::type;
  };


  template <>
  struct FormatInfo< std::chrono::year_month_day, Tn2StrConversionProcess::STRINGSTREAM >
  {
    static auto getDefaultFormatArgs()
    {
      return Format_StringStream<std::chrono::year_month_day>::getDefaultFormatArgs();
    }

    using type = Format_StringStream<std::chrono::year_month_day>::type;
  };


  template <specializedTypes::CompTimeStr FS>
  struct FormatInfo < std::chrono::year_month_day,
                      Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION,
                      FS  // = ConvertFromVal<std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION>::defaultFmt
                          // = "{:%F}" for USE_CHRONO_TOSTREAM_1 == 1, ELSE
                          // =   "%F"  for USE_DATELIB_TOSTREAM_2 == 1
                    >
  {
    static auto getDefaultFormatArgs()
    {
      return Format_SpecializedImplementation < std::chrono::year_month_day, FS >::getDefaultFormatArgs();
    }

    using type = Format_SpecializedImplementation < std::chrono::year_month_day, FS >::type;
  };


  // ]=============================================================] COMMON_FORMAT
}
