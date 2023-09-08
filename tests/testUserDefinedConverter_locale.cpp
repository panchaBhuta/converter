
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


template<typename T>
using convertS2T_stream =
    converter::ConvertFromStr<T,
                              converter::S2T_Format_StreamAsIs<T,
                                                               converter::FailureS2Tprocess::THROW_ERROR,
                                                               char>
                             >;



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
    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testUserDefinedConverter_locale-1",
                 "8,589973e+9", 8.589973e9f, "8,5899735e+09", std::numeric_limits<float>::digits10, ',');
                                          // "8,589973e+9"
    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>, ConvertFromVal_loc<double>>("testUserDefinedConverter_locale-2",
                 "8,589973e+9", 8.589973e9, "8.589.973.000", std::numeric_limits<double>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<long double, ConvertFromStr_loc<long double>, ConvertFromVal_loc<long double>>("testUserDefinedConverter_locale-3",
                 "8,589973e+9", 8.589973e9L, "8.589.973.000", std::numeric_limits<long double>::digits10, ',');


    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testUserDefinedConverter_locale-4",
                 "1,123456789", 1.123456789f, "1,12345", std::numeric_limits<float>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>, ConvertFromVal_loc<double>>("testUserDefinedConverter_locale-5",
                 "2,1234567890123456789", 2.1234567890123456789, "2,12345678901234", std::numeric_limits<double>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<long double, ConvertFromStr_loc<long double>, ConvertFromVal_loc<long double>>("testUserDefinedConverter_locale-6",
                 "3,123456789012345678901", 3.123456789012345678901L, "3,12345678901234567", std::numeric_limits<long double>::digits10, ',');


    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>, ConvertFromVal_loc<double>>("testUserDefinedConverter_locale-7",
                 "9007199254740993", 9007199254740993.0, "9.007.199.254.740.992", std::numeric_limits<double>::digits10, ',');
                                                   //    "9.007.199.254.740.993"
    unsigned indexOS = 0;
// https://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
    indexOS = 1;
#endif
    std::string expected_longDouble_9007199254740993[] = { "9.007.199.254.740.993",
                                                           "9.007.199.254.740.992" // Windows
                                                         };
    checkRoundTripConversion_txt2Val2txt<long double, ConvertFromStr_loc<long double>, ConvertFromVal_loc<long double>>("testUserDefinedConverter_locale-8",
                 "9007199254740993", 9007199254740993L, expected_longDouble_9007199254740993[indexOS], std::numeric_limits<long double>::digits10, ',');


    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testUserDefinedConverter_locale-9",
                 "3,3123", 3.3123f, "3,31229", std::numeric_limits<float>::digits10, ',');
                              //    "3,3123"
    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testUserDefinedConverter_locale-10",
                 "3,3123412e+38", 3.3123412E38f, "3,3123412e+38", std::numeric_limits<float>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<float, ConvertFromStr_loc<float>, ConvertFromVal_loc<float>>("testUserDefinedConverter_locale-11",
                 "3,3123412e-38", 3.3123412E-38f, "3,3123412e-38", std::numeric_limits<float>::digits10, ',');
    checkRoundTripConversion_txt2Val2txt<double, ConvertFromStr_loc<double>, ConvertFromVal_loc<double>>("testUserDefinedConverter_locale-12",
                 "4,3123412e-38", 4.3123412E-38, "4,31234119999999", std::numeric_limits<double>::digits10, ',');
                                              // "4,3123412e-38"


    unittest::ExpectEqual(char, convertS2T_stream<char>::ToVal("C"), 'C');

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}