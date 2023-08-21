
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
//???? isCombinedStream needed :: check

template<converter::c_floating_point T>
using ConvertFromStr_loc = converter::ConvertFromStr<T, deLocal_iss<T> >;




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
    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>>("testFloatingPointPrecision",
                 "8,589973e+9", 8.589973e9f, "8589973000");
    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>>("testFloatingPointPrecision",
                 "8,589973e+9", 8.589973e9, "8589973000");
    checkRoundTripConversion_txt2Val2txt<long double, ConvertFromStr_loc<long double>>("testFloatingPointPrecision",
                 "8,589973e+9", 8.589973e9L, "8589973000");


    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>>("testFloatingPointPrecision",
                 "1,123456789", 1.123456789f, "1.123456789");
    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>>("testFloatingPointPrecision",
                 "1,1234567890123456789", 1.1234567890123456789, "1.1234567890123456789");
    checkRoundTripConversion_txt2Val2txt<long double, ConvertFromStr_loc<long double>>("testFloatingPointPrecision",
                 "1,123456789012345678901", 1.123456789012345678901L, "1.123456789012345678901");


    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>>("testFloatingPointPrecision",
                 "9007199254740993", 9007199254740993, "9007199254740993");
    checkRoundTripConversion_txt2Val2txt<long double, ConvertFromStr_loc<long double>>("testFloatingPointPrecision",
                 "9007199254740993", 9007199254740993L, "9007199254740993");


    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>>("testFloatingPointPrecision",
                 "3,3123", 3.3123f, "3.3123");
    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>>("testFloatingPointPrecision",
                 "3,3123412e+38", 3.3123412E38f, "3.3123412e+38");
    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>>("testFloatingPointPrecision",
                 "3,3123412e-38", 3.3123412E-38f, "3.3123412e-38");
    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>>("testFloatingPointPrecision",
                 "3,3123412e-38", 3.3123412E-38, "3.3123412e-38");
  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}