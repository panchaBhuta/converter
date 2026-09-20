#include <string>
#include <charconv>
#include <array>
#include <limits>
#include <cassert>
#include <iostream>

#if defined(_MSC_VER)
#include <crtdbg.h>
#include <iostream>
#include <stdlib.h>

void disable_msvc_popups() {
    // Disable the "Assertion Failed" dialog boxes
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

    // Prevent the Windows Error Reporting (WER) dialog for crashes
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
}
#endif

//  ./manualBuild.sh cmake check_floatingPoint_toString


int main(int, char**)
{
#if defined(_MSC_VER)
    disable_msvc_popups();
#endif

  std::array<char, std::numeric_limits<float>::digits +5 > str;  // +5 just to be on the safe side :)
  float value = 11.0f;
  std::string strVal = std::to_string(value); // std::to_string(11.0f) -> "11.0000000" is not what is expected "11"
  std::cerr << "BEFORE_ASSERT_11\n";
  assert(strVal == "11");

  value = 1.2345f;
  strVal = std::to_string(value);
  assert(strVal == "1.2345");

  return 0;
}
