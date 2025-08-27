/*
 * RegexString.hpp
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.4
 *
 * Copyright (c) 2023-2025 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#pragma once

#include <string>
#include <regex>

#include <specializedTypes/CompTimeStr.h>
#include <specializedTypes/RegexString.h>


/*
 * include this file "RegexString.hpp" in only ONE of the the projects ".cpp" file, probably "main.cpp".
 * As the definition below needs to exist in ONE binary file to satisfy ...
 * One Definition Rule (ODR): Defining the static member in a single ".cpp" file
 * ensures compliance with the ODR, preventing linker errors.
 */

namespace specializedTypes
{
  template<CompTimeStr CTS>
  const std::regex RegexString<CTS>::_regex_pattern{CTS.data};
}
