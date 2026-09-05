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
#include <locale>
#include <algorithm>
#include <cctype>

#include <converter/_common.h>
#include <specializedTypes/CompTimeStr.h>


#include <converter/_convertDateWorkaround.h>
#include <converter/specializedHelper/_dateFormat.h>


/*
                         converter
                            │
              ┌─────────────┴─────────────┐
              │                           │
       Convert date/string          Stream date
              │                           │
       _convertDate.h               _dateFormat.h
              │                           │
              ↓                           ↓
  _convertDateWorkaround.h       date_format<> + pword
              │                           │
       chrono / HH-date          converter::operator<<
*/




namespace converter
{



  // [=============================================================[ COMMON_FORMAT

  // Optional-Locale and no Format
  // if OS-compiler combination supports istringstream << std::chrono::year_month_day
  // then   ConvertFromVal< T, Tn2StrConversionProcess::STRINGSTREAM >:: ToStr(const T& val,
  //                                                                           const typename Format_StringStream<T>::type& format_ss)
  // gets enabled where T = std::chrono::year_month_day, refer _convertT2Str.h
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



  // Optional-Locale and CompileTime-evaluation of std::format_string
  /*
   * FmtStr must appear in the specialization argument list.In your current snippet,
   * CompTimeStr<N> is provided as the second argument, but the template parameter FmtStr
   * is completely unreferenced in the signature <std::chrono::year_month_day, CompTimeStr<N>>.
   * Because FmtStr cannot be deduced by the compiler, compilation will fail.

      template <std::size_t N, specializedTypes::CompTimeStr<N> FmtStr>
      struct Format_SpecializedImplementation<std::chrono::year_month_day, specializedTypes::CompTimeStr<N> >
  */
  //template <std::size_t N, specializedTypes::CompTimeStr<N> FmtStr>   // FmtStr = "{:%d-%m-%Y}" with curly braces
  template < specializedTypes::CompTimeStr FmtStr>   // FmtStr = "{:%d-%m-%Y}" with curly braces
  struct Format_SpecializedImplementation<std::chrono::year_month_day, FmtStr>
    : public Format_StringStream_Base
  {
    // Evaluates to this specific instantiated class type
    using type = Format_SpecializedImplementation<std::chrono::year_month_day, FmtStr>;

    //inline static constexpr specializedTypes::CompTimeStr<N> dateFormat_compileTime{FmtStr};
    inline static constexpr specializedTypes::CompTimeStr dateFormat_compileTime{FmtStr};

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


  // Optional-Locale and RunTime-Date-Format
  /*
   * use this
  */
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
                        const std::string& fmt, // "{:%d-%m-%Y}"
                        const std::locale& loc) // = std::locale{},
                        //std::ios_base::fmtflags flags = std::ios_base::fmtflags(0)  -> use later if needed
        : Format_StringStream_Base(loc), _dateFormat_runTime(fmt)
    {}

    Format_SpecializedImplementation(const std::string& fmt) // "{:%d-%m-%Y}"
        : Format_StringStream_Base(), _dateFormat_runTime(fmt)
    {}

    // Rule of 3 / 5 Rule Compliance
    Format_SpecializedImplementation(const Format_SpecializedImplementation& other) = default;
    Format_SpecializedImplementation(Format_SpecializedImplementation&& other) = default;

    // 2. Returns a default instance using the fallback literal string format
    static type getDefaultFormatArgs()
    {
      return type("{:%F}"); // "{:%F}" standard equivalent to "{:%d-%m-%Y}"
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
      return _dateFormat_runTime;  //"{:%d-%m-%Y}"
    }

