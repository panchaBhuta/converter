
// test NaN conversion exceptions for floating-point-types


#include <iostream>
#include <cmath>
#include <typeinfo>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <functional>
#include <iterator>

#include <vector>
#include <string>

#include <converter/converter.h>
#include <commonUtils/disable_msvc_popups.h>
#include <specializedTypes/case_insensitive_string.h>

#include "unittest.h"
#include "utilities.h"



using namespace converter;



template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER >
          requires ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ||
                     ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN )
inline void conversionFP_S2T_C_NAN(const std::string& vStr)
{
  if constexpr ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN )
  {
    if constexpr (std::numeric_limits<T>::has_quiet_NaN)
    {
      T cVal = ConvertFromStr<T , CONV_PROCESS , ERR_HANDLER>::ToVal(vStr);
      unittest::ExpectTrue(std::isnan(cVal));
    } else {
      std::cout << "conversionFP_S2T_C_NAN<" << specializedTypes::get_name<T>() << "," << CONV_PROCESS << ", QUIET_NAN>(" << vStr << ") skipped as has_quiet_NaN = false\n";
    }
  } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN ) {
    if constexpr (std::numeric_limits<T>::has_signaling_NaN)
    {
      T cVal = ConvertFromStr<T , CONV_PROCESS , ERR_HANDLER>::ToVal(vStr);
      unittest::ExpectTrue(std::isnan(cVal));
    } else {
      std::cout << "conversionFP_S2T_C_NAN<" << specializedTypes::get_name<T>() << "," << CONV_PROCESS << ", SIGNAL_NAN>(" << vStr << ") skipped as has_signal_NaN = false\n";
    }
  }
}

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t IDX > // Note : since it's error test, IDX != 0
    requires ( IDX == 1 || IDX == 2 )
inline void conversionFP_S2T_C_VARIANT(const std::string& vStr)
{
  auto cVal = ConvertFromStr<T , CONV_PROCESS , Str2TnErrorHandler::VARIANT_VAL>::ToVal(vStr);
  unittest::ExpectEqual(size_t,      cVal.index(), IDX);
  unittest::ExpectEqual(std::string, std::get<IDX>(cVal).value, vStr);
  if constexpr ( IDX == 1 )
  {
    unittest::ExpectEqual(std::string, std::get<Str2TNaNInput>(cVal).value, vStr);
  } else {
    unittest::ExpectEqual(std::string, std::get<Str2TConversionFailure>(cVal).value, vStr);
  }
}

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS >
inline void conversionFP_S2T_C_THROW_ERROR(const std::string& vStr)
{
  ExpectException(ConvertFromStr<T COMMA CONV_PROCESS COMMA Str2TnErrorHandler::THROW_ERROR>::ToVal(vStr), std::invalid_argument);
}



template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER >
inline void conversionFP_S2T_C_E(const std::string& vStr)
{
  if constexpr (ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ||
                ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN)
  {
    conversionFP_S2T_C_NAN<T, CONV_PROCESS, ERR_HANDLER>(vStr);
  } else if constexpr (ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL) {
    specializedTypes::ci_string    ciNaN{"nan"};
    if ( ciNaN == vStr )
    {
      conversionFP_S2T_C_VARIANT<T, CONV_PROCESS, 1>(vStr);
    } else {
      conversionFP_S2T_C_VARIANT<T, CONV_PROCESS, 2>(vStr);
    }
  } else if constexpr (ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR) {
    conversionFP_S2T_C_THROW_ERROR<T, CONV_PROCESS>(vStr);
  } else {
    static_assert(always_false<T>::value, "Unsupported ERROR-HANDLER for FP-conversion");
  }
}

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t IDX_ERR_HANDLER >
void conversionFP_S2T_iC_iE(const std::string& vStr)
{
  if constexpr ( IDX_ERR_HANDLER  <  DefaultStr2Tn<T>::validErrorHandlers().size() )
  {
    constexpr Str2TnErrorHandler ERR_HANDLER =
            DefaultStr2Tn<T>::validErrorHandlers()[IDX_ERR_HANDLER];
    conversionFP_S2T_C_E<T,  CONV_PROCESS,  ERR_HANDLER >(vStr);

    conversionFP_S2T_iC_iE<T,  CONV_PROCESS,
                               (IDX_ERR_HANDLER+1) >(vStr);
  }
}

