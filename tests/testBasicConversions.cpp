
// test default conversions for inbuilt-types


#include <iostream>

#include <converter/converter.h>
#include <converter/specializedTypes/date.h>
#include <converter/specializedTypes/case_insensitive_string.h>

#include "unittest.h"


template <typename T>
void conversionEqualCheck(const T& val, const std::string& vStr)
{
  unittest::ExpectEqual(T,           converter::ConvertFromStr<T>::ToVal(vStr), val);
  unittest::ExpectEqual(std::string, converter::ConvertFromVal<T>::ToStr(val), vStr);
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
    conversionEqualCheck<wchar_t>('d',  "d");
#if USE_CLANG_STRING_WORKS_1 == e_ENABLE_FEATURE || USE_CLANG_STRING_WORKAROUND_2 == e_ENABLE_FEATURE
    conversionEqualCheck<char8_t>('e',  "e");
    conversionEqualCheck<char16_t>('f',  "f");
    conversionEqualCheck<char32_t>('g',  "g");
#else
  #warning Possibly wrong build initialization. Either USE_CLANG_STRING_WORKS_1  or USE_CLANG_STRING_WORKAROUND_2 should be enabled.
#endif


    conversionEqualCheck<std::chrono::year_month_day>(
            std::chrono::year_month_day( std::chrono::year(2023),
                                         std::chrono::month(8),
                                         std::chrono::day(15)
                                       ),
            "2023-08-15");
    using t_fmtdbY = converter::format_year_month_day<converter::dbY_fmt, converter::FailureS2Tprocess::THROW_ERROR>;
    conversionEqualCheck<t_fmtdbY>(
            t_fmtdbY( std::chrono::year(2023),
                      std::chrono::month(8),
                      std::chrono::day(15)
                    ),
            "15-Aug-2023");
    using t_fmtYMD = converter::format_year_month_day<converter::defYMDfmt, converter::FailureS2Tprocess::THROW_ERROR>;
    conversionEqualCheck<t_fmtYMD>(
            t_fmtYMD( std::chrono::year(2023),
                      std::chrono::month(8),
                      std::chrono::day(15)
                    ),
            "2023-08-15");

    converter::ci_string    caps{"SHIV SHANKAR"};
    converter::ci_string    lowr{"shiv shankar"};
    unittest::ExpectTrue(caps.compare(lowr) == 0);
    unittest::ExpectEqual(std::string, converter::ConvertFromVal<converter::ci_string>::ToStr(caps), "SHIV SHANKAR");
    unittest::ExpectEqual(std::string, converter::ConvertFromVal<converter::ci_string>::ToStr(lowr), "shiv shankar");
    unittest::ExpectEqual(converter::ci_string, converter::ConvertFromStr<converter::ci_string>::ToVal("SHIV SHANKAR"), caps);
    unittest::ExpectEqual(converter::ci_string, converter::ConvertFromStr<converter::ci_string>::ToVal("shiv shankar"), lowr);

    /**
     * NOTE : Before adding conversion tests here, check tests for template-instantiation in
     *        'testAllTemplateInstantiation'.
     *
     *        Next check UID  in 'testDefaultTemplateInstantiation.cpp'
     *
     *        In order to select right conversion algo, templates
     *        'S2T_Format_*', 'T2S_Format_*', 'OnError' needs to be type-specialized as well
     *        along with type-specialization for 'ConvertFromVal' and 'ConvertFromStr'.
    */

  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}