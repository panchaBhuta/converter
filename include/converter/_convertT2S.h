/*
 * _convertT2S.h
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


namespace converter
{
  // [=============================================================[ T2S_FORMAT

  // [[============[[ Conversion formats
  struct T2S_Format_WorkAround {};
  struct T2S_Format_std_TtoS {};


  template <c_char CH>
  using T2S_Format_StreamAsIs = Format_StreamAsIs< std::basic_ostringstream<CH> >;

  template <c_char CH>
  using T2S_Format_StreamUseClassicLocale = Format_StreamUseClassicLocale< std::basic_ostringstream<CH> >;

  template <c_char CH, const char* usrLoc>
  using T2S_Format_StreamUserLocale = Format_StreamUserLocale<std::basic_ostringstream<CH>, usrLoc>;


  template<c_floating_point T, int decimalPrecision = getDecimalPrecision<T>()>
  struct T2S_Format_StreamDecimalPrecision
  {
    using stream_type = std::ostringstream;

    static void streamUpdate(std::ostringstream& oss)
    {
      //oss << std::defaultfloat << std::setprecision(decimalPrecision);
      oss << std::setprecision(decimalPrecision);
      /*
       * NOTE : T2S_Format_StreamDecimalPrecision is not needed as
       *        it doesn't seem to have any effect as per code
       *        
       * long double pi2;
       * std::istringstream iss("3.141592653589793239");
       * iss >> std::setprecision(5) >> pi2;
       * std::cout << std::setprecision(19) << pi2 << '\n';
       * -------
       * 3.141592653589793239    :: even after setting 'iss >> std::setprecision(5)', the 'pi' output is precise upto 19 digits
       * -------
       *   1234567801234567890   :: precision digits
       * 
       * refer ->  https://en.cppreference.com/w/cpp/io/manip/setprecision
       * refer ->  https://en.cppreference.com/w/cpp/io/manip/fixed
       */
    }
  };
  template<c_floating_point T, int decimalPrecision>
  struct     T2S_Format_StreamDecimalPrecision< std::variant<T , std::string>, decimalPrecision >
    : public T2S_Format_StreamDecimalPrecision< T, decimalPrecision > {};
  // ]]============]] Conversion formats


  // [[============[[ type - Default Conversion format
  template<typename T, typename = void >
  struct T2S_DefaultFormat
  {
    using type = T2S_Format_StreamUseClassicLocale<char>;
  };

/*
  template<c_integer_type T>
  struct T2S_DefaultFormat<T, void >
  {
    using type = T2S_Format_std_TtoS;
  };
*/

#if USE_FLOATINGPOINT_TO_CHARS_1  ==  e_DISABLE_FEATURE
  template<c_floating_point T>
  struct T2S_DefaultFormat<T, void >
  {
    using type = T2S_Format_StreamDecimalPrecision<T>;
    //using type = T2S_Format_StreamDecimalPrecision<long double>;
  };
#endif

