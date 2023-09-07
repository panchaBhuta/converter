// test if default template instantiation is happening as expected


#include <cmath>
#include <iomanip>

#include <converter/converter.h>

#include "unittest.h"

template<typename T>
void checkDefaultFormatInstance_Integer()
{
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<T>::type,
                                        converter::S2T_Format_std_CtoT<T, converter::FailureS2Tprocess::THROW_ERROR>>);

  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<T>::type,
                                        converter::T2S_Format_std_TtoC>);
}

template<typename T>
void checkDefaultFormatInstance_Float()
{
#if   USE_FLOATINGPOINT_FROM_CHARS_1  ==  _e_ENABLE_FEATURE_
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<T>::type,
                                        converter::S2T_Format_std_CtoT<T, converter::FailureS2Tprocess::SIGNAL_NAN>>);
#else
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<T>::type,
                                        converter::S2T_Format_std_StoT<T, converter::FailureS2Tprocess::SIGNAL_NAN>>);
#endif

#if   USE_FLOATINGPOINT_TO_CHARS_1  ==  _e_ENABLE_FEATURE_
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<T>::type,
                                        converter::T2S_Format_std_TtoC>);
#else
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<T>::type,
                                        converter::T2S_Format_StreamDecimalPrecision<T>>);
                                        // instead of converter::T2S_Format_std_TtoS
#endif
}

template<typename T>
void checkDefaultFormatInstance_WA()
{
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<T>::type,
                                        converter::S2T_Format_WorkAround<T, converter::FailureS2Tprocess::THROW_ERROR>>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<T>::type,
                                        converter::T2S_Format_WorkAround>);
}

template<typename T>
void checkDefaultFormatInstance_Date()
{
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<T>::type,
                                        converter::S2T_Format_StreamYMD<converter::defYMDfmt, converter::FailureS2Tprocess::THROW_ERROR>>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<T>::type,
                                        converter::T2S_Format_StreamYMD<converter::defYMDfmt>>);
}




template<typename T , int s2t_uid , int t2s_uid = -s2t_uid>
void checkDefaultFunctionInstance()
{
  static_assert(converter::ConvertFromStr<T>::template_uid == s2t_uid);
  static_assert(converter::ConvertFromVal<T>::template_uid == t2s_uid);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T>::ToVal), T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T>::ToStr), std::string(*)(const T&)>);
}

#if   USE_FLOATINGPOINT_FROM_CHARS_1  ==  _e_ENABLE_FEATURE_
  static constexpr const int fp_S2T_uid = 103;
#else
  static constexpr const int fp_S2T_uid = 3;
#endif

#if   USE_FLOATINGPOINT_TO_CHARS_1  ==  _e_ENABLE_FEATURE_
  static constexpr const int fp_T2S_uid = -103;
#else
  static constexpr const int fp_T2S_uid = -1;
#endif


