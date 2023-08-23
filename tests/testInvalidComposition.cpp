
// test locale for conversions


#include <iostream>
#include <cmath>

#include <converter/converter.h>

#include "unittest.h"
#include "utilities.h"




// string literal object with static storage duration
constexpr char de_Loc[] = "de_DE"; // uses comma (,) as decimal separator

template<typename T>
using deLocal_iss = converter::S2T_Format_StreamUserLocale<T, converter::FailureS2Tprocess::THROW_ERROR, char, de_Loc>;




template<typename T>
using deLocal_oss = converter::T2S_Format_StreamUserLocale<char, de_Loc>;

template<typename T>
using combine_deLocal_oss =
    converter::T2S_Format_floating_StreamCombine< T,
                                                  converter::T2S_Format_StreamDecimalPrecision<T>,
                                                  deLocal_oss<T>
                                                  >;




template<converter::c_floating_point T>
using ConvertFromStr_loc = converter::ConvertFromStr<T, combine_deLocal_oss<T> >;

template<converter::c_floating_point T>
using ConvertFromVal_loc = converter::ConvertFromVal<T, deLocal_iss<T> >;



int main()
{
  int rv = 0;

  float uid1 = ConvertFromStr_loc<float>::template_uid;
  float uid2 = ConvertFromVal_loc<float>::template_uid;
  //ConvertFromStr_loc<float>::instanceEvaluater();


  return rv;
}