
// test INF conversion exceptions for floating-point-types


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
inline void conversionFP_S2T_C_NAN(const std::string& vStr, const T& valInf,
                                   const std::string& filePath, int lineNo)
{
  if constexpr (std::numeric_limits<T>::has_infinity)
  {
    T cVal = ConvertFromStr<T , CONV_PROCESS , ERR_HANDLER>::ToVal(vStr);
    unittest::ExpectTrue2(std::isinf(cVal));
    unittest::ExpectEqual2(T, cVal, valInf);
  } else {
    std::cout << "conversionFP_S2T_C_NAN<" << specializedTypes::get_name<T>() << ", " << CONV_PROCESS << ", " << ERR_HANDLER << ", " << VARIANTIDX << ">(" << vStr << ") skipped as has_infinity = false\n";
  }
}


template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t VARIANTIDX >
    requires ( VARIANTIDX == 0 || VARIANTIDX == 1 || VARIANTIDX == 2 )
inline void conversionFP_S2T_C_VARIANT(const std::string& vStr, const T& variantValInf,
                                       const std::string& filePath, int lineNo)
{
  if constexpr (std::numeric_limits<T>::has_infinity)
  {
    auto cVal0 = ConvertFromStr<T , CONV_PROCESS , Str2TnErrorHandler::VARIANT_VAL>::ToVal(vStr);
    unittest::ExpectEqual2(size_t,      cVal0.index(), 0);
    unittest::ExpectEqual2(T, std::get<T>(cVal0), variantValInf);

    // check for sign bit
    if ( vStr.size() > 1 )
    {
      T conVal = std::get<T>(cVal0);
      bool isStringSigned = ( '-' == vStr[0] );
      bool isValueSigned  = std::signbit(conVal);
      unittest::ExpectEqual2(bool,      isStringSigned, isValueSigned);
    }

  } else {
    std::cout << "conversionFP_S2T_C_VARIANT<" << specializedTypes::get_name<T>() << ", " << CONV_PROCESS << ", " << VARIANTIDX << ">(" << vStr << ") skipped as has_infinity = false\n";
  }
}

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t VARIANTIDX >
inline void conversionFP_S2T_C_THROW_ERROR(const std::string& vStr, const T& valInf,
                                           const std::string& filePath, int lineNo)
{
  if constexpr (std::numeric_limits<T>::has_infinity)
  {
    // "inf" is a valid conversion, we donot expect any error to be thrown
    T cVal = ConvertFromStr<T , CONV_PROCESS , Str2TnErrorHandler::THROW_ERROR>::ToVal(vStr);
    unittest::ExpectTrue2(std::isinf(cVal));
    unittest::ExpectEqual2(T, cVal, valInf);
  } else {
    std::cout << "conversionFP_S2T_C_THROW_ERROR<" << specializedTypes::get_name<T>() << ", " << CONV_PROCESS << ", " << VARIANTIDX << ">(" << vStr << ") skipped as has_infinity = false\n";
  }
}



template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, Str2TnErrorHandler ERR_HANDLER,
           size_t FROMCHARS_VARIANTIDX, size_t S2N_VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
inline void conversionFP_S2T_C_E(const std::string& vStr, const T& valInf,
                                 const std::string& filePath, int lineNo)
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
    conversionFP_S2T_C_NAN<T, CONV_PROCESS, ERR_HANDLER, CONV_VARIANTIDX>(vStr, valInf, filePath, lineNo);
  } else if constexpr (ERR_HANDLER == Str2TnErrorHandler::VARIANT_VAL) {
    conversionFP_S2T_C_VARIANT<T, CONV_PROCESS, CONV_VARIANTIDX>(vStr, valInf, filePath, lineNo);
  } else if constexpr (ERR_HANDLER == Str2TnErrorHandler::THROW_ERROR) {
    conversionFP_S2T_C_THROW_ERROR<T, CONV_PROCESS, CONV_VARIANTIDX>(vStr, valInf, filePath, lineNo);
  } else {
    static_assert(always_false<T>::value, "Unsupported ERROR-HANDLER for FP-conversion");
  }
}

template <c_floating_point T, Str2TnConversionProcess CONV_PROCESS, size_t IDX_ERR_HANDLER,
          size_t FROMCHARS_VARIANTIDX, size_t S2N_VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
    requires ( IDX_ERR_HANDLER  <  DefaultStr2Tn<T>::validErrorHandlers().size() )
