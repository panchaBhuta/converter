/*
 * _convertS2T.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.1
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

  // [[============[[ Error Handling
  enum FailureS2Tprocess { THROW_ERROR = 0, SIGNAL_NAN = 1, QUIET_NAN = 2, VARIANT_NAN = 3 };

  template<typename T, FailureS2Tprocess PROCESS_ERR>
  struct OnError;

  template<c_basic_string T>
  struct OnError<T, FailureS2Tprocess::THROW_ERROR>
  {
    using return_type = T;
  };

  template<c_NOT_basic_string T>
  struct OnError<T, FailureS2Tprocess::THROW_ERROR>
  {
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = T;

    static constexpr const FailureS2Tprocess processErr = FailureS2Tprocess::THROW_ERROR;

    inline static return_type handler(const std::string&, const auto& err) { throw err; }
  };

  template<c_signaling_NaN T>
  struct OnError<T, FailureS2Tprocess::SIGNAL_NAN>
  {
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = T;

    static constexpr const FailureS2Tprocess processErr = FailureS2Tprocess::SIGNAL_NAN;

    inline static return_type handler(const std::string& , const auto&)
    {
      return std::numeric_limits<T>::signaling_NaN();
    }
  };

  template<c_quiet_NaN T>
  struct OnError<T, FailureS2Tprocess::QUIET_NAN>
  {
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = T;

    static constexpr const FailureS2Tprocess processErr = FailureS2Tprocess::QUIET_NAN;

    inline static return_type handler(const std::string& , const auto&)
    {
      return std::numeric_limits<T>::quiet_NaN();
    }
  };

  template<c_NOT_basic_string T>
  struct OnError<T, FailureS2Tprocess::VARIANT_NAN>
  {
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = std::variant<T, std::string>;

    static constexpr const FailureS2Tprocess processErr = FailureS2Tprocess::VARIANT_NAN;

    inline static return_type handler(const std::string& sVal, const auto& )
    {
      return return_type{ sVal };
    }
  };
  // ]]============]] Error Handling



  // [[============[[ Conversion formats
  template<typename T, FailureS2Tprocess PROCESS_ERR>
  struct S2T_Format_WorkAround : public OnError<T, PROCESS_ERR> {};

  template<typename T, FailureS2Tprocess PROCESS_ERR>
  struct S2T_Format_std_StoT : public OnError<T, PROCESS_ERR> {};

  template <typename T, FailureS2Tprocess PROCESS_ERR, c_char CH>
  struct S2T_Format_StreamAsIs : public Format_StreamAsIs<std::basic_istringstream<CH> >,
                                 public OnError<T, PROCESS_ERR> {};

  template <typename T, FailureS2Tprocess PROCESS_ERR, c_char CH>
  struct S2T_Format_StreamUseClassicLocale : public Format_StreamUseClassicLocale<std::basic_istringstream<CH> >,
                                             public OnError<T, PROCESS_ERR> {};

    //  for e.g ->           constexpr char de_Loc[] = "de_DE"; 
  template <typename T, FailureS2Tprocess PROCESS_ERR, c_char CH, const char* usrLoc>
  struct S2T_Format_StreamUserLocale : public Format_StreamUserLocale<std::basic_istringstream<CH>, usrLoc>,
                                       public OnError<T, PROCESS_ERR> {};
  // ]]============]] Conversion formats


  // [[============[[ type - Default Conversion format
  template<typename T, typename = void >
  struct S2T_DefaultFormat
  {
    using type = S2T_Format_StreamUseClassicLocale<T, FailureS2Tprocess::THROW_ERROR, char>;
  };

/*
  template<c_integer_type T>
  struct S2T_DefaultFormat< T >
  {
    using type = S2T_Format_std_StoT<T, FailureS2Tprocess::THROW_ERROR>;
  };
*/

#if USE_FLOATINGPOINT_FROM_CHARS_1  ==  e_DISABLE_FEATURE
  template<c_floating_point T>
  struct S2T_DefaultFormat< T >
  {
    using type = S2T_Format_std_StoT<T, FailureS2Tprocess::SIGNAL_NAN>;
  };
#endif

