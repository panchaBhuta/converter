#include <cassert>
#include <iostream>
#include <string>

#ifdef _MSC_VER
#include <crtdbg.h>
#endif

int main()
{
#ifdef _MSC_VER
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
#endif

    float value = 11.0f;
    std::string strVal = std::to_string(value);
    std::cout << "value=11.0f result=[" << strVal << "]\n";

    assert(strVal == "11");

    value = 1.2345f;
    strVal = std::to_string(value);
    std::cout << "value=1.2345f result=[" << strVal << "]\n";

    assert(strVal == "1.2345");

    return 0;
}
