
// test conversions for 'Date'


#include <tuple>
#include <chrono>
#include <string>
#include <vector>
#include <iostream>

#include <converter/converter.h>
#include <specializedTypes/date.h>
#include <specializedTypes/case_insensitive_string.h>

#include "unittest.h"

template <typename ... T>
void conversionVectorEqualCheck(const std::vector<std::string>& rowVecInput, const std::tuple<T...>& chkTuple,
                                const std::vector<std::string>* expectedOutputVector = nullptr)
{
  std::cout << "######  rowVecInput" << /*rowVecInput <<*/ std::endl;
  using t_tupleRow = std::tuple<T...>;
  t_tupleRow convTuple;
  converter::GetTuple<converter::t_S2Tconv_c<T>...>(rowVecInput, convTuple);
  if(chkTuple != convTuple)
  {
    throw std::runtime_error("vector->tuple row mismatch");
  }

  std::vector<std::string> rowOutput;
  converter::SetTuple<converter::t_T2Sconv_c<T>...>(convTuple, rowOutput);
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
  std::string convTuple_toStr = converter::ConvertFromTuple<T...>::ToStr(convTuple);
  std::cout << "++++++  convTuple_toStr=" << convTuple_toStr << std::endl;
  std::string chkTuple_toStr = converter::ConvertFromTuple<T...>::ToStr(chkTuple);
  std::cout << "++++++  chkTuple_toStr=" << chkTuple_toStr << std::endl;
  if(expectedCharOutput == nullptr)
  {
    unittest::ExpectEqual(std::string, rowStrInput, convTuple_toStr);
  } else {
    std::string expStr(expectedCharOutput);
    unittest::ExpectEqual(std::string, expStr, convTuple_toStr);
  }
  if(chkTuple != convTuple)
  {
    throw std::runtime_error("(chkTuple != convTuple) row mismatch");
  }
}

#if defined(__APPLE__) && defined(__MACH__)
  #if USE_FLOATINGPOINT_FROM_CHARS_1  ==  e_ENABLE_FEATURE && USE_FLOATINGPOINT_TO_CHARS_1  ==  e_ENABLE_FEATURE
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


using t_fmtdbY = specializedTypes::format_year_month_day<converter::dbY_fmt, converter::FailureS2Tprocess::THROW_ERROR>;
using t_fmtYMD = specializedTypes::format_year_month_day<converter::defYMDfmt, converter::FailureS2Tprocess::THROW_ERROR>;



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


    std::tuple<std::string, specializedTypes::ci_string, std::string>
      chkStrTuple { "PRIVATE", "limited", "CoMpAnY"};

    std::vector<std::string> inputStrVector {"PRIVATE", "limited", "CoMpAnY"};
    conversionStringEqualCheck<std::string, specializedTypes::ci_string, std::string>
        ("PRIVATE,limited,CoMpAnY", chkStrTuple);
    conversionVectorEqualCheck<std::string, specializedTypes::ci_string, std::string>
        (inputStrVector, chkStrTuple);



    std::tuple<std::chrono::year_month_day, t_fmtdbY, t_fmtYMD>
      chkTuple2{std::chrono::year_month_day(std::chrono::year(2023),
                                            std::chrono::month(2),
                                            std::chrono::day(21)),
                t_fmtdbY( std::chrono::year(2023),
                          std::chrono::month(2),
                          std::chrono::day(22)),
                t_fmtYMD( std::chrono::year(2023),
                          std::chrono::month(2),
                          std::chrono::day(23))};

    std::vector<std::string> inputVector2{"2023-02-21", "22-Feb-2023", "2023-02-23"};

    conversionStringEqualCheck<std::chrono::year_month_day, t_fmtdbY, t_fmtYMD>
        ("2023-02-21,22-Feb-2023,2023-02-23", chkTuple2);

    conversionVectorEqualCheck<std::chrono::year_month_day, t_fmtdbY, t_fmtYMD>
        (inputVector2, chkTuple2);

    // [============[  NOT recommended : use type  t_fmtdbY
    using t_tupleRowDD = std::tuple<std::chrono::year_month_day,  // pure data type
                                    std::chrono::year_month_day>;
    t_tupleRowDD chkTupleDD{  std::chrono::year_month_day(std::chrono::year(2023),
                                                          std::chrono::month(2),
                                                          std::chrono::day(21)),
                              std::chrono::year_month_day(std::chrono::year(2023),
                                                          std::chrono::month(2),
                                                          std::chrono::day(22))};

    std::string rowDateStrInput {"21-Feb-2023,2023-02-22"};
    std::cout << "######  rowDateStrInput=" << rowDateStrInput << std::endl;
    t_tupleRowDD convTupleDD =
          converter::ConvertFromString< converter::ConvertFromStr_toDbY,  // S2T-converter type
                                        std::chrono::year_month_day  >::ToVal(rowDateStrInput);
    std::string convTupleDD_toStr =
          converter::ConvertFromTuple< converter::ConvertFromDbY_toStr,  // T2S-converter type
                                       std::chrono::year_month_day  >::ToStr(convTupleDD);
    std::cout << "++++++  convTupleDD_toStr=" << convTupleDD_toStr << std::endl;

    std::string chkTupleDD_toStr =
          converter::ConvertFromTuple< converter::ConvertFromDbY_toStr,  // T2S-converter type
                                       std::chrono::year_month_day  >::ToStr(chkTupleDD);
    std::cout << "++++++  chkTupleDD_toStr=" << chkTupleDD_toStr << std::endl;
    unittest::ExpectEqual(std::string, rowDateStrInput, convTupleDD_toStr);
    if(chkTupleDD != convTupleDD)
    {
      throw std::runtime_error("(chkTuple != convTuple) row mismatch");
    }
    // ]============]  NOT recommended : use type  t_fmtdbY


  } catch (const std::exception& ex) {
    std::cout << "Unexpected exception in testTupleConversions: " << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}
