
// test conversions for 'Date'


#include <iostream>

#include <converter/converter.h>

#include "unittest.h"

template <typename T>
void conversionEqualCheck(const T& val, const std::string& vStr)
{
  std::cout << "######  date-conversion testing for date=" << vStr << std::endl;
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
 
    std::cout << "######  date-conversion testing for date=[1900-02-29]" << std::endl;
    std::chrono::year_month_day invalidDate1( std::chrono::year(1900),
                                              std::chrono::month(2),
                                              std::chrono::day(29) );  // NOT leap year (every 100 years)
    // The below checks are to ascertain that 'std::chrono::*' behavior is same across different OS's
    unittest::ExpectEqual(bool, invalidDate1.ok(), false);
    unittest::ExpectEqual(bool, invalidDate1.year().ok(), true);
    unittest::ExpectEqual(bool, invalidDate1.month().ok(), true);
    unittest::ExpectEqual(bool, invalidDate1.day().ok(), true);
    unittest::ExpectEqual(int, int(invalidDate1.year()), 1900);
    unittest::ExpectEqual(unsigned, unsigned(invalidDate1.month()), 2);
    unittest::ExpectEqual(unsigned, unsigned(invalidDate1.day()), 29);
    std::chrono::year_month_day invalidConversionDate1 =
                converter::ConvertFromStr<std::chrono::year_month_day>::ToVal("1900-02-29");
    unittest::ExpectEqual(bool, invalidConversionDate1.ok(), false);
    unittest::ExpectEqual(bool, invalidConversionDate1.year().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate1.month().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate1.day().ok(), true);
    unittest::ExpectEqual(int, int(invalidConversionDate1.year()), 1900);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate1.month()), 2);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate1.day()), 29);
    unittest::ExpectEqual(std::string,
                          converter::ConvertFromVal<std::chrono::year_month_day>::ToStr(invalidDate1),
                          "1900-02-29");  // NOT leap year (every 100 years)


    std::cout << "######  date-conversion testing for date=[2023-1-32]" << std::endl;
    std::chrono::year_month_day invalidDate2( std::chrono::year(2023),
                                              std::chrono::month(1),
                                              std::chrono::day(32) );  // invalid date
    // The below checks are to ascertain that 'std::chrono::*' behavior is same across different OS's
    unittest::ExpectEqual(bool, invalidDate2.ok(), false);
    unittest::ExpectEqual(bool, invalidDate2.year().ok(), true);
    unittest::ExpectEqual(bool, invalidDate2.month().ok(), true);
    unittest::ExpectEqual(bool, invalidDate2.day().ok(), false);  // is False 32
    unittest::ExpectEqual(int, int(invalidDate2.year()), 2023);
    unittest::ExpectEqual(unsigned, unsigned(invalidDate2.month()), 1);
    unittest::ExpectEqual(unsigned, unsigned(invalidDate2.day()), 32);
    std::chrono::year_month_day invalidConversionDate2 =
                converter::ConvertFromStr<std::chrono::year_month_day>::ToVal("2023-1-32");
    unittest::ExpectEqual(bool, invalidConversionDate2.ok(), false);
    unittest::ExpectEqual(bool, invalidConversionDate2.year().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate2.month().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate2.day().ok(), false); // is False 32
    unittest::ExpectEqual(int, int(invalidConversionDate2.year()), 2023);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate2.month()), 1);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate2.day()), 32);
    unittest::ExpectEqual(std::string,
                          converter::ConvertFromVal<std::chrono::year_month_day>::ToStr(invalidDate2),
                          "2023-01-32");  // NOT leap year (every 100 years)

  } catch (const std::exception& ex) {
    std::cout << "Unexpected exception in testDateConversion: " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}