
// test underlying Templates which are the building blocks


#include <iostream>
#include <sstream>

#include <converter/converter.h>
//#include <specializedTypes/date.h>
#include <specializedTypes/case_insensitive_string.h>

#include "unittest.h"
#include "utilities.h"




#if    SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_CHRONO
  #define T2S_CHRONO_YMD_STRINGSTREAM  Tn2StrConversionProcess::STRINGSTREAM COMMA
#else
  #define T2S_CHRONO_YMD_STRINGSTREAM
#endif

#if (HAS_FLOATINGPOINT_FROM_CHARS & 1) == 1
  #define S2T_FLOATINGPOINT_FROM_CHARS_float  Str2TnConversionProcess::FROM_CHARS COMMA
#else
  #define S2T_FLOATINGPOINT_FROM_CHARS_float
#endif

#if (HAS_FLOATINGPOINT_FROM_CHARS & 2) == 2
  #define S2T_FLOATINGPOINT_FROM_CHARS_double  Str2TnConversionProcess::FROM_CHARS COMMA
#else
  #define S2T_FLOATINGPOINT_FROM_CHARS_double
#endif

#if (HAS_FLOATINGPOINT_FROM_CHARS & 4) == 4
  #define S2T_FLOATINGPOINT_FROM_CHARS_long_double  Str2TnConversionProcess::FROM_CHARS COMMA
#else
  #define S2T_FLOATINGPOINT_FROM_CHARS_long_double
#endif

#if (HAS_FLOATINGPOINT_TO_CHARS & 1) == 1
  #define T2S_FLOATINGPOINT_TO_CHARS_float  Tn2StrConversionProcess::TO_CHARS COMMA
#else
  #define T2S_FLOATINGPOINT_TO_CHARS_float
#endif

#if (HAS_FLOATINGPOINT_TO_CHARS & 2) == 2
  #define T2S_FLOATINGPOINT_TO_CHARS_double  Tn2StrConversionProcess::TO_CHARS COMMA
#else
  #define T2S_FLOATINGPOINT_TO_CHARS_double
#endif

#if (HAS_FLOATINGPOINT_TO_CHARS & 4) == 4
  #define T2S_FLOATINGPOINT_TO_CHARS_long_double  Tn2StrConversionProcess::TO_CHARS COMMA
#else
  #define T2S_FLOATINGPOINT_TO_CHARS_long_double
#endif

#if ENABLE_FLOATINGPOINT_TO_STRING == 1
  #define T2S_FLOATINGPOINT_TO_STRING  Tn2StrConversionProcess::TO_STRING COMMA
#else
  #define T2S_FLOATINGPOINT_TO_STRING
#endif



using namespace converter;

template <typename T>
using t_arrConversionProcessesS2T = typename DefaultStr2Tn<T>::t_arrConversionProcesses;
template <typename T>
using t_arrErrorHandlers          = typename DefaultStr2Tn<T>::t_arrErrorHandlers;

template <typename T>
using t_arrConversionProcessesT2S = typename DefaultTn2Str<T>::t_arrConversionProcesses;




template <typename T>
void checkAvailableParameters(const t_arrConversionProcessesS2T<T>& valProcessesS2T,
                              const t_arrErrorHandlers<T>&          valHandlers,
                              const t_arrConversionProcessesT2S<T>& valProcessesT2S)
{
  std::cout << "Checking available parameters for type: " << specializedTypes::get_name<T>() << std::endl;

  if constexpr (std::is_floating_point_v<T>)
  {
    constexpr size_t bitMask =
          std::is_same_v<T, float>       ? 1 :
          std::is_same_v<T, double>      ? 2 :
          std::is_same_v<T, long double> ? 4 :
          0;

    std::cout << "c_isFromCharsSupported<" << specializedTypes::get_name<T>() << "> = " << c_isFromCharsSupported<T>
              << " :: ((HAS_FLOATINGPOINT_FROM_CHARS & " << bitMask << ") == " << bitMask << ") = "
                   << ((HAS_FLOATINGPOINT_FROM_CHARS & bitMask) == bitMask)
              << std::endl;

    static_assert( c_isFromCharsSupported<T> !=
                      ((HAS_FLOATINGPOINT_FROM_CHARS & bitMask) == bitMask),
                   "failure in expression c_isFromCharsSupported<T> != ((HAS_FLOATINGPOINT_FROM_CHARS & bitMask) == bitMask)" );

    if constexpr (!c_isFromCharsSupported<T>)
    {
      std::cout << "isBumpedTypeS2NConversionAvailable<" << specializedTypes::get_name<T>() << ", FROM_CHARS>::value = "
                << isBumpedTypeS2NConversionAvailable<T, Str2TnConversionProcess::FROM_CHARS>::value
                << std::endl;
    }
  }

  unittest::ExpectEqual(t_arrConversionProcessesS2T<T>, DefaultStr2Tn<T>::validConversionProcesses(), valProcessesS2T);
  unittest::ExpectEqual(t_arrErrorHandlers<T>,          DefaultStr2Tn<T>::validErrorHandlers(),       valHandlers);
  unittest::ExpectEqual(t_arrConversionProcessesT2S<T>, DefaultTn2Str<T>::validConversionProcesses(), valProcessesT2S);
}





