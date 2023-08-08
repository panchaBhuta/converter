/*
 * _convertNaN.h
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

#include <converter/_common.h>
#include <converter/_convertS2T.h>
#include <converter/_convertT2S.h>


namespace converter
{
  // [=============================================================[ ConvertFromStr

  /**
   * @brief     Convertor class implementation for (floating) types which support default Not-A-Number (NaN) values.
   * @tparam  T                     'type' converted to, from string data. Floating types which supports signaling_NaN.
   * @tparam  S2T_FORMAT            Class which encapsulates conversion parameters/logic such as 'Locale'.
   */
  template<c_signaling_NaN T, typename S2T_FORMAT = typename S2T_DefaultFormat<T>::type >
  struct ConvertFromStr_fNaN
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = T;

    // this implimentation works only for floating types
    // refer  https://en.cppreference.com/w/cpp/types/numeric_limits/has_signaling_NaN
    /**
     * @brief   Converts string holding a numerical value to numerical data-type representation.
     *          It's a wrapper over 'ConvertFromStr<T, S2T_FORMAT>::ToVal(str)' ; returning a
     *          signaling_NaN() in case of conversion failure.
     * @param   str                 input string.
     * @returns Numerical value if conversion succeeds.
     *          Else returns std::numeric_limits<T>::signaling_NaN() on conversion failure.
     */
    inline static T ToVal(const std::string& str)
    {
      try
      {
        return ConvertFromStr<T, S2T_FORMAT>::ToVal(str);
      } catch (...) {
        return std::numeric_limits<T>::signaling_NaN();
      }
    }

    // TODO unit tests
    /**
     * @brief   Converts string holding a numerical value to numerical data-type representation.
     * @param   ...args             variadic arguments that needs to be forwarded to underlying
     *                              'ConvertFromStr<T, S2T_FORMAT>::ToVal_args(str, ...)'
     * @param   str                 input string.
     * @returns Numerical value if conversion succeeds.
     *          Else returns std::numeric_limits<T>::signaling_NaN() on conversion failure.
     */
    template<typename ... ARGS>
    inline static T ToVal_args(const std::string& str, ARGS&& ... args)
    {
      try
      {
        return ConvertFromStr<T, S2T_FORMAT>::ToVal_args(str, std::forward<ARGS>(args)...);
      } catch (...) {
        return std::numeric_limits<T>::signaling_NaN();
      }
    }
  };

  /**
   * @brief   Convertor class implementation for any numeric types, with indicators for Not-A-Number (NaN) and Null values.
   * @tparam  T                     'type' converted to, from string data.
   * @tparam  S2T_FORMAT            Class which encapsulates conversion parameters/logic such as 'Locale'.
   */
  template<c_NOT_string T, typename S2T_FORMAT = typename S2T_DefaultFormat<T>::type >
  struct ConvertFromStr_gNaN
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = std::variant<T, std::string>;

    /**
     * @brief   Converts string holding a numerical value to numerical datatype representation.
     * @param   str                 input string.
     * @returns A std::variant<T, std::string> , holding either numerical-value,
     *          or raw string on conversion failure.
     *          It's a Numerical value if conversion succeeds.
     *          Else on conversion failure, the underlying string value that caused failure.
     */
    inline static return_type
    ToVal(const std::string& str)
    {
      try
      {
        T val = ConvertFromStr<T, S2T_FORMAT>::ToVal(str);
        if constexpr (std::numeric_limits<T>::has_signaling_NaN)
        {
          if(std::isnan(val))
            return return_type{str};
        }
        return return_type{ val };
      } catch (...) {
        return return_type{str};
      }
    }

    // TODO unit tests
    /**
     * @brief   Converts string holding a numerical value to numerical datatype representation.
     * @param   str                 input string.
     * @param   ...args             variadic arguments that needs to be forwarded to underlying
     *                              'ConvertFromStr<T, S2T_FORMAT>::ToVal_args(str, ...)'
     * @returns A std::variant<T, std::string>.
     *          It's a Numerical value if conversion succeeds.
     *          Else on conversion failure, the underlying string value that caused failure.
     */
    template<typename ... ARGS>
    inline static return_type
    ToVal_args(const std::string& str, ARGS&& ... args)
    {
      try
      {
        T val = ConvertFromStr<T, S2T_FORMAT>::ToVal_args(str, std::forward<ARGS>(args)...);
        if constexpr (std::numeric_limits<T>::has_signaling_NaN)
        {
          if(std::isnan(val))
            return return_type{str};
        }
        return return_type{ val };
      } catch (...) {
        return return_type{str};
      }
    }
  };

  // ]=============================================================] ConvertFromStr



  // [=============================================================[ ConvertFromVal

  /**
   * @brief   Convertor class implementation for any numeric types, with indicators for Not-A-Number (NaN) and Null values.
   * @tparam  T                     'type' converted from, to string.
   * @tparam  T2S_FORMAT            Class which encapsulates conversion parameters/directives (optional argument).
   *                                Specialization for custom datatype conversions, using this template parameter.
   */
  template<c_NOT_string T, typename T2S_FORMAT = typename T2S_DefaultFormat<T>::type >
  struct ConvertFromVal_gNaN
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = T;
    /**
     * @brief   'type' definition expected by the convertor. 'variant' can be either T or string(for invalid value)
     */
    using input_type = std::variant<T, std::string>;

    /**
     * @brief   Converts integer datatype to string.
     * @param   val                 A std::variant<T, std::string> , holding either
     *                              numerical-value or raw string.
     * @returns string holding a integer represenation or raw-string of the variant<T,string> type.
     *          Else throws error on conversion failure.
     */
    inline static std::string
    ToStr(const input_type& val)
    {
      if(val.index()==0)
      {
        return ConvertFromVal<T, T2S_FORMAT>::ToStr(std::get<T>(val));
      } else {
        return std::get<std::string>(val);
      }
    }
  };

  // ]=============================================================] ConvertFromVal

}
