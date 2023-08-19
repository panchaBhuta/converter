/*
 * _convertDate.h
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

#include <chrono>

#include <converter/_common.h>
#include <converter/_convertS2T.h>
#include <converter/_convertT2S.h>

#include <converter/_dateConfig.h>


#if USE_DATE_FROMSTREAM  ||  USE_DATE_TOSTREAM
  #include <date/date.h>
#endif

#if  USE_CHRONO_TOSTREAM 
  //#include <string_view>
  //#include <format>
  #include <cstring>
#endif

#if   USE_CHRONO_TOSTREAM  &&  USE_DATE_TOSTREAM
  #include <array>
#endif

namespace converter
{
  // [=============================================================[ COMMON_FORMAT
  constexpr char defYMDfmt[] = "%F";  // string literal object with static storage duration
  template<c_iostream IOSS, const char* _ymdFormat = defYMDfmt> // %F -> "%Y-%m-%d"
  struct Format_StreamYMD
  {
    using stream_type = IOSS;
    constexpr static const char* ymdFormat = _ymdFormat;

    constexpr static inline
    std::enable_if_t< std::is_same_v<typename IOSS::char_type,char>, void>
    streamUpdate([[maybe_unused]] IOSS& ss) {}
  };
  // ]=============================================================] COMMON_FORMAT


  // [=============================================================[ S2T_FORMAT
  // [[============[[ Conversion formats
  template < const char* formatYMD = defYMDfmt, // %F -> "%Y-%m-%d"
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
    using return_type = S2T_FORMAT_YMD::return_type; //std::chrono::year_month_day;

    static const int template_uid = 10000;

    // TODO unit tests
    /**
     * @brief   Converts string holding 'year_month_day' value. The string has the format "%F" -> "%Y-%m-%d"
     * @param   str                 input string.
     * @param   fmt                 a format string
     * @param   abbrev              if not null, pointer to an object that will hold the time zone abbreviation or name corresponding to the %Z specifier
     * @param   offset              if not null, pointer to an object that will hold the offset from UTC corresponding to the %z specifier 
     * @returns 'year_month_day'.
     */
    inline static return_type
    ToVal_args(  const std::string& str,
                 const std::string::value_type* fmt,
                 std::string* abbrev  = nullptr,
                 std::chrono::minutes* offset = nullptr)
#if   USE_CHRONO_FROMSTREAM  ||  USE_DATE_FROMSTREAM
    {
  #if   USE_CHRONO_FROMSTREAM
      namespace _dateLib = std::chrono;
      const std::string dateClass = "std::chrono::year_month_day";
  #elif USE_DATE_FROMSTREAM
      namespace _dateLib = date;
      const std::string dateClass = "date::year_month_day";
  #endif

      _dateLib::year_month_day ymd;
      std::istringstream iss(str);
      S2T_FORMAT_YMD::streamUpdate(iss);
      _dateLib::from_stream(iss, fmt, ymd, abbrev, offset);
      if (iss.fail() || iss.bad()) // || (!iss.eof()))
      {
        static const std::string errMsg("std::chrono::year_month_day _ConvertFromStr<std::chrono::year_month_day, S2T_FORMAT_YMD>::ToVal_args(const std::string& str)");
        return S2T_FORMAT_YMD::handler(str, errMsg);
      }

  #if   USE_CHRONO_FROMSTREAM
        return ymd;
  #elif USE_DATE_FROMSTREAM
        return std::chrono::year_month_day{ std::chrono::year(int(ymd.year())),
                                            std::chrono::month(unsigned(ymd.month())),
                                            std::chrono::day(unsigned(ymd.day())) };
  #endif
    }
#else
    { 
      // Due to limitations of underlying libs, no definition is provided here.
      // If needed, user might declare their own specific implementation of this method in their code base.
    }
