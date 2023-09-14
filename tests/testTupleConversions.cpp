
// test conversions for 'Date'


#include <tuple>
#include <chrono>
#include <string>
#include <iostream>

#include <converter/converter.h>

#include "unittest.h"

template <typename ... T>
void conversionEqualCheck(const std::string& rowInput, const std::tuple<T...>& chkTuple,
                          const char* expectedCharOutput = nullptr)
{
  std::cout << "######  rowInput=" << rowInput << std::endl;
  using t_tupleRow = std::tuple<T...>;
  t_tupleRow convTuple = converter::ConvertFromString<T...>::ToVal(rowInput);
  if(chkTuple != convTuple)
  {
    throw std::runtime_error("tuple row mismatch");
  }
  std::string rowOutput = converter::ConvertFromTuple<T...>::ToStr(convTuple);
  if(expectedCharOutput == nullptr)
  {
    unittest::ExpectEqual(std::string, rowInput, rowOutput);
  } else {
    std::string expStr(expectedCharOutput);
    unittest::ExpectEqual(std::string, rowInput, expStr);
  }
  //unittest::ExpectEqual(T, converter::ConvertFromStr<T...>::ToVal(vStr), val);
}

int main()
{
  int rv = 0;
  try {
    std::tuple<std::chrono::year_month_day,unsigned,float,double,int>
      chkTuple{std::chrono::year_month_day(std::chrono::year(2023),
                                           std::chrono::month(2),
                                           std::chrono::day(21)),
              1,2.3f,-3.4,-5};
#if defined(__APPLE__) && defined(__MACH__)
    conversionEqualCheck<std::chrono::year_month_day,unsigned,float,double,int>
        ("2023-02-21,1,2.3,-3.4,-5", chkTuple,
         "2023-02-21,1,2.29999995,-3.39999999999999991,-5");
#else
    conversionEqualCheck<std::chrono::year_month_day,unsigned,float,double,int>
        ("2023-02-21,1,2.3,-3.4,-5", chkTuple);
#endif
  } catch (const std::exception& ex) {
    std::cout << "Unexpected exception in testTupleConversions: " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}