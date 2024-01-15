
// test conversions for 'Date'


#include <iostream>
#include <sstream>

#include <converter/converter.h>
#include <converter/specializedTypes/date.h>

#include "unittest.h"

using t_fmtdbY = converter::format_year_month_day<converter::dbY_fmt, converter::FailureS2Tprocess::THROW_ERROR>;

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
    [[maybe_unused]]t_fmtdbY emptyDate;

    conversionEqualCheck<t_fmtdbY>(
            t_fmtdbY( std::chrono::year(2023),
                      std::chrono::month(2),
                      std::chrono::day(28)
                    ),
            "28-Feb-2023"); // NOT leap year

    conversionEqualCheck<t_fmtdbY>(
            t_fmtdbY( std::chrono::year(2020),
                      std::chrono::month(2),
                      std::chrono::day(29)
                    ),
            "29-Feb-2020"); // leap year (every 4 years)

    conversionEqualCheck<t_fmtdbY>(
            t_fmtdbY( std::chrono::year(2000),
                      std::chrono::month(2),
                      std::chrono::day(29)
                    ),
            "29-Feb-2000"); // leap year (every 400 years)
 
    std::cout << "######  date-conversion testing for date=[29-Feb-1900]" << std::endl;
    t_fmtdbY invalidDate1( std::chrono::year(1900),
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
    t_fmtdbY invalidConversionDate1 =
                converter::ConvertFromStr<t_fmtdbY>::ToVal("29-Feb-1900");
    unittest::ExpectEqual(bool, invalidConversionDate1.ok(), false);
    unittest::ExpectEqual(bool, invalidConversionDate1.year().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate1.month().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate1.day().ok(), true);
    unittest::ExpectEqual(int, int(invalidConversionDate1.year()), 1900);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate1.month()), 2);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate1.day()), 29);
    unittest::ExpectEqual(std::string,
                          converter::ConvertFromVal<t_fmtdbY>::ToStr(invalidDate1),
                          "29-Feb-1900");  // NOT leap year (every 100 years)


    std::cout << "######  date-conversion testing for date=[32-Jan-2023]" << std::endl;
    t_fmtdbY invalidDate2( std::chrono::year(2023),
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
    t_fmtdbY invalidConversionDate2 =
                converter::ConvertFromStr<t_fmtdbY>::ToVal("32-Jan-2023");
    unittest::ExpectEqual(bool, invalidConversionDate2.ok(), false);
    unittest::ExpectEqual(bool, invalidConversionDate2.year().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate2.month().ok(), true);
    unittest::ExpectEqual(bool, invalidConversionDate2.day().ok(), false); // is False 32
    unittest::ExpectEqual(int, int(invalidConversionDate2.year()), 2023);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate2.month()), 1);
    unittest::ExpectEqual(unsigned, unsigned(invalidConversionDate2.day()), 32);
    unittest::ExpectEqual(std::string,
                          converter::ConvertFromVal<t_fmtdbY>::ToStr(invalidDate2),
                          "32-Jan-2023");  // NOT leap year (every 100 years)


    t_fmtdbY  chkD1( std::chrono::year(2024), std::chrono::month(1), std::chrono::day(12));
    unittest::ExpectEqual(std::string, chkD1.toStr(), "12-Jan-2024");

    t_fmtdbY  chkD2;
    chkD2.fromStr("12-Jan-2024");
    unittest::ExpectEqual(t_fmtdbY,    chkD1,         chkD2);

    std::ostringstream os;
    os << chkD1;
    unittest::ExpectEqual(std::string, os.str(), "12-Jan-2024");

    std::istringstream is("12-Jan-2024");
    t_fmtdbY  chkD3;
    is >> chkD3;
    unittest::ExpectEqual(t_fmtdbY,    chkD1,         chkD3);

    unittest::ExpectTrue(chkD1 == chkD2);
    unittest::ExpectTrue((chkD1 <=> chkD2) == 0);


  } catch (const std::exception& ex) {
    std::cout << "Unexpected exception in testDateYMD_format_dbY: " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
