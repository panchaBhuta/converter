
// test precison during conversions between string and floating-point-types


#include <iostream>
#include <cmath>

#include <converter/converter.h>

#include "unittest.h"
#include "utilities.h"

/*
  refer   https://en.cppreference.com/w/cpp/types/numeric_limits/digits10
*/

int main()
{
  int rv = 0;
  try {
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision",
                 "8.589973e+9", 8.589973e9f, "8589973504");
                                          // "8589973000"
    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision",
                 "8.589973e+9", 8.589973e9, "8589973000");
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision",
                 "8.589973e+9", 8.589973e9L, "8589973000");


    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision",
                 "1.123456789", 1.123456789f, "1.12345");  // 6 digits
                                          //  "1.12346"
    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision",
                 "1.1234567890123456789", 1.1234567890123456789, "1.12345678901234");   // 15 digits
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision",
                 "1.123456789012345678901", 1.123456789012345678901L, "1.12345678901234567");  //18 digits
                                                                  //  "1.12345678901234568"


    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision",
                 "9007199254740993", 9007199254740993, "9007199254740992");
                                                 //    "9007199254740993"
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision",
                 "9007199254740993", 9007199254740993L, "9007199254740993");


    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision",
                 "3.3123", 3.3123f, "3.3123");
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision",
                 "3.3123412e+38", 3.3123412E38f, "3.3123412e+38");
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision",
                 "3.3123412e-38", 3.3123412E-38f, "3.3123412e-38");
    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision",
                 "3.3123412e-38", 3.3123412E-38, "3.3123412e-38");

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}