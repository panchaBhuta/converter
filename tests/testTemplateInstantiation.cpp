// test template instantiation is happening as expected


#include <cmath>
#include <iomanip>

#include <converter/converter.h>

#include "unittest.h"


int main()
{
  using T2S_Format_SDPfloat  =  converter::T2S_Format_StreamDecimalPrecision<float>;
  using T2S_Format_SDPdouble =  converter::T2S_Format_StreamDecimalPrecision<double>;

  static_assert(std::is_same_v<converter::S2T_Format_StreamUseClassicLocale<char>::stream_type, std::istringstream>);
  static_assert(std::is_same_v<converter::T2S_Format_StreamAsIs<char>::stream_type, std::ostringstream>);
  static_assert(std::is_same_v<T2S_Format_SDPfloat::stream_type, std::ostringstream>);
  static_assert(std::is_same_v<T2S_Format_SDPdouble::stream_type, std::ostringstream>);

  // checks for typename 'STREAM_FORMAT::stream_type' and static function 'STREAM_FORMAT::streamUpdate' with specific signature
  static_assert(converter::has_streamUpdate<converter::S2T_Format_StreamUseClassicLocale<char>>::value);
  static_assert(converter::has_streamUpdate<converter::T2S_Format_StreamAsIs<char>>::value);
  static_assert(converter::has_streamUpdate<T2S_Format_SDPfloat>::value);
  static_assert(converter::has_streamUpdate<T2S_Format_SDPdouble>::value);

  static_assert(converter::is_formatSS<converter::S2T_Format_StreamUseClassicLocale<char> >::value);
  static_assert(converter::is_formatSS<converter::T2S_Format_StreamAsIs<char>>::value);
  static_assert(converter::is_formatSS<T2S_Format_SDPfloat>::value);
  static_assert(converter::is_formatSS<T2S_Format_SDPdouble>::value);

  static_assert(converter::is_formatISS<converter::S2T_Format_StreamUseClassicLocale<char> >::value);
  static_assert(converter::is_formatOSS<converter::T2S_Format_StreamAsIs<char>>::value);
  static_assert(converter::is_formatOSS<T2S_Format_SDPfloat>::value);
  static_assert(converter::is_formatOSS<T2S_Format_SDPdouble>::value);
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

  // check for default S2T_FORMAT types
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<int>::type, converter::S2T_Format_std_StoT>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<long>::type, converter::S2T_Format_std_StoT>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<float>::type, converter::S2T_Format_std_StoT>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<double>::type, converter::S2T_Format_std_StoT>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<std::string>::type, converter::S2T_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<char>::type, converter::S2T_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<signed char>::type, converter::S2T_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<unsigned char>::type, converter::S2T_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<wchar_t>::type, converter::S2T_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<char8_t>::type, converter::S2T_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<char16_t>::type, converter::S2T_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<char32_t>::type, converter::S2T_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<bool>::type, converter::S2T_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<std::chrono::year_month_day>::type, converter::S2T_Format_StreamYMD< converter::defYMDfmt > >);

  // check for default T2S_FORMAT types
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<int>::type, converter::T2S_Format_std_TtoS>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<long>::type, converter::T2S_Format_std_TtoS>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<float>::type, T2S_Format_SDPfloat>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<double>::type, T2S_Format_SDPdouble>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<std::string>::type, converter::T2S_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<char>::type, converter::T2S_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<signed char>::type, converter::T2S_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<unsigned char>::type, converter::T2S_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<wchar_t>::type, converter::T2S_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<char8_t>::type, converter::T2S_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<char16_t>::type, converter::T2S_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<char32_t>::type, converter::T2S_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<bool>::type, converter::T2S_Format_WorkAround>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<std::chrono::year_month_day>::type, converter::T2S_Format_StreamYMD< converter::defYMDfmt > >);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<int>::ToVal),    int(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<double>::ToStr), std::string(*)(const double&)>);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<int, converter::S2T_Format_StreamAsIs<char> >::ToVal), int(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<int, converter::T2S_Format_StreamAsIs<char> >::ToStr), std::string(*)(const int&)>);

  // this fails to compile as designed
  //static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<double, converter::T2S_Format_std_TtoS>::ToStr), std::string(*)(const double&)>);


  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<char>::ToVal),    char(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<char>::ToStr), std::string(*)(const char&)>);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<wchar_t>::ToVal), wchar_t(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<wchar_t>::ToStr), std::string(*)(const wchar_t&)>);

  int rv = 0;


  return rv;
}