/*
  template<c_char CH>
  struct T2S_DefaultFormat<CH, void>
  {
    using type = T2S_Format_StreamUseClassicLocale<CH>;
  };
*/

  template<typename T>
  struct T2S_DefaultFormat< T,
                            typename std::enable_if_t< std::is_same_v<T, std::string> ||
                                                       is_char<T>::value ||
                                                       std::is_same_v<T, bool>
                                                     >
                          >
  {
    using type = T2S_Format_WorkAround;
  };
  // ]]============]] type - Default Conversion format



  // [[============[[ OSS -> ostringstream :: concept
  template <typename, typename = void>
  struct is_formatOSS : std::false_type {};

  template <typename FMT>  // FMT -> expected define types :: 'stream_type'
  struct is_formatOSS<  FMT,
                        typename std::enable_if_t< std::is_same_v< typename FMT::stream_type,
                                                                   std::basic_ostringstream<typename FMT::stream_type::char_type>
                                                                 >
                                                 >
                     >
            : is_formatSS<FMT>
  {};
  template <typename FMT>
  concept c_formatOSS = is_formatOSS<FMT>::value;
  // ]]============]] OSS -> ostringstream :: concept

  template<c_floating_point T,
           c_formatOSS FORMAT_1 = T2S_Format_StreamDecimalPrecision<T> ,
           c_formatOSS FORMAT_2 = T2S_Format_StreamUseClassicLocale<char> >
  using T2S_Format_floating_StreamCombine = Format_StreamCombineFormat<
                                                          std::ostringstream,
                                                          FORMAT_1, FORMAT_2>;



  // [[============[[ T2S-converter :: concept
  template <typename, typename = void>
  struct is_T2Sconverter : std::false_type {};

  // CFV -> Converter Function from Value
  template <typename CFV>
  struct is_T2Sconverter< CFV, std::void_t< decltype(CFV::ToStr),     // check for the presence of 'static' member function  CFV::ToStr
                                            typename CFV::value_type, // check for the presence of type-def CFV::value_type
                                            typename CFV::input_type  // check for the presence of type-def CFV::input_type
                                          >
                        >
           : std::is_same<decltype(&CFV::ToStr), std::string(*)(const typename CFV::input_type&)>  // member is function
  {};

  template <typename CFV>
  concept c_T2Sconverter = is_T2Sconverter<CFV>::value;

  template <typename CFV>
  concept c_NOT_T2Sconverter = !is_T2Sconverter<CFV>::value;
  // ]]============]] T2S-converter :: concept



  // ]=============================================================] T2S_FORMAT




  // [=============================================================[ ConvertFromVal

  // https://stackoverflow.com/questions/68058640/using-concepts-to-select-class-template-specialization?rq=3
  /**
   * @brief     Class declaration for any types, TO string.
   * @tparam  T                     'type' converted from, to string data.
   * @tparam  T2S_FORMAT            Class which encapsulates conversion parameters/directives such as using 'Locale'.
   *                                Optional argument, default's to 'T2S_DefaultFormat<T>::type'.
   */
  template<typename T, typename T2S_FORMAT = typename T2S_DefaultFormat<T>::type >
  struct ConvertFromVal
  {
    /*
     *  Normally, this template instance should not get initialized.
     *  In case it does that would mean template parameters passed
     *  are not as expected. The static function 'instanceEvaluater()'
     *  will help in figuring out the problem.
     */
    static float instanceEvaluater()
    {
      if constexpr(has_streamUpdate<T2S_FORMAT>::value)
      {
        static_assert(converter::is_iostream<typename T2S_FORMAT::stream_type>::value);
        static_assert(converter::is_formatSS<T2S_FORMAT>::value);
        //std::cout << "typename T2S_FORMAT::stream_type = "
        //          << typeid(typename T2S_FORMAT::stream_type).name() << std::endl;
        //std::cout << "std::basic_ostringstream<typename T2S_FORMAT::stream_type::char_type> = "
        //          << typeid(std::basic_ostringstream<typename T2S_FORMAT::stream_type::char_type>).name() << std::endl;

        static_assert(!std::is_same_v< typename T2S_FORMAT::stream_type,
                                      std::basic_istringstream<typename T2S_FORMAT::stream_type::char_type>
                                    >);  // ERROR :: invalid template param for FORMATTING -> 'ostringstream' should be passed instead of 'istringstream'
        static_assert(std::is_same_v< typename T2S_FORMAT::stream_type,
                                      std::basic_ostringstream<typename T2S_FORMAT::stream_type::char_type>
                                    >);
        static_assert(converter::is_formatOSS<T2S_FORMAT>::value);
        
        std::cout << "WARNING : Formater is ostringstream type" << std::endl;
      } else {
        std::cout << "WARNING : Formater is NOT ostringstream type" << std::endl;
      }
      return -0.1f;
    }

    static constexpr float template_uid = instanceEvaluater();
  };


  /**
   * @brief     Convertor class implementation for any types, TO string; using 'std::ostringstream'.
   * @tparam  T                     'type' converted from, to string data. (Not Applicable for string to string conversion)
   * @tparam  T2S_FORMAT_STREAM      Class which encapsulates conversion parameters/directives such as using 'Locale'.
   */
  template<c_NOT_string T, c_formatOSS T2S_FORMAT_STREAM >
  struct ConvertFromVal<T, T2S_FORMAT_STREAM>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = T;
    /**
     * @brief   'type' definition expected by the convertor.
     */
    using input_type = T;

    static const int template_uid = -1;

    /**
     * @brief   Converts numerical datatype from string holding a possibly-numerical value.
     * @param   val                 input possibly-numerical value.
     * @returns Output string if conversion succeeds. This type is any type that's supported by 'std::ostringstream'.
     *          Else throws 'std::invalid_argument' on conversion failure.
     */
    inline static std::string
    ToStr(const T& val)
    {
      using stream_type = typename T2S_FORMAT_STREAM::stream_type;

      stream_type oss;
      T2S_FORMAT_STREAM::streamUpdate(oss);
      oss << val;
      if (oss.fail() || oss.bad()) // || oss.eof())
      {
  #if ENABLE_CONVERTER_DEBUG_LOG == 1
        std::ostringstream eoss;
        try {
          eoss << "val='" << val << "'";
        } catch (...) {} // on error do-nothing.
        CONVERTER_DEBUG_LOG(_errMsg << " ::: ostringstream-conversion failed." << eoss.str()     \
                                    << std::boolalpha << "   iss.fail() = " << oss.fail()        \
                                    << " : iss.bad() = " << oss.bad() << " : iss.eof() = " << oss.eof());
  #endif
        throw _err;
      }
      return oss.str();
    }

  private:
    inline static const std::string _errMsg{"Invalid argument passed to function 'std::string ConvertFromVal<c_NOT_string T, c_formatOSS T2S_FORMAT>::ToStr(const T& val)'"};
    inline static const std::invalid_argument _err{_errMsg};
  };

  // for types refer :: https://en.cppreference.com/w/cpp/language/type
  /**
   * @brief     Convertor class implementation from integer types TO string.
   *            This conversion is achieved using 'std::to_string'.
   * @tparam  T                     'type' converted from, to string data.
   */
  template<c_integer_type T>
  struct ConvertFromVal<T, T2S_Format_std_TtoS >
  {
    /**
     * @brief   'integer type' definition being declared for.
     */
    using value_type = T;
    /**
     * @brief   'integer type' definition expected by the convertor.
     */
    using input_type = T;

    static const int template_uid = -2;

    /**
     * @brief   Converts integer datatype to string.
     * @param   val                 numerical value.
     * @returns string holding a integer represenation.
     *          Else throws error on conversion failure.
     */
    inline static std::string
    ToStr( const T& val)
    {
      // refer :: https://en.cppreference.com/w/cpp/string/basic_string/to_string
      return std::to_string(val);
    }
  };

  // DONOT USE
  template<c_floating_point T>
  struct ConvertFromVal<T, T2S_Format_std_TtoS >
  {
    /**
     * @brief   'floating-point type' definition being declared for.
     */
    using value_type = T;
    /**
     * @brief   'floating-point type' definition expected by the convertor.
     */
    using input_type = T;

    static const int template_uid = -3;

    inline static
#ifdef ENABLE_STD_TtoS
    std::string
#else
    // on purpose, rigged for compilation failure if used.
    // for explanation read warning note below
    std::enable_if_t<!std::is_floating_point_v<T>, std::string>
#endif
    ToStr(const T& val)
    {
      /*
         WARNING :: With floating point types std::to_string may yield unexpected results as the number
                    of significant digits in the returned string can be zero, for e.g: pVal = 1e-09

                    The return value may differ significantly from what std::cout prints by default, see the example.

          That's why this specialization is enabled with macro ENABLE_STD_TtoS

         std::cout: 23.43
         to_string: 23.430000

         std::cout: 1e-09
         to_string: 0.000000

         std::cout: 1e+40
         to_string: 10000000000000000303786028427003666890752.000000

         std::cout: 1e-40
         to_string: 0.000000

         std::cout: 1.23457e+08
         to_string: 123456789.000000
       */
      return std::to_string(val);
    }
  };

  /**
   * @brief     Specialized implementation handling string to string conversion.
   */
  template<>
  struct ConvertFromVal<std::string, T2S_Format_WorkAround>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = std::string;
    /**
     * @brief   'type' definition expected by the convertor.
     */
    using input_type = std::string;

    static const int template_uid = -4;

    /**
     * @brief   Converts string to string.
     * @param   val                 input string.
     * @returns string.
     */
    inline static std::string ToStr(const std::string& val) { return val; }
  };

  /**
   * @brief     Specialized implementation handling char to string conversion.
   * @tparam  CH                    'char-type' converted from, to string data.
   */
  template<c_char CH>
  struct ConvertFromVal<CH, T2S_Format_WorkAround>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = CH;
    /**
     * @brief   'type' definition expected by the convertor.
     */
    using input_type = CH;

    static const int template_uid = -5;

    // TODO unit tests
    /**
     * @brief   Converts char value to string.
     * @param   val                 input string.
     * @returns string.
     */
    inline static std::string
    ToStr(const CH& val)
    {
      // https://stackoverflow.com/questions/27720553/conversion-of-wchar-t-to-string
      std::basic_string<CH> ssVal(&val,1);
      return std::string(ssVal.begin(),ssVal.end());
    }
  };


  /**
   * @brief     Specialized implementation handling bool to string conversion.
   */
  template<>
  struct ConvertFromVal<bool, T2S_Format_WorkAround>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = bool;
    /**
     * @brief   'type' definition expected by the convertor.
     */
    using input_type = bool;

    static const int template_uid = -6;

    // TODO unit tests
    /**
     * @brief   Converts bool value to string.
     * @param   val                 input bool value.
     * @returns string. either "0" or "1"  (for : 'false' or 'true')
     */
    inline static std::string ToStr(const bool& val)
    {
      return std::to_string(static_cast<int>(val));
    }
  };


  /**
   * @brief     Convertor class implementation for any types, TO string.
   *            Data is 'variant<T, string>' represents either of type `T`
   *            or string if underlying data has error.
   * @tparam  T                     'type' converted from, to string data. (Not Applicable for string to string conversion)
   * @tparam  T2S_FORMAT            Class which encapsulates conversion parameters/directives such as using 'Locale'.
   */
  template< c_NOT_string T, typename T2S_FORMAT >
  struct ConvertFromVal<std::variant<T, std::string>, T2S_FORMAT>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type = T;
    /**
     * @brief   'type' definition expected by the convertor.
     *           Here it's a variant of T or std::string.
     */
    using input_type = std::variant<T, std::string>;

    static constexpr const char* const compositeTemplate_uid = "variant_userDefinedFormat";

    // TODO unit tests
    /**
     * @brief   Converts `variant<T, string>` value to string.
     *          If input variant has T then 'T->S' conversion is applied,
     *          else variant's underlying string value is returned.
     * @param   val                 input 'variant<T,string>' value.
     * @returns string.
     */
    inline static std::string ToStr(const input_type& val)
    {
      if(val.index()==0)
      {
        return ConvertFromVal<T,T2S_FORMAT>::ToStr(std::get<T>(val));
      } else {
        return std::get<std::string>(val);
      }
    }
  };
  // ]=============================================================] ConvertFromVal


  // [=============================================================[ T2S conversion-type definition
  template< typename T, typename TI,
            std::string (*CONV_T2S)(const TI&)
          >
  struct T2SwrapperFunction
  {
    using value_type = T;
    using input_type = TI;

    //inline static std::string
    inline static typename std::enable_if_t< (!std::is_same_v<std::string, TI>),std::string>
    ToStr(const input_type& val)
    {
      return CONV_T2S(val);
    }
  };

  // refer : https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2016/p0127r1.html
  /**
   * @brief   convert a function with signature 'std::string (*CONV_T2S)(const T_V&)'
   *          to a converter type that satisfies concept 'c_T2Sconverter'.
   * @tparam  CONV_T2S                   a function with signature 'std::string (*CONV_T2S)(const T_V&)'.
   *                                     T_V represents either numeric type 'T' or 'variant std::variant<T, std::string>'
   */
  template<auto CONV_T2S >
  struct f_T2Sconv;

  template< typename T_V, std::string (*CONV_T2S)(const T_V&) >
  struct f_T2Sconv<CONV_T2S>
  {
  private:
    /**
     * @brief input type of function CONV_T2S
     */
    using _input_type = T_V;
    /**
     * @brief infers the underlying data-type if _input_type is an variant instance.
     *        or it's same as input type of CONV_T2S.
     */
    using _value_type  = typename InferValueType<_input_type>::value_type;
  public:
    /**
     * @brief a converter alias that satisfies concept 'c_T2Sconverter'
     */
    using conv_type = T2SwrapperFunction< _value_type, _input_type, CONV_T2S >;
  };

  //template< typename T_V, std::string (*CONV_T2S)(const T_V&) >
  template<auto CONV_T2S >
  using f_T2Sconv_c = typename f_T2Sconv<CONV_T2S>::conv_type;


  /**
   * @brief   If a 'type-C' satisfies concept 'c_T2Sconverter', then use that 'type-C';   else
   *          assume it's a 'type-T' and bumped up using 'ConvertFromVal< type-T >' to create
   *          a class staisfying concept 'c_T2Sconverter'.
   *          This mechanism enables 'template-converter-algorithm' to handle both 'type-C' and
   *          'type-T' using the same code base, (i.e. reduces code duplicity).
   * @tparam  T_C                   T can be data-type such as int, double etc ;
   *                                xOR  C -> Conversion class statisfying concept 'c_T2Sconverter'.
   */
  template< typename T_C >
  struct t_T2Sconv;
  /*
  template< typename T_C, auto ... CONV_T2S >
  struct t_T2Sconv;

  template< auto CONV_T2S >
  struct t_T2Sconv< void, CONV_T2S > {
    using conv_type = f_T2Sconv_c<CONV_T2S>;
  };
  */

  template< c_NOT_T2Sconverter T >
  struct t_T2Sconv<T> {
    using conv_type = ConvertFromVal< T >;
  };

  template< c_T2Sconverter C >
  struct t_T2Sconv<C> {
    using conv_type = C;
  };



  template< typename T_C >
  using t_T2Sconv_c = typename t_T2Sconv< T_C >::conv_type;

  // ]=============================================================] T2S conversion-type definition
}