#endif

    /**
     * @brief   Converts string holding 'year_month_day' value. The string has the format "%F" -> "%Y-%m-%d"
     * @param   str                 input string.
     * @returns 'year_month_day'.
     */
    inline static return_type //std::chrono::year_month_day
    ToVal(const std::string& str)
    {
      try {
       return ToVal_args( str, S2T_FORMAT_YMD::ymdFormat); // %F -> "%Y-%m-%d"
      } catch (const std::exception& err) {
        //static const std::string errMsg("std::chrono::year_month_day _ConvertFromStr<std::chrono::year_month_day, S2T_FORMAT_YMD>::ToVal(const std::string& str)");
        return S2T_FORMAT_YMD::handler(str, err.what());
      }
    }
  };

  // ]=============================================================] ConvertFromStr





  // [=============================================================[ T2S_FORMAT

  // [[============[[ Conversion formats
  template < const char* formatYMD = defYMDfmt > // %F -> "%Y-%m-%d"
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

    // TODO unit tests
    /**
     * @brief   Converts variable holding 'year_month_day' value to string. The string has the format "%F" -> "%Y-%m-%d"
     * @param   val                 input 'year_month_day'.
     * @param   fmt                 a format string
     * @param   abbrev              if not null, pointer to an object that will hold the time zone abbreviation or name corresponding to the %Z specifier
     * @param   offset_sec          if not null, pointer to an object that will hold the offset from UTC corresponding to the %z specifier 
     * @returns string.
     */
    inline static std::string
    ToStr_args(  const std::chrono::year_month_day& val,
                 const std::string::value_type* fmt,
                 std::string* abbrev  = nullptr,
                 std::chrono::seconds* offset_sec = nullptr)
    {
      std::ostringstream oss;
      T2S_FORMAT_YMD::streamUpdate(oss);

#if   USE_CHRONO_TOSTREAM
      const std::string dateClass = "std::chrono::year_month_day";

      // As of writing this code, no compiler supports chrono::to_stream() yet.
      // This code here is for future reference once compiler starts supporting.
      // refer https://omegaup.com/docs/cpp/en/cpp/chrono/local_t/to_stream.html
      // no OS in particular
      std::chrono::to_stream(oss, fmt, val, abbrev, offset_sec); // does not compile
#elif USE_DATE_TOSTREAM
      const std::string dateClass = "date::year_month_day";

      // gcc and clang does not support the required 'chrono::to_stream' functionality as of writing this code
      // UNIX-like , macOS
      date::year_month_day valDate{ date::year(int(val.year())),
                                    date::month(unsigned(val.month())),
                                    date::day(unsigned(val.day())) };
      using CS = std::chrono::seconds;
      date::fields<CS> fds{valDate};
      date::to_stream(oss, fmt, fds, abbrev, offset_sec);
#else

      // msvc supports only std::chrono::from_stream and not std::chrono::to_stream.
      // date-lib is not compatible with msvc (min/max macro clash), therefore
      // we are left with std::format() alternative
      //WINDOWS
      /*  this does not work
      std::string_view fmt_view{fmt};
      std::make_format_args val_fmt_arg{val};
      oss << std::vformat(fmt_view, val_fmt_arg);  // for now , no support for abbrev, offset_sec
      */
      // due to limitations of underlying libs, forced to brute force.
      // The conversion here handles a limited sub-set of conversion specifiers

      std::function<void(char)> write;
      write = [&oss,&write,&val] (char convSpecifier) -> void
      {
        const std::string dateClass = "internal_implementation";
        static const std::array<std::string, 13> monthShort = {
          "%", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
        };

        static const std::array<std::string, 13> monthLong = {
          "%", "Janary", "February", "March", "April", "May", "June",
               "July", "August", "September", "October", "November", "December"
        };

        switch(convSpecifier)
        {
          case 'Y' :
            oss << std::setfill('0') << std::setw(4) << int(val.year()); break;
          case 'm':
            oss << std::setfill('0') << std::setw(2) << unsigned(val.month()); break;
          case 'd':
            oss << std::setfill('0') << std::setw(2) << unsigned(val.day()); break;
          case 'F': //  "%Y-%m-%d"
            write('Y'); oss << '-'; write('m'); oss << '-'; write('d'); break;
          case 'D': //  "%m/%d/%y"
            write('m'); oss << '/'; write('d'); oss << '/'; write('y'); break;
          case 'y' :
            oss << std::setfill('0') << std::setw(2) << (int(val.year()) % 100); break;
          case 'b' :
            try {
              oss << monthShort.at(unsigned(val.month()));
            } catch (std::out_of_range) {
              oss << '%' << convSpecifier;
            }
            break;
          case 'B' :
            try {
              oss << monthLong.at(unsigned(val.month()));
            } catch (std::out_of_range) {
              oss << '%' << convSpecifier;
            }
            break;
          default:
            oss << '%' << convSpecifier;
        }
      };

      if (strlen(fmt)>1)
      {
        enum _ParseState { READ, REPLACE };
        _ParseState state = _ParseState::READ;
        char present = fmt[0];
        if( '%' == present )
          state = _ParseState::REPLACE;
        for(size_t iii = 1; iii < strlen(fmt); ++iii)
        {
          present = fmt[iii];
          if(_ParseState::REPLACE == state)
          {
            write(present);
            state = _ParseState::READ;
          } else {
            if( '%' == present )
            {
              state = _ParseState::REPLACE;
            } else {
              oss << present;
              //state = _ParseState::READ;
            }
          }
        }
      } else {
        if (strlen(fmt)==1)
          oss << fmt[1];
      }
#endif

      if (oss.fail() || oss.bad()) // || oss.eof())
      {
        std::ostringstream eoss;
        eoss << __CONVERTER_PREFERRED_PATH__ << " : ERROR : rapidcsv :: in function 'std::string _ConvertFromVal<std::chrono::year_month_date, T2S_FORMAT_YMD>::ToStr_args(const std::chrono::year_month_date& val)' ::: ";
        try {
          eoss << "year{" << int(val.year()) << "}-month{" << unsigned(val.month()) << "}-day{" << unsigned(val.day()) << "}' : val.ok()=" << val.ok() << " format='" << fmt << "'";
        } catch (...) {} // do-nothing on error
        eoss << " ostringstream-conversion<" << dateClass << "> failed.";
        eoss << std::boolalpha << "   iss.fail() = " << oss.fail()
                               << " : iss.bad() = " << oss.bad()
                               << " : iss.eof() = " << oss.eof() << std::endl;
        throw std::invalid_argument(eoss.str());
      }
      return oss.str();
    }
  };

  // ]=============================================================] ConvertFromVal
}
