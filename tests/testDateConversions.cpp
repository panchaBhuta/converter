
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
    conversionEqualCheck<std::chrono::year_month_day>(
            std::chrono::year_month_day( std::chrono::year(2023),
                                         std::chrono::month(2),
                                         std::chrono::day(28)
                                       ),
            "2023-02-28"); // NOT leap year

    conversionEqualCheck<std::chrono::year_month_day>(
            std::chrono::year_month_day( std::chrono::year(2020),
                                         std::chrono::month(2),
                                         std::chrono::day(29)
                                       ),
            "2020-02-29"); // leap year (every 4 years)

    conversionEqualCheck<std::chrono::year_month_day>(
            std::chrono::year_month_day( std::chrono::year(2000),
                                         std::chrono::month(2),
                                         std::chrono::day(29)
                                       ),
            "2000-02-29"); // leap year (every 400 years)
 
    ExpectException(converter::ConvertFromStr<std::chrono::year_month_day>::ToVal("1900-02-29"),  // NOT leap year (every 100 years)
                    std::invalid_argument);
    std::chrono::year_month_day invalidDate(  std::chrono::year(1900),
                                              std::chrono::month(2),
                                              std::chrono::day(29) );  // NOT leap year (every 100 years)
    unittest::ExpectEqual(bool, invalidDate.ok(), false);
    ExpectException(converter::ConvertFromVal<std::chrono::year_month_day>::ToStr(invalidDate),  // NOT leap year (every 100 years)
                    std::invalid_argument);

  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}