int main()
{
  using T2S_Format_SDPfloat  =  converter::T2S_Format_StreamDecimalPrecision<float>;
  using T2S_Format_SDPdouble =  converter::T2S_Format_StreamDecimalPrecision<double>;
  using T2S_Format_SDPlongdouble =  converter::T2S_Format_StreamDecimalPrecision<long double>;

  static_assert(std::is_same_v<converter::S2T_Format_StreamUseClassicLocale<int, converter::FailureS2Tprocess::THROW_ERROR, char>::stream_type, std::istringstream>);
  static_assert(std::is_same_v<converter::T2S_Format_StreamAsIs<char>::stream_type, std::ostringstream>);
  static_assert(std::is_same_v<T2S_Format_SDPfloat::stream_type, std::ostringstream>);
  static_assert(std::is_same_v<T2S_Format_SDPdouble::stream_type, std::ostringstream>);
  static_assert(std::is_same_v<T2S_Format_SDPlongdouble::stream_type, std::ostringstream>);

  // checks for typename 'STREAM_FORMAT::stream_type' and static function 'STREAM_FORMAT::streamUpdate' with specific signature
  static_assert(converter::has_streamUpdate<converter::S2T_Format_StreamUseClassicLocale<float, converter::FailureS2Tprocess::THROW_ERROR, char>>::value);
  static_assert(converter::has_streamUpdate<converter::T2S_Format_StreamAsIs<char>>::value);
  static_assert(converter::has_streamUpdate<T2S_Format_SDPfloat>::value);
  static_assert(converter::has_streamUpdate<T2S_Format_SDPdouble>::value);
  static_assert(converter::has_streamUpdate<T2S_Format_SDPlongdouble>::value);

  static_assert(converter::is_formatSS<converter::S2T_Format_StreamUseClassicLocale<long, converter::FailureS2Tprocess::THROW_ERROR, char> >::value);
  static_assert(converter::is_formatSS<converter::T2S_Format_StreamAsIs<char>>::value);
  static_assert(converter::is_formatSS<T2S_Format_SDPfloat>::value);
  static_assert(converter::is_formatSS<T2S_Format_SDPdouble>::value);
  static_assert(converter::is_formatSS<T2S_Format_SDPlongdouble>::value);

  static_assert(converter::is_formatISS<converter::S2T_Format_StreamUseClassicLocale<long long, converter::FailureS2Tprocess::THROW_ERROR, char> >::value);
  static_assert(converter::is_formatOSS<converter::T2S_Format_StreamAsIs<char>>::value);
  static_assert(converter::is_formatOSS<T2S_Format_SDPfloat>::value);
  static_assert(converter::is_formatOSS<T2S_Format_SDPdouble>::value);
  static_assert(converter::is_formatOSS<T2S_Format_SDPlongdouble>::value);
  static_assert(converter::is_formatOSS<typename converter::T2S_Format_floating_StreamCombine< float  > >::value);
  static_assert(converter::is_formatOSS<typename converter::T2S_Format_floating_StreamCombine< double > >::value);
  static_assert(converter::is_formatOSS<
                                        typename  converter::T2S_Format_floating_StreamCombine
                                                  <
                                                        float,
                                                        T2S_Format_SDPfloat,
                                                        typename converter::T2S_Format_StreamUseClassicLocale<char>
                                                  >
                                      >::value);
  static_assert(converter::is_formatOSS<
                                        typename  converter::T2S_Format_floating_StreamCombine
                                                  <
                                                        double,
                                                        T2S_Format_SDPdouble,
                                                        typename converter::T2S_Format_StreamUseClassicLocale<char>
                                                  >
                                      >::value);
  static_assert(converter::is_formatOSS<
                                        typename  converter::T2S_Format_floating_StreamCombine
                                                  <
                                                        long double,
                                                        T2S_Format_SDPlongdouble,
                                                        typename converter::T2S_Format_StreamUseClassicLocale<char>
                                                  >
                                      >::value);

  static_assert(converter::is_formatYMDiss< converter::S2T_Format_StreamYMD< converter::defYMDfmt > >::value);
  static_assert(converter::is_formatYMDoss< converter::T2S_Format_StreamYMD< converter::defYMDfmt > >::value);

  
  // check for default S2T_FORMAT and default T2S_FORMAT types
  checkDefaultFormatInstance_Integer<short>();
  checkDefaultFormatInstance_Integer<int>();
  checkDefaultFormatInstance_Integer<long>();
  checkDefaultFormatInstance_Integer<long long>();
  checkDefaultFormatInstance_Integer<unsigned short>();
  checkDefaultFormatInstance_Integer<unsigned int>();
  checkDefaultFormatInstance_Integer<unsigned long>();
  checkDefaultFormatInstance_Integer<unsigned long long>();
  checkDefaultFormatInstance_Float<float>();
  checkDefaultFormatInstance_Float<double>();
  checkDefaultFormatInstance_Float<long double>();
  checkDefaultFormatInstance_WA<std::string>();
  checkDefaultFormatInstance_WA<char>();
  checkDefaultFormatInstance_WA<signed char>();
  checkDefaultFormatInstance_WA<unsigned char>();
  checkDefaultFormatInstance_WA<wchar_t>();
  checkDefaultFormatInstance_WA<char8_t>();
  checkDefaultFormatInstance_WA<char16_t>();
  checkDefaultFormatInstance_WA<char32_t>();
  checkDefaultFormatInstance_WA<bool>();
  checkDefaultFormatInstance_Date<std::chrono::year_month_day>();


  // check for function signature
  checkDefaultFunctionInstance<short,102>();  // 2
  checkDefaultFunctionInstance<int,102>();    // 2
  checkDefaultFunctionInstance<long,102>();   // 2
  checkDefaultFunctionInstance<long long,102>();         // 2
  checkDefaultFunctionInstance<unsigned short,102>();    // 2
  checkDefaultFunctionInstance<unsigned int,102>();      // 2
  checkDefaultFunctionInstance<unsigned long,102>();     // 2
  checkDefaultFunctionInstance<unsigned long long,102>();// 2
  checkDefaultFunctionInstance<float,fp_S2T_uid,fp_T2S_uid>();    // (103 | 3) , (-103 | -1)
  checkDefaultFunctionInstance<double,fp_S2T_uid,fp_T2S_uid>();   // (103 | 3) , (-103 | -1)
  checkDefaultFunctionInstance<long double,fp_S2T_uid,fp_T2S_uid>();  // (103 | 3) , (-103 | -1)
  checkDefaultFunctionInstance<std::string,4>();
  checkDefaultFunctionInstance<char,5>();
  checkDefaultFunctionInstance<signed char,5>();
  checkDefaultFunctionInstance<unsigned char,5>();
  checkDefaultFunctionInstance<wchar_t,5>();
  checkDefaultFunctionInstance<char8_t,5>();
  checkDefaultFunctionInstance<char16_t,5>();
  checkDefaultFunctionInstance<char32_t,5>();
  checkDefaultFunctionInstance<bool,6>();
  checkDefaultFunctionInstance<std::chrono::year_month_day,10000>();




  int rv = 0;


  return rv;
}
