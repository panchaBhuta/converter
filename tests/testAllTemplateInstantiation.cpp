// test if all possible template instantiation is happening as expected


#include <cmath>
#include <iomanip>

#include <converter/converter.h>
#include <converter/specializedTypes/date.h>
#include <specializedTypes/case_insensitive_string.h>

#include "unittest.h"

constexpr char de_Loc[] = "de_DE";

template<typename T>
void checkFormatInstance_General()
{
  {
    static_assert(std::is_same_v<T,
                                 typename converter::S2T_Format_WorkAround<T, converter::FailureS2Tprocess::THROW_ERROR>::return_type>);
    static_assert(std::is_same_v<std::variant<T, std::string>,
                                 typename converter::S2T_Format_WorkAround<T, converter::FailureS2Tprocess::VARIANT_NAN>::return_type>);
    [[maybe_unused]] converter::T2S_Format_WorkAround fmtT2S;
  }

  {
    static_assert(std::is_same_v<T,
                                 typename converter::S2T_Format_std_StoT<T, converter::FailureS2Tprocess::THROW_ERROR>::return_type>);
    static_assert(std::is_same_v<std::variant<T, std::string>,
                                 typename converter::S2T_Format_std_StoT<T, converter::FailureS2Tprocess::VARIANT_NAN>::return_type>);
    [[maybe_unused]] converter::T2S_Format_std_TtoS fmtT2S;
  }

  {
    static_assert(std::is_same_v<T,
                                 typename converter::S2T_Format_StreamAsIs<T, converter::FailureS2Tprocess::THROW_ERROR,char>::return_type>);
    static_assert(std::is_same_v<std::variant<T, std::string>,
                                 typename converter::S2T_Format_StreamAsIs<T, converter::FailureS2Tprocess::VARIANT_NAN,char>::return_type>);
    [[maybe_unused]] converter::T2S_Format_StreamAsIs<char> fmtT2S;
  }

  {
    static_assert(std::is_same_v<T,
                                 typename converter::S2T_Format_StreamUseClassicLocale<T, converter::FailureS2Tprocess::THROW_ERROR,char>::return_type>);
    static_assert(std::is_same_v<std::variant<T, std::string>,
                                 typename converter::S2T_Format_StreamUseClassicLocale<T, converter::FailureS2Tprocess::VARIANT_NAN,char>::return_type>);
    [[maybe_unused]] converter::T2S_Format_StreamUseClassicLocale<char> fmtT2S;
  }

  {
    static_assert(std::is_same_v<T,
                                 typename converter::S2T_Format_StreamUserLocale<T, converter::FailureS2Tprocess::THROW_ERROR,char,de_Loc>::return_type>);
    static_assert(std::is_same_v<std::variant<T, std::string>,
                                 typename converter::S2T_Format_StreamUserLocale<T, converter::FailureS2Tprocess::VARIANT_NAN,char,de_Loc>::return_type>);
    [[maybe_unused]] converter::T2S_Format_StreamUserLocale<char,de_Loc> fmtT2S;
  }
}

template<typename T>
void checkFormatInstance_Integer()
{
  checkFormatInstance_General<T>();

  {
    static_assert(std::is_same_v<T,
                                 typename converter::S2T_Format_std_CtoT<T, converter::FailureS2Tprocess::THROW_ERROR>::return_type>);
    static_assert(std::is_same_v<std::variant<T, std::string>,
                                 typename converter::S2T_Format_std_CtoT<T, converter::FailureS2Tprocess::VARIANT_NAN>::return_type>);
    [[maybe_unused]] converter::T2S_Format_std_TtoC fmtT2S;
  }
}


