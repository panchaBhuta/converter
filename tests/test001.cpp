
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
    conversionEqualCheck<int>  (-10, "-10");
    conversionEqualCheck<long> (-10, "-10");
    conversionEqualCheck<long long>(-10, "-10");

    conversionEqualCheck<unsigned short>(10, "10");
    conversionEqualCheck<unsigned int>  (10, "10");
    conversionEqualCheck<unsigned long> (10, "10");
    conversionEqualCheck<unsigned long long>(10, "10");


    conversionEqualCheck<bool>(true,  "1");
    conversionEqualCheck<bool>(false, "0");


    conversionEqualCheck<char>('a',  "a");
    conversionEqualCheck<signed char>('b', "b");
    conversionEqualCheck<unsigned char>('c', "c");
    conversionEqualCheck<wchar_t>('d',  "d");
    conversionEqualCheck<char8_t>('e',  "e");
    conversionEqualCheck<char16_t>('f',  "f");
    conversionEqualCheck<char32_t>('g',  "g");

  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}