
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
    converter::ci_string    cistr{"+ci_string"};
    unittest::ExpectEqual(converter::ci_string, cistr, "+Ci_STrING");
    unittest::ExpectTrue( cistr == "+Ci_STrING" );
    unittest::ExpectTrue( cistr != "+Ci_STrING___" );
    conversionEqualCheck(cistr, "+ci_string");
    std::string  str{"+string"};

    std::string  str_cistr = str + cistr;
    unittest::ExpectEqual(std::string, str_cistr, "+string+ci_string");

    converter::ci_string    cistr_str = cistr + str;
    unittest::ExpectEqual(converter::ci_string, cistr_str, "+ci_string+string");



    converter::ci_string    cistr2{"ci_string"};
    cistr2 += std::string{"+=string"};
    unittest::ExpectEqual(converter::ci_string, cistr2, "ci_string+=string");

    std::string  str3{"string"};
    str3 += converter::ci_string{"+=ci_string"};
    unittest::ExpectEqual(std::string, str3, "string+=ci_string");

  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}