template <c_floating_point T, size_t IDX_CONV_PROCESS >
void conversionFP_S2T_iC(const std::string& vStr)
{
  if constexpr ( IDX_CONV_PROCESS  <  DefaultStr2Tn<T>::validConversionProcesses().size() )
  {
    constexpr Str2TnConversionProcess CONV_PROCESS =
            DefaultStr2Tn<T>::validConversionProcesses()[IDX_CONV_PROCESS];
    conversionFP_S2T_iC_iE<T, CONV_PROCESS, 0 >(vStr);

    conversionFP_S2T_iC   <T, (IDX_CONV_PROCESS+1) >(vStr);
  }
}


template <c_floating_point T>
void conversionFP_FailureCheck_S2T(const std::string& vStr)
{
  std::cout << std::endl;
  std::cout << "S2T : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultStr2Tn<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  std::cout << "S2T : ERR_HANDLER for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validErrorHandlers().size() ; ++i) std::cout << DefaultStr2Tn<T>::validErrorHandlers()[i] << ", ";
  std::cout << std::endl;
  conversionFP_S2T_iC<T, 0 >(vStr);
}







template <c_floating_point T, size_t IDX_CONV_PROCESS>
void conversionFP_T2S_iC(const T& val, const std::vector<std::string>& vStr)
{
  if constexpr ( IDX_CONV_PROCESS < DefaultTn2Str<T>::validConversionProcesses().size() )
  {
    //std::string vStr{"nan"};
    constexpr Tn2StrConversionProcess CONV_PROCESS =
	        DefaultTn2Str<T>::validConversionProcesses()[IDX_CONV_PROCESS];
    std::string sVal = ConvertFromVal<T , CONV_PROCESS>::ToStr(val);
    if(vStr.size() == 1) {
      // is quiet NaN
      unittest::ExpectEqual(std::string, sVal, vStr[0]);
    } else {
      // TODO : add to README about signaling-NaN T2S conversion resulting in
      // "nan(snan)" instead of "nan" in some platforms and STRINGSTREAM conversions

      bool validResult = std::any_of(vStr.cbegin(), vStr.cend(), [&sVal](const std::string& strCmp) { return sVal == strCmp; } );

      unittest::ExpectTrue(validResult);
    }

    conversionFP_T2S_iC  <T, (IDX_CONV_PROCESS+1) >(val, vStr);
  }
}
/*
variantReturnType<T>
*/


template <c_floating_point T>
void conversionFP_FailureCheck_T2S(const T& val, const std::vector<std::string>& vStr)
{
  std::cout << "T2S : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultTn2Str<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultTn2Str<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  conversionFP_T2S_iC<T, 0 >(val, vStr);
}