  private:
    const std::string _dateFormat_runTime;  // "{:%d-%m-%Y}"
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
            const char* fmt )
    {
      datelibFrom::year_month_day ymd;

      // Ensure the stream is strictly configured NOT to throw(e.g std::ios_base::failure) under any flag changes
      iss.exceptions(std::ios_base::goodbit);  // IMPORTANT flag

      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromStr< std::chrono::year_month_day, Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION, " << ERR_HANDLER << ">_ToVal()->  calling " << workaround::DateAdapterS2T<datelibFrom::year_month_day>::functionName);

      workaround::DateAdapterS2T<datelibFrom::year_month_day>::fromStream(iss, fmt, ymd);    // "%F" -> "%Y-%m-%d"

      const bool is_failed = iss.fail();
      const bool is_bad = iss.bad();
      //const bool is_eof = iss.eof();

      // Validate stringstream is parsed as expected
      if (is_failed || is_bad)
      {
        std::ostringstream ess;

        ess << workaround::DateAdapterS2T<datelibFrom::year_month_day>::dateLibName << " ::: strYMD='" << str << "' , format='" << fmt << "' stream-parse failed.";
        ess << " iss.fail()=" << is_failed << " iss.bad()="  << is_bad;

        return ErrorHandler<value_type, ERR_HANDLER>::template handle<std::invalid_argument>(str, ess.str());
      }

      // Validate structural boundaries for final object safety
      if (!ymd.ok())
      {
        std::ostringstream ess{};

        ess << workaround::DateAdapterS2T<datelibFrom::year_month_day>::dateLibName << " ::: strYMD='" << str << "' , format='" << fmt << "' invalid-date, conversion failed." << std::endl;

        return ErrorHandler<value_type, ERR_HANDLER>::template handle<std::invalid_argument>(str, ess.str());
      }


      return workaround::DateAdapter::toChrono(ymd);
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
            const char* fmt = "%F" )
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

     /**
      * @brief   Converts string holding a possibly-numerical value to floating-point datatype representation.
      * @param   str                 input string.
      * @param   format_ss           locale for stringstream based conversion.
      * @returns type value if conversion succeeds.
      */
    inline static return_type
    ToStr(const std::chrono::year_month_day& val,
          const typename Format_SpecializedImplementation<std::chrono::year_month_day>::type& format_ss) // "{:%F}" -> "{:%Y-%m-%d}"
          // = Format_SpecializedImplementation<std::chrono::year_month_day>::getDefaultFormatArgs() )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >::ToStr(" << _chrono2string(val) << ", format_ss)");

      std::ostringstream oss;
      format_ss.applyFormatArgs(oss); // set locale for stringstream based conversion

      workaround::DateAdapterT2S<datelibTo::year_month_day>::toStream(oss, format_ss.getDateFormat(), val);  // "{:%F}" -> "{:%Y-%m-%d}"
      return oss.str();
    }

    //template <std::size_t N, specializedTypes::CompTimeStr<N> FmtStr
    template <specializedTypes::CompTimeStr FmtStr = "{:%F}" >
    inline static return_type
    ToStr(const std::chrono::year_month_day& val,
          const typename Format_SpecializedImplementation<std::chrono::year_month_day, FmtStr>::type& format_ss
           = Format_SpecializedImplementation<std::chrono::year_month_day, FmtStr>::getDefaultFormatArgs() )
    {
      CONVERTER_DEBUG_LOG("trace :: return_type ConvertFromVal< std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION >::ToStr<" << FmtStr.data << ">(" << _chrono2string(val) << ", format_ss)");

      if (format_ss.hasLocParam())
      {
        return workaround::DateAdapterT2S<datelibTo::year_month_day>::format<FmtStr.data>( format_ss.getLoc(), val );
      } else {
        return workaround::DateAdapterT2S<datelibTo::year_month_day>::format<FmtStr.data>( val );
      }
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
    inline static std::string _chrono2string(const std::chrono::year_month_day& val)
    {

      std::ostringstream oss;
      oss << "{ YYYY[" << static_cast<int>(val.year()) << "]/mm[" << static_cast<unsigned>(val.month())
          << "]/dd[" << static_cast<unsigned>(val.day()) << "] }" << std::endl;

      return oss.str();
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

    using type = typename Format_StringStream<std::chrono::year_month_day>::type;
  };


  template <>
  struct FormatInfo< std::chrono::year_month_day, Tn2StrConversionProcess::STRINGSTREAM >
  {
    static auto getDefaultFormatArgs()
    {
      return Format_StringStream<std::chrono::year_month_day>::getDefaultFormatArgs();
    }

    using type = typename Format_StringStream<std::chrono::year_month_day>::type;
  };


  template <specializedTypes::CompTimeStr FS>
  struct FormatInfo < std::chrono::year_month_day,
                      Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION,
                      FS  // = ConvertFromVal<std::chrono::year_month_day, Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION>::defaultFmt
                          // = "{:%F}"
                    >
  {
    static auto getDefaultFormatArgs()
    {
      return Format_SpecializedImplementation < std::chrono::year_month_day, FS >::getDefaultFormatArgs();
    }

    using type = typename Format_SpecializedImplementation < std::chrono::year_month_day, FS >::type;
  };


  // ]=============================================================] COMMON_FORMAT
}
