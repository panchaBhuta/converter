
// test NaN conversion exceptions for floating-point-types


#include <iostream>
#include <cmath>
#include <typeinfo>
#include <type_traits>
#include <limits>
#include <algorithm>
#include <functional>
#include <iterator>
#include <cassert>

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

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER, size_t VARIANTIDX >
          requires ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ||
                     ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN )
inline void conversionFP_S2T_C_NAN(const std::string& vStr,
                                   const std::string& filePath, int lineNo,
                                   const T chkVal = std::numeric_limits<T>::quiet_NaN() )
{
  if constexpr ( ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN )
  {
    if constexpr (std::numeric_limits<T>::has_quiet_NaN)
    {
      T conVal = ConvertFromStr<T , CONV_PROCESS , ERR_HANDLER>::ToVal(vStr);
      if constexpr (VARIANTIDX != 0) {
        unittest::ExpectTrue2(std::isnan(conVal));
      } else {
        unittest::ExpectEqual2(T, conVal, chkVal);  // use case ::: Str2TnConversionProcess::S2N && vStr = ' 123.45'
      }
    } else {
      std::cout << "conversionFP_S2T_C_NAN<" << specializedTypes::get_name<T>() << "," << CONV_PROCESS << ", QUIET_NAN, " << VARIANTIDX << ">(" << vStr << ") skipped as has_quiet_NaN = false\n";
    }
  } else if constexpr ( ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN ) {
    if constexpr (std::numeric_limits<T>::has_signaling_NaN)
    {
      T conVal = ConvertFromStr<T , CONV_PROCESS , ERR_HANDLER>::ToVal(vStr);
      if constexpr (VARIANTIDX != 0) {
        unittest::ExpectTrue2(std::isnan(conVal));
      } else {
        unittest::ExpectEqual2(T, conVal, chkVal);  // use case ::: Str2TnConversionProcess::S2N && vStr = ' 123.45'
      }
    } else {
      std::cout << "conversionFP_S2T_C_NAN<" << specializedTypes::get_name<T>() << "," << CONV_PROCESS << ", SIGNAL_NAN, " << VARIANTIDX << ">(" << vStr << ") skipped as has_signal_NaN = false\n";
    }
  }
}


template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t VARIANTIDX >
    requires ( VARIANTIDX == 0 || VARIANTIDX == 1 || VARIANTIDX == 2 )
inline void conversionFP_S2T_C_VARIANT(const std::string& vStr,
                                       const std::string& filePath, int lineNo,
                                       const T chkVal = std::numeric_limits<T>::quiet_NaN() )
{
  auto cVal12 = ConvertFromStr<T , CONV_PROCESS , Str2TnErrorHandler::VARIANT_VAL>::ToVal(vStr);
  unittest::ExpectEqual2(size_t,      cVal12.index(), VARIANTIDX);
  if constexpr (VARIANTIDX != 0)
  {
    unittest::ExpectEqual2(std::string, std::get<VARIANTIDX>(cVal12).value, vStr);
    if ( VARIANTIDX == 1 )
    {
      unittest::ExpectEqual2(std::string, std::get<Str2TNaNInput>(cVal12).value, vStr);

      if constexpr (std::numeric_limits<T>::has_quiet_NaN)
      {
        // check for sign bit
        if ( vStr.size() > 1 )
        {
          std::cout << "TESTLOG :: Calling with QUIET_NAN -> ConvertFromStr<T , CONV_PROCESS , Str2TnErrorHandler::QUIET_NAN>::ToVal(vStr) to figure out the converted NAN value" << std::endl;
          T conVal = ConvertFromStr<T , CONV_PROCESS , Str2TnErrorHandler::QUIET_NAN>::ToVal(vStr);
          bool isStringSigned = ( '-' == vStr[0] );
          bool isValueSigned  = std::signbit(conVal);
          unittest::ExpectEqual2(bool,      isStringSigned, isValueSigned);
        }
      }

    } else if ( VARIANTIDX == 2 )
    {
      unittest::ExpectEqual2(std::string, std::get<Str2TConversionFailure>(cVal12).value, vStr);
    }
  } else {
    // use case ::: Str2TnConversionProcess::S2N && vStr = ' 123.45'
    unittest::ExpectEqual2(T, std::get<VARIANTIDX>(cVal12), chkVal);
    unittest::ExpectEqual2(T, std::get<T>(cVal12), chkVal);
  }
}

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t VARIANTIDX >
inline void conversionFP_S2T_C_THROW_ERROR(const std::string& vStr,
                                           const std::string& filePath, int lineNo,
                                           const T chkVal = std::numeric_limits<T>::quiet_NaN())
{
  if constexpr (VARIANTIDX != 0) {
    ExpectException(ConvertFromStr<T COMMA CONV_PROCESS COMMA Str2TnErrorHandler::THROW_ERROR>::ToVal(vStr), std::invalid_argument);
  } else {
    T conVal = ConvertFromStr<T , CONV_PROCESS , Str2TnErrorHandler::THROW_ERROR>::ToVal(vStr);
    unittest::ExpectEqual2(T, conVal, chkVal);  // use case ::: Str2TnConversionProcess::S2N && vStr = ' 123.45'
  }
}



