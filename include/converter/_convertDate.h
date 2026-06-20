/*
 * _convertDate.h
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
#include <converter/_convertS2T.h>
#include <converter/_convertT2S.h>

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
#elif  USE_DATE_TOSTREAM_2 == e_ENABLE_FEATURE
  namespace datelibTo = date;
#endif


namespace converter
{

  //template<>
  //struct isSpecializedImplementationAvailable<std::chrono::year_month_day> : std::true_type {};

  // [=============================================================[ COMMON_FORMAT

  /*
   *  IMPORTANT : constexpr char[] should be inline . Else only constexpr refers to char*
   *  (i.e pointer address instead of the underlying value) to an internal linkage for EVERY translation unit.
   *
   *  compilation fails with message "has a field  whose type uses the anonymous namespace [-Wsubobject-linkage]"
   *
   *  https://stackoverflow.com/questions/72801432/why-do-i-get-the-child-has-a-base-whose-type-uses-the-anonymous-namespace-warn
   *
   *  "inline implies external linkage (even if the variable is const)"
   *
   *  For more details on anonymous namespace , refer ::
   * https://comp.lang.cpp.moderated.narkive.com/kbqPsoZR/warning-has-a-field-whose-type-uses-the-anonymous-namespace
   */
  inline constexpr char defYMDfmt[] = "%F";  // string literal object with static storage duration; inline implies external linkage


  template<c_iostream IOSS, specializedTypes::CompTimeStr _ymdFormat = defYMDfmt> // %F -> "%Y-%m-%d"
  struct Format_StreamYMD
  {
    using stream_type = IOSS;
    constexpr static const char* ymdFormat = _ymdFormat.data;

    constexpr static inline
    std::enable_if_t< std::is_same_v<typename IOSS::char_type,char>, void>
    streamUpdate([[maybe_unused]] IOSS& ss) {}
  };
  // ]=============================================================] COMMON_FORMAT


  // [=============================================================[ S2T_FORMAT
  // [[============[[ Conversion formats
  template < specializedTypes::CompTimeStr formatYMD = defYMDfmt, // %F -> "%Y-%m-%d"
             FailureS2Tprocess PROCESS_ERR = FailureS2Tprocess::THROW_ERROR >
  struct S2T_Format_StreamYMD : public Format_StreamYMD<std::istringstream, formatYMD>,
                                public OnError<std::chrono::year_month_day, PROCESS_ERR> {};
  // ]]============]] Conversion formats


  // [[============[[ type - Default Conversion format
  template<>
  struct S2T_DefaultFormat<std::chrono::year_month_day, void>
  {
    using type = S2T_Format_StreamYMD< defYMDfmt, FailureS2Tprocess::THROW_ERROR>; // %F -> "%Y-%m-%d"
  };
  // ]]============]] type - Default Conversion format



  // [[============[[ ISS -> istringstream :: concept
  template <typename, typename = void>
  struct is_formatYMDiss : std::false_type {};

  template <typename FMT>
  struct is_formatYMDiss< FMT,
                          typename std::enable_if_t< std::is_same_v<typename FMT::stream_type, std::istringstream>
                                                   >
                        >
            : is_formatYMDss<FMT>
  {};
  template <typename FMT>
  concept c_formatYMDiss = is_formatYMDiss<FMT>::value;
  // ]]============]] ISS -> istringstream :: concept

  // ]=============================================================] S2T_FORMAT



  // [=============================================================[ ConvertFromStr
  /**
   * @brief     Specialized implementation handling string to 'year_month_day' conversion.
   * @tparam  S2T_FORMAT_YMD        Class which encapsulates conversion parameters/logic such as 'Locale' specific for 'year_month_day'.
   */
  template<c_formatYMDiss S2T_FORMAT_YMD>
  struct ConvertFromStr< std::chrono::year_month_day, S2T_FORMAT_YMD>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = std::chrono::year_month_day;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = typename S2T_FORMAT_YMD::return_type; //std::chrono::year_month_day;

    static const int template_uid = 10000;

    /**
     * @brief   Converts string holding 'year_month_day' value. The string has the format "%F" -> "%Y-%m-%d"
     * @param   str                 input string representing date.
     * @returns 'year_month_day'.
     */
    inline static return_type //std::chrono::year_month_day
    ToVal(const std::string& str)
    {
      try {
        return ToVal_args( str, S2T_FORMAT_YMD::ymdFormat); // %F -> "%Y-%m-%d"
      } catch (const std::invalid_argument& err) {
        return S2T_FORMAT_YMD::handler(str, err);
      } catch (const std::exception& err) {
        //static const std::string errMsg("std::chrono::year_month_day _ConvertFromStr<std::chrono::year_month_day, S2T_FORMAT_YMD>::ToVal(const std::string& str)");
        return S2T_FORMAT_YMD::handler(str, err);
      }
    }

    /**
     * @brief   Converts string holding 'year_month_day' value. The string has the format "%F" -> "%Y-%m-%d"
     * @param   str                 input string representing date.
     * @param   fmt                 a format string of date-string.
     * @returns 'year_month_day'.
     */
    inline static return_type
    ToVal_args(  const std::string& str,
                 const std::string::value_type* fmt)
    {
      datelibFrom::year_month_day ymd;
      std::istringstream iss(str);

#if    USE_CHRONO_FROMSTREAM_1 == 1
      CONVERTER_DEBUG_LOG("ConvertFromStr< std::chrono::year_month_day, S2T_FORMAT_YMD>ToVal_args()->  calling std::chrono::parse()");
      // Parse string into chrono::year_month_day object (C++20)
      iss >> std::chrono::parse(fmt, ymd);
#else
      CONVERTER_DEBUG_LOG("ConvertFromStr< std::chrono::year_month_day, S2T_FORMAT_YMD>ToVal_args()->  calling date::from_stream()");
      date::from_stream(iss, fmt, ymd);
#endif
      CONVERTER_DEBUG_LOG("after call. checking stream status....");
      try { CONVERTER_DEBUG_LOG(" iss.fail()=" << iss.fail()); } catch (...) { std::cout << "call-failed  iss.fail()" << std::endl; }
      try { CONVERTER_DEBUG_LOG(" iss.bad()="  << iss.bad());  } catch (...) { std::cout << "call-failed  iss.fail()" << std::endl; }
      try { CONVERTER_DEBUG_LOG(" iss.eof()="  << iss.eof());  } catch (...) { std::cout << "call-failed  iss.fail()" << std::endl; }

      // Validate stringstream is parsed as expected
      if (iss.fail() || iss.bad())
      {
        std::ostringstream ess{};
        ess << ((USE_CHRONO_FROMSTREAM_1 == e_ENABLE_FEATURE) ? "std::chrono" : "(lib->)date" );
        ess << " ::: strYMD='" << str << "' , format='" << fmt << "' stream-parse failed.";
        ess << " iss.fail()=" << iss.fail() << "  iss.bad()=" << iss.bad() << std::endl;

        std::invalid_argument err{ess.str()};

        return S2T_FORMAT_YMD::handler(str, err);
      }

      CONVERTER_DEBUG_LOG("after stream status. checking ymd status....");

      // Validate structural boundaries for final object safety
      if (!ymd.ok())
      {
        std::ostringstream ess{};
        ess << ((USE_CHRONO_FROMSTREAM_1 == e_ENABLE_FEATURE) ? "std::chrono" : "(lib->)date" );
        ess << " ::: strYMD='" << str << "' , format='" << fmt << "' invalid-date, conversion failed." << std::endl;

        std::invalid_argument err{ess.str()};

        return S2T_FORMAT_YMD::handler(str, err);
      }

      CONVERTER_DEBUG_LOG("after ymd status. returning ymd....");


#if    USE_CHRONO_FROMSTREAM_1 == 1
      return ymd;
#else
      std::chrono::year_month_day chrono_ymd {
          std::chrono::year{static_cast<int>(ymd.year())},
          std::chrono::month{static_cast<unsigned>(ymd.month())},
          std::chrono::day{static_cast<unsigned>(ymd.day())}
      };

      return chrono_ymd;
#endif
    }
  };

  // ]=============================================================] ConvertFromStr





  // [=============================================================[ T2S_FORMAT

  // [[============[[ Conversion formats
  template < specializedTypes::CompTimeStr formatYMD = defYMDfmt > // %F -> "%Y-%m-%d"
  using T2S_Format_StreamYMD = Format_StreamYMD<std::ostringstream, formatYMD>;
  // ]]============]] Conversion formats


  // [[============[[ type - Default Conversion format
  template<>
  struct T2S_DefaultFormat<std::chrono::year_month_day, void>
  {
    using type = T2S_Format_StreamYMD< defYMDfmt >; // %F -> "%Y-%m-%d"
  };
  // ]]============]] type - Default Conversion format



  // [[============[[ OSS -> ostringstream :: concept
  template <typename, typename = void>
  struct is_formatYMDoss : std::false_type {};

  template <typename FMT>
  struct is_formatYMDoss< FMT,
                          typename std::enable_if_t< std::is_same_v< typename FMT::stream_type, std::ostringstream>
                                                                   >
                        >
            : is_formatYMDss<FMT>
  {};
  template <typename FMT>
  concept c_formatYMDoss = is_formatYMDoss<FMT>::value;
  // ]]============]] OSS -> ostringstream :: concept

  // ]=============================================================] T2S_FORMAT





  // [=============================================================[ ConvertFromVal
  /**
   * @brief     Specialized implementation handling 'year_month_day' to string conversion.
   * @tparam  T2S_FORMAT_YMD        Class which encapsulates conversion parameters/directives such as 'Locale' specific for 'year_month_day'.
   */
  template<c_formatYMDoss T2S_FORMAT_YMD>
  struct ConvertFromVal< std::chrono::year_month_day, T2S_FORMAT_YMD>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = std::chrono::year_month_day;
    /**
     * @brief   'type' definition expected by the convertor.
     */
    using input_type = std::chrono::year_month_day;

    static const int template_uid = -10000;

    /**
     * @brief   Converts variable holding 'year_month_day' value to string. The string has the format "%F" -> "%Y-%m-%d"
     * @param   val                 input 'year_month_day'.
     * @returns string.
     */
    inline static std::string
    ToStr(  const std::chrono::year_month_day& val)
    {
      return ToStr_args( val, T2S_FORMAT_YMD::ymdFormat); // %F -> "%Y-%m-%d"
    }

    /**
     * @brief   Converts variable holding 'year_month_day' value to string. The string has the format "%F" -> "%Y-%m-%d"
     * @param   val                 input 'year_month_day'.
     * @param   fmt                 a format string specifying the output string format.
     * @returns string.
     */
    inline static std::string
    ToStr_args(  const std::chrono::year_month_day& val,
                 const std::string::value_type* fmt )
    {
      std::ostringstream oss;

#if    USE_CHRONO_TOSTREAM_1 == 1
      //std::chrono::to_stream(oss, fmt, pYmd);

      std::string fmtV("{:");
      fmtV += fmt;
      fmtV += "}";
      oss << std::vformat(oss.getloc(), fmtV, std::make_format_args(val));
#else // if  USE_DATELIB_TOSTREAM_2 == 1
      date::year_month_day date_val {
          date::year{static_cast<int>(val.year())},
          date::month{static_cast<unsigned>(val.month())},
          date::day{static_cast<unsigned>(val.day())}
      };
      oss << date::format(fmt, date_val);
#endif

      return oss.str();
    }
  };
  // ]=============================================================] ConvertFromVal


  // [=============================================================[ Helpers
  inline constexpr char dbY_fmt[] = "%d-%b-%Y";  // string literal object with static storage duration; inline implies external linkage

  using ConvertFromDbY_toStr = ConvertFromVal< std::chrono::year_month_day,
                                               T2S_Format_StreamYMD< dbY_fmt >
                                             >;

  constexpr std::string (*toStr_dbY)(const std::chrono::year_month_day& val) = &ConvertFromDbY_toStr::ToStr;


  using ConvertFromStr_toDbY = ConvertFromStr< std::chrono::year_month_day,
                                               S2T_Format_StreamYMD< dbY_fmt >
                                             >;

  constexpr std::chrono::year_month_day (*fromStr_dbY)(const std::string& str) = &ConvertFromStr_toDbY::ToVal;



  inline constexpr char dmY_fmt[] = "%d-%m-%Y";  // string literal object with static storage duration; inline implies external linkage

  using ConvertFromDmY_toStr = ConvertFromVal< std::chrono::year_month_day,
                                               T2S_Format_StreamYMD< dmY_fmt >
                                             >;

  constexpr std::string (*toStr_dmY)(const std::chrono::year_month_day& val) = &ConvertFromDmY_toStr::ToStr;


  using ConvertFromStr_toDmY = ConvertFromStr< std::chrono::year_month_day,
                                               S2T_Format_StreamYMD< dmY_fmt >
                                             >;

  constexpr std::chrono::year_month_day (*fromStr_dmY)(const std::string& str) = &ConvertFromStr_toDmY::ToVal;
  // ]=============================================================] Helpers
}