void conversionFP_S2T_iC_iE(const std::string& vStr, const T& valInf,
                            const std::string& filePath, int lineNo)
{
  constexpr Str2TnErrorHandler ERR_HANDLER =
          DefaultStr2Tn<T>::validErrorHandlers()[IDX_ERR_HANDLER];
  conversionFP_S2T_C_E<T,  CONV_PROCESS,  ERR_HANDLER,
                           FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, valInf, filePath, lineNo);

  if constexpr ( (IDX_ERR_HANDLER+1)  <  DefaultStr2Tn<T>::validErrorHandlers().size() )
  {
    conversionFP_S2T_iC_iE<T,  CONV_PROCESS,
                               (IDX_ERR_HANDLER+1),
                               FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, valInf, filePath, lineNo);
  }
}

template <c_floating_point T, size_t IDX_CONV_PROCESS,
          size_t FROMCHARS_VARIANTIDX, size_t S2N_VARIANTIDX, size_t STRINGSTREAM_VARIANTIDX >
    requires ( IDX_CONV_PROCESS  <  DefaultStr2Tn<T>::validConversionProcesses().size() )
void conversionFP_S2T_iC(const std::string& vStr, const T& valInf,
                         const std::string& filePath, int lineNo)
{
    constexpr Str2TnConversionProcess CONV_PROCESS =
            DefaultStr2Tn<T>::validConversionProcesses()[IDX_CONV_PROCESS];
    conversionFP_S2T_iC_iE<T, CONV_PROCESS, 0,
      FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, valInf, filePath, lineNo);

  if constexpr ( (IDX_CONV_PROCESS+1)  <  DefaultStr2Tn<T>::validConversionProcesses().size() )
  {
    conversionFP_S2T_iC   <T, (IDX_CONV_PROCESS+1),
        FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, valInf, filePath, lineNo);
  }
}


template <c_floating_point T,
          size_t FROMCHARS_VARIANTIDX = 0, size_t S2N_VARIANTIDX = 0, size_t STRINGSTREAM_VARIANTIDX = 0 >
    requires ( STRINGSTREAM_VARIANTIDX == 0 || STRINGSTREAM_VARIANTIDX == 2 )
void conversionFP_FailureCheck_S2T(const std::string& vStr, const T& valInf,
                                   const std::string& filePath, int lineNo)
{
  std::cout << std::endl;
  std::cout << "S2T : CONV_PROCESS for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validConversionProcesses().size() ; ++i) std::cout << DefaultStr2Tn<T>::validConversionProcesses()[i] << ", ";
  std::cout << std::endl;
  std::cout << "S2T : ERR_HANDLER for type [" << specializedTypes::get_name<T>() << "] : ";
  for(size_t i = 0; i < DefaultStr2Tn<T>::validErrorHandlers().size() ; ++i) std::cout << DefaultStr2Tn<T>::validErrorHandlers()[i] << ", ";
  std::cout << std::endl;
  conversionFP_S2T_iC<T, 0,
      FROMCHARS_VARIANTIDX, S2N_VARIANTIDX, STRINGSTREAM_VARIANTIDX >(vStr, valInf, filePath, lineNo);
}







template <c_floating_point T, size_t IDX_CONV_PROCESS, typename V>
      requires ( std::is_same_v<V, T> || std::is_same_v<V, variantReturnType<T>> )
void conversionFP_T2S_iC(const V& val, const std::string& vStr,
                                       const std::string& filePath, int lineNo)
{
  if constexpr ( IDX_CONV_PROCESS < DefaultTn2Str<T>::validConversionProcesses().size() )
  {
    //std::string vStr{"nan"};
    constexpr Tn2StrConversionProcess CONV_PROCESS =
	        DefaultTn2Str<T>::validConversionProcesses()[IDX_CONV_PROCESS];
    std::string sVal = ConvertFromVal<V , CONV_PROCESS>::ToStr(val);
    unittest::ExpectEqual2(std::string, sVal, vStr);

    conversionFP_T2S_iC  <T, (IDX_CONV_PROCESS+1), V >(val, vStr, filePath, lineNo);
  }
}


template <c_floating_point T, typename V = T>
      requires ( std::is_same_v<V, T> || std::is_same_v<V, variantReturnType<T>> )
