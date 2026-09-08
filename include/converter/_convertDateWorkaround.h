/*
 * _convertDateWorkaround.h
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


/*
 * Date functionality workaround
 *
 * std::chrono support differs across compiler/standard-library
 * implementations.
 *
 * SUPPORTED_DATE_LIB_FOR_FROMSTREAM:
 *     Selects the date library that provides the required
 *     string-to-date parsing capability.
 *
 * SUPPORTED_DATE_LIB_FOR_TOSTREAM:
 *     Selects the date library that provides the required
 *     date-to-string formatting capability.
 *
 * std::chrono is always preferred.
 * Howard Hinnant date is used only as a fallback.
 *
 * This file is expected to become redundant as support for the
 * required std::chrono functionality becomes universally available
 * across widely used OSes and C++ compilers.
 *
 *    Today
 *    ─────
 *
 *    _convertDate.h
 *        │
 *        └── _convertDateWorkaround.h
 *                  │
 *                  ├── std::chrono when supported
 *                  └── HH-date fallback
 *
 *
 *    A few years later
 *    ─────────────────
 *
 *    _convertDate.h
 *        │
 *        └── std::chrono
 *
 *    *delete _convertDateWorkaround.h*
 *
 * The macros SUPPORTED_DATE_LIB_FOR_FROMSTREAM and SUPPORTED_DATE_LIB_FOR_TOSTREAM
 * are declared in <converter/_workaroundConfig.h>, which is not
 * part of converter repository, but an header artifact created
 * during the cmake's config call. cmake run's a few probes to
 * determine the availabilty of required std::chrono functionality
 * and if it fails checks for the same in the fall-back Howard Hinnant date.
 * Accordingly SUPPORTED_DATE_LIB_FOR_FROMSTREAM and SUPPORTED_DATE_LIB_FOR_TOSTREAM
 * are configured with the probes results.
 * The header artifact <converter/_workaroundConfig.h> lies in the
 * build-folder that cmake creates and then cmake includes that
 * build-folder-path as well at the time of compilation.
 */







#include <chrono>
//#include <string_view>
#include <cstring>
#include <string>


#include <converter/_workaroundConfig.h>

#include <converter/_common.h>
#include <specializedTypes/CompTimeStr.h>



#if    SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_CHRONO
  #include <format>
#endif

#if    SUPPORTED_DATE_LIB_FOR_FROMSTREAM == e_DATE_HHDATE ||  SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_HHDATE
  #include <date/date.h>
#endif

#if    SUPPORTED_DATE_LIB_FOR_FROMSTREAM == e_DATE_CHRONO
  using t_datelibFrom = std::chrono::year_month_day;
#elif  SUPPORTED_DATE_LIB_FOR_FROMSTREAM == e_DATE_HHDATE
  using t_datelibFrom = date::year_month_day;
#else
  static_assert(
    SUPPORTED_DATE_LIB_FOR_FROMSTREAM != e_DATE_NO_LIB,
    "No supported date library provides the required from_stream functionality."
  );
#endif

#if    SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_CHRONO
  using t_datelibTo   = std::chrono::year_month_day;
#elif  SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_HHDATE
  using t_datelibTo   = date::year_month_day;
#else
  static_assert(
    SUPPORTED_DATE_LIB_FOR_TOSTREAM != e_DATE_NO_LIB,
    "No supported date library provides the required to_stream functionality."
  );
#endif


namespace converter
{

  // [=============================================================[ WORKAROUND ADAPTER

  namespace workaround
  {
    struct DateAdapter
    {
      inline static std::chrono::year_month_day toChrono(const std::chrono::year_month_day& val) { return val; }

#if  SUPPORTED_DATE_LIB_FOR_FROMSTREAM == e_DATE_HHDATE ||  SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_HHDATE
      inline static std::chrono::year_month_day toChrono(const date::year_month_day& val)
      {
        return std::chrono::year_month_day {
            std::chrono::year{static_cast<int>(val.year())},
            std::chrono::month{static_cast<unsigned>(val.month())},
            std::chrono::day{static_cast<unsigned>(val.day())}
        };
      }

      inline static date::year_month_day toDate(const std::chrono::year_month_day& val)
      {
        return date::year_month_day {
            date::year{static_cast<int>(val.year())},
            date::month{static_cast<unsigned>(val.month())},
            date::day{static_cast<unsigned>(val.day())}
        };
      }
#endif
    };



    // ================================================================
    // std::chrono date adapter
    // ================================================================
    template <typename ResultDateType>
    struct DateAdapterS2T
    { };

#if SUPPORTED_DATE_LIB_FOR_FROMSTREAM == e_DATE_CHRONO
    template <>
    struct DateAdapterS2T<std::chrono::year_month_day>
    {
      inline static constexpr std::string_view dateLibName  = "std::chrono";
      inline static constexpr std::string_view functionName = "std::chrono::parse()";

