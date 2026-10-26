
// test default conversions for inbuilt-types


#include <iostream>

#include <converter/converter.h>
//#include <specializedTypes/date.h>
#include <specializedTypes/case_insensitive_string.h>

#include "unittest.h"
#include "utilities.h"

using namespace converter;

template <typename T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER >
void conversionEqualCheckS2T_C_E(const T& val, const std::string& vStr)
{
  if constexpr ( ERR_HANDLER != Str2TnErrorHandler::VARIANT_VAL )
  {
    T cVal = ConvertFromStr<T , CONV_PROCESS , ERR_HANDLER>::ToVal(vStr);
    unittest::ExpectEqual(T,           cVal, val);
  } else {
    auto cVal = ConvertFromStr<T , CONV_PROCESS , ERR_HANDLER>::ToVal(vStr);
    unittest::ExpectEqual(size_t,      cVal.index(), 0);
    unittest::ExpectEqual(T,           std::get<0>(cVal), val);
  }
}

template <typename T, Str2TnConversionProcess CONV_PROCESS, size_t IDX_ERR_HANDLER >
void conversionEqualCheckS2T_iC_iE(const T& val, const std::string& vStr)
{
  if constexpr ( IDX_ERR_HANDLER  <  DefaultStr2Tn<T>::validErrorHandlers().size() )
  {
    constexpr Str2TnErrorHandler ERR_HANDLER =
            DefaultStr2Tn<T>::validErrorHandlers()[IDX_ERR_HANDLER];
    conversionEqualCheckS2T_C_E<T,  CONV_PROCESS,  ERR_HANDLER >(val, vStr);

    conversionEqualCheckS2T_iC_iE<T,  CONV_PROCESS,
                                  (IDX_ERR_HANDLER+1) >(val, vStr);
  }
}

template <typename T, size_t IDX_CONV_PROCESS >
void conversionEqualCheckS2T_iC(const T& val, const std::string& vStr)
{
  if constexpr ( IDX_CONV_PROCESS  <  DefaultStr2Tn<T>::validConversionProcesses().size() )
  {
    constexpr Str2TnConversionProcess CONV_PROCESS =
            DefaultStr2Tn<T>::validConversionProcesses()[IDX_CONV_PROCESS];
    conversionEqualCheckS2T_iC_iE<T, CONV_PROCESS, 0 >(val, vStr);

    conversionEqualCheckS2T_iC   <T, (IDX_CONV_PROCESS+1) >(val, vStr);
  }
}









template <typename T, size_t IDX_CONV_PROCESS>
void conversionEqualCheckT2S_iC(const T& val, const std::string& vStr)
{
  if constexpr ( IDX_CONV_PROCESS < DefaultTn2Str<T>::validConversionProcesses().size() )
  {
    constexpr Tn2StrConversionProcess CONV_PROCESS =
	        DefaultTn2Str<T>::validConversionProcesses()[IDX_CONV_PROCESS];
    std::string sVal = ConvertFromVal<T , CONV_PROCESS>::ToStr(val);
    unittest::ExpectEqual(std::string, sVal, vStr);

    conversionEqualCheckT2S_iC  <T, (IDX_CONV_PROCESS+1) >(val, vStr);
  }
}

template <typename T>
void conversionEqualCheck_all(const T& val, const std::string& vStr)
{
  std::cout << std::endl;
  std::cout << "S2T : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultStr2Tn<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  std::cout << "S2T : ERR_HANDLER for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validErrorHandlers().size() ; ++i) std::cout << DefaultStr2Tn<T>::validErrorHandlers()[i] << ", ";
  std::cout << std::endl;
  conversionEqualCheckS2T_iC<T, 0 >(val, vStr);

  std::cout << "T2S : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultTn2Str<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultTn2Str<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  conversionEqualCheckT2S_iC<T, 0 >(val, vStr);
}


int main()
{
  int rv = 0;
  try {
    conversionEqualCheck_all<short>(-10, "-10");
    conversionEqualCheck_all<int>  (-20, "-20");
    conversionEqualCheck_all<long> (-30, "-30");
    conversionEqualCheck_all<long long>(-40, "-40");

    conversionEqualCheck_all<unsigned short>(10, "10");
    conversionEqualCheck_all<unsigned int>  (20, "20");
    conversionEqualCheck_all<unsigned long> (30, "30");
    conversionEqualCheck_all<unsigned long long>(40, "40");

    conversionEqualCheck_all<float> (-11, "-11");
    conversionEqualCheck_all<double>(-21, "-21");
    conversionEqualCheck_all<long double>(-31, "-31");

    conversionEqualCheck_all<float> (11, "11");
    conversionEqualCheck_all<double>(21, "21");
    conversionEqualCheck_all<long double>(31, "31");

    conversionEqualCheck_all<bool>(true,  "1");
    conversionEqualCheck_all<bool>(false, "0");


    conversionEqualCheck_all<char>('a',  "a");
    conversionEqualCheck_all<signed char>('b', "b");
    conversionEqualCheck_all<unsigned char>('c', "c");
    conversionEqualCheck_all<wchar_t>('d',  "d"); /// MSVC Warning
    conversionEqualCheck_all<char8_t>('e',  "e");
    conversionEqualCheck_all<char16_t>('f',  "f");
    conversionEqualCheck_all<char32_t>('g',  "g");

    conversionEqualCheck_all<std::string>("test std::string",  "test std::string");

    specializedTypes::ci_string    caps{"SHIV SHANKAR"};
    specializedTypes::ci_string    lowr{"shiv shankar"};
    unittest::ExpectTrue(caps.compare(lowr) == 0);
    conversionEqualCheck_all<specializedTypes::ci_string>("SHIV-SHANKAR",  "SHIV-SHANKAR");
    conversionEqualCheck_all<specializedTypes::ci_string>("shiv.shankar",  "shiv.shankar");



    conversionEqualCheck_all<std::chrono::year_month_day>(
            std::chrono::year_month_day( std::chrono::year(2023),
                                         std::chrono::month(8),
                                         std::chrono::day(15)
                                       ),
            "2023-08-15");
/*  TODO
    using t_fmtdbY = specializedTypes::format_year_month_day<dbY_fmt, FailureS2Tprocess::THROW_ERROR>;
    conversionEqualCheck_all<t_fmtdbY>(
            t_fmtdbY( std::chrono::year(2023),
                      std::chrono::month(8),
                      std::chrono::day(15)
                    ),
            "15-Aug-2023");
    using t_fmtYMD = specializedTypes::format_year_month_day<defYMDfmt, FailureS2Tprocess::THROW_ERROR>;
    conversionEqualCheck_all<t_fmtYMD>(
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