void conversionFP_FailureCheck_T2S(const V& val, const std::string& vStr,
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

    std::cout << "testFloatingPointINF : S2T :" << std::endl;
    conversionFP_FailureCheck_S2T<float>      ("inf",  std::numeric_limits<float>::infinity(),       __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double>     ("inf",  std::numeric_limits<double>::infinity(),      __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double>("inf",  std::numeric_limits<long double>::infinity(), __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float>      ("+inf",  std::numeric_limits<float>::infinity(),       __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double>     ("+inf",  std::numeric_limits<double>::infinity(),      __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double>("+inf",  std::numeric_limits<long double>::infinity(), __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float>      ("-inf",  -std::numeric_limits<float>::infinity(),       __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double>     ("-inf",  -std::numeric_limits<double>::infinity(),      __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double>("-inf",  -std::numeric_limits<long double>::infinity(), __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float>      ("infinity",  std::numeric_limits<float>::infinity(),       __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double>     ("infinity",  std::numeric_limits<double>::infinity(),      __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double>("infinity",  std::numeric_limits<long double>::infinity(), __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float>      ("+infinity",  std::numeric_limits<float>::infinity(),       __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double>     ("+infinity",  std::numeric_limits<double>::infinity(),      __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double>("+infinity",  std::numeric_limits<long double>::infinity(), __FILE__, __LINE__);

    conversionFP_FailureCheck_S2T<float>      ("-infinity",  -std::numeric_limits<float>::infinity(),       __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<double>     ("-infinity",  -std::numeric_limits<double>::infinity(),      __FILE__, __LINE__);
    conversionFP_FailureCheck_S2T<long double>("-infinity",  -std::numeric_limits<long double>::infinity(), __FILE__, __LINE__);

    std::cout << "testFloatingPointINF : T2S : has_infinity="
              << std::numeric_limits<float>::has_infinity << std::endl;
    std::string pINFresult {"inf"};
    std::string nINFresult { "-inf" };
    if constexpr (std::numeric_limits<float>::has_infinity)
    {
      conversionFP_FailureCheck_T2S<float>      ( std::numeric_limits<float>::infinity(), pINFresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<float>      (-std::numeric_limits<float>::infinity(), nINFresult, __FILE__, __LINE__);

      variantReturnType<float>  pveINF {
                                  std::in_place_index<0>,
                                  std::numeric_limits<float>::infinity()
                                };
      variantReturnType<float>  nveINF {
                                  std::in_place_index<0>,
                                  -std::numeric_limits<float>::infinity()
                                };
      conversionFP_FailureCheck_T2S<float, variantReturnType<float>>      ( pveINF, pINFresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<float, variantReturnType<float>>      ( nveINF, nINFresult, __FILE__, __LINE__);
    }

    if constexpr (std::numeric_limits<double>::has_infinity)
    {
      conversionFP_FailureCheck_T2S<double>     ( std::numeric_limits<double>::infinity(), pINFresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<double>     (-std::numeric_limits<double>::infinity(), nINFresult, __FILE__, __LINE__);

      variantReturnType<double>  pveINF {
                                  std::in_place_index<0>,
                                  std::numeric_limits<double>::infinity()
                                };
      variantReturnType<double>  nveINF {
                                  std::in_place_index<0>,
                                  -std::numeric_limits<double>::infinity()
                                };
      conversionFP_FailureCheck_T2S<double, variantReturnType<double>>      ( pveINF, pINFresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<double, variantReturnType<double>>      ( nveINF, nINFresult, __FILE__, __LINE__);
    }

    if constexpr (std::numeric_limits<long double>::has_infinity)
    {
      conversionFP_FailureCheck_T2S<long double>( std::numeric_limits<long double>::infinity(), pINFresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<long double>(-std::numeric_limits<long double>::infinity(), nINFresult, __FILE__, __LINE__);

      variantReturnType<long double>  pveINF {
                                  std::in_place_index<0>,
                                  std::numeric_limits<long double>::infinity()
                                };
      variantReturnType<long double>  nveINF {
                                  std::in_place_index<0>,
                                  -std::numeric_limits<long double>::infinity()
                                };
      conversionFP_FailureCheck_T2S<long double, variantReturnType<long double>>      ( pveINF, pINFresult, __FILE__, __LINE__);
      conversionFP_FailureCheck_T2S<long double, variantReturnType<long double>>      ( nveINF, nINFresult, __FILE__, __LINE__);
    }



  } catch (const std::exception& ex) {
    std::cout << "Test Failed : err-msg : " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
