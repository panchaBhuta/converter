#include <cassert>

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

int main() {
#if defined(_MSC_VER)
    disable_msvc_popups();
#endif

    assert(false);

    return 0;
}