template<typename T>
void checkFormatInstance_Float()
{
#if USE_FLOATINGPOINT_FROM_CHARS_1  ==  e_ENABLE_FEATURE
  checkFormatInstance_Integer<T>();
#else
  checkFormatInstance_General<T>();
#endif

  {
    static_assert(std::is_same_v<T, typename converter::S2T_Format_WorkAround<T, converter::FailureS2Tprocess::QUIET_NAN>::return_type>);
    static_assert(std::is_same_v<T, typename converter::S2T_Format_WorkAround<T, converter::FailureS2Tprocess::SIGNAL_NAN>::return_type>);
  }

#if USE_FLOATINGPOINT_FROM_CHARS_1  ==  e_ENABLE_FEATURE
  {
    static_assert(std::is_same_v<T, typename converter::S2T_Format_std_CtoT<T, converter::FailureS2Tprocess::QUIET_NAN>::return_type>);
    static_assert(std::is_same_v<T, typename converter::S2T_Format_std_CtoT<T, converter::FailureS2Tprocess::SIGNAL_NAN>::return_type>);
  }
#endif

  {
    static_assert(std::is_same_v<T, typename converter::S2T_Format_std_StoT<T, converter::FailureS2Tprocess::QUIET_NAN>::return_type>);
    static_assert(std::is_same_v<T, typename converter::S2T_Format_std_StoT<T, converter::FailureS2Tprocess::SIGNAL_NAN>::return_type>);
  }

  {
    static_assert(std::is_same_v<T, typename converter::S2T_Format_StreamAsIs<T, converter::FailureS2Tprocess::QUIET_NAN, char>::return_type>);
    static_assert(std::is_same_v<T, typename converter::S2T_Format_StreamAsIs<T, converter::FailureS2Tprocess::SIGNAL_NAN, char>::return_type>);
  }

  {
    static_assert(std::is_same_v<T, typename converter::S2T_Format_StreamUseClassicLocale<T, converter::FailureS2Tprocess::QUIET_NAN, char>::return_type>);
    static_assert(std::is_same_v<T, typename converter::S2T_Format_StreamUseClassicLocale<T, converter::FailureS2Tprocess::SIGNAL_NAN, char>::return_type>);
  }

  {
    static_assert(std::is_same_v<T, typename converter::S2T_Format_StreamUserLocale<T, converter::FailureS2Tprocess::QUIET_NAN, char, de_Loc>::return_type>);
    static_assert(std::is_same_v<T, typename converter::S2T_Format_StreamUserLocale<T, converter::FailureS2Tprocess::SIGNAL_NAN, char, de_Loc>::return_type>);
  }

  [[maybe_unused]] converter::T2S_Format_StreamDecimalPrecision<T> fmtT2S;
}

#define S2T_FORMAT_MACRO2(FORMAT_TEMPLATE, PROCESS)  converter::FORMAT_TEMPLATE<T, converter::FailureS2Tprocess::PROCESS>
#define S2T_FORMAT_MACRO3(FORMAT_TEMPLATE, PROCESS)  converter::FORMAT_TEMPLATE<T, converter::FailureS2Tprocess::PROCESS, char>
#define S2T_FORMAT_MACRO4(FORMAT_TEMPLATE, PROCESS)  converter::FORMAT_TEMPLATE<T, converter::FailureS2Tprocess::PROCESS, char, de_Loc>

template<typename T>
void Format_WorkAround_Process2()
{
  using t_vNaN = std::variant<T, std::string>;
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_WorkAround,THROW_ERROR)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_WorkAround,VARIANT_NAN)>::ToVal),
                                t_vNaN (*)(const std::string&)>);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter::T2S_Format_WorkAround>::ToStr),
                                std::string(*)(const T&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaN, converter::T2S_Format_WorkAround>::ToStr),
                                std::string(*)(const t_vNaN&)>);
}

template<typename T>
void Format_std_CtoT_Process2() // needed for integer types
{
  using t_vNaN = std::variant<T, std::string>;
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_CtoT,THROW_ERROR)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_CtoT,VARIANT_NAN)>::ToVal),
                                t_vNaN (*)(const std::string&)>);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter::T2S_Format_std_TtoC>::ToStr),
                                std::string(*)(const T&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaN, converter::T2S_Format_std_TtoC>::ToStr),
                                std::string(*)(const t_vNaN&)>);
}
#if USE_FLOATINGPOINT_TO_CHARS_1  ==  e_ENABLE_FEATURE
template<typename T>
void Format_std_CtoT_Process4()
{
  Format_std_CtoT_Process2<T>();
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_CtoT,QUIET_NAN)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_CtoT,SIGNAL_NAN)>::ToVal),
                                T(*)(const std::string&)>);
}
#endif

template<typename T>
void Format_std_StoT_Process2()
{
  using t_vNaN = std::variant<T, std::string>;
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_StoT,THROW_ERROR)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_StoT,VARIANT_NAN)>::ToVal),
                                t_vNaN (*)(const std::string&)>);

  // T2S_Format_std_TtoS is applicable when T is integer-type , but not applicable for float-type
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter::T2S_Format_std_TtoS>::ToStr),
                                std::string(*)(const T&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaN, converter::T2S_Format_std_TtoS>::ToStr),
                                std::string(*)(const t_vNaN&)>);
}