      inline static auto fromStream(
          std::istringstream& iss,
          const char* fmt,    // "%F" -> "%Y-%m-%d"
          std::chrono::year_month_day& value)
              -> decltype(
                  std::chrono::from_stream(iss, fmt, value)
              )
      {
        return std::chrono::from_stream(iss, fmt, value);
        /*
          template< class CharT, class Traits, class Alloc = std::allocator<CharT> >
          std::basic_istream<CharT, Traits>&
          std::chrono::from_stream(std::basic_istream<CharT, Traits>& is, const CharT* fmt,
                                   std::chrono::year_month_day& ymd,
                                   std::basic_string<CharT, Traits, Alloc>* abbrev = nullptr,
                                   std::chrono::minutes* offset = nullptr );
        */
      }
    };
#endif


    template <typename InputDateType>
    struct DateAdapterT2S
    { };


#if SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_CHRONO
    template <>
    struct DateAdapterT2S<std::chrono::year_month_day>
    {
      inline static constexpr std::string_view dateLibName   = "std::chrono";
      inline static constexpr std::string_view functionName1 = "std::vformat()";
      inline static constexpr std::string_view functionName2 = "std::format()";

      // Run-Time evaluation
      inline static std::ostream&
      toStream( std::ostream& oss,
                const std::string& fmt,    // "{:%F}" -> "{:%Y-%m-%d}"
                const std::chrono::year_month_day& val)
      {
        //std::chrono::to_stream(oss, fmt, val);

        oss.exceptions(std::ios::failbit | std::ios::badbit);
        oss << std::vformat(fmt, std::make_format_args(val));  // fmt = "{:%F}" -> "{:%Y-%m-%d}"
            // return std::format(fmt, today); this only works when fmt is string literal
            // like "{:%d %B %Y}"  gets converted to "std::format_string<Args...> fmt"
            // Compile-Time Validation: it is a template wrapper,
            // the C++ compiler parses your format string ("{:%d %B %Y}") during compilation.
        return oss;
      }



      // Compile-Time evaluation using std::format_string<> argument of std::format()
      template< specializedTypes::CompTimeStr FmtStr > // "{:%F}" -> "{:%Y-%m-%d}"
      inline static std::string
      format( const std::locale& loc,
              //const std::string& fmt,
              const std::chrono::year_month_day& val)
      {
        //std::format_string<std::chrono::year_month_day> fmt_str{FmtStr.data};
        return std::format(loc, FmtStr.data, val);
      }

      // Compile-Time evaluation using std::format_string<> argument of std::format()
      template< specializedTypes::CompTimeStr FmtStr > // "{:%F}" -> "{:%Y-%m-%d}"
      inline static std::string
      format( //const std::string& fmt,
              const std::chrono::year_month_day& val)
      {
        //std::format_string<std::chrono::year_month_day> fmt_str{FmtStr.data};
        return std::format(FmtStr.data, val);
      }
    };
#endif





    // ================================================================
    // Howard Hinnant date adapter
    // ================================================================
#if SUPPORTED_DATE_LIB_FOR_FROMSTREAM == e_DATE_HHDATE
    template <>
    struct DateAdapterS2T<date::year_month_day>
    {
      inline static constexpr std::string_view dateLibName  = "HH-date";
      inline static constexpr std::string_view functionName = "date::from_stream()";

      inline static auto fromStream(
          std::istringstream& iss,
          const char* fmt,    // "%F" -> "%Y-%m-%d"
          date::year_month_day& value)
              -> decltype(
                  date::from_stream(iss, fmt, value)
              )
      {
        return date::from_stream(iss, fmt, value);
        /*
          template <class CharT, class Traits, class Duration, class Alloc>
          std::basic_istream<CharT, Traits>&
          date::from_stream(std::basic_istream<CharT, Traits>& is, const CharT* fmt,
                            fields<Duration>& fds,
                            std::basic_string<CharT, Traits, Alloc>* abbrev = nullptr,
                            std::chrono::minutes* offset = nullptr);
        */
      }
    };
#endif


#if SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_HHDATE
    template <>
    struct DateAdapterT2S<date::year_month_day>
    {
      inline static constexpr std::string_view dateLibName  = "HH-date";
      inline static constexpr std::string_view functionName1 = "date::to_stream()";
      inline static constexpr std::string_view functionName2 = "date::format()";