template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER,
           size_t FROMCHARS_VARIANTIDX, size_t S2N_VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
inline void conversionFP_S2T_C_E(const std::string& vStr,
                                 const std::string& filePath, int lineNo,
                                 const T chkVal = std::numeric_limits<T>::quiet_NaN())
{
  constexpr size_t CONV_VARIANTIDX = (   CONV_PROCESS == Str2TnConversionProcess::FROM_CHARS    ?  FROMCHARS_VARIANTIDX     :
                                       ( CONV_PROCESS == Str2TnConversionProcess::S2N           ?  S2N_VARIANTIDX           :
                                        (CONV_PROCESS == Str2TnConversionProcess::STRINGSTREAM  ?  STRINGSTREAM_VARIANTIDX  :  100 )
                                       )
                                     );

  if constexpr ( CONV_VARIANTIDX == 100 )
  {
    std::cout << "missing unit-test for Str2TnConversionProcess[" << CONV_PROCESS << "]\n";
    assert(false);
    //static_assert(always_false<T>::value, "Unsupported ERROR-HANDLER for FP-conversion");
  }

  if constexpr (ERR_HANDLER == Str2TnErrorHandler::QUIET_NAN ||
                ERR_HANDLER == Str2TnErrorHandler::SIGNAL_NAN)
  {
    conversionFP_S2T_C_NAN<T, CONV_PROCESS, ERR_HANDLER, CONV_VARIANTIDX>(vStr, filePath, lineNo, chkVal);
  } else if constexpr (ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL) {
    conversionFP_S2T_C_VARIANT<T, CONV_PROCESS, CONV_VARIANTIDX>(vStr, filePath, lineNo, chkVal);
  } else if constexpr (ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR) {
    conversionFP_S2T_C_THROW_ERROR<T, CONV_PROCESS, CONV_VARIANTIDX>(vStr, filePath, lineNo, chkVal);
  } else {
    static_assert(always_false<T>::value, "Unsupported ERROR-HANDLER for FP-conversion");
  }
}

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t IDX_ERR_HANDLER,
          size_t FROMCHARS_VARIANTIDX, size_t S2N_VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
    requires ( IDX_ERR_HANDLER  <  DefaultStr2Tn<T>::validErrorHandlers().size() )
void conversionFP_S2T_iC_iE(const std::string& vStr,
                            const std::string& filePath, int lineNo,
                            const T chkVal = std::numeric_limits<T>::quiet_NaN())
{
  constexpr Str2TnErrorHandler ERR_HANDLER =
          DefaultStr2Tn<T>::validErrorHandlers()[IDX_ERR_HANDLER];
  conversionFP_S2T_C_E<T,  CONV_PROCESS,  ERR_HANDLER,
                           FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo, chkVal);

  if constexpr ( (IDX_ERR_HANDLER+1)  <  DefaultStr2Tn<T>::validErrorHandlers().size() )
  {
    conversionFP_S2T_iC_iE<T,  CONV_PROCESS,
                               (IDX_ERR_HANDLER+1),
                               FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo, chkVal);
  }
}

template <c_floating_point T, size_t IDX_CONV_PROCESS,
          size_t FROMCHARS_VARIANTIDX, size_t S2N_VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
    requires ( IDX_CONV_PROCESS  <  DefaultStr2Tn<T>::validConversionProcesses().size() )
