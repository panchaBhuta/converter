// testFloatingPointLocale.cpp - numeric c locale

#include <clocale>
#include <converter/converter.h>

#include "unittest.h"

template<typename T>
using convertS2T_stream =
    converter::ConvertFromStr<T,
                              converter::S2T_Format_StreamAsIs<T,
                                                               converter::FailureS2Tprocess::THROW_ERROR,
                                                               char>
                             >;

template<typename T>
using convertS2T_streamClassic =
    converter::ConvertFromStr<T,
                              converter::S2T_Format_StreamUseClassicLocale< T,
                                                                            converter::FailureS2Tprocess::THROW_ERROR,
                                                                            char>
                             >;


template<typename T>
using convertS2T_StoT =
    converter::ConvertFromStr<T,
                              converter::S2T_Format_std_StoT<T,
                                                             converter::FailureS2Tprocess::THROW_ERROR>
                             >;



static constexpr char loc[] = "de_DE.UTF-8"; // uses comma (,) as decimal separator

template<typename T>
using deLocal_iss = converter::S2T_Format_StreamUserLocale<T, converter::FailureS2Tprocess::THROW_ERROR, char, loc>;

template<typename T>
using convertS2T_userLocale =
    converter::ConvertFromStr<T, deLocal_iss<T> >;



int main()
{
  int rv = 0;
  bool testUserLocale = true;

  try {
    std::istringstream iss;
    deLocal_iss<float>::streamUpdate(iss);
  }
  catch (const std::exception& ex)
  {
    std::cout << "#1# locale " << loc << " not available (" << ex.what()
              << "), skipping userlocale-checks.\n";
    // pass test for systems without locale present. for ci testing, make.sh
    // ensures that the necessary locale is installed.
    testUserLocale = false;
  }

  try {
    {
      unittest::ExpectEqual(float, converter::ConvertFromStr<float>::ToVal("0.1"), 0.1f);

      unittest::ExpectEqual(float, convertS2T_stream<float>::ToVal("0.2"), 0.2f);

      unittest::ExpectEqual(float, convertS2T_streamClassic<float>::ToVal("0.3"), 0.3f);

      unittest::ExpectEqual(float, convertS2T_StoT<float>::ToVal("0.4"), 0.4f);  //  <<  decimal-point
    }

    if(testUserLocale) {
      unittest::ExpectEqual(float, convertS2T_userLocale<float>::ToVal("0,5"), 0.5f);  //  <<  comma
    }
  } catch (const std::exception& ex) {
    std::cout << "#2# " << ex.what() << std::endl;
  }

  try {
    if (std::setlocale(LC_ALL, loc) == nullptr) {
      std::cout << "locale " << loc << " not available, skipping user-locale test.\n";
      // pass test for systems without locale present. for ci testing, make.sh
      // ensures that the necessary locale is installed.
      return 0;
    }
  } catch (const std::exception& ex) {
    std::cout << "#3# locale " << loc << " not available(" << ex.what()
              << "), skipping user-locale test.\n";
    return 0;
  }

  try {
#if USE_FLOATINGPOINT_FROM_CHARS_1  ==  e_ENABLE_FEATURE
    static_assert(converter::ConvertFromStr<float>::template_uid ==  103);
  #if defined(__APPLE__) && defined(__MACH__) && __GNUC__ == 11
    // NOTE : DEPENDENT ion system-locale, this is a BUG in (macOS:g++11) combination
    unittest::ExpectEqual(float, converter::ConvertFromStr<float>::ToVal("0,11"), 0.11f);
  #else
    // independent of system-locale
    unittest::ExpectEqual(float, converter::ConvertFromStr<float>::ToVal("0.11"), 0.11f);
  #endif
#else
    static_assert(converter::ConvertFromStr<float>::template_uid ==  3);
    // for compiler 'AppleClang'
    // as converter::S2T_Format_std_StoT<T,...> is fall-back and this is dependent on C locale
    unittest::ExpectEqual(float, converter::ConvertFromStr<float>::ToVal("0,12"), 0.12f);  //  <<  comma
#endif

    // independent of system-locale
    unittest::ExpectEqual(float, convertS2T_stream<float>::ToVal("0.2"), 0.2f);

    // independent of system-locale
    unittest::ExpectEqual(float, convertS2T_streamClassic<float>::ToVal("0.3"), 0.3f);

    // dependent on system-locale
    unittest::ExpectEqual(float, convertS2T_StoT<float>::ToVal("0,4"), 0.4f);  //  <<  comma
  } catch (const std::exception& ex) {
    std::cout << "#4# " << ex.what() << std::endl;
    rv = 1;
  }


  return rv;
}