template<typename T>
void Format_StreamAsIs_Process2()
{
  using t_vNaN = std::variant<T, std::string>;
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO3(S2T_Format_StreamAsIs,THROW_ERROR)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO3(S2T_Format_StreamAsIs,VARIANT_NAN)>::ToVal),
                                t_vNaN (*)(const std::string&)>);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter::T2S_Format_StreamAsIs<char>>::ToStr),
                                std::string(*)(const T&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaN, converter::T2S_Format_StreamAsIs<char>>::ToStr),
                                std::string(*)(const t_vNaN&)>);
}
template<typename T>
void Format_StreamAsIs_Process4()
{
  Format_StreamAsIs_Process2<T>();
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO3(S2T_Format_StreamAsIs,QUIET_NAN)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO3(S2T_Format_StreamAsIs,SIGNAL_NAN)>::ToVal),
                                T(*)(const std::string&)>);
}

template<typename T>
void Format_StreamUseClassicLocale_Process2()
{
  using t_vNaN = std::variant<T, std::string>;
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO3(S2T_Format_StreamUseClassicLocale,THROW_ERROR)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO3(S2T_Format_StreamUseClassicLocale,VARIANT_NAN)>::ToVal),
                                t_vNaN (*)(const std::string&)>);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter:: T2S_Format_StreamUseClassicLocale<char>>::ToStr),
                                std::string(*)(const T&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaN, converter::T2S_Format_StreamUseClassicLocale<char>>::ToStr),
                                std::string(*)(const t_vNaN&)>);
}
template<typename T>
void Format_StreamUseClassicLocale_Process4()
{
  Format_StreamUseClassicLocale_Process2<T>();
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO3(S2T_Format_StreamUseClassicLocale,QUIET_NAN)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO3(S2T_Format_StreamUseClassicLocale,SIGNAL_NAN)>::ToVal),
                                T(*)(const std::string&)>);
}

template<typename T>
void Format_StreamUserLocale_Process2()
{
  using t_vNaN = std::variant<T, std::string>;
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO4(S2T_Format_StreamUserLocale,THROW_ERROR)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO4(S2T_Format_StreamUserLocale,VARIANT_NAN)>::ToVal),
                                t_vNaN (*)(const std::string&)>);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter:: T2S_Format_StreamUserLocale<char, de_Loc>>::ToStr),
                                std::string(*)(const T&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaN, converter::T2S_Format_StreamUserLocale<char, de_Loc>>::ToStr),
                                std::string(*)(const t_vNaN&)>);
}
template<typename T>
void Format_StreamUserLocale_Process4()
{
  Format_StreamUserLocale_Process2<T>();
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO4(S2T_Format_StreamUserLocale,QUIET_NAN)>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO4(S2T_Format_StreamUserLocale,SIGNAL_NAN)>::ToVal),
                                T(*)(const std::string&)>);
}


template<typename T>
void checkFunctionInstance_Integer()
{
  /*
   * S2T : For Integer types , following params are not applicable
   *   FORMAT_TEMPLATE ->   S2T_Format_WorkAround<>
   *   PROCESS         ->   SIGNAL_NAN, QUIET_NAN
   * 
   * T2S : For Integer types , following params are not applicable
   *   FORMAT_TEMPLATE ->   T2S_Format_WorkAround, T2S_Format_StreamDecimalPrecision<>,
   *                        T2S_Format_floating_StreamCombine<>
  **/

  Format_std_CtoT_Process2<T>();
  Format_std_StoT_Process2<T>();
  Format_StreamAsIs_Process2<T>();
  Format_StreamUseClassicLocale_Process2<T>();
  Format_StreamUserLocale_Process2<T>();
}

