
// test conversions for 'Date'


#include <iostream>

#include <converter/converter.h>

#include "unittest.h"


constexpr char dmY_fmt[] = "%d-%m-%Y";  // string literal object with static storage duration
namespace chr = std::chrono;

constexpr chr::year_month_day (*ToVal_dmY)(const std::string& str) =
                    &converter::ConvertFromStr< chr::year_month_day,
                                                converter::S2T_Format_StreamYMD< dmY_fmt >
                                              >::ToVal;

constexpr std::string (*FromStr_dmY)(const chr::year_month_day& val) =
                    &converter::ConvertFromVal< chr::year_month_day,
                                                converter::T2S_Format_StreamYMD< dmY_fmt >
                                              >::ToStr;


int main()
{
  int rv = 0;
  try {
    unittest::ExpectEqual(chr::year_month_day,
                          ToVal_dmY("27-12-2020"),
                          chr::year_month_day{chr::year{2020} COMMA chr::month{12} COMMA chr::day{27} });
    unittest::ExpectEqual(std::string,
                          FromStr_dmY( chr::year_month_day{chr::year{1980} COMMA chr::month{2} COMMA chr::day{28} } ),
                          "28-02-1980");


    unittest::ExpectEqual(chr::year_month_day,
                          converter::fromStr_dbY("27-Dec-2020"),
                          chr::year_month_day{chr::year{2020} COMMA chr::month{12} COMMA chr::day{27} });
    unittest::ExpectEqual(std::string,
                          converter::toStr_dbY( chr::year_month_day{chr::year{1980} COMMA chr::month{2} COMMA chr::day{28} } ),
                          "28-Feb-1980");
  } catch (const std::exception& ex) {
    std::cout << "Unexpected exception in testDateConversionFormat: " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
