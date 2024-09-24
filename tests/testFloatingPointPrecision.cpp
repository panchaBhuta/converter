
// test precison during conversions between string and floating-point-types


#include <iostream>
#include <cmath>

#include <converter/converter.h>

#include "unittest.h"
#include "utilities.h"
#include "osIdx_TemplateID.h"

/*
  refer   https://en.cppreference.com/w/cpp/types/numeric_limits/digits10
*/

extern const unsigned indexOS;

#if TEMPLATE_UID == 103
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
#elif TEMPLATE_UID == 3
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
#endif


int main()
{
  checkConversionTemplateInstances<float>();
  checkConversionTemplateInstances<double>();
  checkConversionTemplateInstances<long double>();

  int rv = 0;
  try {

    std::string expected_float_8d589973ep9[] = { "8589973504",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                                                 "8.5899735e+09",  // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_GNU
#else  // default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                 "8589973504",
#endif
                                                 "8589973504", };
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-1",
                 "8.589973e+9", 8.589973e9f, expected_float_8d589973ep9[indexOS]);

    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-2",
                 "8.589973e+9", 8.589973e9, "8589973000");
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-3",
                 "8.589973e+9", 8.589973e9L, "8589973000");

    std::string expected_float_1d123456789[] = { "1.1234568",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_CLANG   \
     ||             MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                                                 "1.12345684",   // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                 "1.1234568",
#endif
                                                 "1.1234568" };
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-4",
                 "1.123456789", 1.123456789f, expected_float_1d123456789[indexOS]);  // 6 digits
    std::string expected_double_2d1234567890123456789[] = { "2.1234567890123457",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_CLANG   \
      ||            MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                                                            "2.12345678901234569",   // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                            "2.1234567890123457",
#endif
                                                            "2.1234567890123457" };
    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-5",
                 "2.1234567890123456789", 2.1234567890123456789,
                 expected_double_2d1234567890123456789[indexOS]);   // 15 digits
    std::string expected_longDouble_3d123456789012345678901[] = { "3.1234567890123456789",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_CLANG
                                                                  "3.12345678901234569",
#elif               MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_GNU
                                                                  "3.1234567890123457",
#elif               MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                                                                  "3.12345678901234567889",
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                                  "3.1234567890123456789",
#endif
                                                                  "3.1234567890123457" };  // Windows
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-6",
                 "3.123456789012345678901", 3.123456789012345678901L,
                 expected_longDouble_3d123456789012345678901[indexOS]);  //18 digits


    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-7",
                 "9007199254740993", 9007199254740993.0, "9007199254740992");
                                                   //    "9007199254740993"
    std::string expected_longDouble_9007199254740993[] = { "9007199254740993",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_CLANG    \
      ||            MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_GNU
                                                          "9007199254740992",
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                           "9007199254740993",
#endif
                                                           "9007199254740992" }; // Windows
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-8",
                 "9007199254740993", 9007199254740993.0L, expected_longDouble_9007199254740993[indexOS]);


    std::string expected_float_3d3123[] = { "3.3123",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_CLANG    \
       ||           MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                                            "3.31229997",  // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                            "3.3123",
#endif
                                            "3.3123", };
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-9",
                 "3.3123", 3.3123f, expected_float_3d3123[indexOS],
                 ((indexOS==1)?4:std::numeric_limits<float>::digits10) ); // macOS

    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-10",
                 "3.3123412e+38", 3.3123412E38f, "3.3123412e+38");

    std::string expected_float_3d3123412en38[] = { "3.312341e-38",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                                                   "3.31234111e-38",  // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_CLANG
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                   "3.312341e-38",
#endif
                                                   "3.312341e-38", };
    checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-11",
                 "3.3123412e-38", 3.3123412E-38f, expected_float_3d3123412en38[indexOS]);

    std::string expected_double_4d3123412en38[] = { "4.3123412e-38",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                                                    "4.31234119999999987e-38",  // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_CLANG
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                    "4.3123412e-38",
#endif
                                                    "4.3123412e-38", };
    checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-12",
                 "4.3123412e-38", 4.3123412E-38, expected_double_4d3123412en38[indexOS],
                 ((indexOS==1)?7:std::numeric_limits<double>::digits10) ); // macOS

    std::string expected_longdouble_5d3123412en38[] = { "5.3123412e-38",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_POST_MACOS14_ARM_CLANG
                                                        "5.31234120000000035e-38",
#else
                                                        "5.3123412000000000001e-38", // macOS
#endif
                                                        "5.3123412e-38", };
    checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-13",
                 "5.3123412e-38", 5.3123412E-38L, expected_longdouble_5d3123412en38[indexOS],
                 ((indexOS==1)?8:std::numeric_limits<long double>::digits10) ); // macOS

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