void conversionFP_S2T_iC(const std::string& vStr,
                         const std::string& filePath, int lineNo,
                         const T chkVal = std::numeric_limits<T>::quiet_NaN())
{
  constexpr Str2TnConversionProcess CONV_PROCESS =
          DefaultStr2Tn<T>::validConversionProcesses()[IDX_CONV_PROCESS];
  conversionFP_S2T_iC_iE<T, CONV_PROCESS, 0,
      FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo, chkVal);

  if constexpr ( (IDX_CONV_PROCESS+1)  <  DefaultStr2Tn<T>::validConversionProcesses().size() )
  {
    conversionFP_S2T_iC   <T, (IDX_CONV_PROCESS+1),
        FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo, chkVal);
  }
}


template <c_floating_point T,
          size_t FROMCHARS_VARIANTIDX, size_t S2N_VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
    requires ( STRINGSTREAM_VARIANTIDX == 0 || STRINGSTREAM_VARIANTIDX == 2 )
void conversionFP_FailureCheck_S2T(const std::string& vStr,
                                   const std::string& filePath, int lineNo,
                                   const T chkVal = std::numeric_limits<T>::quiet_NaN())
{
  std::cout << std::endl;
  std::cout << "S2T : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultStr2Tn<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  std::cout << "S2T : ERR_HANDLER for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validErrorHandlers().size() ; ++i) std::cout << DefaultStr2Tn<T>::validErrorHandlers()[i] << ", ";
  std::cout << std::endl;
  conversionFP_S2T_iC<T, 0,
      FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, filePath, lineNo, chkVal);
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
    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("nan", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("Nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("Nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("Nan", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("nAn", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("nAn", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("nAn", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("naN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("naN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("naN", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("NAn", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("NAn", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("NAn", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("nAN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("nAN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("nAN", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("NaN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("NaN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("NaN", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("NAN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("NAN", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("NAN", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 1, 2 >      ("+nan", __FILE__, __LINE__); // gnu:g++ : std::from_char() -> error ; std::sto*() -> nan
    conversionFP_FailureCheck_S2T<double, 2, 1, 2 >     ("+nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 1, 2 >("+nan", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("-nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("-nan", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("-nan", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("-nan(ind)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("-nan(ind)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("-nan(ind)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 1, 2 >      ("+nan(ind)", __FILE__, __LINE__); // gnu:g++ : std::from_char() -> error ; std::sto*() -> nan
    conversionFP_FailureCheck_S2T<double, 2, 1, 2 >     ("+nan(ind)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 1, 2 >("+nan(ind)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("nan(snan)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 1, 2 >      ("+nan(snan)", __FILE__, __LINE__); // gnu:g++ : std::from_char() -> error ; std::sto*() -> nan
    conversionFP_FailureCheck_S2T<double, 2, 1, 2 >     ("+nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 1, 2 >("+nan(snan)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 1, 1, 2>      ("-nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 1, 1, 2>     ("-nan(snan)", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 1, 1, 2>("-nan(snan)", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 2, 2>      ("junkChars", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2, 2>     ("junkChars", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2, 2>("junkChars", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 2, 2>      ("123abc", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2, 2>     ("123abc", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2, 2>("123abc", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 2, 2>      ("", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2, 2>     ("", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2, 2>("", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 2, 2>      (" ", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2, 2>     (" ", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2, 2>(" ", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 2, 2>      ("123.45xyz", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2, 2>     ("123.45xyz", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2, 2>("123.45xyz", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 0, 0>      (" 123.45", __FILE__, __LINE__, 123.45F);
    conversionFP_FailureCheck_S2T<double, 2, 0, 0>     (" 123.45", __FILE__, __LINE__, 123.45);
    conversionFP_FailureCheck_S2T<long double, 2, 0, 0>(" 123.45", __FILE__, __LINE__, 123.45L);

    conversionFP_FailureCheck_S2T<float, 2, 2, 2>      ("123.45 ", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2, 2>     ("123.45 ", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2, 2>("123.45 ", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 2, 2>      ("+-1.0", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2, 2>     ("+-1.0", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2, 2>("+-1.0", __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float, 2, 2, 2>      ("1.2.3", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double, 2, 2, 2>     ("1.2.3", __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double, 2, 2, 2>("1.2.3", __FILE__, __LINE__);


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
