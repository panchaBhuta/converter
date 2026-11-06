
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


#define ExpectEqual2(t, a, b) ExpectEqualFun<t>(a, b, #a, #b, filePath, lineNo)
#define ExpectTrue2(a) ExpectTrueFun(a, #a, filePath, lineNo)

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER >
          requires ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ||
                     ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN )
inline void conversionFP_S2T_C_NAN(const std::string& vStr,
                                   const std::string& filePath, int lineNo)
{
  if constexpr ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN )
  {
    if constexpr (std::numeric_limits<T>::has_quiet_NaN)
    {
      T cVal = ConvertFromStr<T , CONV_PROCESS , ERR_HANDLER>::ToVal(vStr);
      unittest::ExpectTrue2(std::isnan(cVal));
    } else {
      std::cout << "conversionFP_S2T_C_NAN<" << specializedTypes::get_name<T>() << "," << CONV_PROCESS << ", QUIET_NAN>(" << vStr << ") skipped as has_quiet_NaN = false\n";
    }
  } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN ) {
    if constexpr (std::numeric_limits<T>::has_signaling_NaN)
    {
      T cVal = ConvertFromStr<T , CONV_PROCESS , ERR_HANDLER>::ToVal(vStr);
      unittest::ExpectTrue2(std::isnan(cVal));
    } else {
      std::cout << "conversionFP_S2T_C_NAN<" << specializedTypes::get_name<T>() << "," << CONV_PROCESS << ", SIGNAL_NAN>(" << vStr << ") skipped as has_signal_NaN = false\n";
    }
  }
}


template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t VARIANTIDX > // Note : since it's error test, VARIANTIDX != 0
    requires ( VARIANTIDX == 1 || VARIANTIDX == 2 || VARIANTIDX == (1 | 2) )  // here (1 | 2)=3 means either 1 or 2
inline void conversionFP_S2T_C_VARIANT(const std::string& vStr,
                                       const std::string& filePath, int lineNo)
{
  auto cVal12 = ConvertFromStr<T , CONV_PROCESS , Str2TnErrorHandler::VARIANT_VAL>::ToVal(vStr);
  if constexpr ( VARIANTIDX == 1 || VARIANTIDX == 2 )
  {
    unittest::ExpectEqual2(size_t,      cVal12.index(), VARIANTIDX);
    unittest::ExpectEqual2(std::string, std::get<VARIANTIDX>(cVal12).value, vStr);
  } else {
    size_t actIndex = (VARIANTIDX & cVal12.index()); // can be either 1 or 2 based on conversion mechanism and input vStr
    unittest::ExpectEqual2(size_t,      cVal12.index(), actIndex );
  }
  if constexpr ( VARIANTIDX == 1 ) {
    unittest::ExpectEqual2(std::string, std::get<Str2TNaNInput>(cVal12).value, vStr);
  } else if constexpr ( VARIANTIDX == 2 ) {
    unittest::ExpectEqual2(std::string, std::get<Str2TConversionFailure>(cVal12).value, vStr);
  } else {
    size_t actIndex = (VARIANTIDX & cVal12.index());
    if ( actIndex == 1 ) {
      unittest::ExpectEqual2(std::string, std::get<Str2TNaNInput>(cVal12).value, vStr);
      unittest::ExpectEqual2(std::string, std::get<1>(cVal12).value, vStr);
    } else { // actIndex == 2
      unittest::ExpectEqual2(std::string, std::get<Str2TConversionFailure>(cVal12).value, vStr);
      unittest::ExpectEqual2(std::string, std::get<2>(cVal12).value, vStr);
    }
  }
}

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS >
inline void conversionFP_S2T_C_THROW_ERROR(const std::string& vStr)
{
  ExpectException(ConvertFromStr<T COMMA CONV_PROCESS COMMA Str2TnErrorHandler::THROW_ERROR>::ToVal(vStr), std::invalid_argument);
}



template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER,
           size_t VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
