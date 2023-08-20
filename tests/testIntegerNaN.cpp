
// test NaN conversion exceptionss for integer-types


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
void conversion_String2Integer_FailureCheck(const std::string& vStr)
{
  {
    ExpectException(_ConvS2T_CtoT<T COMMA converter::FailureS2Tprocess::THROW_ERROR>::ToVal(vStr), std::invalid_argument);

    typename converter::OnError<T, converter::FailureS2Tprocess::VARIANT_NAN>::return_type varT = 
      _ConvS2T_CtoT<T , converter::FailureS2Tprocess::VARIANT_NAN>::ToVal(vStr);
    unittest::ExpectTrue(varT.index() == 1);
    unittest::ExpectEqual(std::string, std::get<std::string>(varT), vStr);
  }

  {
    ExpectException(_ConvS2T_StoT<T COMMA converter::FailureS2Tprocess::THROW_ERROR>::ToVal(vStr), std::invalid_argument);
    typename converter::OnError<T, converter::FailureS2Tprocess::VARIANT_NAN>::return_type varT = 
      _ConvS2T_StoT<T , converter::FailureS2Tprocess::VARIANT_NAN>::ToVal(vStr);
    unittest::ExpectTrue(varT.index() == 1);
    unittest::ExpectEqual(std::string, std::get<std::string>(varT), vStr);
  }

  {
    ExpectException(_ConvS2T_ISS<T COMMA converter::FailureS2Tprocess::THROW_ERROR>::ToVal(vStr), std::invalid_argument);

    typename converter::OnError<T, converter::FailureS2Tprocess::VARIANT_NAN>::return_type varT = 
      _ConvS2T_ISS<T , converter::FailureS2Tprocess::VARIANT_NAN>::ToVal(vStr);
    unittest::ExpectTrue(varT.index() == 1);
    unittest::ExpectEqual(std::string, std::get<std::string>(varT), vStr);
  }
}

template <typename T>
void conversion_IntegerNAN2String_FailureCheck(const std::string& vStr)
{
  {
    unittest::ExpectEqual(std::string, converter::ConvertFromVal< std::variant<T COMMA std::string> COMMA
                                                                  converter::T2S_Format_std_TtoC
                                                    >::ToStr(std::variant<T COMMA std::string>{vStr}),
                                       vStr);
    unittest::ExpectEqual(std::string, converter::ConvertFromVal< std::variant<T COMMA std::string> COMMA
                                                                  converter::T2S_Format_std_TtoC
                                                    >::ToStr(std::variant<T COMMA std::string>{vStr}),
                                       vStr);
  }

  {
    unittest::ExpectEqual(std::string, converter::ConvertFromVal< std::variant<T COMMA std::string> COMMA
                                                                  converter::T2S_Format_StreamUseClassicLocale<char>
                                                    >::ToStr(std::variant<T COMMA std::string>{vStr}),
                                       vStr);
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
    // [========[  String to Integer-NaN
    conversion_String2Integer_FailureCheck<short>("rc");
    conversion_String2Integer_FailureCheck<int>("ab");
    conversion_String2Integer_FailureCheck<long>("xy");
    conversion_String2Integer_FailureCheck<long long>("xy");

    conversion_String2Integer_FailureCheck<short>("1.2x1");
    conversion_String2Integer_FailureCheck<int>("2.3.4");
    conversion_String2Integer_FailureCheck<long>("3r.422123445");
    conversion_String2Integer_FailureCheck<long long>("3r.422123445");
    // ]========]  String to Integer-NaN

    // [========[  Integer-NaN to String
    conversion_IntegerNAN2String_FailureCheck<short>("rc");
    conversion_IntegerNAN2String_FailureCheck<int>("ab");
    conversion_IntegerNAN2String_FailureCheck<long>("xy");
    conversion_IntegerNAN2String_FailureCheck<long long>("xy");

    conversion_IntegerNAN2String_FailureCheck<short>("1.2x1");
    conversion_IntegerNAN2String_FailureCheck<int>("2.3.4");
    conversion_IntegerNAN2String_FailureCheck<long>("3r.422123445");
    conversion_IntegerNAN2String_FailureCheck<long long>("3r.422123445");
    // ]========]  Integer-NaN to String
  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}