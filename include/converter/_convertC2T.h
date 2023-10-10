/*
 * _convertC2T.h
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
#include <system_error>
#include <string>
	

#include <converter/_common.h>


namespace converter
{
  // [=============================================================[ S2T_FORMAT
  // [[============[[ Conversion formats
#if USE_FLOATINGPOINT_FROM_CHARS_1  ==  e_ENABLE_FEATURE
  template<typename T, FailureS2Tprocess PROCESS_ERR>
  struct S2T_Format_std_CtoT : public OnError<T, PROCESS_ERR> {};
#else
  template<c_NOT_floating_point T, FailureS2Tprocess PROCESS_ERR>
  struct S2T_Format_std_CtoT : public OnError<T, PROCESS_ERR> {}; // "for floating-point types, compiler does not support 'std::from_chars(...)' function"
#endif
  // ]]============]] Conversion formats


  // [[============[[ type - Default Conversion format
  template<c_integer_type T>
  struct S2T_DefaultFormat< T >
  {
    using type = S2T_Format_std_CtoT<T, FailureS2Tprocess::THROW_ERROR>;
  };

#if USE_FLOATINGPOINT_FROM_CHARS_1  ==  e_ENABLE_FEATURE
  template<c_floating_point T>
  struct S2T_DefaultFormat< T >
  {
    using type = S2T_Format_std_CtoT<T, FailureS2Tprocess::SIGNAL_NAN>;
  };
#endif
  // ]]============]] type - Default Conversion format
  // ]=============================================================] S2T_FORMAT


  // [=============================================================[ ConvertFromStr
 
  /*
   * This a helper internal class, not meant to be called by upstream users.
   */
  template <typename T, FailureS2Tprocess PROCESS_ERR>
  class pConvertFromStr_POS_Cargs
  {
    friend struct converter::ConvertFromStr< T, S2T_Format_std_CtoT<T, PROCESS_ERR> >;

    /*
     * Function wrapper to query if the complete input string was read during conversion.
     */
    inline static T
    _toVal_args(const std::string& str, std::size_t* pos, auto format_args)
    {
      CONVERTER_DEBUG_LOG("trace :: pConvertFromStr_POS_Cargs< T, PROCESS_ERR >::_toVal_args('" << str << "')");
      //  https://en.cppreference.com/w/cpp/utility/from_chars
      //  std::from_chars is locale-independent, non-allocating, and non-throwing
      T result{};

      // NOTE : str.c_str() instead of str.data() ; as we donot need null terminated char*
      auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result, format_args);

      if(pos != nullptr) {
        (*pos) =  static_cast<size_t>(std::distance(str.data(), ptr));
      }

      static const std::string funcName = 
        " : 'T pConvertFromStr_POS_Cargs<T>::_toVal_args(const std::string& str, std::size_t* pos, auto format_args)'";
      if (ec == std::errc()) {
        return result;
      } else if (ec == std::errc::invalid_argument) {
        static const std::string errMsg = std::make_error_code(std::errc::invalid_argument).message() + funcName;
        static const std::invalid_argument err(errMsg);
        //std::cerr << errMsg << " :: str[" << str << "]" << std::endl;
        CONVERTER_DEBUG_LOG( errMsg << "  str = '" << str << "'");
        throw err;
      } else if (ec == std::errc::result_out_of_range) {
        static const std::string errMsg = std::make_error_code(std::errc::result_out_of_range).message() + funcName;
        static const std::out_of_range err(errMsg);
        //std::cerr << errMsg << " :: str[" << str << "]" << std::endl;
        CONVERTER_DEBUG_LOG( errMsg << "  str = '" << str << "'");
        throw err;
      } else {
        static const std::string errMsg = std::make_error_code(ec).message() + funcName;
        static const std::runtime_error err(errMsg);
        //std::cerr << errMsg << " :: str[" << str << "]" << std::endl;
        CONVERTER_DEBUG_LOG( errMsg << "  str = '" << str << "'");
        throw err;
      }
    }
  };

  /**
   * @brief     Convertor class implementation for integer types FROM string.
   *            This conversion is achieved using 'std::from_chars'.
   * @tparam  T                     'integer-type' converted to, from string data.
   * @tparam  PROCESS_ERR            enum variable of type FailureS2Tprocess.
   */
  template <c_integer_type T, FailureS2Tprocess PROCESS_ERR>
  struct ConvertFromStr<T, S2T_Format_std_CtoT<T, PROCESS_ERR> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = typename S2T_Format_std_CtoT<T, PROCESS_ERR >::return_type;

    static const int template_uid = 102;

    // TODO unit tests
    /**
     * @brief   Converts string holding a integer represenation to integer datatype.
     * @param   str                 input string.
     * @param   pos                 address of an integer to store the number of characters processed.
     * @param   base                integer base to use: a value between 2 and 36 (inclusive). 
     * @returns Numerical value if conversion succeeds.
     *          Else throws error on conversion failure.
     */
    inline static T
    ToVal_args(const std::string& str, std::size_t* pos = nullptr, int base = 10)
    {
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_integer_type T, S2T_Format_std_CtoT<T, PROCESS_ERR> >::ToVal_args('" << str << "')");
      CONVERTER_DEBUG_TRY_START
        return pConvertFromStr_POS_Cargs<T, PROCESS_ERR>::_toVal_args(str, pos, base);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::invalid_argument)
      CONVERTER_DEBUG_TRY_CATCH(std::out_of_range)
      CONVERTER_DEBUG_TRY_CATCH(std::runtime_error)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }
  
    /**
     * @brief   Converts string holding a integer represenation to integer datatype.
     * @param   str                 input string.
     * @returns Numerical value if conversion succeeds.
     *          Else throws error on conversion failure.
     */
    inline static return_type
    ToVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_integer_type T, S2T_Format_std_CtoT<T, PROCESS_ERR> >::ToVal('" << str << "')");
      CONVERTER_DEBUG_TRY_START
        return pConvertFromStr_POS<T, PROCESS_ERR, S2T_Format_std_CtoT<T, PROCESS_ERR> >::_toVal(str);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::invalid_argument)
      CONVERTER_DEBUG_TRY_CATCH(std::out_of_range)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }
  };

