/*
 * _convertS2T.h
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
  // [=============================================================[ S2T_FORMAT
  template <typename T, typename T2,
            T (*StoT)( const std::string& str, std::size_t* pos, int base) >
  inline T str2T( const std::string& str, std::size_t* pos = nullptr, int base = 10 )
  {
    if constexpr( std::is_same_v<T, T2> ) {
      return StoT(str, pos, base);
    } else {
      return static_cast<T>(StoT(str, pos, base));
    }
  }

  struct S2T_Format_WorkAround {};
  struct S2T_Format_std_StoT {};
  
  template <c_char CH>
  using S2T_Format_StreamAsIs = Format_StreamAsIs<std::basic_istringstream<CH> >;
  
  template <c_char CH>
  using S2T_Format_StreamUseClassicLocale = Format_StreamUseClassicLocale<std::basic_istringstream<CH> >;
  
  template <c_char CH, const char* usrLoc>
  using S2T_Format_StreamUserLocale = Format_StreamUserLocale<std::basic_istringstream<CH>, usrLoc>;


  template <typename, typename = void>
  struct is_formatISS : std::false_type {};

  template <typename FMT>  // FMT -> expected defined types :: 'stream_type'
  struct is_formatISS<  FMT,
                        typename std::enable_if_t< std::is_same_v<typename FMT::stream_type,
                                                                  std::basic_istringstream<typename FMT::stream_type::char_type> >
                                                 >
                     >
            : is_formatSS<FMT>
  {};
  template <typename FMT>
  concept c_formatISS = is_formatISS<FMT>::value;




  template<typename T, typename = void >
  struct S2T_DefaultFormat
  {
    using type = S2T_Format_StreamUseClassicLocale<char>;
  };

  template<typename T>
  struct S2T_DefaultFormat< T,
                            typename  std::enable_if_t< is_integer_type<T>::value ||
                                                        std::is_floating_point_v<T>
                                                      >
                          > 
  {
    using type = S2T_Format_std_StoT;
  };

/*
  template<c_char CH >
  struct S2T_DefaultFormat<CH, void>
  {
    using type = S2T_Format_StreamUseClassicLocale<CH>;
  };
*/

  template<typename T>
  struct S2T_DefaultFormat< T,
                            typename  std::enable_if_t< std::is_same_v<T, std::string> ||
                                                        is_char<T>::value ||
                                                        std::is_same_v<T, bool>
                                                      >
                          >
  {
    using type = S2T_Format_WorkAround;
  };


  template <typename, typename = void>
  struct is_S2Tconverter : std::false_type {};

  // CFS -> Converter Function from String
  template <typename CFS>
  struct is_S2Tconverter< CFS, std::void_t< decltype(CFS::ToVal),      // check for the presence of 'static' member function CFS::ToVal
                                            typename CFS::value_type,  // check for the presence of type-def CFS::value_type
                                            typename CFS::return_type  // check for the presence of type-def CFS::return_type
                                          >
                        >
           : std::is_same<decltype(&CFS::ToVal), typename CFS::return_type(*)(const std::string&)>  // member is function
  {};

  template <typename CFS>
  concept c_S2Tconverter = is_S2Tconverter<CFS>::value;

  template <typename CFS>
  concept c_NOT_S2Tconverter = !is_S2Tconverter<CFS>::value;


  template< typename T,
            auto (*CONV_S2T)(const std::string&)
          >
  struct S2TwrapperFunction
  {
    using value_type  = T;
    using return_type = typename std::invoke_result_t< decltype(CONV_S2T),
                                                       const std::string& >;

    inline static return_type
    ToVal(const std::string& str)
    {
      return CONV_S2T(str);
    }
  };

  // ]=============================================================] S2T_FORMAT

