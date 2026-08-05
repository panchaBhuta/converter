
// test underlying Templates which are the building blocks


#include <iostream>
#include <sstream>

#include <converter/converter.h>
//#include <specializedTypes/date.h>
#include <specializedTypes/case_insensitive_string.h>

#include "unittest.h"
#include "utilities.h"




#if    USE_CHRONO_TOSTREAM_1 == 1
  #define T2S_CHRONO_YMD_STRINGSTREAM  Tn2StrConversionProcess::STRINGSTREAM COMMA
#else
  #define T2S_CHRONO_YMD_STRINGSTREAM
#endif

#if USE_FLOATINGPOINT_FROM_CHARS_1 == 1
  #define S2T_FLOATINGPOINT_FROM_CHARS  Str2TnConversionProcess::FROM_CHARS COMMA
#else
  #define S2T_FLOATINGPOINT_FROM_CHARS
#endif

#if USE_FLOATINGPOINT_TO_CHARS_1 == 1
  #define T2S_FLOATINGPOINT_TO_CHARS  Tn2StrConversionProcess::TO_CHARS COMMA
#else
  #define T2S_FLOATINGPOINT_TO_CHARS
#endif

#if USE_FLOATINGPOINT_TO_STRING_1 == 1
  #define T2S_FLOATINGPOINT_TO_STRING  Tn2StrConversionProcess::TO_STRING COMMA
#else
  #define T2S_FLOATINGPOINT_TO_STRING
#endif



using namespace converter;

template <typename T>
using t_arrConversionProcessesS2T = DefaultStr2Tn<T>::t_arrConversionProcesses;
template <typename T>
using t_arrErrorHandlers          = DefaultStr2Tn<T>::t_arrErrorHandlers;

template <typename T>
using t_arrConversionProcessesT2S = DefaultTn2Str<T>::t_arrConversionProcesses;




template <typename T>
void checkAvailableParameters(const t_arrConversionProcessesS2T<T>& valProcessesS2T,
                              const t_arrErrorHandlers<T>&          valHandlers,
                              const t_arrConversionProcessesT2S<T>& valProcessesT2S)
{
  std::cout << "Checking available parameters for type: " << typeid(T).name() << std::endl;
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


    std::cout << "floating-point's" << std::endl;
    checkAvailableParameters<float> ( { S2T_FLOATINGPOINT_FROM_CHARS
                                        Str2TnConversionProcess::S2N,
                                        Str2TnConversionProcess::STRINGSTREAM },
                                      { Str2TnErrorHandler::QUIET_NAN,
                                        Str2TnErrorHandler::SIGNAL_NAN,
                                        Str2TnErrorHandler::THROW_ERROR,
                                        Str2TnErrorHandler::VARIANT_VAL },
                                      { T2S_FLOATINGPOINT_TO_CHARS
                                        T2S_FLOATINGPOINT_TO_STRING
                                        Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<double> ( { S2T_FLOATINGPOINT_FROM_CHARS
                                         Str2TnConversionProcess::S2N,
                                         Str2TnConversionProcess::STRINGSTREAM },
                                       { Str2TnErrorHandler::QUIET_NAN,
                                         Str2TnErrorHandler::SIGNAL_NAN,
                                         Str2TnErrorHandler::THROW_ERROR,
                                         Str2TnErrorHandler::VARIANT_VAL },
                                       { T2S_FLOATINGPOINT_TO_CHARS
                                         T2S_FLOATINGPOINT_TO_STRING
                                         Tn2StrConversionProcess::STRINGSTREAM } );
    checkAvailableParameters<long double> ( { S2T_FLOATINGPOINT_FROM_CHARS
                                              Str2TnConversionProcess::S2N,
                                              Str2TnConversionProcess::STRINGSTREAM },
                                            { Str2TnErrorHandler::QUIET_NAN,
                                              Str2TnErrorHandler::SIGNAL_NAN,
                                              Str2TnErrorHandler::THROW_ERROR,
                                              Str2TnErrorHandler::VARIANT_VAL },
                                            { T2S_FLOATINGPOINT_TO_CHARS
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
