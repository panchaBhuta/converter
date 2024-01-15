/*
 * date.h
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
#include <string>

#include <converter/converter.h>


namespace converter
{
  template <typename T>
  concept c_is_base_of_ymd_inclusive = std::is_base_of_v<std::chrono::year_month_day, T>;

  template <typename T>
  concept c_is_base_of_ymd_exclusive = std::is_base_of_v<std::chrono::year_month_day, T> &&
                                       (!std::is_same_v <std::chrono::year_month_day, T>);

  // [=====================================[    format_year_month_day
  /**
   * @brief     Template Class representing a CSV document.
   */
  template < const char* dateFMT, // %F -> "%Y-%m-%d"
             FailureS2Tprocess PROCESS_ERR >
  struct format_year_month_day : public std::chrono::year_month_day
  {
    constexpr static const char* formatYMD = dateFMT;
    constexpr static const FailureS2Tprocess errProcessing = PROCESS_ERR ;

    /**
     * @brief   Constructor
     * @tparam  ...Args               Type list of constructor arguments.
     * @param   ...args               constructor arguments as required for underlying base class `std::chrono::year_month_day`.
     */
    template<typename... Args>
    format_year_month_day(Args&&... args) :
        std::chrono::year_month_day(std::forward<Args>(args)...) {}

          std::chrono::year_month_day& getYMD()       { return (*this); }
    const std::chrono::year_month_day& getYMD() const { return (*this); }

    std::string toStr() const;
    void fromStr(const std::string& dateStr);
  };
  // ]=====================================]    format_year_month_day

  // [=============================================================[ S2T_FORMAT
  // [[============[[ Conversion formats
  template < const char* formatYMD, // %F -> "%Y-%m-%d"
             FailureS2Tprocess PROCESS_ERR >
  struct S2T_Format_StreamFormatYMD : public Format_StreamYMD<std::istringstream, formatYMD>,
                                      public OnError<format_year_month_day<formatYMD,PROCESS_ERR>, PROCESS_ERR> {};
  // ]]============]] Conversion formats


  // [[============[[ type - Default Conversion format
  template<const char* formatYMD>
  struct S2T_DefaultFormat<format_year_month_day<formatYMD, FailureS2Tprocess::THROW_ERROR>, void>
  {
    using type = S2T_Format_StreamFormatYMD<formatYMD, FailureS2Tprocess::THROW_ERROR>;
  };
  // ]]============]] type - Default Conversion format
  // ]=============================================================] S2T_FORMAT

  // [=============================================================[ ConvertFromStr
  /**
   * @brief     Specialized implementation handling string to 'format_year_month_day' conversion.
   * @tparam  dateFMT        date-format to read/write from string.
   * @tparam  PROCESS_ERR    enum defining the error handling process during string-to-date conversion.
   */
  template< const char* dateFMT, // %F -> "%Y-%m-%d"
            FailureS2Tprocess PROCESS_ERR >
  struct ConvertFromStr< format_year_month_day      <dateFMT,PROCESS_ERR>,
                         S2T_Format_StreamFormatYMD <dateFMT,PROCESS_ERR> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = format_year_month_day<dateFMT,PROCESS_ERR>;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = typename S2T_Format_StreamFormatYMD <dateFMT,PROCESS_ERR>::return_type; //format_year_month_day<dateFMT,PROCESS_ERR>;

    static const int template_uid = 10001;

    inline static return_type //format_year_month_day<dateFMT,PROCESS_ERR>
    ToVal(const std::string& str)
    {
      return format_year_month_day<dateFMT,PROCESS_ERR>(
            ConvertFromStr< std::chrono::year_month_day,
                            S2T_Format_StreamYMD <dateFMT,PROCESS_ERR> >::ToVal(str) );
    }
  };
  // ]=============================================================] ConvertFromStr


  // [=============================================================[ T2S_FORMAT
  // [[============[[ Conversion formats
  template < const char* formatYMD > // %F -> "%Y-%m-%d"
  using T2S_Format_StreamFormatYMD = Format_StreamYMD<std::ostringstream, formatYMD>;
  // ]]============]] Conversion formats



  // [[============[[ type - Default Conversion format
  template<const char* dateFMT>
  struct T2S_DefaultFormat<format_year_month_day<dateFMT, FailureS2Tprocess::THROW_ERROR>, void>
  {
    using type = T2S_Format_StreamFormatYMD< dateFMT >;
  };
  // ]]============]] type - Default Conversion format
  // ]=============================================================] T2S_FORMAT

  // [=============================================================[ ConvertFromVal
  template< const char* dateFMT, // %F -> "%Y-%m-%d"
            FailureS2Tprocess PROCESS_ERR >
  struct ConvertFromVal< format_year_month_day<dateFMT,PROCESS_ERR>,
                         T2S_Format_StreamFormatYMD <dateFMT> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = format_year_month_day<dateFMT,PROCESS_ERR>;
    /**
     * @brief   'type' definition expected by the convertor.
     */
    using input_type = format_year_month_day<dateFMT,PROCESS_ERR>;

    static const int template_uid = -10001;

    /**
     * @brief   Converts variable holding 'year_month_day' value to string. The string has the format "%F" -> "%Y-%m-%d"
     * @param   val                 input 'year_month_day'.
     * @returns string.
     */
    inline static std::string
    ToStr(  const format_year_month_day<dateFMT,PROCESS_ERR>& val)
    {
      return ConvertFromVal<  std::chrono::year_month_day,
                              T2S_Format_StreamYMD< dateFMT > >::ToStr( val); // %F -> "%Y-%m-%d"
    }
  };
  // ]=============================================================] ConvertFromVal



  // [=====================================[    format_year_month_day
  template < const char* dateFMT, // %F -> "%Y-%m-%d"
             FailureS2Tprocess PROCESS_ERR >
  std::string format_year_month_day<dateFMT,PROCESS_ERR>::toStr() const
  {
    return
    ConvertFromVal< format_year_month_day<dateFMT,PROCESS_ERR>,
                    T2S_Format_StreamFormatYMD <dateFMT>
                  >::ToStr(*this);
  };


  template < const char* dateFMT, // %F -> "%Y-%m-%d"
             FailureS2Tprocess PROCESS_ERR >
  void format_year_month_day<dateFMT,PROCESS_ERR>::fromStr(const std::string& dateStr)
  {
    (*this) =
    ConvertFromStr< format_year_month_day<dateFMT,PROCESS_ERR>,
                    S2T_Format_StreamFormatYMD <dateFMT,PROCESS_ERR>
                  >::ToVal(dateStr);
  }

  template< class CharT, class Traits,
            const char* dateFMT, // %F -> "%Y-%m-%d"
            FailureS2Tprocess PROCESS_ERR >
  std::basic_ostream<CharT, Traits>&
    operator<<( std::basic_ostream<CharT, Traits>& os,
                const format_year_month_day<dateFMT,PROCESS_ERR>& fmtymd )
  {
    os << fmtymd.toStr().c_str();
    return os;
  }

  template< class CharT, class Traits,
            const char* dateFMT, // %F -> "%Y-%m-%d"
            FailureS2Tprocess PROCESS_ERR >
  std::basic_istream<CharT, Traits>&
    operator>>( std::basic_istream<CharT, Traits>& is,
                format_year_month_day<dateFMT,PROCESS_ERR>& fmtymd )
  {
    std::basic_string<CharT, Traits> str;
    is >> str;
    fmtymd.fromStr(str);
    return is;
  }

/*
 * use default, no need to explicitly provide

  template < const char* dateFMT, // %F -> "%Y-%m-%d"
             FailureS2Tprocess PROCESS_ERR >
  constexpr bool operator==( const format_year_month_day<dateFMT,PROCESS_ERR>& x,
                             const format_year_month_day<dateFMT,PROCESS_ERR>& y ) noexcept
  {
    std::chrono::year_month_day& dx = x;
    std::chrono::year_month_day& dy = y;

    return dx == dy;
  }

  template < const char* dateFMT, // %F -> "%Y-%m-%d"
             FailureS2Tprocess PROCESS_ERR >
  constexpr std::strong_ordering
    operator<=>( const format_year_month_day<dateFMT,PROCESS_ERR>& x,
                 const format_year_month_day<dateFMT,PROCESS_ERR>& y ) noexcept
  {
    std::chrono::year_month_day& dx = x;
    std::chrono::year_month_day& dy = y;

    return dx <=> dy;
  }
*/
  // ]=====================================]    format_year_month_day

}