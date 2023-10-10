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
#include <limits>
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

#if USE_FLOATINGPOINT_TO_CHARS_1  ==  e_ENABLE_FEATURE
  template<c_floating_point T>
  struct T2S_DefaultFormat<T, void >
  {
    using type = T2S_Format_std_TtoC;
  };
#endif
  // ]]============]] type - Default Conversion format
  // ]=============================================================] T2S_FORMAT




  // [=============================================================[ ConvertFromVal

  /*
   * This a helper internal class, not meant to be called by upstream users.
  */
  template<typename T>
  class pConvertFromVal
  {
    friend struct converter::ConvertFromVal<T, T2S_Format_std_TtoC >;

    /*
     * Function wrapper to query if the conversion was succes after calling 'std::to_chars'.
     */
    inline static std::string
    _toStr_args(const T& val, auto... format_args)
    {
      std::array<char, std::numeric_limits<T>::digits +5 > str;  // +5 just to be on the safe side :)

      if (
            auto [pos, ec] = std::to_chars(str.data(), str.data() + str.size(), val, format_args...);
            ec == std::errc()
         ) {
        return std::string(str.data(), pos);
      } else {
        static const std::string funcName(" : 'std::string pConvertFromVal<T>::_toStr_args(const T& val, auto... format_args)'");
        static const std::string errMsg = std::make_error_code(ec).message() + funcName;
        static const std::runtime_error err(errMsg);
        //std::cerr << errMsg << " :: T[" << val << "]" << std::endl;
        CONVERTER_DEBUG_LOG( errMsg ); //<< "  val = '" << val << "'");
        throw err;
      }
    }
  };

  // for types refer :: https://en.cppreference.com/w/cpp/language/type
  /**
   * @brief     Convertor class implementation from integer types TO string.
   *            This conversion is achieved using 'std::to_chars'.
   * @tparam  T                     'type' converted from, to string data.
   */
  template<c_integer_type T>
  struct ConvertFromVal<T, T2S_Format_std_TtoC >
  {
    /**
     * @brief   'integr type' definition being declared for.
     */
    using value_type = T;
    /**
     * @brief   'integer type' definition expected by the convertor.
     */
    using input_type = T;

    static const int template_uid = -102;

    /**
     * @brief   Converts integer datatype to string.
     * @param   val                 numerical value.
     * @returns string holding a integer represenation.
     *          Else throws error on conversion failure.
     */
    inline static std::string
    ToStr( const T& val)
    {
      CONVERTER_DEBUG_TRY_START
        return ToStr_args(val);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::runtime_error)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }

    /**
     * @brief   Converts integer datatype to string.
     * @param   val                 numerical value.
     * @param   base                integer base to use: a value between 2 and 36 (inclusive). 
     * @returns string holding a integer represenation.
     *          Else throws error on conversion failure.
     */
    inline static std::string
    ToStr_args( const T& val, int base = 10)
    {
      CONVERTER_DEBUG_TRY_START
        return pConvertFromVal<T>::_toStr_args(val, base);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::runtime_error)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }
  };

#if USE_FLOATINGPOINT_TO_CHARS_1  ==  e_ENABLE_FEATURE
  // for types refer :: https://en.cppreference.com/w/cpp/language/type
  /**
   * @brief     Convertor class implementation from floating-point types TO string.
   *            This conversion is achieved using 'std::to_chars'.
   * @tparam  T                     'floating-point type' converted from, to string data.
   */
  template<c_floating_point T>
  struct ConvertFromVal<T, T2S_Format_std_TtoC >
  {
    /**
     * @brief   'floating-point type' definition being declared for.
     */
    using value_type = T;
    /**
     * @brief   'floating-point type' definition expected by the convertor.
     */
    using input_type = T;

    static const int template_uid = -103;

    /**
     * @brief   Converts floating-point datatype to string.
     * @param   val                 numerical value.
     * @returns string holding a floating-point represenation.
     *          Else throws error on conversion failure.
     */
    inline static std::string
    ToStr( const T& val)
    {
      CONVERTER_DEBUG_TRY_START
        return pConvertFromVal<T>::_toStr_args(val);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::runtime_error)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }

    /**
     * @brief   Converts floating-point datatype to string.
     * @param   val                 numerical value.
     * @param   fmt                 floating-point formatting to use, a bitmask of type 'std::chars_format'
     * @returns string holding a floating-point represenation.
     *          Else throws error on conversion failure.
     */
    inline static std::string
    ToStr_args( const T& val, std::chars_format fmt)
    {
      CONVERTER_DEBUG_TRY_START
        return pConvertFromVal<T>::_toStr_args(val, fmt);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::runtime_error)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }

    /**
     * @brief   Converts floating-point datatype to string.
     * @param   val                 numerical value.
     * @param   fmt                 floating-point formatting to use, a bitmask of type std::chars_format
     * @param   precision           floating-point precision to use
     * @returns string holding a floating-point represenation.
     *          Else throws error on conversion failure.
     */
    inline static std::string
    ToStr_args( const T& val, std::chars_format fmt, int precision)
    {
      CONVERTER_DEBUG_TRY_START
        return pConvertFromVal<T>::_toStr_args(val, fmt, precision);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::runtime_error)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }
  };
#endif

  // ]=============================================================] ConvertFromVal
}