#if USE_FLOATINGPOINT_FROM_CHARS_1  ==  e_ENABLE_FEATURE
  /**
   * @brief     Convertor class implementation for floating-point types FROM string.
   *            This conversion is achieved using 'std::from_chars'.
   * @tparam  T                     'floating-point-type' converted to, from string data.
   * @tparam  PROCESS_ERR           enum variable of type FailureS2Tprocess.
   */
  template <c_floating_point T, FailureS2Tprocess PROCESS_ERR>
  struct ConvertFromStr<T, S2T_Format_std_CtoT<T, PROCESS_ERR> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = typename S2T_Format_std_CtoT<T, PROCESS_ERR >::return_type;

    static const int template_uid = 103;

    /**
     * @brief   Converts string holding a floating-number represenation to floating datatype.
     * @param   str                 input string.
     * @param   pos                 address of an integer to store the number of characters processed.
     * @param   fmt                 floating-point formatting to use, a bitmask of type std::chars_format.
     * @returns Numerical value if conversion succeeds.
     *          Else throws error on conversion failure.
     */
    inline static T
    ToVal_args(const std::string& str, std::size_t* pos = nullptr,
               std::chars_format fmt = std::chars_format::general)
    {
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_floating_point T, S2T_Format_std_CtoT<T, PROCESS_ERR> >::ToVal_args('" << str << "')");
      CONVERTER_DEBUG_TRY_START
        return pConvertFromStr_POS_Cargs<T, PROCESS_ERR>::_toVal_args(str, pos, fmt);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::invalid_argument)
      CONVERTER_DEBUG_TRY_CATCH(std::out_of_range)
      CONVERTER_DEBUG_TRY_CATCH(std::runtime_error)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }
  
    /**
     * @brief   Converts string holding a floating-number represenation to floating datatype.
     * @param   str                 input string.
     * @returns Numerical value if conversion succeeds.
     *          Else throws error on conversion failure.
     */
    inline static return_type
    ToVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_floating_point T, S2T_Format_std_CtoT<T, PROCESS_ERR> >::ToVal('" << str << "')");
      CONVERTER_DEBUG_TRY_START
        return pConvertFromStr_POS<T, PROCESS_ERR, S2T_Format_std_CtoT<T, PROCESS_ERR> >::_toVal(str);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::invalid_argument)
      CONVERTER_DEBUG_TRY_CATCH(std::out_of_range)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
    }
  };
#endif
  // ]=============================================================] ConvertFromStr
}