template<typename T>
void checkFunctionInstance_Float()
{
  /*
   * S2T : For Integer types , following params are not applicable
   *   FORMAT_TEMPLATE ->   S2T_Format_WorkAround<>
   *   PROCESS         ->   {empty-list}
   * 
   * T2S : For Integer types , following params are not applicable
   *   FORMAT_TEMPLATE ->   T2S_Format_WorkAround, T2S_Format_std_TtoS
  **/

#if USE_FLOATINGPOINT_TO_CHARS_1  ==  e_ENABLE_FEATURE
  Format_std_CtoT_Process4<T>();
#endif

  using t_vNaN = std::variant<T, std::string>;
  {
    static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_StoT,THROW_ERROR)>::ToVal),
                                  T(*)(const std::string&)>);
    static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_StoT,QUIET_NAN)>::ToVal),
                                  T(*)(const std::string&)>);
    static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_StoT,SIGNAL_NAN)>::ToVal),
                                  T(*)(const std::string&)>);
    static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, S2T_FORMAT_MACRO2(S2T_Format_std_StoT,VARIANT_NAN)>::ToVal),
                                  t_vNaN (*)(const std::string&)>);

    static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter::T2S_Format_StreamDecimalPrecision<T>>::ToStr),
                                  std::string(*)(const T&)>);
    static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaN, converter::T2S_Format_StreamDecimalPrecision<T>>::ToStr),
                                  std::string(*)(const t_vNaN&)>);
    static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter::T2S_Format_floating_StreamCombine<T>>::ToStr),
                                  std::string(*)(const T&)>);
    static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaN, converter::T2S_Format_floating_StreamCombine<T>>::ToStr),
                                  std::string(*)(const t_vNaN&)>);
#ifdef ENABLE_STD_TtoS
    static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter::T2S_Format_std_TtoS>::ToStr),
                                  std::string(*)(const T&)>);
    static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaN, converter::T2S_Format_std_TtoS>::ToStr),
                                  std::string(*)(const t_vNaN&)>);
#endif
  }

  Format_StreamAsIs_Process4<T>();
  Format_StreamUseClassicLocale_Process4<T>();
  Format_StreamUserLocale_Process4<T>();
}

template<typename T>
void checkFunctionInstance_char()
{
  /*
   * S2T : For Integer types , following params are not applicable
   *   FORMAT_TEMPLATE ->   S2T_Format_std_StoT<>
   *   PROCESS         ->   SIGNAL_NAN, QUIET_NAN
   * 
   * T2S : For Integer types , following params are not applicable
   *   FORMAT_TEMPLATE ->   T2S_Format_std_TtoS, T2S_Format_StreamDecimalPrecision<>,
   *                        T2S_Format_floating_StreamCombine<>
  **/

  Format_WorkAround_Process2<T>();
  Format_StreamAsIs_Process2<T>();
  Format_StreamUseClassicLocale_Process2<T>();
  Format_StreamUserLocale_Process2<T>();
}

template<typename T>
void checkFunctionInstance_bool()
{
  /*
   * S2T : For Integer types , following params are not applicable
   *   FORMAT_TEMPLATE ->   S2T_Format_std_StoT<>
   *   PROCESS         ->   SIGNAL_NAN, QUIET_NAN
   * 
   * T2S : For Integer types , following params are not applicable
   *   FORMAT_TEMPLATE ->   T2S_Format_std_TtoS, T2S_Format_StreamDecimalPrecision<>,
   *                        T2S_Format_floating_StreamCombine<>
  **/

  Format_WorkAround_Process2<T>();
  Format_StreamAsIs_Process2<T>();
  Format_StreamUseClassicLocale_Process2<T>();
  Format_StreamUserLocale_Process2<T>();
}

template<typename T, const char* formatDATE = converter::defYMDfmt>
void checkFunctionInstance_date()
{
  using t_vNaD = std::variant<T, std::string>;
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, converter::S2T_Format_StreamYMD<formatDATE, converter::FailureS2Tprocess::THROW_ERROR>>::ToVal),
                                T(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<T, converter::S2T_Format_StreamYMD<formatDATE, converter::FailureS2Tprocess::VARIANT_NAN>>::ToVal),
                                t_vNaD (*)(const std::string&)>);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<T, converter::T2S_Format_StreamYMD<formatDATE>>::ToStr),
                                std::string(*)(const T&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaD, converter::T2S_Format_StreamYMD<formatDATE>>::ToStr),
                                std::string(*)(const t_vNaD&)>);
}

template<const char* formatDATE>
void checkFunctionInstance_formatDate()
{
  using t_tNaD = converter::format_year_month_day<formatDATE, converter::FailureS2Tprocess::THROW_ERROR>;

  using t_vNaD = std::variant< converter::format_year_month_day<formatDATE, converter::FailureS2Tprocess::VARIANT_NAN>,
                               std::string>;

  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<t_tNaD,
                                                                    converter::S2T_Format_StreamFormatYMD <formatDATE, converter::FailureS2Tprocess::THROW_ERROR>>::ToVal),
                                t_tNaD(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<t_vNaD,
                                                                    converter::S2T_Format_StreamFormatYMD <formatDATE, converter::FailureS2Tprocess::VARIANT_NAN>>::ToVal),
                                t_vNaD(*)(const std::string&)>);

  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_tNaD,
                                                                    converter::T2S_Format_StreamFormatYMD <formatDATE>>::ToStr),
                                std::string(*)(const t_tNaD&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<t_vNaD,
                                                                    converter::T2S_Format_StreamFormatYMD<formatDATE>>::ToStr),
                                std::string(*)(const t_vNaD&)>);
}