inline void conversionFP_S2T_C_E(const std::string& vStr,
                                 const std::string& filePath, int lineNo)
{
  if constexpr (ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ||
                ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN)
  {
    conversionFP_S2T_C_NAN<T, CONV_PROCESS, ERR_HANDLER>(vStr, filePath, lineNo);
  } else if constexpr (ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL) {
    if constexpr (CONV_PROCESS != Str2TnConversionProcess::STRINGSTREAM)
      conversionFP_S2T_C_VARIANT<T, CONV_PROCESS, VARIANTIDX>(vStr, filePath, lineNo);
    else
      conversionFP_S2T_C_VARIANT<T, CONV_PROCESS, STRINGSTREAM_VARIANTIDX>(vStr, filePath, lineNo);
  } else if constexpr (ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR) {
    conversionFP_S2T_C_THROW_ERROR<T, CONV_PROCESS>(vStr);
  } else {
    static_assert(always_false<T>::value, "Unsupported ERROR-HANDLER for FP-conversion");
  }
}

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t IDX_ERR_HANDLER,
                              size_t VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
void conversionFP_S2T_iC_iE(const std::string& vStr,
                            const std::string& filePath, int lineNo)
{
  if constexpr ( IDX_ERR_HANDLER  <  DefaultStr2Tn<T>::validErrorHandlers().size() )
  {
    constexpr Str2TnErrorHandler ERR_HANDLER =
            DefaultStr2Tn<T>::validErrorHandlers()[IDX_ERR_HANDLER];
    conversionFP_S2T_C_E<T,  CONV_PROCESS,  ERR_HANDLER,
                             VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo);

    conversionFP_S2T_iC_iE<T,  CONV_PROCESS,
                               (IDX_ERR_HANDLER+1),
                               VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo);
  }
}

template <c_floating_point T, size_t IDX_CONV_PROCESS, size_t VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
void conversionFP_S2T_iC(const std::string& vStr,
                         const std::string& filePath, int lineNo)
{
  if constexpr ( IDX_CONV_PROCESS  <  DefaultStr2Tn<T>::validConversionProcesses().size() )
  {
    constexpr Str2TnConversionProcess CONV_PROCESS =
            DefaultStr2Tn<T>::validConversionProcesses()[IDX_CONV_PROCESS];
    conversionFP_S2T_iC_iE<T, CONV_PROCESS, 0, VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo);

    conversionFP_S2T_iC   <T, (IDX_CONV_PROCESS+1), VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo);
  }
}


template <c_floating_point T, size_t VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
        requires ( STRINGSTREAM_VARIANTIDX == 2 )
void conversionFP_FailureCheck_S2T(const std::string& vStr,
                                   const std::string& filePath, int lineNo)
{
  std::cout << std::endl;
  std::cout << "S2T : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultStr2Tn<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  std::cout << "S2T : ERR_HANDLER for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validErrorHandlers().size() ; ++i) std::cout << DefaultStr2Tn<T>::validErrorHandlers()[i] << ", ";
  std::cout << std::endl;
  conversionFP_S2T_iC<T, 0, VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo);
}







template <c_floating_point T, size_t IDX_CONV_PROCESS, typename V>
      requires ( std::is_same_v<V, T> || std::is_same_v<V, variantReturnType<T>> )
void conversionFP_T2S_iC(const V& val, const std::vector<std::string>& vStr,
                                       const std::string& filePath, int lineNo)
{
  if constexpr ( IDX_CONV_PROCESS < DefaultTn2Str<T>::validConversionProcesses().size() )
  {
    //std::string vStr{"nan"};
    constexpr Tn2StrConversionProcess CONV_PROCESS =
	        DefaultTn2Str<T>::validConversionProcesses()[IDX_CONV_PROCESS];
    std::string sVal = ConvertFromVal<V , CONV_PROCESS>::ToStr(val);
    if(vStr.size() == 1) {
      // is quiet NaN
      unittest::ExpectEqual2(std::string, sVal, vStr[0]);
    } else {
      // TODO : add to README about signaling-NaN T2S conversion resulting in
      // "nan(snan)" instead of "nan" in some platforms and STRINGSTREAM conversions

      std::stringstream ss;
      //ss << std::setprecision(std::numeric_limits<long double>::digits10 + 1);
      ss << "val{" << val << "} == vStr{ ";
      bool validResult = std::any_of(vStr.cbegin(), vStr.cend(), [&sVal, &ss](const std::string& strCmp) { ss << " " << strCmp << " ," ; return sVal == strCmp; } );

      if (!validResult)
      {
        ss << "} ::: Expect-AnyOf-Equal failed. :: ";
        ss << unittest::detail::FileName(__FILE__) << ":" << std::to_string(__LINE__) << std::endl;

        throw std::runtime_error(ss.str());
      }
    }

    conversionFP_T2S_iC  <T, (IDX_CONV_PROCESS+1), V >(val, vStr, filePath, lineNo);
  }
}


