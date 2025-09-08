
// test conversions for 'Date'


#include <iostream>
#include <sstream>

#include <converter/converter.h>
#include <specializedTypes/date.h>

#include "unittest.h"

using t_fmtYMD = specializedTypes::format_year_month_day<converter::defYMDfmt, converter::FailureS2Tprocess::THROW_ERROR>;

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
    [[maybe_unused]]t_fmtYMD emptyDate;

    conversionEqualCheck<t_fmtYMD>(
            t_fmtYMD( std::chrono::year(2023),
                      std::chrono::month(2),
                      std::chrono::day(28)
                    ),
            "2023-02-28"); // NOT leap year

    conversionEqualCheck<t_fmtYMD>(
            t_fmtYMD( std::chrono::year(2020),
                      std::chrono::month(2),
                      std::chrono::day(29)
                    ),
            "2020-02-29"); // leap year (every 4 years)

    conversionEqualCheck<t_fmtYMD>(
            t_fmtYMD( std::chrono::year(2000),
                      std::chrono::month(2),
                      std::chrono::day(29)
                    ),
            "2000-02-29"); // leap year (every 400 years)
 
    std::cout << "######  date-conversion testing for date=[1900-02-29]" << std::endl;
    t_fmtYMD invalidDate1( std::chrono::year(1900),
                           std::chrono::month(2),
                           std::chrono::day(29) );  // NOT leap year (every 100 years)
    // The below checks are to ascertain that 'std::chrono::*' behavior is same across different OS's
    unittest::ExpectEqual(bool, invalidDate1.getYMD().ok(), false);
    unittest::ExpectEqual(bool, invalidDate1.getYMD().year().ok(), true);
    unittest::ExpectEqual(bool, invalidDate1.getYMD().month().ok(), true);
    unittest::ExpectEqual(bool, invalidDate1.getYMD().day().ok(), true);
    unittest::ExpectEqual(int, int(invalidDate1.getYMD().year()), 1900);
    unittest::ExpectEqual(unsigned, unsigned(invalidDate1.getYMD().month()), 2);
    unittest::ExpectEqual(unsigned, unsigned(invalidDate1.getYMD().day()), 29);
    t_fmtYMD invalidConversionDate1 =
                converter::ConvertFromStr<t_fmtYMD>::ToVal("1900-02-29");
    unittest::ExpectEqual(bool, invalidConversionDate1.getYMD().ok(), false);
    unittest::ExpectEqual(bool, invalidConversionDate1.getYMD().year().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate1.getYMD().month().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate1.getYMD().day().ok(), true);
    unittest::ExpectEqual(int, int(invalidConversionDate1.getYMD().year()), 1900);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate1.getYMD().month()), 2);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate1.getYMD().day()), 29);
    unittest::ExpectEqual(std::string,
                          converter::ConvertFromVal<t_fmtYMD>::ToStr(invalidDate1),
                          "1900-02-29");  // NOT leap year (every 100 years)


    std::cout << "######  date-conversion testing for date=[2023-01-32]" << std::endl;
    t_fmtYMD invalidDate2( std::chrono::year(2023),
                           std::chrono::month(1),
                           std::chrono::day(32) );  // invalid date
    // The below checks are to ascertain that 'std::chrono::*' behavior is same across different OS's
    unittest::ExpectEqual(bool, invalidDate2.getYMD().ok(), false);
    unittest::ExpectEqual(bool, invalidDate2.getYMD().year().ok(), true);
    unittest::ExpectEqual(bool, invalidDate2.getYMD().month().ok(), true);
    unittest::ExpectEqual(bool, invalidDate2.getYMD().day().ok(), false);  // is False 32
    unittest::ExpectEqual(int, int(invalidDate2.getYMD().year()), 2023);
    unittest::ExpectEqual(unsigned, unsigned(invalidDate2.getYMD().month()), 1);
    unittest::ExpectEqual(unsigned, unsigned(invalidDate2.getYMD().day()), 32);
    t_fmtYMD invalidConversionDate2 =
                converter::ConvertFromStr<t_fmtYMD>::ToVal("2023-01-32");
    unittest::ExpectEqual(bool, invalidConversionDate2.getYMD().ok(), false);
    unittest::ExpectEqual(bool, invalidConversionDate2.getYMD().year().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate2.getYMD().month().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate2.getYMD().day().ok(), false); // is False 32
    unittest::ExpectEqual(int, int(invalidConversionDate2.getYMD().year()), 2023);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate2.getYMD().month()), 1);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate2.getYMD().day()), 32);
    unittest::ExpectEqual(std::string,
                          converter::ConvertFromVal<t_fmtYMD>::ToStr(invalidDate2),
                          "2023-01-32");  // NOT leap year (every 100 years)


    t_fmtYMD  chkD1( std::chrono::year(2024), std::chrono::month(1), std::chrono::day(12));
    unittest::ExpectEqual(std::string, chkD1.toStr(), "2024-01-12");

    t_fmtYMD  chkD2;
    chkD2.fromStr("2024-01-12");
    unittest::ExpectEqual(t_fmtYMD,    chkD1,         chkD2);

    std::ostringstream os;
    os << chkD1;
    unittest::ExpectEqual(std::string, os.str(), "2024-01-12");

    std::istringstream is("2024-01-12");
    t_fmtYMD  chkD3;
    is >> chkD3;
    unittest::ExpectEqual(t_fmtYMD,    chkD1,         chkD3);

    unittest::ExpectTrue(chkD1 == chkD2);
#if  USE_THREE_WAY_COMPARISON  ==   e_ENABLE_FEATURE
    unittest::ExpectTrue((chkD1 <=> chkD2) == 0);
#endif

  } catch (const std::exception& ex) {
    std::cout << "Unexpected exception in testDateYMD_format_YMD: " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
