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
            const T& valExpected, const std::string& strRoundtripExpected,
            int decimalPrecision = std::numeric_limits<T>::digits10, char decimalSeperator = '.')
{
  T valConv = TConvertFromStr::ToVal(strInput);
  unittest::ExpectEqual(T, valExpected, valConv);

  std::string strRoundtripActual = TConvertFromVal::ToStr(valConv);

  if(strRoundtripExpected.compare(strRoundtripActual)!=0)
  {
    std::string::size_type nRdTpExp = strRoundtripExpected.find(decimalSeperator);
    std::string::size_type nRdTpAct = strRoundtripActual.find(decimalSeperator);
    if(nRdTpExp != std::string::npos)
    {
      if(nRdTpAct != std::string::npos)
      {
        //std::string subStrRdTpExp = strRoundtripExpected.substr(0,  static_cast<size_t>(static_cast<int>(nRdTpExp)+decimalPrecision));
        //std::string subStrRdTpAct = strRoundtrip.substr(0, static_cast<size_t>(static_cast<int>(nRdTpAct)+decimalPrecision));
        std::string subStrRdTpExp = strRoundtripExpected.substr(0, static_cast<size_t>(decimalPrecision)+1); // +1 for decimal-character
        std::string subStrRdTpAct = strRoundtripActual.substr(0, static_cast<size_t>(decimalPrecision)+1); // +1 for decimal-character

        unittest::ExpectEqual(std::string, subStrRdTpExp, subStrRdTpAct);
        return;
      }
    }

    std::ostringstream ossConv;
    //ossConv << "valConv{without-precision}=" << valConv << std::endl;
    ossConv << std::setprecision(std::numeric_limits<T>::digits10 + 5);
    ////oss << std::setprecision(LDBL_DIG + 5);
    //ossConv << "valConv{with-precision}=" << valConv << std::endl;
    ossConv << valConv;

    std::cout << std::setprecision(LDBL_DIG + 5) << std::boolalpha;
    std::cout << testID << " :: roundtrip conversion value does not match for type=" << typeid(T).name() << " ..." << std::endl;
    std::cout << "      input-text{" << strInput      << "} -> valConv{" << ossConv.str() << "}" << std::endl;
    std::cout << "         valConv{" << ossConv.str() << "} -> roundtrip-text{" << strRoundtripActual << "}" << std::endl;
    std::cout << "  roundtrip-text{" << strRoundtripActual << "} !=  roundtrip-expected-text{" << strRoundtripExpected << "} ?  is FALSE" << std::endl << std::endl;

    unittest::ExpectEqual(std::string, strRoundtripExpected, strRoundtripActual);
  }
}

