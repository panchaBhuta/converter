
// test default conversions for inbuilt-types


#include <iostream>

#include <converter/converter.h>

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
#if USE_CLANG_STRING_WORKS_1 == _e_ENABLE_FEATURE_ || USE_CLANG_STRING_WORKAROUND_2 == _e_ENABLE_FEATURE_
    conversionEqualCheck<char8_t>('e',  "e");
    conversionEqualCheck<char16_t>('f',  "f");
    conversionEqualCheck<char32_t>('g',  "g");
#endif


    conversionEqualCheck<std::chrono::year_month_day>(
            std::chrono::year_month_day( std::chrono::year(2023),
                                         std::chrono::month(8),
                                         std::chrono::day(15)
                                       ),
            "2023-08-15");


  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}