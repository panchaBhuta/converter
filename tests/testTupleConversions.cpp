
// test conversions for 'Date'


#include <tuple>
#include <chrono>
#include <string>
#include <vector>
#include <iostream>

#include <converter/converter.h>

#include "unittest.h"

template <typename ... T>
void conversionVectorEqualCheck(const std::vector<std::string>& rowVecInput, const std::tuple<T...>& chkTuple,
                                const std::vector<std::string>* expectedOutputVector = nullptr)
{
  std::cout << "######  rowVecInput" << /*rowVecInput <<*/ std::endl;
  using t_tupleRow = std::tuple<T...>;
  t_tupleRow convTuple;
  converter::GetTuple<converter::t_S2Tconv_c<T>...>(rowVecInput, 0, convTuple);
  if(chkTuple != convTuple)
  {
    throw std::runtime_error("vector->tuple row mismatch");
  }

  std::vector<std::string> rowOutput;
  converter::SetTuple<converter::t_T2Sconv_c<T>...>(convTuple, 0, rowOutput);
  if(expectedOutputVector == nullptr)
  {
    if(rowVecInput != rowOutput)
    {
      throw std::runtime_error("tuple->vector row mismatch  : rowVecInput");
    }
    //unittest::ExpectEqual(std::vector<std::string>, rowVecInput, rowOutput);
  } else {
    if((*expectedOutputVector) != rowOutput)
    {
      throw std::runtime_error("tuple->vector row mismatch  : (*expectedOutputVector)");
    }
    //unittest::ExpectEqual(std::vector<std::string>, (*expectedOutputVector), rowOutput);
  }
}

template <typename ... T>
void conversionStringEqualCheck(const std::string& rowStrInput, const std::tuple<T...>& chkTuple,
                                const char* expectedCharOutput = nullptr)
{
  std::cout << "######  rowStrInput=" << rowStrInput << std::endl;
  using t_tupleRow = std::tuple<T...>;
  t_tupleRow convTuple = converter::ConvertFromString<T...>::ToVal(rowStrInput);
  if(chkTuple != convTuple)
  {
    throw std::runtime_error("string->tuple row mismatch");
  }
  std::string rowOutput = converter::ConvertFromTuple<T...>::ToStr(convTuple);
  if(expectedCharOutput == nullptr)
  {
    unittest::ExpectEqual(std::string, rowStrInput, rowOutput);
  } else {
    std::string expStr(expectedCharOutput);
    unittest::ExpectEqual(std::string, expStr, rowOutput);
  }
  //unittest::ExpectEqual(T, converter::ConvertFromStr<T...>::ToVal(vStr), val);
}

#if defined(__APPLE__) && defined(__MACH__)
  #if USE_FLOATINGPOINT_FROM_CHARS_1  ==  _e_ENABLE_FEATURE_ && USE_FLOATINGPOINT_TO_CHARS_1  ==  _e_ENABLE_FEATURE_
    // when compiler is GNU.
    #define FLOATINGPOINT_DISTORT 0
  #else
    // when compiler is AppleClang.
    // The macro __GNUC__is defined even for AppleClang compiler,
    // hence not able to use system dependent macro here.
    // instead using application macros USE_FLOATINGPOINT_FROM_CHARS_1 and USE_FLOATINGPOINT_TO_CHARS_1.
        // macOS does not support 'std::from_chars()' and
        // 'std::to_chars()'. The fall back functions
        // induces variations in results when compared to other OS's.
    #define FLOATINGPOINT_DISTORT 1
  #endif
#else
  #define FLOATINGPOINT_DISTORT 0
#endif
  

int main()
{
  int rv = 0;
  try {
    std::tuple<std::chrono::year_month_day,unsigned,float,double,int>
      chkTuple{std::chrono::year_month_day(std::chrono::year(2023),
                                           std::chrono::month(2),
                                           std::chrono::day(21)),
               1,2.3f,-3.4,-5};

    std::vector<std::string> inputVector{"2023-02-21", "1", "2.3", "-3.4", "-5"};

#if  FLOATINGPOINT_DISTORT == 1
    conversionStringEqualCheck<std::chrono::year_month_day,unsigned,float,double,int>
        ("2023-02-21,1,2.3,-3.4,-5", chkTuple,
         "2023-02-21,1,2.29999995,-3.39999999999999991,-5");

    std::vector<std::string> expectedOutputVector{"2023-02-21", "1", "2.29999995", "-3.39999999999999991", "-5"};
    conversionVectorEqualCheck<std::chrono::year_month_day,unsigned,float,double,int>
        (inputVector, chkTuple, &expectedOutputVector);
#else
    conversionStringEqualCheck<std::chrono::year_month_day,unsigned,float,double,int>
        ("2023-02-21,1,2.3,-3.4,-5", chkTuple);

    conversionVectorEqualCheck<std::chrono::year_month_day,unsigned,float,double,int>
        (inputVector, chkTuple);
#endif
  } catch (const std::exception& ex) {
    std::cout << "Unexpected exception in testTupleConversions: " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}