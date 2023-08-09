
// test default conversions for inbuilt-types


#include <iostream>
#include <cmath>

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
    unittest::ExpectTrue(std::isnan(converter::ConvertFromStr_fNaN<float>::ToVal("rc")));
    unittest::ExpectTrue(std::isnan(converter::ConvertFromStr_fNaN<double>::ToVal("ab")));
    unittest::ExpectTrue(std::isnan(converter::ConvertFromStr_fNaN<long double>::ToVal("xy")));

    unittest::ExpectTrue(std::isnan(converter::ConvertFromStr_fNaN<float>::ToVal("1.2x1")));
    unittest::ExpectTrue(std::isnan(converter::ConvertFromStr_fNaN<double>::ToVal("2.3.4")));
    unittest::ExpectTrue(std::isnan(converter::ConvertFromStr_fNaN<long double>::ToVal("3r.422123445")));


    conversionEqualCheck<float> (-11, "-11");
    conversionEqualCheck<double>(-21, "-21");
    conversionEqualCheck<float> (11, "11");
    conversionEqualCheck<double>(21, "21");

  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}