int main()
{
  // check for all S2T_FORMAT and all T2S_FORMAT types
  checkFormatInstance_Integer<short>();
  checkFormatInstance_Integer<int>();
  checkFormatInstance_Integer<long>();
  checkFormatInstance_Integer<long long>();
  checkFormatInstance_Integer<unsigned short>();
  checkFormatInstance_Integer<unsigned int>();
  checkFormatInstance_Integer<unsigned long>();
  checkFormatInstance_Integer<unsigned long long>();
  checkFormatInstance_Float<float>();
  checkFormatInstance_Float<double>();
  checkFormatInstance_Float<long double>();
  //checkFormatInstance_General<std::string>();
  static_assert(std::is_same_v<std::string, typename converter::S2T_Format_WorkAround<std::string, converter::FailureS2Tprocess::THROW_ERROR>::return_type>);
  static_assert(std::is_same_v<specializedTypes::ci_string, typename converter::S2T_Format_WorkAround<specializedTypes::ci_string, converter::FailureS2Tprocess::THROW_ERROR>::return_type>);
  checkFormatInstance_General<char>();
  checkFormatInstance_General<signed char>();
  checkFormatInstance_General<unsigned char>();
  checkFormatInstance_General<wchar_t>();
  checkFormatInstance_General<char8_t>();
  checkFormatInstance_General<char16_t>();
  checkFormatInstance_General<char32_t>();
  checkFormatInstance_General<bool>();
  checkFormatInstance_General<std::chrono::year_month_day>();


  // check for function signature
  checkFunctionInstance_Integer<short>();
  checkFunctionInstance_Integer<int>();
  checkFunctionInstance_Integer<long>();
  checkFunctionInstance_Integer<long long>();
  checkFunctionInstance_Integer<unsigned short>();
  checkFunctionInstance_Integer<unsigned int>();
  checkFunctionInstance_Integer<unsigned long>();
  checkFunctionInstance_Integer<unsigned long long>();
  checkFunctionInstance_Float<float>();
  checkFunctionInstance_Float<double>();
  checkFunctionInstance_Float<long double>();
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<std::string, converter::S2T_Format_WorkAround<std::string, converter::FailureS2Tprocess::THROW_ERROR>>::ToVal),
                                std::string(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<std::string, converter::T2S_Format_WorkAround>::ToStr),
                                std::string(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromStr<specializedTypes::ci_string, converter::S2T_Format_WorkAround<specializedTypes::ci_string, converter::FailureS2Tprocess::THROW_ERROR>>::ToVal),
                                specializedTypes::ci_string(*)(const std::string&)>);
  static_assert(std::is_same_v< decltype(&converter::ConvertFromVal<specializedTypes::ci_string, converter::T2S_Format_WorkAround>::ToStr),
                                std::string(*)(const specializedTypes::ci_string&)>);
  checkFunctionInstance_char<char>();
  checkFunctionInstance_char<signed char>();
  checkFunctionInstance_char<unsigned char>();
  checkFunctionInstance_char<wchar_t>();
  checkFunctionInstance_char<char8_t>();
  checkFunctionInstance_char<char16_t>();
  checkFunctionInstance_char<char32_t>();
  checkFunctionInstance_bool<bool>();
  checkFunctionInstance_date<std::chrono::year_month_day>();
  checkFunctionInstance_date<std::chrono::year_month_day, converter::dbY_fmt>();
  checkFunctionInstance_formatDate<converter::defYMDfmt>();
  checkFunctionInstance_formatDate<converter::dbY_fmt>();

  /**
   *   In order to select right conversion algo, templates
   *   'S2T_Format_*', 'T2S_Format_*', 'OnError' needs to be type-specialized as well
   *   along with type-specialization for 'ConvertFromVal' and 'ConvertFromStr'.
   *
   *  Next check UID  in 'testDefaultTemplateInstantiation.cpp'
  */


  int rv = 0;


  return rv;
}
