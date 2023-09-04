/*
 * _convertT2C.h
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

#include <charconv>
#include <iterator>
#include <stdexcept>
#include <array>
#include <string>


#include <converter/_common.h>


namespace converter
{
  // [=============================================================[ T2S_FORMAT
  // [[============[[ Conversion formats
  struct T2S_Format_std_TtoC {};
  // ]]============]] Conversion formats


  // [[============[[ type - Default Conversion format
  template<c_integer_type T>
  struct T2S_DefaultFormat<T, void >
  {
    using type = T2S_Format_std_TtoC;
  };

  template<c_floating_point T>
  struct T2S_DefaultFormat<T, void >
  {
    using type = T2S_Format_std_TtoC;
  };
  // ]]============]] type - Default Conversion format
  // ]=============================================================] T2S_FORMAT




  // [=============================================================[ ConvertFromVal

  template<typename T>
  struct _ConvertFromVal
  {
  protected:
    inline static std::string
    _ToStr_args(const T& val, auto... format_args)
    {
      std::array<char, std::numeric_limits<T>::digits +5 > str;  // +5 just to be on the safe side :)

      if (
            auto [pos, ec] = std::to_chars(str.data(), str.data() + str.size(), val, format_args...);
            ec == std::errc()
         ) {
        return std::string(str.data(), pos);
      } else {
        //static const std::string errMsg("Number to String conversion failed. 'std::string _ConvertFromVal<T>::_ToStr_args(const T& val, auto... format_args)'");
        //std::cerr << errMsg << " :: T[" << val << "]" << std::endl;
        throw std::runtime_error( std::make_error_code(ec).message() );
      }
    }
  };

  // for types refer :: https://en.cppreference.com/w/cpp/language/type
  /**
   * @brief     Convertor class implementation from integer types TO string.
   * @tparam  T                     'type' converted from, to string data.
   */
  template<c_integer_type T>
  struct ConvertFromVal<T, T2S_Format_std_TtoC >
        : protected  _ConvertFromVal<T>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = T;
    /**
     * @brief   'type' definition expected by the convertor.
     */
    using input_type = T;

    static const int template_uid = -102;

    /**
     * @brief   Converts integer datatype to string.
     * @param   val                 numerical value.
     * @returns string holding a integer represenation.
     *          Else throws error on conversion failure.
     */
    inline static typename std::string
    ToStr( const T& val)
    {
      return ToStr_args(val);
    }

    inline static typename std::string
    ToStr_args( const T& val, int base = 10)
    {
      return _ConvertFromVal<T>::_ToStr_args(val, base);
    }
  };

  // for types refer :: https://en.cppreference.com/w/cpp/language/type
  /**
   * @brief     Convertor class implementation from integer types TO string.
   * @tparam  T                     'type' converted from, to string data.
   */
  template<c_floating_point T>
  struct ConvertFromVal<T, T2S_Format_std_TtoC >
        : public  _ConvertFromVal<T>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = T;
    /**
     * @brief   'type' definition expected by the convertor.
     */
    using input_type = T;

    static const int template_uid = -103;

    /**
     * @brief   Converts integer datatype to string.
     * @param   val                 numerical value.
     * @returns string holding a integer represenation.
     *          Else throws error on conversion failure.
     */
    inline static typename std::string
    ToStr( const T& val)
    {
      return _ConvertFromVal<T>::_ToStr_args(val);
    }

    inline static typename std::string
    ToStr_args( const T& val, std::chars_format fmt)
    {
      return _ConvertFromVal<T>::_ToStr_args(val, fmt);
    }

    inline static typename std::string
    ToStr_args( const T& val, std::chars_format fmt, int precision)
    {
      return _ConvertFromVal<T>::_ToStr_args(val, fmt, precision);
    }
  };

  // ]=============================================================] ConvertFromVal
}
