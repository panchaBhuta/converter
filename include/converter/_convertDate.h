/*
 * _convertDate.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.2
 *
 * Copyright (c) 2023-2023 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#pragma once

#include <chrono>
//#include <string_view>
//#include <format>
#include <cstring>
#include <array>
#include <map>
#include <locale>
#include <algorithm>
#include <cctype>

#include <converter/_common.h>
#include <converter/_convertS2T.h>
#include <converter/_convertT2S.h>

#include <converter/_workaroundConfig.h>


#if  USE_DATE_FROMSTREAM_2 == _e_ENABLE_FEATURE_  ||  USE_DATE_TOSTREAM_2 == _e_ENABLE_FEATURE_
  #include <date/date.h>
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
    using return_type = typename S2T_FORMAT_YMD::return_type; //std::chrono::year_month_day;

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
                 [[maybe_unused]]std::string* abbrev  = nullptr,
                 [[maybe_unused]]std::chrono::minutes* offset = nullptr)
#if   USE_JUGAAD_FROMSTREAM_3  !=  _e_ENABLE_FEATURE_
    {
  #if   USE_CHRONO_FROMSTREAM_1  ==  _e_ENABLE_FEATURE_
      namespace _dateLib = std::chrono;
      const std::string dateClass = "std::chrono::year_month_day";
  #elif USE_DATE_FROMSTREAM_2  ==  _e_ENABLE_FEATURE_
      namespace _dateLib = date;
      const std::string dateClass = "date::year_month_day";
  #endif
      CONVERTER_DEBUG_LOG(dateClass << " for string2date input : str=" << str);

      _dateLib::year_month_day ymd;
      std::istringstream iss(str);
      S2T_FORMAT_YMD::streamUpdate(iss);
      _dateLib::from_stream(iss, fmt, ymd, abbrev, offset);

      if(
            ymd.ok() ||
            ( (!iss.fail()) && (!iss.bad()) )
      /*
       *  '29-02-1900' are invalid date even though year, month and day individually could be valid.
       *  In such a case we still create the invalid date from year, month, day; after a call from date::from_stream().
       *  A call to std::chrono::from_stream('29-02-1900') returns a 'date' value (i.e no error thrown),
       *  but ymd.ok() will be false.
       *  Therefore, in order to keep the behvaiour same, we a create std::chrono::year_month_day value,
       *  with invalid params and return that value.
       */
        )
      {
        CONVERTER_DEBUG_LOG(dateClass << " for string2date conversion-succeded : iY=" << (int(ymd.year())) \
                                      << ", iM=" << (unsigned(ymd.month())) << ", iD=" << (unsigned(ymd.day())));
  #if   USE_CHRONO_FROMSTREAM_1  ==  _e_ENABLE_FEATURE_
        return ymd;
  #elif USE_DATE_FROMSTREAM_2  ==  _e_ENABLE_FEATURE_
        return std::chrono::year_month_day{ std::chrono::year(int(ymd.year())),
                                            std::chrono::month(unsigned(ymd.month())),
                                            std::chrono::day(unsigned(ymd.day())) };
  #endif
      }

      /*
       *  '29-02-1900' [date::from_stream()] or '2023-01-32' [from both libs] are invalid dates and streamstring fails.
       */
      std::string fmtStr(fmt);
      if(std::strcmp(fmt, defYMDfmt)==0) {
        fmtStr = "%Y-%m-%d";
      } else if (std::strcmp(fmt, "%D")==0) {
        fmtStr = "%m/%d/%y";
      }

      const size_t lastDelimiterIdx = fmtStr.find_last_of("%");
      if(std::string::npos != lastDelimiterIdx && 0 != lastDelimiterIdx
         && ( !std::isalnum(fmtStr.at(lastDelimiterIdx-1), std::locale::classic())
            ) )
      {
        const char delimiter = fmtStr.at(lastDelimiterIdx-1);  // char before '%' i.e fmtStr = "%Y-%m-%d" it would be '-'
        std::istringstream ss(str);
        std::istringstream ssFmt(fmtStr);
        std::string token;
        std::string fmtToken;

        bool hasYear = false, hasMonth = false, hasDay = false;
        int i = 0;
        int iY=0;
        unsigned iM=0, iD=0;
        // refer ' Format string'  in https://en.cppreference.com/w/cpp/chrono/system_clock/from_stream
        while(std::getline(ss, token, delimiter) && std::getline(ssFmt, fmtToken, delimiter) && (i++) < 5)
        {
          if(token.empty()) continue;

          const char lastCharFmt = fmtToken.back();
          if(!hasYear && (lastCharFmt == 'Y' || lastCharFmt == 'y' || lastCharFmt == 'C') )
          {
            if(lastCharFmt == 'Y') {
              std::istringstream issI(token);
              issI >> iY;
            } else {
              std::istringstream issY(token);
              S2T_FORMAT_YMD::streamUpdate(issY);
              date::year year;
              date::from_stream(issY, fmtToken.c_str(), year);
              iY = int(year);
            }
            hasYear = true;
            continue;
          }
          if(!hasMonth && (lastCharFmt == 'm' || lastCharFmt == 'b' || lastCharFmt == 'B' || lastCharFmt == 'h') )
          {
            if(lastCharFmt == 'm') {
              std::istringstream issI(token);
              issI >> iM;
            } else {
              std::istringstream issM(token);
              S2T_FORMAT_YMD::streamUpdate(issM);
              date::month month;
              date::from_stream(issM, fmtToken.c_str(), month);
              iM = unsigned(month);
            }
            hasMonth = true;
            continue;
          }
          if(!hasDay && (lastCharFmt == 'd' || lastCharFmt == 'e') )
          {
            if(lastCharFmt == 'd') {
              std::istringstream issI(token);
              issI >> iD;
            } else {
              std::istringstream issD(token);
              S2T_FORMAT_YMD::streamUpdate(issD);
              date::day day;
              date::from_stream(issD, fmtToken.c_str(), day);
              iD = unsigned(day);
            }
            hasDay = true;
            continue;
          }
        }
        if(hasDay && hasMonth && hasYear )
        {
          CONVERTER_DEBUG_LOG("jugaad for string2date conversion-succeded : iY=" << iY << ", iM=" << iM << ", iD=" << iD);
          return std::chrono::year_month_day{ std::chrono::year(iY),
                                              std::chrono::month(iM),
                                              std::chrono::day(iD) };
        } else {
          CONVERTER_DEBUG_LOG("jugaad for string2date conversion-failed : iY=" << iY << ", iM=" << iM << ", iD=" << iD);
        }
      }

        static const std::string func("std::chrono::year_month_day converter::ConvertFromStr<std::chrono::year_month_day, S2T_FORMAT_YMD>::ToVal_args(const std::string& str)");
  #ifdef ENABLE_CONVERTER_DEBUG_LOG
        CONVERTER_DEBUG_LOG("ERROR : rapidcsv :: in function '" << func << "' ::: strYMD='" << str <<"' format='" << fmt << "'");
        CONVERTER_DEBUG_LOG("istringstream-conversion<" << dateClass << "> failed."    \
             << std::boolalpha << "   iss.fail() = " << iss.fail()                     \
                               << " : iss.bad() = " << iss.bad()                       \
                               << " : iss.eof() = " << iss.eof());
        CONVERTER_DEBUG_LOG(" dateComponents "                                                                   \
                               << "   ymd.ok() = " << ymd.ok()                                                   \
                               << " : ymd.year(" << int(ymd.year()) << ").ok() = " << ymd.year().ok()            \
                               << " : ymd.month(" << unsigned(ymd.month()) << ").ok() = " << ymd.month().ok()    \
                               << " : ymd.day(" << unsigned(ymd.day()) << ").ok() = " << ymd.day().ok());
  #endif
      //if (iss.fail() || iss.bad()) // || (!iss.eof()))
      //{
        static const std::string errMsg("Invalid date-string received in '"+func+"'");
        CONVERTER_DEBUG_LOG(errMsg << " : string-txt='" << str << "' : format='" << fmt << "'");
        static const std::invalid_argument err(errMsg);
        return S2T_FORMAT_YMD::handler(str, err);
      //}
    }
