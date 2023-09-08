
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
    unsigned indexOS = 0;
#if defined(__APPLE__) && defined(__MACH__)
    indexOS = 1;
#endif
    std::string expected_float_8d589973ep9[] = { "8589973504",      // "8589973000"
                                                 "8.5899735e+09" }; // macOS does not support 'std::from_chars()' and
                                                                    // 'std::to_chars()'. The fall back functions
                                                                    // induces this variation.
    //checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision",
    //             "8.589973e+9", 8.589973e9f, expected_float_8d589973ep9[indexOS]);

    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-1",
                 "8.589973e+9", 8.589973e9, "8589973000");
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-2",
                 "8.589973e+9", 8.589973e9L, "8589973000");


    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-3",
                 "1.123456789", 1.123456789f, "1.12345");  // 6 digits
                                          //  "1.12346"
    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-4",
                 "1.1234567890123456789", 1.1234567890123456789, "1.12345678901234");   // 15 digits
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-5",
                 "1.123456789012345678901", 1.123456789012345678901L, "1.12345678901234567");  //18 digits
                                                                  //  "1.12345678901234568"


    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-6",
                 "9007199254740993", 9007199254740993.0, "9007199254740992");
                                                   //    "9007199254740993"
// https://web.archive.org/web/20191012035921/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
    indexOS = 1;
#else
    indexOS = 0;
#endif
    std::string expected_longDouble_9007199254740993[] = { "9007199254740993",
                                                           "9007199254740992" // Windows
                                                         };
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-7",
                 "9007199254740993", 9007199254740993L, expected_longDouble_9007199254740993[indexOS]);


#if defined(__APPLE__) && defined(__MACH__)
    indexOS = 1;
#else
    indexOS = 0;
#endif
    std::string expected_float_3d3123[] = { "3.3123",
                                            "3.31229" }; // macOS does not support 'std::from_chars()' and
                                                        // 'std::to_chars()'. The fall back functions
                                                        // induces this variation.
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-8",
                 "3.3123", 3.3123f, expected_float_3d3123[indexOS]);

    std::string expected_float_3d3123412ep38[] = { "3.3123412e+38",
                                                   "3.31234119999999" }; // macOS does not support 'std::from_chars()' and
                                                                         // 'std::to_chars()'. The fall back functions
                                                                         // induces this variation.
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-9",
                 "3.3123412e+38", 3.3123412E38f, expected_float_3d3123412ep38[indexOS]);

    std::string expected_float_3d3123412en38[] = { "3.3123412e-38",
                                                   "3.31234119999999" }; // macOS does not support 'std::from_chars()' and
                                                                         // 'std::to_chars()'. The fall back functions
                                                                         // induces this variation.
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-10",
                 "3.3123412e-38", 3.3123412E-38f, expected_float_3d3123412en38[indexOS]);

    std::string expected_double_4d3123412en38[] = { "4.3123412e-38",
                                                    "4.31234119999999" }; // macOS does not support 'std::from_chars()' and
                                                                          // 'std::to_chars()'. The fall back functions
                                                                          // induces this variation.
    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-11",
                 "4.3123412e-38", 4.3123412E-38, expected_double_4d3123412en38[indexOS]);

    std::string expected_longdouble_5d3123412en38[] = { "5.3123412e-38",
                                                        "5.31234119999999" }; // macOS does not support 'std::from_chars()' and
                                                                              // 'std::to_chars()'. The fall back functions
                                                                              // induces this variation.
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-12",
                 "5.3123412e-38", 5.3123412E-38L, expected_longdouble_5d3123412en38[indexOS]);

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}