/*
      template<c_char C>
      class _ConvStr
      {
        const std::basic_string<C> _str;
      public:
        _ConvStr(const std::string& str) : _str(str.begin(), str.end()) {}

        inline const std::basic_string<C>& getStr() { return _str; }
      };

      template<>
      class _ConvStr<char>
      {
        const std::string& _str;
      public:
        _ConvStr(const std::string& str) : _str(str) {}

        inline const std::string& getStr() { return _str; }
      };
*/

  // [=============================================================[ ConvertFromStr

  // https://stackoverflow.com/questions/68058640/using-concepts-to-select-class-template-specialization?rq=3
  /**
   * @brief     Class declaration of convertor FROM string to any type's.
   * @tparam  T                     'type' converted to, from string data.
   * @tparam  S2T_FORMAT            Class which encapsulates conversion parameters/directives such as using 'Locale'.
   *                                Optional argument, default's to 'S2T_DefaultFormat<T>::type'.
   */
  template<typename T, typename S2T_FORMAT = typename S2T_DefaultFormat<T>::type >
  struct ConvertFromStr;

  // https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
  /**
   * @brief     Convertor class implementation for any (no-string)type's, FROM string; using 'std::istringstream'.
   * @tparam  T                     'type' converted to, from string data. (Not Applicable for 'string to string' conversion.)
   * @tparam  S2T_FORMAT            Class which encapsulates conversion parameters/directives such as using 'Locale'.
   */
  template< c_NOT_string T, c_formatISS S2T_FORMAT >
  struct ConvertFromStr<T, S2T_FORMAT>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = T;

    /**
     * @brief   Converts string holding a numerical value to numerical datatype representation.
     * @param   str                 input string.
     * @returns Numerical value if conversion succeeds.
     *          Else throws 'std::invalid_argument' on conversion failure.
     */
    inline static T
    ToVal(const std::string& str)
    {
      using S2T_FORMAT_STREAM = S2T_FORMAT;
      using stream_type = typename S2T_FORMAT::stream_type;

      T val;

      //_ConvStr<typename S2T_FORMAT::stream_type::char_type>  cStr(str);
      //stream_type iss( cStr.getStr() );
      stream_type iss( str );
      S2T_FORMAT_STREAM::streamUpdate(iss);
      iss >> val;
      if( iss.fail() || iss.bad()) // || (!iss.eof()) )
      {
        std::ostringstream eoss;
        eoss << __CONVERTER_PREFERRED_PATH__ << " : ERROR : rapidcsv :: in function 'T _ConvertFromStr<c_NOT_string T, c_formatISS S2T_FORMAT>::ToVal(const std::string& str)' ::: str='";
        eoss << str;
        try {
          eoss << "' -> val='" << val;
        } catch (...) {} // on error do-nothing.
        eoss << "'  istringstream-conversion failed...";
        eoss << std::boolalpha << "   iss.fail() = " << iss.fail()
                               << " : iss.bad() = " << iss.bad()
                               << " : iss.eof() = " << iss.eof() << std::endl;
        throw std::invalid_argument(eoss.str());
      }
      return val;
    }
  };
 
  /**
   * @brief     Convertor class implementation for integer types FROM string.
   * @tparam  T                     'integer-type' converted to, from string data.
   */
  template <c_integer_type T>
  struct ConvertFromStr<T, S2T_Format_std_StoT >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = T;

    // TODO unit tests
    /**
     * @brief   Converts string holding a integer represenation to integer datatype.
     * @param   str                 input string.
     * @param   pos                 address of an integer to store the number of characters processed.
     * @param   base                the number base.
     * @returns Numerical value if conversion succeeds.
     *          Else throws error on conversion failure.
     */
    inline static T
    ToVal_args(const std::string& str, std::size_t* pos = nullptr, int base = 10)
    {
      //std::cout << "_ConvertFromStr<T,S2T_Format_std_StoT>::ToVal(str,...)" << std::endl;
      /*
       * Size of int and short are implementation defined.
       * int and short are 2 bytes on 16 compilers.
       * One 32 bit compilers, int is 4 bytes, while short remains 16 bits. 
       *
       * check for byte sizes of each integral type here ...
       * https://cplusplus.com/forum/general/140825/
       * https://en.cppreference.com/w/cpp/language/types
       */
      if constexpr(std::is_signed_v<T>)
      {
        if constexpr( std::is_same_v<T,long long> ) {
          return str2T<T, long long, &std::stoll>(str, pos, base);
        } else
        if constexpr( std::is_same_v<T,long> ) {
          return str2T<T, long, &std::stol>(str, pos, base);
        } else
        if constexpr( std::is_same_v<T,int> ) {
          return str2T<T, int, &std::stoi>(str, pos, base);
        } else
        if constexpr( std::is_same_v<T,short> ) {
          // as std::stos(...) is not part of the standard. Hence just static_cast from s -> i
          return static_cast<short>(std::stoi(str, pos, base));
        } else {
          //static_assert(false, "COMPILER-ERROR : rapidcsv :: in function 'T ConvertFromStr<T,S2T_Format_std_StoT>::ToVal(const std::string& str)' ::: rule-violation # ( sizeof(T)==sizeof(long long) || sizeof(T)==sizeof(long) || sizeof(T)==sizeof(int) )");
        }
      } else
      if constexpr(std::is_unsigned_v<T>) {
        if constexpr( std::is_same_v<T,unsigned long long> ) {
          return str2T<T, unsigned long long, &std::stoull>(str, pos, base);
        } else
        if constexpr( std::is_same_v<T,unsigned long> ) {
          return str2T<T, unsigned long, &std::stoul>(str, pos, base);
        } else
        if constexpr( std::is_same_v<T,unsigned> || std::is_same_v<T,unsigned short> ) {
          // as std::stou(...) is not part of the standard. Hence just static_cast from ul -> u
          // as std::stous(...) is not part of the standard. Hence just static_cast from us -> u
          return static_cast<T>(std::stoul(str, pos, base));
        } else {
          //static_assert(false, "COMPILER-ERROR : rapidcsv :: in function 'T ConvertFromStr<T,S2T_Format_std_StoT>::ToVal(const std::string& str)' ::: rule-violation # ( sizeof(T)==sizeof(unsigned long long) || sizeof(T)==sizeof(unsigned long) || sizeof(T)==sizeof(unsigned int) )");
        }
      } else {
        //static_assert(false, "COMPILER-ERROR : rapidcsv :: in function 'T ConvertFromStr<T,S2T_Format_std_StoT>::ToVal(const std::string& str)' ::: nexpected error condition reached.");
      }
    }

    /**
     * @brief   Converts string holding a integer represenation to integer datatype.
     * @param   str                 input string.
     * @returns Numerical value if conversion succeeds.
     *          Else throws error on conversion failure.
     */
    inline static T
    ToVal(const std::string& str)
    {
      return ToVal_args(str);
    }
  };

  /**
   * @brief     Convertor class implementation for floating types from string.
   * @tparam  T                     'floating-type' converted to, from string data.
   */
  template<c_floating_point T>
  struct ConvertFromStr<T, S2T_Format_std_StoT >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = T;

    // TODO unit tests
    /**
     * @brief   Converts string holding a floating-number represenation to floating datatype.
     * @param   str                 input string.
     * @param   pos                 address of an integer to store the number of characters processed.
     * @returns Numerical value if conversion succeeds.
     *          Else throws error on conversion failure.
     */
    inline static T
    ToVal_args(const std::string& str, std::size_t* pos = nullptr)
    {
      if constexpr( std::is_same_v<T, float> ) {
        return std::stof(str, pos);
      } else
      if constexpr( std::is_same_v<T, double> ) {
        return std::stod(str, pos);
      } else
      if constexpr( std::is_same_v<T, long double> ) {
        return std::stold(str, pos);
      }
    }

    /**
     * @brief   Converts string holding a floating-number represenation to floating datatype.
     * @param   str                 input string.
     * @returns Numerical value if conversion succeeds.
     *          Else throws error on conversion failure.
     */
    inline static T
    ToVal(const std::string& str)
    {
      return ToVal_args(str);
    }
  };

  /**
   * @brief     Specialized implementation handling string to string conversion.
   */
  template<>
  struct ConvertFromStr<std::string, S2T_Format_WorkAround>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = std::string;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = std::string;

    /**
     * @brief   Converts string to string.
     * @param   str                 input string.
     * @returns string.
     */
    inline static std::string ToVal(const std::string& str) { return str; }
  };

  /**
   * @brief     Specialized implementation handling string to char conversion.
   * @tparam  T                     'char-type' converted to, from string data.
   */
  template<c_char T>
  struct ConvertFromStr< T, S2T_Format_WorkAround >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = T;

    // TODO unit tests
    /**
     * @brief   Converts string holding char value.
     * @param   str                 input string.
     * @returns char-type.
     */
    inline static T
    ToVal(const std::string& str)
    {
      if(str.length()>1)
      {
        std::ostringstream eoss;
        eoss << __CONVERTER_PREFERRED_PATH__ << " : ERROR : rapidcsv :: in function 'T _ConvertFromStr<c_char T,S2T_Format_WorkAround>::ToVal(const std::string& str)' ::: for T=char-type-(un)signed, str='";
        eoss << str << "' which violates expected rule # ( str.length()==1 )";
        throw std::invalid_argument(eoss.str());
      }
      if constexpr( std::is_same<T, std::string::value_type>::value ) {
        return str[0];
      } else {
        return static_cast<T>(str[0]);
      }
    }
  };

  /**
   * @brief     Specialized implementation handling string to bool conversion.
   */
  template<>
  struct ConvertFromStr<bool, S2T_Format_WorkAround>
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = bool;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = bool;

    // TODO unit tests
    /**
     * @brief   Converts string holding bool value.
     * @param   str                 input string.
     * @returns bool.
     */
    inline static bool
    ToVal(const std::string& str)
    {
      unsigned long val = std::stoul(str); //(str, pos, base);
      if(val > 1)
      {
        std::ostringstream eoss;
        eoss << __CONVERTER_PREFERRED_PATH__ << " : ERROR : rapidcsv :: in function 'T _ConvertFromStr<bool,S2T_Format_WorkAround>::ToVal(const std::string& str)' ::: str='";
        eoss << str << "' which violates expected rule # ( val==0 || val==1 )";
        throw std::invalid_argument(eoss.str());
      }
      return val;
    }
  };

  /**
   * @brief   If a 'type-C' satisfies concept 'c_S2Tconverter', then use that 'type-C';   else
   *          assume it's a 'type-T' and bumped up using 'ConvertFromStr< type-T >' to create
   *          a class staisfying concept 'c_S2Tconverter'.
   *          This mechanism enables 'template-converter-algorithm' to handle both 'type-C' and
   *          'type-T' using the same code base, (i.e. reduces code duplicity of Getters and Setters functions).
   * @tparam  T_C                   T can be data-type such as int, double etc; xOR
   *                                C -> Conversion class statisfying concept 'c_S2Tconverter'.
   */
  template< typename T_C >
  struct t_S2Tconv;

  template< c_NOT_S2Tconverter T >
  struct t_S2Tconv<T> {
    using conv_type = ConvertFromStr< T >;
  };

  template< c_S2Tconverter C >
  struct t_S2Tconv<C> {
    using conv_type = C;
  };

  template< typename T_C >
  using t_S2Tconv_c = typename t_S2Tconv<T_C>::conv_type;

  /**
   * @brief   convert a function with signature 'auto (*CONV_S2T)(const std::string&)'
   *          to a converter type that satisfies concept 'c_S2Tconverter'.
   * @tparam  CONV_S2T                   a function with signature 'T_V (*CONV_S2T)(const std::string&)'.
   *                                     T_V represents either numeric type 'T' or 'variant std::variant<T, std::string>'
   */
  template< auto (*CONV_S2T)(const std::string&) >
  struct f_S2Tconv
  {
  private:
    /**
     * @brief return type of function CONV_S2T
     */
    using _return_type = typename std::invoke_result_t< decltype(CONV_S2T),
                                                        const std::string& >;
    /**
     * @brief infers the underlying data-type if _return_type is an variant instance.
     *        or it's same as return type of CONV_S2T.
     */
    using _value_type  = typename InferValueType<_return_type>::value_type;
  public:
    /**
     * @brief a converter alias that satisfies concept 'c_S2Tconverter'
     */
    using conv_type = S2TwrapperFunction< _value_type, CONV_S2T >;
  };

  template< auto (*CONV_S2T)(const std::string&) >
  using f_S2Tconv_c = typename f_S2Tconv<CONV_S2T>::conv_type;

  // ]=============================================================] ConvertFromStr
}
