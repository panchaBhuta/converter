#include <iostream>
#include <cmath>
#include <typeinfo>

#include "unittest.h"

#include <converter/converter.h>


#pragma once


#define GET_TYPENAME(T) #T

template<typename T,
         typename TConvertFromStr = converter::ConvertFromStr<T>,
        typename TConvertFromVal = converter::ConvertFromVal<T> >
void checkRoundTripConversion_txt2Val2txt( const std::string& testID,
            const std::string& strInput,
            const T& valExpected, const std::string& strRoundtripExpected)
{
  T valConv = TConvertFromStr::ToVal(strInput);
  unittest::ExpectEqual(T, valExpected, valConv);

  std::string strRoundTrip = TConvertFromVal::ToStr(valConv);

  if(strRoundtripExpected.compare(strRoundTrip)!=0)
  {
    std::ostringstream ossConv;
    ossConv << std::setprecision(std::numeric_limits<T>::digits10 + 5);
    //oss << std::setprecision(LDBL_DIG + 5);
    ossConv << valConv;

    std::cout << std::setprecision(LDBL_DIG + 5) << std::boolalpha;
    std::cout << testID << " :: roundtrip conversion value does not match for type=" << typeid(T).name() << " ..." << std::endl;
    std::cout << "      input-text{" << strInput      << "} -> valConv{" << ossConv.str() << "}" << std::endl;
    std::cout << "         valConv{" << ossConv.str() << "} -> roundtrip-text{" << strRoundTrip << "}" << std::endl;
    std::cout << "  roundtrip-text{" << strRoundTrip << "} !=  roundtrip-expected-text{" << strRoundtripExpected << "} ?  is FALSE" << std::endl << std::endl;
  }
}

