
// test precison during conversions between string and floating-point-types


#include <iostream>
#include <cmath>
#include <tuple>

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
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                                 "8.5899735e+09",  // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU
#else  // default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                 "8589973504",
#endif
                                                 "8589973504", };
    unittest::CHECKROUNDTRIP(float, "testFloatingPointPrecision-1", \
                 "8.589973e+9", 8.589973e9f, expected_float_8d589973ep9[indexOS]);

    unittest::CHECKROUNDTRIP(double, "testFloatingPointPrecision-2", \
                 "8.589973e+9", 8.589973e9, "8589973000");
    unittest::CHECKROUNDTRIP(long double, "testFloatingPointPrecision-3", \
                 "8.589973e+9", 8.589973e9L, "8589973000");

    std::string expected_float_1d123456789[] = { "1.1234568",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                                 "1.12345684",   // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                 "1.1234568",
#endif
                                                 "1.1234568" };
    unittest::CHECKROUNDTRIP(float, "testFloatingPointPrecision-4", \
                 "1.123456789", 1.123456789f, expected_float_1d123456789[indexOS]);  // 6 digits
    std::string expected_double_2d1234567890123456789[] = { "2.1234567890123457",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                                            "2.12345678901234569",   // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                            "2.1234567890123457",
#endif
                                                            "2.1234567890123457" };
    unittest::CHECKROUNDTRIP(double, "testFloatingPointPrecision-5", \
                 "2.1234567890123456789", 2.1234567890123456789,     \
                 expected_double_2d1234567890123456789[indexOS]);   // 15 digits

    std::tuple<long double, std::string, int>  expected_longDouble_3d123456789012345678901[] = {
#if           UBUNTU_ARRAY_IDX  ==  UBUNTU_ARM64
                {3.1234567890123456789009999999999998574L, "3.123456789012345678901",
                        std::numeric_limits<long double>::digits10},  //  = 33 on arm64 ???? That's strange
#else //      UBUNTU_ARRAY_IDX  ==  UBUNTU_X86_64
                {3.1234567890123456788878L, "3.1234567890123456789",
                        std::numeric_limits<long double>::digits10},  // 18 default
#endif

#if           MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG    \
      ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                {3.1234567890123456788878L, "3.12345678901234569", 16},      // 15 default
#elif         MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU
//#elif          MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
                {3.1234567890123456788878L, "3.1234567890123457", 15},       // 15 g++ [ 14, 13, 12 ]
#elif         MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                {3.1234567890123456788878L, "3.12345678901234567889",
                        std::numeric_limits<long double>::digits10},   // 18 default
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                {3.1234567890123456788878L, "3.1234567890123456789",
                          std::numeric_limits<long double>::digits10},   // 18 g++ [ 14, 13, 12 ]
#endif

#ifdef              BUILD_ENV_MSYS2_GNU
                   // On Windows : MSYS2 env : GNU compiler
                   //  3.123456789012345678901L is actually rounded off to      3.1234567890123456788878, but ...
                   // "3.123456789012345678901" is converted to (string -> FP)  3.1234567890123456912477
                   // Windows (MSYS2) : BIG RED FLAG here
                {3.1234567890123456912477L, "3.1234567890123456912", 16}    // 18 Windows (MSYS2)
#else
                {3.1234567890123456912L, "3.1234567890123457",
                          std::numeric_limits<long double>::digits10}    // 15 Windows (MSVC or Clang)
#endif
                                                          };
    unittest::checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-6",
                "3.123456789012345678901",
                std::get<0>(expected_longDouble_3d123456789012345678901[indexOS]),
                std::get<1>(expected_longDouble_3d123456789012345678901[indexOS]),
                __FILE__, __LINE__,
                std::get<2>(expected_longDouble_3d123456789012345678901[indexOS]));


    unittest::CHECKROUNDTRIP(double, "testFloatingPointPrecision-7",          \
                "9007199254740993", 9007199254740993.0, "9007199254740992");
                                                   //    "9007199254740993"
    std::string expected_longDouble_9007199254740993[] = { "9007199254740993",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG    \
      ||            MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU      \
      ||            MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                                          "9007199254740992",
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                           "9007199254740993",
#endif
                                                           "9007199254740992" }; // Windows
    unittest::checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-8",
                "9007199254740993",
#ifdef              BUILD_ENV_MSYS2_GNU
                   // On Windows : MSYS2 env : GNU compiler
                   //  9007199254740993.0L is stays same                 9007199254740993, but ...
                   // "9007199254740993" is converted to (string -> FP)  9007199254740992
                9007199254740992.0L,  // Windows (MSYS2) : BIG RED FLAG here
#else
                9007199254740993.0L,
#endif
                expected_longDouble_9007199254740993[indexOS],
                __FILE__, __LINE__
#ifdef              BUILD_ENV_MSYS2_GNU
                , std::numeric_limits<long double>::digits10, '.', '_', false
#endif
                 );


    std::string expected_float_3d3123[] = { "3.3123",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                            "3.31229997",  // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                            "3.3123",
#endif
                                            "3.3123", };
    unittest::checkRoundTripConversion_txt2Val2txt<float>("testFloatingPointPrecision-9",
                "3.3123", 3.3123f, expected_float_3d3123[indexOS],
                __FILE__, __LINE__,
                ((indexOS==1)?4:std::numeric_limits<float>::digits10) ); // macOS

    unittest::CHECKROUNDTRIP(float, "testFloatingPointPrecision-10",
                 "3.3123412e+38", 3.3123412E38f, "3.3123412e+38");

    std::string expected_float_3d3123412en38[] = { "3.312341e-38",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                                   "3.31234111e-38",  // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                   "3.312341e-38",
#endif
                                                   "3.312341e-38", };
    unittest::CHECKROUNDTRIP(float, "testFloatingPointPrecision-11",
                 "3.3123412e-38", 3.3123412E-38f, expected_float_3d3123412en38[indexOS]);

    std::string expected_double_4d3123412en38[] = { "4.3123412e-38",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG   \
            ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                                    "4.31234119999999987e-38",  // macOS
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                    "4.3123412e-38",
#endif
                                                    "4.3123412e-38", };
    unittest::checkRoundTripConversion_txt2Val2txt<double>("testFloatingPointPrecision-12",
                "4.3123412e-38", 4.3123412E-38, expected_double_4d3123412en38[indexOS],
                __FILE__, __LINE__,
                ((indexOS==1)?7:std::numeric_limits<double>::digits10) ); // macOS

    std::string expected_longdouble_5d3123412en38[] = { "5.3123412e-38",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                                                        "5.3123412000000000001e-38",
#elif               MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG  \
             ||     MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                                        "5.31234120000000035e-38",
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                        "5.3123412e-38", // macOS
#endif
#ifdef              BUILD_ENV_MSYS2_GNU
                                                        "5.3123412000000003544e-38"  // Windows (MSYS2) : BIG RED FLAG here
#else
                                                        "5.3123412e-38"
#endif
                                                       };
    unittest::checkRoundTripConversion_txt2Val2txt<long double>("testFloatingPointPrecision-13",
                "5.3123412e-38",
#ifdef              BUILD_ENV_MSYS2_GNU
                5.3123412000000003544036E-38L,  // Windows (MSYS2) : BIG RED FLAG here
#else
                5.3123412E-38L,
#endif
                expected_longdouble_5d3123412en38[indexOS],
                __FILE__, __LINE__,
                ((indexOS==1)?7:std::numeric_limits<long double>::digits10) // macOS
#ifdef              BUILD_ENV_MSYS2_GNU
                , '.', '_', false
#endif
                );

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
