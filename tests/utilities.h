#include <iostream>
#include <cmath>
#include <typeinfo>
#include <algorithm>

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
            int decimalPrecision = std::numeric_limits<T>::digits10,
            char decimalSeperator = '.', char currencySeperator = '_')
{
  const T valConv = TConvertFromStr::ToVal(strInput);
  unittest::ExpectEqual(T, valExpected, valConv);

  const std::string strRoundtripActual = TConvertFromVal::ToStr(valConv);
  std::cout << "testID = " << testID << " , strInput = " << strInput << std::endl;
  unittest::ExpectEqual(std::string, strRoundtripExpected, strRoundtripActual);

  if(strInput.compare(strRoundtripActual)!=0)
  {
    std::string _strInput(strInput);
    std::string _strRoundtripActual(strRoundtripActual);
    if(currencySeperator != '_')
    {
      _strInput.erase(remove(_strInput.begin(), _strInput.end(), currencySeperator), _strInput.end());
      _strRoundtripActual.erase(std::remove(_strRoundtripActual.begin(), _strRoundtripActual.end(),
                 currencySeperator), _strRoundtripActual.end());
    }
    std::string::size_type decSep_input  = _strInput.find(decimalSeperator);
    std::string::size_type decSep_actual = _strRoundtripActual.find(decimalSeperator);
    if(decSep_input != std::string::npos  && decSep_actual != std::string::npos)
    {
      std::cout << "fraction check" << std::endl;
      // both input and round-trip-string have decimal seperator
      // for e.g.: '3.3123412e+38'
      if(decSep_input == decSep_actual)
      {
        // floating-point number(s) of both input and round-trip-string, their fraction part in the the same magnitude range
        // for e.g.: '3.3123412e+38' the fraction part here is '3.3123412'
        std::string significantDigits_input = _strInput.substr(0, static_cast<size_t>(decimalPrecision)+1); // +1 for decimal-character
        std::string significantDigits_actual = _strRoundtripActual.substr(0, static_cast<size_t>(decimalPrecision)+1); // +1 for decimal-character

        std::cout << "significant digits check" << std::endl;
        unittest::ExpectEqual(std::string, significantDigits_input, significantDigits_actual);

        std::istringstream iss_input(_strInput);
        std::string exponentInputStr;
        if(std::getline(iss_input, exponentInputStr, 'e') &&
           std::getline(iss_input, exponentInputStr, 'e'))  // getline() called twice since 2nd token is exponent
        {
          int inputExponent;
          std::istringstream expInput_iss(exponentInputStr);
          expInput_iss >> inputExponent;

          std::istringstream iss_actual(_strRoundtripActual);
          std::string exponentActualStr;
          if(std::getline(iss_actual, exponentActualStr, 'e') &&
             std::getline(iss_actual, exponentActualStr, 'e'))  // getline() called twice since 2nd token is exponent
          {
            int actualExponent;
            std::istringstream expActual_iss(exponentInputStr);
            expActual_iss >> actualExponent;

            std::cout << "exponent check" << std::endl;
            unittest::ExpectEqual(int, inputExponent, actualExponent);
          } else {
            std::cout << "no exponent component on converted string : _strRoundtripActual=" << _strRoundtripActual << std::endl;
          }
        } else {
          std::cout << "no exponent component on input string : _strInput=" << _strInput << std::endl;
        }
        return;
      } else {
        std::cout << "decimal position mismatch between _strInput=" << _strInput
                  << " and _strRoundtripActual=" << _strRoundtripActual << std::endl;
      }
    } else if(decSep_input == std::string::npos  && decSep_actual == std::string::npos) {
      std::cout << "integer significant-digits check" << std::endl;
      // both input and round-trip-string have decimal seperator
      // for e.g.: '8589973000'
      std::string significantDigits_input = _strInput.substr(0, static_cast<size_t>(decimalPrecision));
      std::string significantDigits_actual = _strRoundtripActual.substr(0, static_cast<size_t>(decimalPrecision));

      unittest::ExpectEqual(std::string, significantDigits_input, significantDigits_actual);
      return;
    } 

    std::ostringstream ossConv;
    //ossConv << "valConv{without-precision}=" << valConv << std::endl;
    ossConv << std::setprecision(std::numeric_limits<T>::digits10 + 5);
    ////oss << std::setprecision(LDBL_DIG + 5);
    //ossConv << "valConv{with-precision}=" << valConv << std::endl;
    ossConv << valConv;

    std::cout << std::setprecision(LDBL_DIG + 5); // << std::boolalpha;
    std::cout << testID << " :: roundtrip conversion value does not match for type=" << typeid(T).name() << " ..." << std::endl;
    std::cout << "      input-text{" << strInput      << "} -> valConv{" << ossConv.str() << "}" << std::endl;
    std::cout << "         valConv{" << ossConv.str() << "} -> roundtrip-text{" << strRoundtripActual << "}" << std::endl;
    std::cout << "      input-text{" << strInput << "} != roundtrip-text{" << strRoundtripActual << "}" << std::endl;

    //unittest::ExpectEqual(std::string, strInput, strRoundtripActual);
  }
}

