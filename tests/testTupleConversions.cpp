
// test conversions for 'Date'


#include <tuple>
#include <chrono>
#include <string>
#include <iostream>

#include <converter/converter.h>

#include "unittest.h"

template <typename ... T>
void conversionEqualCheck(const std::string& rowInput)
{
  std::cout << "######  rowInput=" << rowInput << std::endl;
  using t_tupleRow = std::tuple<T...>;
  t_tupleRow tup = converter::ConvertFromString<T...>::ToVal(rowInput);
  std::string rowOutput = converter::ConvertFromTuple<T...>::ToStr(tup);
  unittest::ExpectEqual(std::string, rowInput, rowOutput);
  //unittest::ExpectEqual(T,           converter::ConvertFromStr<T...>::ToVal(vStr), val);
}

int main()
{
  int rv = 0;
  try {
    conversionEqualCheck<std::chrono::year_month_day,unsigned,float,double,int>("2023-02-21,1,2.3,-3.4,-5");
  } catch (const std::exception& ex) {
    std::cout << "Unexpected exception in testTupleConversions: " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}