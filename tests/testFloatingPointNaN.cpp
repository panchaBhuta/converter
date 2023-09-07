
// test NaN conversion exceptions for floating-point-types


#include <iostream>
#include <cmath>

#include <converter/converter.h>

#include "unittest.h"

template <typename T, converter::FailureS2Tprocess EP>
using _ConvS2T_CtoT = converter::ConvertFromStr<T, converter::S2T_Format_std_CtoT<T, EP>>;

template <typename T, converter::FailureS2Tprocess EP>
using _ConvS2T_StoT = converter::ConvertFromStr<T, converter::S2T_Format_std_StoT<T, EP>>;

template <typename T, converter::FailureS2Tprocess EP>
using _ConvS2T_ISS = converter::ConvertFromStr<T, converter::S2T_Format_StreamUseClassicLocale<T, EP, char>>;

template <typename T>
using _ConvT2S_TtoC = converter::ConvertFromVal<T, converter::T2S_Format_std_TtoC>;

template <typename T>
using _ConvT2S_TtoS = converter::ConvertFromVal<T, converter::T2S_Format_StreamDecimalPrecision<T>>;

template <typename T>
using _ConvT2S_OSS = converter::ConvertFromVal<T, converter::T2S_Format_StreamUseClassicLocale<char>>;


template <typename T>
void conversion_String2FloatingPoint_FailureCheck(const std::string& vStr)
{
  {
    ExpectException(_ConvS2T_CtoT<T COMMA converter::FailureS2Tprocess::THROW_ERROR>::ToVal(vStr), std::invalid_argument);
    unittest::ExpectTrue(std::isnan(_ConvS2T_CtoT<T COMMA converter::FailureS2Tprocess::QUIET_NAN>::ToVal(vStr)));
    unittest::ExpectTrue(std::isnan(_ConvS2T_CtoT<T COMMA converter::FailureS2Tprocess::SIGNAL_NAN>::ToVal(vStr)));

    typename converter::OnError<T, converter::FailureS2Tprocess::VARIANT_NAN>::return_type varT = 
      _ConvS2T_CtoT<T , converter::FailureS2Tprocess::VARIANT_NAN>::ToVal(vStr);
    unittest::ExpectTrue(varT.index() == 1);
    unittest::ExpectEqual(std::string, std::get<std::string>(varT), vStr);
  }

  {
    ExpectException(_ConvS2T_StoT<T COMMA converter::FailureS2Tprocess::THROW_ERROR>::ToVal(vStr), std::invalid_argument);
    unittest::ExpectTrue(std::isnan(_ConvS2T_StoT<T COMMA converter::FailureS2Tprocess::QUIET_NAN>::ToVal(vStr)));
    unittest::ExpectTrue(std::isnan(_ConvS2T_StoT<T COMMA converter::FailureS2Tprocess::SIGNAL_NAN>::ToVal(vStr)));

    typename converter::OnError<T, converter::FailureS2Tprocess::VARIANT_NAN>::return_type varT = 
      _ConvS2T_StoT<T , converter::FailureS2Tprocess::VARIANT_NAN>::ToVal(vStr);
    unittest::ExpectTrue(varT.index() == 1);
    unittest::ExpectEqual(std::string, std::get<std::string>(varT), vStr);
  }

  {
    ExpectException(_ConvS2T_ISS<T COMMA converter::FailureS2Tprocess::THROW_ERROR>::ToVal(vStr), std::invalid_argument);
    unittest::ExpectTrue(std::isnan(_ConvS2T_ISS<T COMMA converter::FailureS2Tprocess::QUIET_NAN>::ToVal(vStr)));
    unittest::ExpectTrue(std::isnan(_ConvS2T_ISS<T COMMA converter::FailureS2Tprocess::SIGNAL_NAN>::ToVal(vStr)));

    typename converter::OnError<T, converter::FailureS2Tprocess::VARIANT_NAN>::return_type varT = 
      _ConvS2T_ISS<T , converter::FailureS2Tprocess::VARIANT_NAN>::ToVal(vStr);
    unittest::ExpectTrue(varT.index() == 1);
    unittest::ExpectEqual(std::string, std::get<std::string>(varT), vStr);
  }
}

