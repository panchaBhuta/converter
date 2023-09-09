
// test precison during conversions between string and floating-point-types


#include <iostream>
#include <cmath>

#include <converter/converter.h>

#include "unittest.h"
#include "utilities.h"

/*
  refer   https://en.cppreference.com/w/cpp/types/numeric_limits/digits10
*/

// https://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
template<converter::c_floating_point T>
void checkConversionTemplateInstances()
{
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<T>::type,
                                        converter::S2T_Format_std_CtoT<T, converter::FailureS2Tprocess::SIGNAL_NAN>>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<T>::type,
                                        converter::T2S_Format_std_TtoC>);
  static_assert(converter::ConvertFromStr<T>::template_uid ==  103);
  static_assert(converter::ConvertFromVal<T>::template_uid == -103);
}

const unsigned indexOS = 2;
#elif defined(__APPLE__) && defined(__MACH__)
template<converter::c_floating_point T>
void checkConversionTemplateInstances()
{
    static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<T>::type,
                                          converter::S2T_Format_std_StoT<T, converter::FailureS2Tprocess::SIGNAL_NAN>>);
    static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<T>::type,
                                          converter::T2S_Format_StreamDecimalPrecision<T>>);
    static_assert(converter::ConvertFromStr<T>::template_uid ==  3);
    static_assert(converter::ConvertFromVal<T>::template_uid == -1);
}

    // macOS does not support 'std::from_chars()' and
    // 'std::to_chars()'. The fall back functions
    // induces variations in results when compared to other OS's.
const unsigned indexOS = 1;
#else  // ubuntu and other OS's
template<converter::c_floating_point T>
void checkConversionTemplateInstances()
{
    static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<T>::type,
                                          converter::S2T_Format_std_CtoT<T, converter::FailureS2Tprocess::SIGNAL_NAN>>);
    static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<T>::type,
                                          converter::T2S_Format_std_TtoC>);
    static_assert(converter::ConvertFromStr<T>::template_uid ==  103);
    static_assert(converter::ConvertFromVal<T>::template_uid == -103);
}

const unsigned indexOS = 0;
#endif


int main()
{
  checkConversionTemplateInstances<float>();
  checkConversionTemplateInstances<double>();
  checkConversionTemplateInstances<long double>();

  int rv = 0;
  try {

    std::string expected_float_8d589973ep9[] = { "8589973504",
                                                 "8.5899735e+09",  // macOS
                                                 "8589973504", };
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-1",
                 "8.589973e+9", 8.589973e9f, expected_float_8d589973ep9[indexOS]);

    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-2",
                 "8.589973e+9", 8.589973e9, "8589973000");
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-3",
                 "8.589973e+9", 8.589973e9L, "8589973000");

    std::string expected_float_1d123456789[] = { "1.1234568",
                                                 "1.12345684",   // macOS
                                                 "1.1234568" };
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-4",
                 "1.123456789", 1.123456789f, expected_float_1d123456789[indexOS]);  // 6 digits
    std::string expected_double_1d1234567890123456789[] = { "1.1234567890123457",
                                                            "1.12345678901234569",   // macOS
                                                            "1.1234567890123457" };
    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-5",
                 "1.1234567890123456789", 1.1234567890123456789,
                 expected_double_1d1234567890123456789[indexOS]);   // 15 digits
    std::string expected_longDouble_1d123456789012345678901[] = { "1.1234567890123456789",
                                                                  "1.12345678901234567889",
                                                                  "1.1234567890123457" };  // Windows
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-6",
                 "1.123456789012345678901", 1.123456789012345678901L,
                 expected_longDouble_1d123456789012345678901[indexOS]);  //18 digits


    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-7",
                 "9007199254740993", 9007199254740993.0, "9007199254740992");
                                                   //    "9007199254740993"
    std::string expected_longDouble_9007199254740993[] = { "9007199254740993",
                                                           "9007199254740993",
                                                           "9007199254740992" }; // Windows
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-8",
                 "9007199254740993", 9007199254740993L, expected_longDouble_9007199254740993[indexOS]);


    std::string expected_float_3d3123[] = { "3.3123",
                                            "3.31229997",  // macOS
                                            "3.3123", };
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-9",
                 "3.3123", 3.3123f, expected_float_3d3123[indexOS]);

    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-10",
                 "3.3123412e+38", 3.3123412E38f, "3.3123412e+38");

    std::string expected_float_3d3123412en38[] = { "3.312341e-38",
                                                   "3.31234111e-38",  // macOS
                                                   "3.312341e-38", };
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-11",
                 "3.3123412e-38", 3.3123412E-38f, expected_float_3d3123412en38[indexOS]);

    std::string expected_double_4d3123412en38[] = { "4.3123412e-38",
                                                    "4.31234119999999987e-38",  // macOS
                                                    "4.3123412e-38", };
    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-12",
                 "4.3123412e-38", 4.3123412E-38, expected_double_4d3123412en38[indexOS]);

    std::string expected_longdouble_5d3123412en38[] = { "5.3123412e-38",
                                                        "5.3123412000000000001e-38", // macOS
                                                        "5.3123412e-38", };
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-13",
                 "5.3123412e-38", 5.3123412E-38L, expected_longdouble_5d3123412en38[indexOS]);

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}