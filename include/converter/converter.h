/*
 * converter.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.2
 *
 * Copyright (c) 2023-2023 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#pragma once

#include <exception>
#include <functional>
#include <filesystem>
#include <iostream>
#include <cfloat>
#include <climits>
#include <cmath>
#include <limits>
#include <type_traits>
#include <string>
#include <sstream>
#include <iomanip>
#include <utility>
#include <variant>
#include <locale>
#include <stdexcept>

#define CONVERTER__VERSION_MAJOR 1
#define CONVERTER__VERSION_MINOR 2
#define CONVERTER__VERSION_PATCH 2



/*
#define SHOW(...) \
    std::cout << std::setw(29) << #__VA_ARGS__ << " == " << __VA_ARGS__ << '\n'
// usage :: SHOW( std::is_integral_v<float> );
*/

//  Project path is removed from the __FILE__
//  Resulting file-path is relative path from project-root-folder.
#if  USE_FILEPREFIXMAP == 1
  // the project-prefix-path is removed via compilation directive file-prefix-map
  #define __CONVERTER_FILE__    __FILE__
#else
  // https://stackoverflow.com/questions/8487986/file-macro-shows-full-path/40947954#40947954
  // the project-prefix-path is skipped by offsetting to length of project-prefix-path
  #define __CONVERTER_FILE__   (__FILE__ + CONVERTER_SOURCE_PATH_SIZE)
#endif

// to handle windows back-slash path seperator
#define __CONVERTER_PREFERRED_PATH__    std::filesystem::path(__CONVERTER_FILE__).make_preferred().string()


#ifdef ENABLE_CONVERTER_DEBUG_LOG
#define CONVERTER_DEBUG_LOG(aMessage) { std::cout << aMessage << " :: file:" << __CONVERTER_PREFERRED_PATH__ << ":" << __LINE__ << std::endl; }
#else
#define CONVERTER_DEBUG_LOG(aMessage)
#endif



#include <converter/_workaroundConfig.h>
#include <converter/_common.h>
#include <converter/_convertS2T.h>
#include <converter/_convertT2S.h>
#include <converter/_convertDate.h>
#include <converter/_convertTuple.h>
#include <converter/_convertC2T.h>
#include <converter/_convertT2C.h>


namespace converter
{
  /**
   * @brief     Class representing Version number of the project.
  */
  static constexpr struct {
    uint8_t major, minor, patch;
  } version = {
    CONVERTER__VERSION_MAJOR,
    CONVERTER__VERSION_MINOR,
    CONVERTER__VERSION_PATCH
  };
}