template <c_floating_point T, typename V = T>
      requires ( std::is_same_v<V, T> || std::is_same_v<V, variantReturnType<T>> )
void conversionFP_FailureCheck_T2S(const V& val, const std::vector<std::string>& vStr,
                                   const std::string& filePath, int lineNo)
{
  std::cout << "T2S : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultTn2Str<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultTn2Str<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  conversionFP_T2S_iC<T, 0, V >(val, vStr, filePath, lineNo);
}



int main()
{
#if defined(_MSC_VER)
  commonUtils::disable_msvc_popups();  // failed assert() generates popus in msvc
#endif

  int rv = 0;
  try {
    conversionFP_FailureCheck_S2T<float, 1, 2>      ("nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("nan", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("Nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("Nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("Nan", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("nAn", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("nAn", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("nAn", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("naN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("naN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("naN", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("NAn", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("NAn", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("NAn", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("nAN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("nAN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("nAN", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("NaN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("NaN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("NaN", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("NAN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("NAN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("NAN", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, (1 | 2), 2 >      ("+nan", __FILE__, __LINE__); // gnu:g++ : std::from_char() -> error ; std::sto*() -> nan
    conversionFP_FailureCheck_S2T<double, (1 | 2), 2 >     ("+nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, (1 | 2), 2 >("+nan", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("-nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("-nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("-nan", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("-nan(ind)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("-nan(ind)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("-nan(ind)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, (1 | 2), 2 >      ("+nan(ind)", __FILE__, __LINE__); // gnu:g++ : std::from_char() -> error ; std::sto*() -> nan
    conversionFP_FailureCheck_S2T<double, (1 | 2), 2 >     ("+nan(ind)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, (1 | 2), 2 >("+nan(ind)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("nan(snan)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, (1 | 2), 2 >      ("+nan(snan)", __FILE__, __LINE__); // gnu:g++ : std::from_char() -> error ; std::sto*() -> nan
    conversionFP_FailureCheck_S2T<double, (1 | 2), 2 >     ("+nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, (1 | 2), 2 >("+nan(snan)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 2>      ("-nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 2>     ("-nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 2>("-nan(snan)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 2>      ("junkChars", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2>     ("junkChars", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2>("junkChars", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 2>      ("123abc", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2>     ("123abc", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2>("123abc", __FILE__, __LINE__);


    std::cout << "testFloatingPointNaN : T2S : has_quiet_NaN" << std::endl;
    std::vector<std::string> pqNaNresult {"nan"};
    std::vector<std::string> nqNaNresult { "nan",         // STRINGSTREAM across all compilers
                                           "-nan",        // ubuntu
                                           "-nan(ind)" }; // macos and windows

    if constexpr (std::numeric_limits<float>::has_quiet_NaN)
    {
      conversionFP_FailureCheck_T2S<float>      ( std::numeric_limits<float>::quiet_NaN(), pqNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<float>      (-std::numeric_limits<float>::quiet_NaN(), nqNaNresult, __FILE__, __LINE__);

      variantReturnType<float>  pveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"nan"}
                                };
      variantReturnType<float>  nveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"-nan"}
                                };
      conversionFP_FailureCheck_T2S<float, variantReturnType<float>>      ( pveNaN, pqNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<float, variantReturnType<float>>      ( nveNaN, nqNaNresult, __FILE__, __LINE__);
    }

    if constexpr (std::numeric_limits<double>::has_quiet_NaN)
    {
      conversionFP_FailureCheck_T2S<double>     ( std::numeric_limits<double>::quiet_NaN(), pqNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<double>     (-std::numeric_limits<double>::quiet_NaN(), nqNaNresult, __FILE__, __LINE__);

      variantReturnType<double>  pveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"nan"}
                                };
      variantReturnType<double>  nveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"-nan"}
                                };
      conversionFP_FailureCheck_T2S<double, variantReturnType<double>>      ( pveNaN, pqNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<double, variantReturnType<double>>      ( nveNaN, nqNaNresult, __FILE__, __LINE__);
    }

    if constexpr (std::numeric_limits<long double>::has_quiet_NaN)
    {
      conversionFP_FailureCheck_T2S<long double>( std::numeric_limits<long double>::quiet_NaN(), pqNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<long double>(-std::numeric_limits<long double>::quiet_NaN(), nqNaNresult, __FILE__, __LINE__);

      variantReturnType<long double>  pveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"nan"}
                                };
      variantReturnType<long double>  nveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"-nan"}
                                };
      conversionFP_FailureCheck_T2S<long double, variantReturnType<long double>>      ( pveNaN, pqNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<long double, variantReturnType<long double>>      ( nveNaN, nqNaNresult, __FILE__, __LINE__);
    }

    std::cout << "testFloatingPointNaN : T2S : has_signaling_NaN" << std::endl;
    std::vector<std::string> psNaNresult { "nan",  "nan(snan)"};
    std::vector<std::string> nsNaNresult { "nan",          // STRINGSTREAM across all compilers
                                           "-nan",         // ubuntu
                                           "-nan(snan)" }; // macos and windows
    if constexpr (std::numeric_limits<float>::has_signaling_NaN)
    {
      conversionFP_FailureCheck_T2S<float>      ( std::numeric_limits<float>::signaling_NaN(), psNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<float>      (-std::numeric_limits<float>::signaling_NaN(), nsNaNresult, __FILE__, __LINE__);

      variantReturnType<float>  pveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"nan"}
                                };
      variantReturnType<float>  nveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"-nan"}
                                };
      conversionFP_FailureCheck_T2S<float, variantReturnType<float>>      ( pveNaN, psNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<float, variantReturnType<float>>      ( nveNaN, nsNaNresult, __FILE__, __LINE__);
    }

    if constexpr (std::numeric_limits<double>::has_signaling_NaN)
    {
      conversionFP_FailureCheck_T2S<double>     ( std::numeric_limits<double>::signaling_NaN(), psNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<double>     (-std::numeric_limits<double>::signaling_NaN(), nsNaNresult, __FILE__, __LINE__);

      variantReturnType<double>  pveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"nan"}
                                };
      variantReturnType<double>  nveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"-nan"}
                                };
      conversionFP_FailureCheck_T2S<double, variantReturnType<double>>      ( pveNaN, psNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<double, variantReturnType<double>>      ( nveNaN, nsNaNresult, __FILE__, __LINE__);
    }

    if constexpr (std::numeric_limits<long double>::has_signaling_NaN)
    {
      conversionFP_FailureCheck_T2S<long double>( std::numeric_limits<long double>::signaling_NaN(), psNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<long double>(-std::numeric_limits<long double>::signaling_NaN(), nsNaNresult, __FILE__, __LINE__);

      variantReturnType<long double>  pveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"nan"}
                                };
      variantReturnType<long double>  nveNaN {
                                  std::in_place_index<1>,
                                  Str2TNaNInput{"-nan"}
                                };
      conversionFP_FailureCheck_T2S<long double, variantReturnType<long double>>      ( pveNaN, psNaNresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<long double, variantReturnType<long double>>      ( nveNaN, nsNaNresult, __FILE__, __LINE__);
    }

    // T2S variantReturnType<T>
    std::vector<std::string> junk {"junk"};

    variantReturnType<float>  junkNaN_f {
                                std::in_place_index<2>,
                                Str2TConversionFailure{"junk"}
                              };

    variantReturnType<double>  junkNaN_d {
                                std::in_place_index<2>,
                                Str2TConversionFailure{"junk"}
                              };

    variantReturnType<long double>  junkNaN_ld {
                                std::in_place_index<2>,
                                Str2TConversionFailure{"junk"}
                              };

    conversionFP_FailureCheck_T2S<float, variantReturnType<float>>             ( junkNaN_f,  junk, __FILE__, __LINE__);
    conversionFP_FailureCheck_T2S<double, variantReturnType<double>>           ( junkNaN_d,  junk, __FILE__, __LINE__);
    conversionFP_FailureCheck_T2S<long double, variantReturnType<long double>> ( junkNaN_ld, junk, __FILE__, __LINE__);

  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