      // Run-Time evaluation
      inline static std::ostream&
      toStream( std::ostream& oss,
                const std::string& fmt,    // "{:%F}" -> "{:%Y-%m-%d}"
                const std::chrono::year_month_day& val)
      {
        std::string stripped_fmt = _stripFormat(fmt);

        oss.exceptions(std::ios::failbit | std::ios::badbit);
        date::to_stream(oss, stripped_fmt.c_str(), DateAdapter::toDate(val));
        return oss;
      }

      // Run-Time evaluation
      inline static std::string
      format( const std::locale& loc,
              const std::string& fmt, // "{:%F}" -> "{:%Y-%m-%d}"
              const std::chrono::year_month_day& val)
      {
        std::string stripped_fmt =   //  strip "{:%F}" to "%F"
                        _stripFormat(fmt);

        return date::format(loc, stripped_fmt.c_str(), DateAdapter::toDate(val)); // returns val in std::string in the format specified by fmt
            //
            //  template <class CharT, class Traits, class Alloc, class Streamable>               :: Streamable -> date::year_month_day
            //  auto              -> std::basic_string<CharT, Traits, Alloc>
            //  format(const std::locale& loc, const std::basic_string<CharT, Traits, Alloc>& fmt,
            //         const Streamable& tp)
            //      -> decltype(to_stream(std::declval<std::basic_ostream<CharT, Traits>&>(), fmt.c_str(), tp),
            //                  std::basic_string<CharT, Traits, Alloc>{})
            //  {
            //      std::basic_ostringstream<CharT, Traits, Alloc> os;
            //      os.exceptions(std::ios::failbit | std::ios::badbit);
            //      os.imbue(loc);
            //      to_stream(os, fmt.c_str(), tp);
            //      return os.str();
            //  }
            //
      }

      // for symmetry of function-call  DateAdapterT2S<std::chrono::year_month_day>::format<specializedTypes::CompTimeStr>(...)
      // this helps for drop in polymorphic call in _convertDate.h,
      // refer function ConvertFromVal< std::chrono::year_month_day,
      //                                Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION
      //                              >::ToStr<specializedTypes::CompTimeStr>(...)
      template< specializedTypes::CompTimeStr FmtStr > // "{:%F}" -> "{:%Y-%m-%d}"
      inline static std::string
      format( const std::locale& loc,
              //const std::string& fmt,
              const std::chrono::year_month_day& val)
      {
        return format(loc, FmtStr.data, val);
      }



      // Run-Time evaluation
      inline static std::string
      format( const std::string& fmt, // "{:%F}" -> "{:%Y-%m-%d}"
              const std::chrono::year_month_day& val)
      {
        std::string stripped_fmt =   //  strip "{:%F}" to "%F"
                        _stripFormat(fmt);

        return date::format(stripped_fmt.c_str(), DateAdapter::toDate(val)); // returns val in std::string in the format specified by fmt
      }

      // for symmetry of function-call  DateAdapterT2S<std::chrono::year_month_day>::format<specializedTypes::CompTimeStr>(...)
      // this helps for drop in polymorphic call in _convertDate.h,
      // refer function ConvertFromVal< std::chrono::year_month_day,
      //                                Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION
      //                              >::ToStr<specializedTypes::CompTimeStr>(...)
      template< specializedTypes::CompTimeStr FmtStr > // "{:%F}" -> "{:%Y-%m-%d}"
      inline static std::string
      format( //const std::string& fmt,
              const std::chrono::year_month_day& val)
      {
        return format(FmtStr.data, val);
      }

    private:
      inline static std::string
      _stripFormat(const std::string& fmt)
      {
        // remove curly bracket here as it's not used byHH-date, where as std::chrono::format() expects format
        // in "{:FORMAT}" format. To keep uniformaty of API call, we are enforcing "std::chrono::format()"
        // expectation to "HH-date" and remove "{:" and "}" from front and back of the fmt input-variable.
        if( fmt.size() > 3 &&
            fmt[0] == '{' &&
            fmt[1] == ':' &&
            fmt[fmt.size()-1] == '}' )
        {
          return fmt.substr(2, fmt.size() - 3 );
          // fmt.size() - 3 :: as we are removing 3 chars; 2 "{:" from front
          //                   and 1 "}" from back
        } else {
          std::ostringstream errStrm{};
          errStrm << "Error in format parameter passed to DateAdapterT2S<date::year_month_day>::toStream(). format='";
          errStrm << fmt << "' is not in the style '{:DATE-FORMAT}' i.e it should start with '{:' and end with '}'";
          throw std::invalid_argument(errStrm.str());
        }
      }
    };
#endif


  } // namespace workaround

    // ]=============================================================] WORKAROUND ADAPTER
}