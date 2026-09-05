
// test default conversions for inbuilt-types


#include <iostream>

#include <converter/converter.h>
//#include <specializedTypes/date.h>
#include <specializedTypes/case_insensitive_string.h>

#include "unittest.h"
#include "utilities.h"

using namespace converter;

template <typename T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER >
void conversionEqualCheckS2T_3(const T& val, const std::string& vStr)
{
  if constexpr ( ERR_HANDLER != Str2TnErrorHandler::VARIANT_VAL )
  {
    unittest::ExpectEqual(T,           ConvertFromStr<T COMMA CONV_PROCESS COMMA ERR_HANDLER>::ToVal(vStr), val);
  } else {
    auto retVal = ConvertFromStr<T COMMA CONV_PROCESS COMMA ERR_HANDLER>::ToVal(vStr);
    unittest::ExpectEqual(size_t,      retVal.index(), 0);
    unittest::ExpectEqual(T,           std::get<0>(retVal), val);
  }
}

template <typename T, Str2TnConversionProcess CONV_PROCESS, size_t IDX_ERR_HANDLER >
void conversionEqualCheckS2T_2(const T& val, const std::string& vStr)
{
  if constexpr ( IDX_ERR_HANDLER  <  DefaultStr2Tn<T>::validErrorHandlers().size() )
  {
    conversionEqualCheckS2T_3<T,  CONV_PROCESS,
                                  DefaultStr2Tn<T>::validErrorHandlers()[IDX_ERR_HANDLER] >(val, vStr);

    conversionEqualCheckS2T_2<T,  CONV_PROCESS,
                                  (IDX_ERR_HANDLER+1) >(val, vStr);
  }
}

template <typename T, size_t IDX_CONV_PROCESS >
void conversionEqualCheckS2T(const T& val, const std::string& vStr)
{
  if constexpr ( IDX_CONV_PROCESS  <  DefaultStr2Tn<T>::validConversionProcesses().size() )
  {
    conversionEqualCheckS2T_2<T, DefaultStr2Tn<T>::validConversionProcesses()[IDX_CONV_PROCESS], 0 >(val, vStr);

    conversionEqualCheckS2T  <T, (IDX_CONV_PROCESS+1) >(val, vStr);
  }
}








template <typename T, Tn2StrConversionProcess CONV_PROCESS>
void conversionEqualCheckT2S_2(const T& val, const std::string& vStr)
{
  unittest::ExpectEqual(std::string, ConvertFromVal<T COMMA CONV_PROCESS>::ToStr(val), vStr);
}

template <typename T, size_t IDX_CONV_PROCESS>
void conversionEqualCheckT2S(const T& val, const std::string& vStr)
{
  if constexpr ( IDX_CONV_PROCESS < DefaultTn2Str<T>::validConversionProcesses().size() )
  {
    conversionEqualCheckT2S_2<T, DefaultTn2Str<T>::validConversionProcesses()[IDX_CONV_PROCESS] >(val, vStr);

    conversionEqualCheckT2S  <T, (IDX_CONV_PROCESS+1) >(val, vStr);
  }
}

template <typename T>
void conversionEqualCheck(const T& val, const std::string& vStr)
{
  std::cout << std::endl;
  std::cout << "S2T : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultStr2Tn<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  std::cout << "S2T : ERR_HANDLER for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validErrorHandlers().size() ; ++i) std::cout << DefaultStr2Tn<T>::validErrorHandlers()[i] << ", ";
  std::cout << std::endl;
  conversionEqualCheckS2T<T, 0 >(val, vStr);

  std::cout << "T2S : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultTn2Str<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultTn2Str<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  conversionEqualCheckT2S<T, 0 >(val, vStr);
}


int main()
{
  int rv = 0;
  try {
    conversionEqualCheck<short>(-10, "-10");
    conversionEqualCheck<int>  (-20, "-20");
    conversionEqualCheck<long> (-30, "-30");
    conversionEqualCheck<long long>(-40, "-40");

    conversionEqualCheck<unsigned short>(10, "10");
    conversionEqualCheck<unsigned int>  (20, "20");
    conversionEqualCheck<unsigned long> (30, "30");
    conversionEqualCheck<unsigned long long>(40, "40");

    conversionEqualCheck<float> (-11, "-11");
    conversionEqualCheck<double>(-21, "-21");
    conversionEqualCheck<long double>(-31, "-31");

    conversionEqualCheck<float> (11, "11");
    conversionEqualCheck<double>(21, "21");
    conversionEqualCheck<long double>(31, "31");

    conversionEqualCheck<bool>(true,  "1");
    conversionEqualCheck<bool>(false, "0");


    conversionEqualCheck<char>('a',  "a");
    conversionEqualCheck<signed char>('b', "b");
    conversionEqualCheck<unsigned char>('c', "c");
    conversionEqualCheck<wchar_t>('d',  "d"); /// MSVC Warning
#if TEST_HAS_CLANG_STRING_1 == e_ENABLE_FEATURE || TEST_WORKAROUND_CLANG_STRING_2 == e_ENABLE_FEATURE
    conversionEqualCheck<char8_t>('e',  "e");
    conversionEqualCheck<char16_t>('f',  "f");
    conversionEqualCheck<char32_t>('g',  "g");
#else
    #warning Possibly wrong build initialization. Either TEST_HAS_CLANG_STRING_1  or TEST_WORKAROUND_CLANG_STRING_2 should be enabled.
#endif

    conversionEqualCheck<std::string>("test std::string",  "test std::string");

    specializedTypes::ci_string    caps{"SHIV SHANKAR"};
    specializedTypes::ci_string    lowr{"shiv shankar"};
    unittest::ExpectTrue(caps.compare(lowr) == 0);
    conversionEqualCheck<specializedTypes::ci_string>("SHIV-SHANKAR",  "SHIV-SHANKAR");
    conversionEqualCheck<specializedTypes::ci_string>("shiv.shankar",  "shiv.shankar");



    conversionEqualCheck<std::chrono::year_month_day>(
            std::chrono::year_month_day( std::chrono::year(2023),
                                         std::chrono::month(8),
                                         std::chrono::day(15)
                                       ),
            "2023-08-15");
/*
    using t_fmtdbY = specializedTypes::format_year_month_day<dbY_fmt, FailureS2Tprocess::THROW_ERROR>;
    conversionEqualCheck<t_fmtdbY>(
            t_fmtdbY( std::chrono::year(2023),
                      std::chrono::month(8),
                      std::chrono::day(15)
                    ),
            "15-Aug-2023");
    using t_fmtYMD = specializedTypes::format_year_month_day<defYMDfmt, FailureS2Tprocess::THROW_ERROR>;
    conversionEqualCheck<t_fmtYMD>(
            t_fmtYMD( std::chrono::year(2023),
                      std::chrono::month(8),
                      std::chrono::day(15)
                    ),
            "2023-08-15");
*/
  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