int main()
{
#if defined(_MSC_VER)
  commonUtils::disable_msvc_popups();  // failed assert() generates popus in msvc
#endif

  int rv = 0;
  try {
    conversionFP_FailureCheck_S2T<float>      ("nan");
    conversionFP_FailureCheck_S2T<double>     ("nan");
    conversionFP_FailureCheck_S2T<long double>("nan");

    conversionFP_FailureCheck_S2T<float>      ("Nan");
    conversionFP_FailureCheck_S2T<double>     ("Nan");
    conversionFP_FailureCheck_S2T<long double>("Nan");

    conversionFP_FailureCheck_S2T<float>      ("nAn");
    conversionFP_FailureCheck_S2T<double>     ("nAn");
    conversionFP_FailureCheck_S2T<long double>("nAn");

    conversionFP_FailureCheck_S2T<float>      ("naN");
    conversionFP_FailureCheck_S2T<double>     ("naN");
    conversionFP_FailureCheck_S2T<long double>("naN");

    conversionFP_FailureCheck_S2T<float>      ("NAn");
    conversionFP_FailureCheck_S2T<double>     ("NAn");
    conversionFP_FailureCheck_S2T<long double>("NAn");

    conversionFP_FailureCheck_S2T<float>      ("nAN");
    conversionFP_FailureCheck_S2T<double>     ("nAN");
    conversionFP_FailureCheck_S2T<long double>("nAN");

    conversionFP_FailureCheck_S2T<float>      ("NaN");
    conversionFP_FailureCheck_S2T<double>     ("NaN");
    conversionFP_FailureCheck_S2T<long double>("NaN");

    conversionFP_FailureCheck_S2T<float>      ("NAN");
    conversionFP_FailureCheck_S2T<double>     ("NAN");
    conversionFP_FailureCheck_S2T<long double>("NAN");

    std::cout << "testFloatingPointNaN : T2S : has_quiet_NaN" << std::endl;
    std::vector<std::string> pqNaNresult {"nan"};
    std::vector<std::string> nqNaNresult {"-nan",       // ubuntu
                                          "-nan(ind)"}; // macos and windows
    if constexpr (std::numeric_limits<float>::has_quiet_NaN)
    {
      conversionFP_FailureCheck_T2S<float>      ( std::numeric_limits<float>::quiet_NaN(), pqNaNresult);
      conversionFP_FailureCheck_T2S<float>      (-std::numeric_limits<float>::quiet_NaN(), nqNaNresult);
    }

    if constexpr (std::numeric_limits<double>::has_quiet_NaN)
    {
      conversionFP_FailureCheck_T2S<double>     ( std::numeric_limits<double>::quiet_NaN(), pqNaNresult);
      conversionFP_FailureCheck_T2S<double>     (-std::numeric_limits<double>::quiet_NaN(), nqNaNresult);
    }

    if constexpr (std::numeric_limits<long double>::has_quiet_NaN)
    {
      conversionFP_FailureCheck_T2S<long double>( std::numeric_limits<long double>::quiet_NaN(), pqNaNresult);
      conversionFP_FailureCheck_T2S<long double>(-std::numeric_limits<long double>::quiet_NaN(), nqNaNresult);
    }

    std::cout << "testFloatingPointNaN : T2S : has_signaling_NaN" << std::endl;
    std::vector<std::string> psNaNresult { "nan",  "nan(snan)"};
    std::vector<std::string> nsNaNresult {"-nan", "-nan(snan)"};
    if constexpr (std::numeric_limits<float>::has_signaling_NaN)
    {
      conversionFP_FailureCheck_T2S<float>      ( std::numeric_limits<float>::signaling_NaN(), psNaNresult);
      conversionFP_FailureCheck_T2S<float>      (-std::numeric_limits<float>::signaling_NaN(), nsNaNresult);
    }

    if constexpr (std::numeric_limits<double>::has_signaling_NaN)
    {
      conversionFP_FailureCheck_T2S<double>     ( std::numeric_limits<double>::signaling_NaN(), psNaNresult);
      conversionFP_FailureCheck_T2S<double>     (-std::numeric_limits<double>::signaling_NaN(), nsNaNresult);
    }

    if constexpr (std::numeric_limits<long double>::has_signaling_NaN)
    {
      conversionFP_FailureCheck_T2S<long double>( std::numeric_limits<long double>::signaling_NaN(), psNaNresult);
      conversionFP_FailureCheck_T2S<long double>(-std::numeric_limits<long double>::signaling_NaN(), nsNaNresult);
    }

    // TODO   T2S variantReturnType<T>

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
