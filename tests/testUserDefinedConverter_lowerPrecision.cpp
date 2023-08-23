
// test user-defined conversion for floating-point-types


#include <iostream>
#include <cmath>

#include <converter/converter.h>

#include "unittest.h"
#include "utilities.h"


template<converter::c_floating_point T>
constexpr int getLowerDecimalPrecision()
{
  if constexpr (std::is_same_v<T, float>)
    return FLT_DIG-1;
  else
  if constexpr (std::is_same_v<T, double>)
    return DBL_DIG-1;
  else
  if constexpr (std::is_same_v<T, long double>)
    return LDBL_DIG-1;
}
template<converter::c_floating_point T>
using T2S_Format_ldp = converter::T2S_Format_StreamDecimalPrecision<T, getLowerDecimalPrecision<T>() >;

template<converter::c_floating_point T>
using ConvertFromVal_lDP = converter::ConvertFromVal<T, T2S_Format_ldp<T> >;






int main()
{
  int rv = 0;
  try {
    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>, ConvertFromVal_lDP<float>>("testFloatingPointPrecision",
                 "8.589973e+9", 8.589973e9f, "8589973000", getLowerDecimalPrecision<float>());
    checkRoundTripConversion_txt2Val2txt<double, converter::ConvertFromStr<double>, ConvertFromVal_lDP<double>>("testFloatingPointPrecision",
                 "8.589973e+9", 8.589973e9, "8589973000", getLowerDecimalPrecision<double>());
    checkRoundTripConversion_txt2Val2txt<long double, converter::ConvertFromStr<long double>, ConvertFromVal_lDP<long double>>("testFloatingPointPrecision",
                 "8.589973e+9", 8.589973e9L, "8589973000", getLowerDecimalPrecision<long double>());


    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>, ConvertFromVal_lDP<float>>("testFloatingPointPrecision",
                 "1.123456789", 1.123456789f, "1.1235", getLowerDecimalPrecision<float>());   // 5 digits
    checkRoundTripConversion_txt2Val2txt<double, converter::ConvertFromStr<double>, ConvertFromVal_lDP<double>>("testFloatingPointPrecision",
                 "2.1234567890123456789", 2.1234567890123456789, "2.1234567890123", getLowerDecimalPrecision<double>());  // 14 digits
    checkRoundTripConversion_txt2Val2txt<long double, converter::ConvertFromStr<long double>, ConvertFromVal_lDP<long double>>("testFloatingPointPrecision",
                 "3.123456789012345678901", 3.123456789012345678901L, "3.1234567890123457", getLowerDecimalPrecision<long double>());  // 17 digits

    checkRoundTripConversion_txt2Val2txt<double, converter::ConvertFromStr<double>, ConvertFromVal_lDP<double>>("testFloatingPointPrecision",
                 "9007199254740993", 9007199254740993, "9007199254740993", getLowerDecimalPrecision<double>());
    checkRoundTripConversion_txt2Val2txt<long double, converter::ConvertFromStr<long double>, ConvertFromVal_lDP<long double>>("testFloatingPointPrecision",
                 "9007199254740993", 9007199254740993L, "9007199254740993", getLowerDecimalPrecision<long double>());


    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>, ConvertFromVal_lDP<float>>("testFloatingPointPrecision",
                 "3.3123", 3.3123f, "3.3123", getLowerDecimalPrecision<float>());
    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>, ConvertFromVal_lDP<float>>("testFloatingPointPrecision",
                 "3.3123412e+38", 3.3123412E38f, "3.3123412e+38", getLowerDecimalPrecision<float>());
    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>, ConvertFromVal_lDP<float>>("testFloatingPointPrecision",
                 "3.3123412e-38", 3.3123412E-38f, "3.3123412e-38", getLowerDecimalPrecision<float>());
    checkRoundTripConversion_txt2Val2txt<double, converter::ConvertFromStr<double>, ConvertFromVal_lDP<double>>("testFloatingPointPrecision",
                 "3.3123412e-38", 3.3123412E-38, "3.3123412e-38", getLowerDecimalPrecision<double>());

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}