/*
  template<c_char CH >
  struct S2T_DefaultFormat<CH, void>
  {
    using type = S2T_Format_StreamUseClassicLocale<CH>;   //  refer below -> S2T_Format_WorkAround
  };
*/

  template<typename T>
  struct S2T_DefaultFormat< T,
                            typename  std::enable_if_t< is_string<T> || // std::is_same_v<T, std::string> ||
                                                        is_char<T>::value ||
                                                        std::is_same_v<T, bool>
                                                      >
                          >
  {
    using type = S2T_Format_WorkAround<T, FailureS2Tprocess::THROW_ERROR>;
  };
  // ]]============]] type - Default Conversion format



  // [[============[[ ISS -> istringstream :: concept
  template <typename, typename = void>
  struct is_formatISS : std::false_type {};

  template <typename FMT>  // FMT -> expected defined types :: 'stream_type'
  struct is_formatISS<  FMT,
                        typename std::enable_if_t< std::is_same_v< typename FMT::stream_type,
                                                                   std::basic_istringstream<typename FMT::stream_type::char_type>
                                                                 >
                                                 >
                     >
            : is_formatSS<FMT>
  {};
  template <typename FMT>
  concept c_formatISS = is_formatISS<FMT>::value;
  // ]]============]] ISS -> istringstream :: concept



  // [[============[[ S2T-converter :: concept
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
  // ]]============]] S2T-converter :: concept



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
  struct ConvertFromStr
  {
    /*
     *  Normally, this template instance should not get initialized.
     *  In case it does that would mean template parameters passed
     *  are not as expected. The static function 'instanceEvaluater()'
     *  will help in figuring out the problem.
     */
    constexpr static float instanceEvaluater()
    {
      static_assert(std::is_same_v< typename S2T_FORMAT::return_type, T >);
      //S2T_FORMAT::handler("dummyValue", std::exception() );

      if constexpr(has_streamUpdate<S2T_FORMAT>::value)
      {
        static_assert(converter::is_iostream<typename S2T_FORMAT::stream_type>::value);
        static_assert(converter::is_formatSS<S2T_FORMAT>::value);
        //std::cout << "typename S2T_FORMAT::stream_type = "
        //          << typeid(typename S2T_FORMAT::stream_type).name() << std::endl;
        //std::cout << "std::basic_istringstream<typename S2T_FORMAT::stream_type::char_type> = "
        //          << typeid(std::basic_ostringstream<typename S2T_FORMAT::stream_type::char_type>).name() << std::endl;

        static_assert(!std::is_same_v< typename S2T_FORMAT::stream_type,
                                       std::basic_ostringstream<typename S2T_FORMAT::stream_type::char_type>
                                     >);  // ERROR :: invalid template param for FORMATTING -> 'istringstream' should be passed instead of 'ostringstream'
        static_assert(std::is_same_v< typename S2T_FORMAT::stream_type,
                                      std::basic_istringstream<typename S2T_FORMAT::stream_type::char_type>
                                    >);
        static_assert(converter::is_formatISS<S2T_FORMAT>::value);
        //std::cerr << "WARNING : Formater is istringstream type" << std::endl;
      } else {
        //std::cerr << "WARNING : Formater is NOT istringstream type" << std::endl;
      }
      return 0.1f;
    }

    static constexpr float template_uid = instanceEvaluater();
  };

  /*
   * This a helper internal class, not meant to be called by upstream users.
  */
  template < typename                                        T, 
             FailureS2Tprocess                               PROCESS_ERR,
             std::derived_from< OnError<T, PROCESS_ERR > >   ERR_HANDLER
           >
  class pConvertFromStr_POS
  {
    friend struct ConvertFromStr< T, ERR_HANDLER >;

    /*
     * Function wrapper to query if the complete input string was read during conversion.
     */
    inline static typename ERR_HANDLER::return_type
    _toVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: pConvertFromStr_POS< T, PROCESS_ERR, ERR_HANDLER >::_toVal('" << str << "')");
      std::size_t pos = 0;
      try
      {
        T val = ConvertFromStr<T, ERR_HANDLER>::ToVal_args(str, &pos);

        if(pos == str.length())
        {
          if constexpr( std::is_floating_point_v<T>  &&
                        PROCESS_ERR == FailureS2Tprocess::VARIANT_NAN)
          {
            if(std::isnan(val))
              return str;
          }
          return val;
        }
      } catch (const std::invalid_argument& err) {
        CONVERTER_DEBUG_LOG( "got-error: " << err.what() );
        return ERR_HANDLER::handler(str, err);
      } catch (const std::out_of_range& err) {
        CONVERTER_DEBUG_LOG( "got-error: " << err.what() );
        return ERR_HANDLER::handler(str, err);
      } catch (const std::exception& err) {
        CONVERTER_DEBUG_LOG( "got-error: " << err.what() );
        return ERR_HANDLER::handler(str, err);
      }

      CONVERTER_DEBUG_LOG( _errMsg << ":  string-conversion-failure for value '" << str << "'  pos=" << pos );
      return ERR_HANDLER::handler(str, _err);
    }

    inline static const std::string _errMsg{"String isn't a numeric-type. 'return_type pConvertFromStr_POS<T, PROCESS_ERR, ERR_HANDLER>::_toVal(const std::string& str)'"};
    inline static const std::invalid_argument _err{_errMsg};
  };

  // https://eli.thegreenplace.net/2014/perfect-forwarding-and-universal-references-in-c/
  /**
   * @brief     Convertor class implementation for any (non-string)type's, FROM input-string; using 'std::istringstream'.
   * @tparam  T                     'type' converted to, from string data. (Not Applicable for 'string to string' conversion.)
   * @tparam  S2T_FORMAT_STREAM     Class which encapsulates conversion parameters/directives such as using 'Locale'.
   */
  template< c_NOT_basic_string T, c_formatISS S2T_FORMAT_STREAM >
  struct ConvertFromStr<T, S2T_FORMAT_STREAM>
  {
  private:
    inline static bool _checkStreamFailure(const typename S2T_FORMAT_STREAM::stream_type& iss)
    {
      if constexpr (is_char<T>::value)
        return ( iss.fail() || iss.bad() );  // for char-type, !iss.eof() check doesn't work as expected
      else
        return ( iss.fail() || iss.bad() || (!iss.eof()) );
    }

    inline static const std::string _errMsg{"Stream read failure. 'T ConvertFromStr<c_NOT_basic_string T, c_formatISS S2T_FORMAT>::ToVal(const std::string& str)'"};
    inline static const std::invalid_argument _err{_errMsg};

  public:

    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     *           Can be 'T' or 'variant<T,string>' type.
     */
    using return_type = typename S2T_FORMAT_STREAM::return_type;

    static const int template_uid = 1;

    /**
     * @brief   Converts string holding a possibly-numerical value to numerical datatype representation.
     * @param   str                 input string.
     * @returns type value if conversion succeeds. This type is any type that's supported by 'std::istringstream'.
     *          Else throws 'std::invalid_argument' on conversion failure.
     */
    inline static return_type
    ToVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_NOT_basic_string T, c_formatISS S2T_FORMAT_STREAM >::ToVal('" << str << "')");
      using stream_type = typename S2T_FORMAT_STREAM::stream_type;

      T val;

      //_ConvStr<typename S2T_FORMAT::stream_type::char_type>  cStr(str);
      //stream_type iss( cStr.getStr() );
      stream_type iss( str );
      S2T_FORMAT_STREAM::streamUpdate(iss);
      iss >> val;

      if( _checkStreamFailure(iss) )
      {
        CONVERTER_DEBUG_LOG(_errMsg << " input-string-stream-failure for value '" << str << "'"           \
                                    << std::boolalpha << " iss.fail()=" << iss.fail() << "  iss.bad()="   \
                                    << iss.bad() << "  iss.eof()=" << iss.eof());
        return S2T_FORMAT_STREAM::handler(str, _err);
      }
      return val;
    }
  };



  /**
   * @brief     Convertor class implementation for integer types FROM string.
   *            This conversion is achieved using either of 'std::stoi' or 'std::stol',
   *            and similar std-functions based on type 'T'.
   * @tparam  T                     'integer-type' converted to, from string data.
   * @tparam  PROCESS_ERR            enum variable of type FailureS2Tprocess.
   */
  template <c_integer_type T, FailureS2Tprocess PROCESS_ERR>
  struct ConvertFromStr<T, S2T_Format_std_StoT<T, PROCESS_ERR> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = typename S2T_Format_std_StoT<T, PROCESS_ERR >::return_type;

    static const int template_uid = 2;

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
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_integer_type T, S2T_Format_std_StoT<T, PROCESS_ERR> >::ToVal_args('" << str << "')");
      /*
       * Size of int and short are implementation defined.
       * int and short are 2 bytes on 16 compilers.
       * On 32 bit compilers, int is 4 bytes, while short remains 16 bits. 
       *
       * check for byte sizes of each integral type here ...
       * https://cplusplus.com/forum/general/140825/
       * https://en.cppreference.com/w/cpp/language/types
       */
      // numeric formats may be accepted by the currently installed C locale.
      if constexpr(std::is_signed_v<T>)
      {
        if constexpr( std::is_same_v<T,long long> ) {
          return std::stoll(str, pos, base);
        } else
        if constexpr( std::is_same_v<T,long> ) {
          return std::stol(str, pos, base);
        } else
        if constexpr( std::is_same_v<T,int> ) {
          return std::stoi(str, pos, base);
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
          return std::stoull(str, pos, base);
        } else
        if constexpr( std::is_same_v<T,unsigned long> ) {
          return std::stoul(str, pos, base);
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
    inline static return_type
    ToVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_integer_type T, S2T_Format_std_StoT<T, PROCESS_ERR> >::ToVal('" << str << "')");
      CONVERTER_DEBUG_TRY_START
        return pConvertFromStr_POS<T, PROCESS_ERR, S2T_Format_std_StoT<T, PROCESS_ERR> >::_toVal(str);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::invalid_argument)
      CONVERTER_DEBUG_TRY_CATCH(std::out_of_range)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
      //                        WARNING
      // std::exception :: behaviour change during debugging, as any derived std::exception
      //                   will get caught here and when will be rethrown as std::exception.
      //                   This will potentially impact the callee which has a catch-block
      //                   for that derived exception; as the exception will not be caught
      //                   in that particular catch and land up in a different catch block,
      //                   thus any applcation will might behave differently in debug mode.
    }
  };

  /**
   * @brief     Convertor class implementation for floating types from string.
   *            This conversion is achieved using either of 'std::stof' or 'std::stod',
   *            or 'std::stdold' based on type 'T'.
   * @tparam  T                     'floating-type' converted to, from string data.
   * @tparam  PROCESS_ERR            enum variable of type FailureS2Tprocess.
   */
  template<c_floating_point T, FailureS2Tprocess PROCESS_ERR>
  struct ConvertFromStr<T, S2T_Format_std_StoT<T, PROCESS_ERR> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = T;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = typename S2T_Format_std_StoT<T, PROCESS_ERR >::return_type;

    static const int template_uid = 3;

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
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_floating_point T, S2T_Format_std_StoT<T, PROCESS_ERR> >::ToVal_args('" << str << "')");
      // any other expression that may be accepted by the currently installed C locale
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
    inline static return_type
    ToVal(const std::string& str)
    {
      CONVERTER_DEBUG_LOG("trace :: ConvertFromStr< c_floating_point T, S2T_Format_std_StoT<T, PROCESS_ERR> >::ToVal('" << str << "')");
      CONVERTER_DEBUG_TRY_START
        return pConvertFromStr_POS<T, PROCESS_ERR, S2T_Format_std_StoT<T, PROCESS_ERR> >::_toVal(str);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::invalid_argument)
      CONVERTER_DEBUG_TRY_CATCH(std::out_of_range)
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
      //                        WARNING
      // std::exception :: behaviour change during debugging, as any derived std::exception
      //                   will get caught here and when will be rethrown as std::exception.
      //                   This will potentially impact the callee which has a catch-block
      //                   for that derived exception; as the exception will not be caught
      //                   in that particular catch and land up in a different catch block,
      //                   thus any applcation will might behave differently in debug mode.
    }
  };

  /**
   * @brief     Specialized implementation handling string to string conversion.
   */
  template<>
  struct ConvertFromStr<std::string, S2T_Format_WorkAround<std::string, FailureS2Tprocess::THROW_ERROR> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = std::string;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = std::string;

    static const int template_uid = 4;

    /**
     * @brief   Converts string to string.
     * @param   str                 input string.
     * @returns string.
     */
    inline static std::string ToVal(const std::string& str) { return str; }
  };
  template<c_basic_string BSTR>
  struct ConvertFromStr<BSTR, S2T_Format_WorkAround<BSTR, FailureS2Tprocess::THROW_ERROR> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = BSTR;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = BSTR;

    static const int template_uid = 14;

    /**
     * @brief   Converts string to string.
     * @param   str                 input string.
     * @returns string.
     */
    inline static BSTR ToVal(const std::string& str)
    {
      BSTR bstr = str.c_str();
      return bstr;
    }
  };

  /**
   * @brief     Specialized implementation handling string to char conversion.
   * @tparam  CH                     'char-type' converted to, from string data.
   * @tparam  PROCESS_ERR            enum variable of type FailureS2Tprocess.
   */
  template<c_char CH, FailureS2Tprocess PROCESS_ERR>
  struct ConvertFromStr< CH, S2T_Format_WorkAround<CH, PROCESS_ERR> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = CH;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = typename S2T_Format_WorkAround<CH, PROCESS_ERR>::return_type;

    static const int template_uid = 5;

    // TODO unit tests
    /**
     * @brief   Converts string holding char value.
     * @param   str                 input string.
     * @returns char-type.
     */
    inline static return_type
    ToVal(const std::string& str)
    {
      if(str.length()>1)
      {
        CONVERTER_DEBUG_LOG( _errMsg <<  " string2charT-conversion-failure for value '" << str << "'");
        return S2T_Format_WorkAround<CH, PROCESS_ERR>::handler(str, _err);
      }

      if constexpr( std::is_same<CH, std::string::value_type>::value ) {
        return str[0];
      } else {
        return static_cast<CH>(str[0]);
      }
    }

  private:
    inline static const std::string _errMsg{"String isn't a char-type. 'CH ConvertFromStr<c_char CH,S2T_Format_WorkAround>::ToVal(const std::string& str)'"};
    inline static const std::invalid_argument _err{_errMsg};
  };

  /**
   * @brief     Specialized implementation handling string to bool conversion.
   * @tparam  PROCESS_ERR             enum variable of type FailureS2Tprocess.
   */
  template<FailureS2Tprocess PROCESS_ERR>
  struct ConvertFromStr<bool, S2T_Format_WorkAround<bool, PROCESS_ERR> >
  {
    /**
     * @brief   'type' definition being declared for.
     */
    using value_type  = bool;
    /**
     * @brief   'type' definition returned by the convertor.
     */
    using return_type = typename S2T_Format_WorkAround<bool, PROCESS_ERR>::return_type;

    static const int template_uid = 6;

    // TODO unit tests
    /**
     * @brief   Converts string holding bool value.
     * @param   str                 input string.
     * @returns bool.
     */
    inline static return_type
    ToVal(const std::string& str)
    {
      unsigned long val = std::stoul(str); //(str, pos, base);
      if(val > 1)
      {
        CONVERTER_DEBUG_LOG( _errMsg << " string2bool-conversion-failure for value '" << str << "'" );
        return S2T_Format_WorkAround<bool, PROCESS_ERR>::handler(str, _err);
      }
      return val;
    }

  private:
    inline static const std::string _errMsg{"String isn't a bool. 'T ConvertFromStr<bool,S2T_Format_WorkAround>::ToVal(const std::string& str)'"};
    inline static const std::invalid_argument _err{_errMsg};
  };

  // ]=============================================================] ConvertFromStr




  // [=============================================================[ S2T conversion-type definition
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
      CONVERTER_DEBUG_LOG("trace :: S2TwrapperFunction< T, ... >::ToVal('" << str << "')");
      CONVERTER_DEBUG_TRY_START
        return CONV_S2T(str);
      CONVERTER_DEBUG_TRY_END
      CONVERTER_DEBUG_TRY_CATCH(std::exception)
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

  // ]=============================================================] S2T conversion-types definition
}