int main()
{
  int rv = 0;
  try {
    std::cout << "signed int's" << std::endl;
    checkAvailableParameters<short>(  { Str2TnConversionProcess::FROM_CHARS,
                                        Str2TnConversionProcess::S2N, // isBumpedTypeS2NConversionAvailable
                                        Str2TnConversionProcess::STRINGSTREAM },
                                      { Str2TnErrorHandler::THROW_ERROR,
                                        Str2TnErrorHandler::VARIANT_VAL },
                                      { Tn2StrConversionProcess::TO_CHARS,
                                        Tn2StrConversionProcess::TO_STRING,
                                        Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<int>  (  { Str2TnConversionProcess::FROM_CHARS,
                                        Str2TnConversionProcess::S2N,
                                        Str2TnConversionProcess::STRINGSTREAM },
                                      { Str2TnErrorHandler::THROW_ERROR,
                                        Str2TnErrorHandler::VARIANT_VAL },
                                      { Tn2StrConversionProcess::TO_CHARS,
                                        Tn2StrConversionProcess::TO_STRING,
                                        Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<long> (  { Str2TnConversionProcess::FROM_CHARS,
                                        Str2TnConversionProcess::S2N,
                                        Str2TnConversionProcess::STRINGSTREAM },
                                      { Str2TnErrorHandler::THROW_ERROR,
                                        Str2TnErrorHandler::VARIANT_VAL },
                                      { Tn2StrConversionProcess::TO_CHARS,
                                        Tn2StrConversionProcess::TO_STRING,
                                        Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<long long>(  { Str2TnConversionProcess::FROM_CHARS,
                                            Str2TnConversionProcess::S2N,
                                            Str2TnConversionProcess::STRINGSTREAM },
                                          { Str2TnErrorHandler::THROW_ERROR,
                                            Str2TnErrorHandler::VARIANT_VAL },
                                          { Tn2StrConversionProcess::TO_CHARS,
                                            Tn2StrConversionProcess::TO_STRING,
                                            Tn2StrConversionProcess::STRINGSTREAM } );

    std::cout << "unsigned int's" << std::endl;
    checkAvailableParameters<unsigned short>( { Str2TnConversionProcess::FROM_CHARS,
                                                Str2TnConversionProcess::S2N, // isBumpedTypeS2NConversionAvailable
                                                Str2TnConversionProcess::STRINGSTREAM },
                                              { Str2TnErrorHandler::THROW_ERROR,
                                                Str2TnErrorHandler::VARIANT_VAL },
                                              { Tn2StrConversionProcess::TO_CHARS,
                                                Tn2StrConversionProcess::TO_STRING,
                                                Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<unsigned int>  ( { Str2TnConversionProcess::FROM_CHARS,
                                                Str2TnConversionProcess::S2N, // isBumpedTypeS2NConversionAvailable
                                                Str2TnConversionProcess::STRINGSTREAM },
                                              { Str2TnErrorHandler::THROW_ERROR,
                                                Str2TnErrorHandler::VARIANT_VAL },
                                              { Tn2StrConversionProcess::TO_CHARS,
                                                Tn2StrConversionProcess::TO_STRING,
                                                Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<unsigned long> ( { Str2TnConversionProcess::FROM_CHARS,
                                                Str2TnConversionProcess::S2N,
                                                Str2TnConversionProcess::STRINGSTREAM },
                                              { Str2TnErrorHandler::THROW_ERROR,
                                                Str2TnErrorHandler::VARIANT_VAL },
                                              { Tn2StrConversionProcess::TO_CHARS,
                                                Tn2StrConversionProcess::TO_STRING,
                                                Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<unsigned long long>( { Str2TnConversionProcess::FROM_CHARS,
                                                    Str2TnConversionProcess::S2N,
                                                    Str2TnConversionProcess::STRINGSTREAM },
                                                  { Str2TnErrorHandler::THROW_ERROR,
                                                    Str2TnErrorHandler::VARIANT_VAL },
                                                  { Tn2StrConversionProcess::TO_CHARS,
                                                    Tn2StrConversionProcess::TO_STRING,
                                                    Tn2StrConversionProcess::STRINGSTREAM } );


    std::cout << "floating-point's  HAS_FLOATINGPOINT_FROM_CHARS=" << HAS_FLOATINGPOINT_FROM_CHARS << " , HAS_FLOATINGPOINT_TO_CHARS=" << HAS_FLOATINGPOINT_TO_CHARS << std::endl;
    checkAvailableParameters<float> ( { S2T_FLOATINGPOINT_FROM_CHARS_float
                                        Str2TnConversionProcess::S2N,
                                        Str2TnConversionProcess::STRINGSTREAM },
                                      { Str2TnErrorHandler::QUIET_NAN,
                                        Str2TnErrorHandler::SIGNAL_NAN,
                                        Str2TnErrorHandler::THROW_ERROR,
                                        Str2TnErrorHandler::VARIANT_VAL },
                                      { T2S_FLOATINGPOINT_TO_CHARS_float
                                        T2S_FLOATINGPOINT_TO_STRING
                                        Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<double> ( { S2T_FLOATINGPOINT_FROM_CHARS_double
                                         Str2TnConversionProcess::S2N,
                                         Str2TnConversionProcess::STRINGSTREAM },
                                       { Str2TnErrorHandler::QUIET_NAN,
                                         Str2TnErrorHandler::SIGNAL_NAN,
                                         Str2TnErrorHandler::THROW_ERROR,
                                         Str2TnErrorHandler::VARIANT_VAL },
                                       { T2S_FLOATINGPOINT_TO_CHARS_double
                                         T2S_FLOATINGPOINT_TO_STRING
                                         Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<long double> ( { S2T_FLOATINGPOINT_FROM_CHARS_long_double
                                              Str2TnConversionProcess::S2N,
                                              Str2TnConversionProcess::STRINGSTREAM },
                                            { Str2TnErrorHandler::QUIET_NAN,
                                              Str2TnErrorHandler::SIGNAL_NAN,
                                              Str2TnErrorHandler::THROW_ERROR,
                                              Str2TnErrorHandler::VARIANT_VAL },
                                            { T2S_FLOATINGPOINT_TO_CHARS_long_double
                                              T2S_FLOATINGPOINT_TO_STRING
                                              Tn2StrConversionProcess::STRINGSTREAM } );

    std::cout << "bool" << std::endl;
    checkAvailableParameters<bool>( { Str2TnConversionProcess::FROM_CHARS, // isBumpedTypeS2NConversionAvailable
                                      Str2TnConversionProcess::S2N, // isBumpedTypeS2NConversionAvailable
                                      Str2TnConversionProcess::STRINGSTREAM },
                                    { Str2TnErrorHandler::THROW_ERROR,
                                      Str2TnErrorHandler::VARIANT_VAL },
                                    { Tn2StrConversionProcess::TO_CHARS, // isBumpedTypeN2SConversionAvailable
                                      Tn2StrConversionProcess::TO_STRING, // isBumpedTypeN2SConversionAvailable
                                      Tn2StrConversionProcess::STRINGSTREAM } );

    std::cout << "char's" << std::endl;
    checkAvailableParameters<char>( { Str2TnConversionProcess::STRINGSTREAM,
                                      Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                    { Str2TnErrorHandler::THROW_ERROR,
                                      Str2TnErrorHandler::VARIANT_VAL },
                                    { Tn2StrConversionProcess::STRINGSTREAM,
                                      Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION } );
    checkAvailableParameters<signed char>( { Str2TnConversionProcess::STRINGSTREAM,
                                             Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                           { Str2TnErrorHandler::THROW_ERROR,
                                             Str2TnErrorHandler::VARIANT_VAL },
                                           { Tn2StrConversionProcess::STRINGSTREAM,
                                             Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION } );
    checkAvailableParameters<unsigned char>( { Str2TnConversionProcess::STRINGSTREAM,
                                               Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                             { Str2TnErrorHandler::THROW_ERROR,
                                               Str2TnErrorHandler::VARIANT_VAL },
                                             { Tn2StrConversionProcess::STRINGSTREAM,
                                               Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION } );
    checkAvailableParameters<char8_t>( { Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                       { Str2TnErrorHandler::THROW_ERROR,
                                         Str2TnErrorHandler::VARIANT_VAL },
                                       { Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION } );
    checkAvailableParameters<char16_t>( { Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                        { Str2TnErrorHandler::THROW_ERROR,
                                          Str2TnErrorHandler::VARIANT_VAL },
                                        { Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION } );
    checkAvailableParameters<char32_t>( { Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                        { Str2TnErrorHandler::THROW_ERROR,
                                          Str2TnErrorHandler::VARIANT_VAL },
                                        { Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION } );

    std::cout << "std::string" << std::endl;
    checkAvailableParameters<std::string>( { Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                           { Str2TnErrorHandler::INAPPLICABLE },
                                           { Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION } );

    std::cout << "std::wstring" << std::endl;
    checkAvailableParameters<std::wstring>( { Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                            { Str2TnErrorHandler::INAPPLICABLE },
                                            { Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION } );

    std::cout << "specializedTypes::ci_string" << std::endl;
    checkAvailableParameters<specializedTypes::ci_string>( { Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                                           { Str2TnErrorHandler::INAPPLICABLE },
                                                           { Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION } );


    std::cout << "std::chrono::year_month_day" << std::endl;
    checkAvailableParameters<std::chrono::year_month_day>( { Str2TnConversionProcess::SPECIALIZED_IMPLEMENTATION },
                                                           { Str2TnErrorHandler::THROW_ERROR, Str2TnErrorHandler::VARIANT_VAL },
                                                           { T2S_CHRONO_YMD_STRINGSTREAM
                                                             Tn2StrConversionProcess::SPECIALIZED_IMPLEMENTATION
                                                           } );


  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
