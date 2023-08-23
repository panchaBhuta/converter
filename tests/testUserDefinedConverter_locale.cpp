
// test locale for conversions


#include <iostream>
#include <cmath>

#include <converter/converter.h>

#include "unittest.h"
#include "utilities.h"



/*
 * To run this test, locale 'de_DE' needs to be installed on the system.
 * on ubuntu steps are...
 * $ locale -a                                     # to check existing locale(s)
 * $ sudo apt-get install language-pack-de
 * $ locale -a                                     # check again
 * $ sudo locale-gen de_DE
 * $ sudo update-locale
 * 
 * refer ::: https://askubuntu.com/questions/76013/how-do-i-add-locale-to-ubuntu-server
 */


// string literal object with static storage duration
constexpr char de_Loc[] = "de_DE"; // uses comma (,) as decimal separator

template<typename T>
using deLocal_iss = converter::S2T_Format_StreamUserLocale<T, converter::FailureS2Tprocess::THROW_ERROR, char, de_Loc>;

template<converter::c_floating_point T>
using ConvertFromStr_loc = converter::ConvertFromStr<T, deLocal_iss<T> >;




template<typename T>
using deLocal_oss = converter::T2S_Format_StreamUserLocale<char, de_Loc>;

template<typename T>
using combine_deLocal_oss =
    converter::T2S_Format_floating_StreamCombine< T,
                                                  converter::T2S_Format_StreamDecimalPrecision<T>,
                                                  deLocal_oss<T>
                                                  >;


template<converter::c_floating_point T>
using ConvertFromVal_loc = converter::ConvertFromVal<T, combine_deLocal_oss<T> >;





int main()
{
  int rv = 0;

  //std::cout << "ConvertFromStr_loc<float>::template_uid=" << ConvertFromStr_loc<float>::template_uid << std::endl;
  //ConvertFromStr_loc<float>::instanceEvaluater();

  try
  {
    std::istringstream iss;
    deLocal_iss<float>::streamUpdate(iss);
  }
  catch (const std::exception& ex)
  {
    std::cout << "locale " << de_Loc << " not available (" << ex.what()
              << "), skipping test.\n";
    // pass test for systems without locale present. for ci testing, make.sh
    // ensures that the necessary locale is installed.
    return 0;
  }


  try {
    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testFloatingPointPrecision",
                 "8,589973e+9", 8.589973e9f, "8.589.973.000", std::numeric_limits<float>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>, ConvertFromVal_loc<double>>("testFloatingPointPrecision",
                 "8,589973e+9", 8.589973e9, "8.589.973.000", std::numeric_limits<double>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<long double, ConvertFromStr_loc<long double>, ConvertFromVal_loc<long double>>("testFloatingPointPrecision",
                 "8,589973e+9", 8.589973e9L, "8.589.973.000", std::numeric_limits<long double>::digits10, ',');


    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testFloatingPointPrecision",
                 "1,123456789", 1.123456789f, "1,12345", std::numeric_limits<float>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>, ConvertFromVal_loc<double>>("testFloatingPointPrecision",
                 "2,1234567890123456789", 2.1234567890123456789, "2,12345678901234", std::numeric_limits<double>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<long double, ConvertFromStr_loc<long double>, ConvertFromVal_loc<long double>>("testFloatingPointPrecision",
                 "3,123456789012345678901", 3.123456789012345678901L, "3,12345678901234567", std::numeric_limits<long double>::digits10, ',');


    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>, ConvertFromVal_loc<double>>("testFloatingPointPrecision",
                 "9007199254740993", 9007199254740993, "9.007.199.254.740.992", std::numeric_limits<double>::digits10, ',');
                                                 //    "9.007.199.254.740.993"
    checkRoundTripConversion_txt2Val2txt<long double, ConvertFromStr_loc<long double>, ConvertFromVal_loc<long double>>("testFloatingPointPrecision",
                 "9007199254740993", 9007199254740993L, "9.007.199.254.740.993", std::numeric_limits<long double>::digits10, ',');


    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testFloatingPointPrecision",
                 "3,3123", 3.3123f, "3,31229", std::numeric_limits<float>::digits10, ',');
                              //    "3,3123"
    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testFloatingPointPrecision",
                 "3,3123412e+38", 3.3123412E38f, "3,3123412e+38", std::numeric_limits<float>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testFloatingPointPrecision",
                 "3,3123412e-38", 3.3123412E-38f, "3,3123412e-38", std::numeric_limits<float>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>, ConvertFromVal_loc<double>>("testFloatingPointPrecision",
                 "4,3123412e-38", 4.3123412E-38, "4,31234119999999", std::numeric_limits<double>::digits10, ',');
                                              // "4,3123412e-38"
  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}