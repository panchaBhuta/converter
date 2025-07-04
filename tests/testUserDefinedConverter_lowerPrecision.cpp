
// test user-defined conversion for floating-point-types


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

    std::string expected_longDouble_1d123456789012345678901[] = {
#if                 UBUNTU_ARRAY_IDX  ==  UBUNTU_ARM64
                                                                  "3.123456789012345678901",  // 16 arm64
#else //            UBUNTU_ARRAY_IDX  ==  UBUNTU_X86_64
                                                                  "3.1234567890123457",       // 16 default
#endif
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG   \
        ||          MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU     \
        ||          MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                                                                  "3.1234567890123",
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                                                                  "3.1234567890123457",
#endif
#ifdef              BUILD_ENV_MSYS2_GNU
                                                                  "3.123456789012345691"  // Windows (MSYS2)
#else
                                                                  "3.1234567890123"       // Windows (MSVC or Clang)
#endif
                                                                };
    checkRoundTripConversion_txt2Val2txt<long double, converter::ConvertFromStr<long double>,
                                                      ConvertFromVal_lDP<long double>>("testUserDefinedConverter_lowerPrecision-6",
                 "3.123456789012345678901", 3.123456789012345678901L,
                 expected_longDouble_1d123456789012345678901[indexOS],
                 (indexOS==0)?16:(
                 (indexOS==1)?
#if                 MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_CLANG   \
        ||          MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS14_ARM_GNU     \
        ||          MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
                 13
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_CLANG
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_MACOS15_ARM_GNU
//#elif             MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_CLANG
#else //  default   MACH_MACOS_ARRAY_IDX  ==  MACH_PRE_MACOS14_GNU
                 16
                 // getLowerDecimalPrecision<long double>() = 17 digits
#endif
                 :13)
                );
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
                                                           "9.007199254741e+15" }; // Windows
    checkRoundTripConversion_txt2Val2txt<long double, converter::ConvertFromStr<long double>,
                                                      ConvertFromVal_lDP<long double>>("testUserDefinedConverter_lowerPrecision-8",
                 "9007199254740993", 9007199254740993.0L,
                 expected_longDouble_9007199254740993[indexOS],
                 getLowerDecimalPrecision<long double>());


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