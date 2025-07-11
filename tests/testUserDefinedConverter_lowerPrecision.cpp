
// test user-defined conversion for floating-point-types


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



template<converter::c_floating_point T>
void checkConversionTemplateInstances()
{
/*
  static_assert(std::is_same_v<typename converter::S2T_DefaultFormat<T>::type,
                                        converter::S2T_Format_std_StoT<T, converter::FailureS2Tprocess::SIGNAL_NAN>>);
  static_assert(std::is_same_v<typename converter::T2S_DefaultFormat<T>::type,
                                        converter::T2S_Format_StreamDecimalPrecision<T>>);
  static_assert(ConvertFromStr_loc<T>::template_uid ==  1);
*/
  static_assert(ConvertFromVal_lDP<T>::template_uid == -1);
}



int main()
{
  checkConversionTemplateInstances<float>();
  checkConversionTemplateInstances<double>();
  checkConversionTemplateInstances<long double>();

  int rv = 0;
  try {
    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>,
                                                ConvertFromVal_lDP<float>>("testUserDefinedConverter_lowerPrecision-1",
                 "8.589973e+9", 8.589973e9f, "8.59e+09",
                 2 // getLowerDecimalPrecision<float>() = 5
                );
    checkRoundTripConversion_txt2Val2txt<double, converter::ConvertFromStr<double>,
                                                 ConvertFromVal_lDP<double>>("testUserDefinedConverter_lowerPrecision-2",
                 "8.589973e+9", 8.589973e9, "8589973000", getLowerDecimalPrecision<double>());
    checkRoundTripConversion_txt2Val2txt<long double, converter::ConvertFromStr<long double>, ConvertFromVal_lDP<long double>>("testUserDefinedConverter_lowerPrecision-3",
                 "8.589973e+9", 8.589973e9L, "8589973000", getLowerDecimalPrecision<long double>());


    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>,
                                                ConvertFromVal_lDP<float>>("testUserDefinedConverter_lowerPrecision-4",
                 "1.123456789", 1.123456789f, "1.1235",
                 4  //  getLowerDecimalPrecision<float>()) = 5 digits
                );
    checkRoundTripConversion_txt2Val2txt<double, converter::ConvertFromStr<double>,
                                                 ConvertFromVal_lDP<double>>("testUserDefinedConverter_lowerPrecision-5",
                 "2.1234567890123456789", 2.1234567890123456789, "2.1234567890123", getLowerDecimalPrecision<double>());  // 14 digits

    std::tuple<long double, std::string, int>  expected_longDouble_3d123456789012345678901[] = {
#if           UBUNTU_ARRAY_IDX  ==  UBUNTU_ARM64
                {3.1234567890123456788878000000000001895L, "3.1234567890123456789009999999999998574",
                        std::numeric_limits<long double>::digits10},  //  = 33 on arm64 ???? That's strange
#else //      UBUNTU_ARRAY_IDX  ==  UBUNTU_X86_64
                {3.1234567890123456788878L, "3.1234567890123457", 16},  // 18 default
#endif

#if           MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG    \
      ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU      \
      ||      MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
//#elif          MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
                {3.1234567890123456788878L, "3.1234567890123", 13},      // 15 default
/*
#elif         MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU
                {3.1234567890123456788878L, "3.1234567890123457", 15},       // 15 g++ [ 14, 13, 12 ]
#elif         MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
                {3.1234567890123456788878L, "3.12345678901234567889",
                        std::numeric_limits<long double>::digits10},   // 18 default
*/
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                {3.1234567890123456788878L, "3.1234567890123457", 15},   // 18 g++ [ 14, 13, 12 ]
#endif

#ifdef              BUILD_ENV_MSYS2_GNU
                   // On Windows : MSYS2 env : GNU compiler
                   //  3.123456789012345678901L is actually rounded off to      3.1234567890123456788878, but ...
                   // "3.123456789012345678901" is converted to (string -> FP)  3.1234567890123456912477
                   // Windows (MSYS2) : BIG RED FLAG here
                {3.1234567890123456912477L, "3.1234567890123457", 13}    // 18 Windows (MSYS2)
#else
                {3.1234567890123456912L, "3.1234567890123", 13}          // 15 Windows (MSVC or Clang)
#endif
                                                          };
    checkRoundTripConversion_txt2Val2txt<long double, converter::ConvertFromStr<long double>,
                                                      ConvertFromVal_lDP<long double>>("testUserDefinedConverter_lowerPrecision-6",
                 "3.123456789012345678901",
                 std::get<0>(expected_longDouble_3d123456789012345678901[indexOS]),
                 std::get<1>(expected_longDouble_3d123456789012345678901[indexOS]),
                 std::get<2>(expected_longDouble_3d123456789012345678901[indexOS]));

    checkRoundTripConversion_txt2Val2txt<double, converter::ConvertFromStr<double>,
                                                 ConvertFromVal_lDP<double>>("testUserDefinedConverter_lowerPrecision-7",
                 "9007199254740993", 9007199254740993.0, "9.007199254741e+15", getLowerDecimalPrecision<double>());

    std::string expected_longDouble_9007199254740993[] = { "9007199254740993",
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG    \
      ||            MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU      \
      ||            MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                                           "9.007199254741e+15",
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                           "9007199254740993",
#endif
#ifdef              BUILD_ENV_MSYS2_GNU
                                                           "9007199254740992"   // Windows
#else
                                                           "9.007199254741e+15" // Windows
#endif
                                                          };
    checkRoundTripConversion_txt2Val2txt<long double, converter::ConvertFromStr<long double>,
                                                      ConvertFromVal_lDP<long double>>("testUserDefinedConverter_lowerPrecision-8",
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
#ifdef              BUILD_ENV_MSYS2_GNU
                 getLowerDecimalPrecision<long double>(), '.', '_', false
#else
                 getLowerDecimalPrecision<long double>()
#endif
                 );


    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>,
                                                ConvertFromVal_lDP<float>>("testUserDefinedConverter_lowerPrecision-9",
                 "3.3123", 3.3123f, "3.3123", getLowerDecimalPrecision<float>());
    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>,
                                                ConvertFromVal_lDP<float>>("testUserDefinedConverter_lowerPrecision-10",
                 "3.3123412e+38", 3.3123412E38f, "3.3123e+38", getLowerDecimalPrecision<float>());
    checkRoundTripConversion_txt2Val2txt<float, converter::ConvertFromStr<float>,
                                                ConvertFromVal_lDP<float>>("testUserDefinedConverter_lowerPrecision-11",
                 "3.3123412e-38", 3.3123412E-38f, "3.3123e-38", getLowerDecimalPrecision<float>());
    checkRoundTripConversion_txt2Val2txt<double, converter::ConvertFromStr<double>,
                                                 ConvertFromVal_lDP<double>>("testUserDefinedConverter_lowerPrecision-12",
                 "3.3123412e-38", 3.3123412E-38, "3.3123412e-38", getLowerDecimalPrecision<double>());

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}