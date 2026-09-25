/*
 * disable_msvc_popups.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v2.4
 *
 * Copyright (c) 2023-2026 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */



#pragma once

#if defined(_MSC_VER)

#include <crtdbg.h>
#include <stdlib.h>

namespace commonUtils
{

  /*
   * when calls to assert() fails on github runner,
   * the unit-tests hangs as there is no one to click on
   * the dialog boxes on msvc builds. This method disables
   * msvc popups.
   *
   * USAGE : add following lines to main()
   *
   *  #if defined(_MSC_VER)
   *   commonUtils::disable_msvc_popups();
   *  #endif
   *
   */
  void disable_msvc_popups() {
    // Disable the "Assertion Failed" dialog boxes
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ASSERT, _CRTDBG_FILE_STDERR);
    
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_FILE);
    _CrtSetReportFile(_CRT_ERROR, _CRTDBG_FILE_STDERR);

    // Prevent the Windows Error Reporting (WER) dialog for crashes
    _set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
  }

}


#endif