template <typename T>
void conversion_FloatingPointNAN2String_FailureCheck(const std::string& vStr)
{
  {
    unittest::ExpectEqual(std::string, _ConvT2S_TtoC<T>::ToStr(std::numeric_limits<T>::quiet_NaN()), "nan");
#if defined(_WIN64) || defined(_WIN32)
    unittest::ExpectEqual(std::string, _ConvT2S_TtoC<T>::ToStr(std::numeric_limits<T>::signaling_NaN()), "nan(snan)");
#else
    unittest::ExpectEqual(std::string, _ConvT2S_TtoC<T>::ToStr(std::numeric_limits<T>::signaling_NaN()), "nan");
#endif
    unittest::ExpectEqual(std::string, converter::ConvertFromVal< std::variant<T COMMA std::string> COMMA
                                                                  converter::T2S_Format_std_TtoC
                                                    >::ToStr(std::variant<T COMMA std::string>{vStr}),
                                       vStr);
  }

  {
    unittest::ExpectEqual(std::string, _ConvT2S_TtoS<T>::ToStr(std::numeric_limits<T>::quiet_NaN()), "nan");
    std::cout << "_ConvT2S_TtoS<T>::ToStr(std::numeric_limits<T>::signaling_NaN()) = #" << 
                  _ConvT2S_TtoS<T>::ToStr(std::numeric_limits<T>::signaling_NaN()) << "#" << std::endl;
#if defined(WIN64) || defined(_WIN64) || defined(__WIN64) || defined(__WIN64__)
    unittest::ExpectEqual(std::string, _ConvT2S_TtoS<T>::ToStr(std::numeric_limits<T>:: signaling_NaN()), "nan(snan)");
#else
    unittest::ExpectEqual(std::string, _ConvT2S_TtoS<T>::ToStr(std:: numeric_limits<T>::signaling_NaN()), "nan");
#endif
    unittest::ExpectEqual(std::string, converter::ConvertFromVal< std::variant<T COMMA std::string> COMMA
                                                                  converter::T2S_Format_StreamDecimalPrecision<T>
                                                    >::ToStr(std::variant<T COMMA std::string>{vStr}),
                                       vStr);
#ifdef ENABLE_STD_TtoS
    unittest::ExpectEqual(std::string, converter::ConvertFromVal< std::variant<T COMMA std::string> COMMA
                                                                  converter::T2S_Format_std_TtoS
                                                    >::ToStr(std::variant<T COMMA std::string>{vStr}),
                                       vStr);
#endif
  }

  {
    unittest::ExpectEqual(std::string, _ConvT2S_OSS<T>::ToStr(std::numeric_limits<T>::quiet_NaN()), "nan");
#if defined(_WIN64) || defined(_WIN32)
    unittest::ExpectEqual(std::string, _ConvT2S_OSS<T>::ToStr(std::numeric_limits<T>::signaling_NaN()), "nan(snan)");
#else
    unittest::ExpectEqual(std::string, _ConvT2S_OSS<T>::ToStr(std::numeric_limits<T>::signaling_NaN()), "nan");
#endif
    unittest::ExpectEqual(std::string, converter::ConvertFromVal< std::variant<T COMMA std::string> COMMA
                                                                  converter::T2S_Format_StreamUseClassicLocale<char>
                                                    >::ToStr(std::variant<T COMMA std::string>{vStr}),
                                       vStr);
  }
}


int main()
{
  int rv = 0;
  try {
    // [========[  String to FloatingPoint-NaN
    conversion_String2FloatingPoint_FailureCheck<float>("rc");
    conversion_String2FloatingPoint_FailureCheck<double>("ab");
    conversion_String2FloatingPoint_FailureCheck<long double>("xy");

    conversion_String2FloatingPoint_FailureCheck<float>("1.2x1");
    conversion_String2FloatingPoint_FailureCheck<double>("2.3.4");
    conversion_String2FloatingPoint_FailureCheck<long double>("3r.422123445");
    // ]========]  String to FloatingPoint-NaN


    // [========[  FloatingPoint-NaN to String
    conversion_FloatingPointNAN2String_FailureCheck<float>("rc");
    conversion_FloatingPointNAN2String_FailureCheck<double>("ab");
    conversion_FloatingPointNAN2String_FailureCheck<long double>("xy");

    conversion_FloatingPointNAN2String_FailureCheck<float>("1.2x1");
    conversion_FloatingPointNAN2String_FailureCheck<double>("2.3.4");
    conversion_FloatingPointNAN2String_FailureCheck<long double>("3r.422123445");
    // ]========]  FloatingPoint-NaN to String
  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}