#else
    {
      const std::string dateClass = "workaround_datelib::from_stream";
      CONVERTER_DEBUG_LOG(dateClass << " for string2date input : str=" << str);

      static const std::map<std::string, unsigned> monthsIndex = {
        {"jan",  1}, {"january", 1},
        {"feb",  2}, {"february", 1},
        {"mar",  3}, {"march", 1},
        {"apr",  4}, {"april", 1},
        {"may",  5}, 
        {"jun",  6}, {"june", 1},
        {"jul",  7}, {"july", 1},
        {"aug",  8}, {"august", 1},
        {"sep",  9}, {"september", 1},
        {"oct", 10}, {"october", 1},
        {"nov", 11}, {"november", 1},
        {"dec", 12}, {"december", 1},
      };

      std::string fmtStr(fmt);
      if(std::strcmp(fmt, defYMDfmt)==0) {
        fmtStr = "%Y-%m-%d";
      } else if (std::strcmp(fmt, "%D")==0) {
        fmtStr = "%m/%d/%y";
      }

      const size_t lastDelimiterIdx = fmtStr.find_last_of("%");
      if(std::string::npos != lastDelimiterIdx && 0 != lastDelimiterIdx
         && ( !std::isalnum(fmtStr.at(lastDelimiterIdx-1), std::locale::classic())
            ) )
      {
        const char delimiter = fmtStr.at(lastDelimiterIdx-1);
        std::istringstream ss(str);
        std::istringstream ssFmt(fmtStr);
        std::string token;
        std::string fmtToken;

        bool hasYear = false, hasMonth = false, hasDay = false;
        int iY=0;
        unsigned iM=0, iD=0;
        // refer ' Format string'  in https://en.cppreference.com/w/cpp/chrono/system_clock/from_stream
        while(std::getline(ss, token, delimiter) && std::getline(ssFmt, fmtToken, delimiter))
        {
          if(token.empty()) continue;

          const char lastCharFmt = fmtToken.back();
          if(!hasYear && (lastCharFmt == 'Y')) // || lastCharFmt == 'y' || lastCharFmt == 'C') )
          {
            std::istringstream issI(token);
            issI >> iY;
            hasYear = true;
            continue;
          }
          if(!hasMonth && (lastCharFmt == 'm' || lastCharFmt == 'b' || lastCharFmt == 'B' || lastCharFmt == 'h') )
          {
            if(lastCharFmt == 'm') {
              std::istringstream issI(token);
              issI >> iM;
            } else {
              std::transform(token.begin(), token.end(), token.begin(),
                              [](unsigned char c){ return std::tolower(c); });
              if(auto search = monthsIndex.find(token); search != monthsIndex.end()) {
                iM = search->second;
              } else {
                continue;
              }
            }
            hasMonth = true;
            continue;
          }
          if(!hasDay && (lastCharFmt == 'd'))  // || lastCharFmt == 'e') )
          {
            std::istringstream issI(token);
            issI >> iD;
            hasDay = true;
            continue;
          }
        }
        if(hasDay && hasMonth && hasYear )
        {
          CONVERTER_DEBUG_LOG(dateClass << " for string2date conversion-succeded : iY=" << iY << ", iM=" << iM << ", iD=" << iD);
          return std::chrono::year_month_day{ std::chrono::year(iY),
                                              std::chrono::month(iM),
                                              std::chrono::day(iD) };
        }
      }
      static const std::string func("std::chrono::year_month_day converter::ConvertFromStr<std::chrono::year_month_day, S2T_FORMAT_YMD>::ToVal_args(const std::string& str)");
  #ifdef ENABLE_CONVERTER_DEBUG_LOG
      CONVERTER_DEBUG_LOG("ERROR : rapidcsv :: in function '" << func << "' ::: strYMD='" << str <<"' format='" << fmt << "'");
      CONVERTER_DEBUG_LOG("istringstream-conversion<" << dateClass << "> failed.");
  #endif
      static const std::string errMsg("Invalid date-string received in '"+func+"'");
      CONVERTER_DEBUG_LOG(errMsg << " : string-txt='" << str << "' : format='" << fmt << "'");
      static const std::invalid_argument err(errMsg);
      return S2T_FORMAT_YMD::handler(str, err);
    }
    /*
    WARNING : Due to limitations of 'std::chrono' and 'date' libs, internal implementation of string-to-Date
              conversions is provided. This conversion support a limited sub-set of date-formats.
    */
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
      } catch (const std::invalid_argument& err) {
        return S2T_FORMAT_YMD::handler(str, err);
      } catch (const std::exception& err) {
        //static const std::string errMsg("std::chrono::year_month_day _ConvertFromStr<std::chrono::year_month_day, S2T_FORMAT_YMD>::ToVal(const std::string& str)");
        return S2T_FORMAT_YMD::handler(str, err);
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
#if USE_DATE_TOSTREAM_2  ==  _e_ENABLE_FEATURE_ ||  USE_JUGAAD_TOSTREAM_3  ==  _e_ENABLE_FEATURE_
  private:
    inline static void
    _jugaad(  const std::chrono::year_month_day& val,
              const std::string::value_type* fmt,
              std::ostringstream& oss)
    {
      std::function<void(char)> write;
      write = [&oss,&write,&val] (char convSpecifier) -> void
      {
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
            } catch (const std::out_of_range&) {
              oss << '%' << convSpecifier;
            }
            break;
          case 'B' :
            try {
              oss << monthLong.at(unsigned(val.month()));
            } catch (const std::out_of_range&) {
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
    }
  #endif

  public:
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
                 [[maybe_unused]]std::string* abbrev  = nullptr,
                 [[maybe_unused]]std::chrono::seconds* offset_sec = nullptr)
    {
      std::ostringstream oss;
      T2S_FORMAT_YMD::streamUpdate(oss);
#if   USE_CHRONO_TOSTREAM_1  ==  _e_ENABLE_FEATURE_
      const std::string dateClass = "std::chrono::year_month_day";
      CONVERTER_DEBUG_LOG(dateClass << " for date2string input : YMD Y=" << (int(val.year())) \
                << "  M=" << (unsigned(val.month())) << "  D=" << (unsigned(val.day())) );

      // As of writing this code, no compiler supports chrono::to_stream() yet.
      // This code here is for future reference once compiler starts supporting.
      // refer https://omegaup.com/docs/cpp/en/cpp/chrono/local_t/to_stream.html
      // no OS in particular
      std::chrono::to_stream(oss, fmt, val, abbrev, offset_sec); // does not compile
#elif USE_DATE_TOSTREAM_2  ==  _e_ENABLE_FEATURE_
      const std::string dateClass = "date::year_month_day";
      CONVERTER_DEBUG_LOG(dateClass << " for date2string input : YMD Y=" << (int(val.year())) \
                << "  M=" << (unsigned(val.month())) << "  D=" << (unsigned(val.day())) );

      // gcc and clang does not support the required 'chrono::to_stream' functionality as of writing this code
      // UNIX-like , macOS
      date::year_month_day valDate{ date::year(int(val.year())),
                                    date::month(unsigned(val.month())),
                                    date::day(unsigned(val.day())) };
      using CS = std::chrono::seconds;
      date::fields<CS> fds{valDate};
      date::to_stream(oss, fmt, fds, abbrev, offset_sec);
      if (oss.fail() || oss.bad()) // || oss.eof())
      {
        std::ostringstream oss2;
        T2S_FORMAT_YMD::streamUpdate(oss2);

        _jugaad( val, fmt, oss2);
        if (!(oss2.fail() || oss2.bad())) // || oss.eof())
        {
          CONVERTER_DEBUG_LOG(dateClass << ":_jugaad for date2string output : str=" << oss2.str());
          return oss2.str();
        }
      }
#elif USE_JUGAAD_TOSTREAM_3  ==  _e_ENABLE_FEATURE_
      const std::string dateClass = "workaround_datelib::to_stream";
      CONVERTER_DEBUG_LOG(dateClass << " for date2string input : YMD Y=" << (int(val.year())) \
                << "  M=" << (unsigned(val.month())) << "  D=" << (unsigned(val.day())) );

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
      // The conversion-function "_jugaad()" handles a limited sub-set of date-conversion specifiers
      _jugaad( val, fmt, oss);
#endif

      if (oss.fail() || oss.bad()) // || oss.eof())
      {
        static const std::string func("std::string ConvertFromVal<std::chrono::year_month_date, T2S_FORMAT_YMD>::ToStr_args(const std::chrono::year_month_date& val)");
  #ifdef ENABLE_CONVERTER_DEBUG_LOG
        std::ostringstream eoss;
        eoss << "ERROR : rapidcsv :: in function '" << func << "' ::: ";
        try {
          eoss << "year{" << int(val.year()) << "}-month{" << unsigned(val.month()) << "}-day{" 
          << unsigned(val.day()) << "}' : val.ok()=" << val.ok() << " format='" << fmt << "'  resultString='"
          << oss.str() << "'" << std::endl;
        } catch (...) {} // do-nothing on error
        eoss << "ostringstream-conversion<" << dateClass << "> failed.";
        eoss << std::boolalpha << "   iss.fail() = " << oss.fail()
                               << " : iss.bad() = " << oss.bad()
                               << " : iss.eof() = " << oss.eof();
        CONVERTER_DEBUG_LOG(eoss.str());
  #endif
        static const std::string errMsg("error in date-conversion in '"+func+"'");
        throw std::invalid_argument(errMsg);
      }
      CONVERTER_DEBUG_LOG(dateClass << " for date2string output : str=" << oss.str());
      return oss.str();
    }
  };

  // ]=============================================================] ConvertFromVal
}
