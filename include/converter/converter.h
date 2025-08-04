/*
 * converter.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.3
 *
 * Copyright (c) 2023-2025 Gautam Dhar
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

#define CONVERTER_VERSION_MAJOR 1
#define CONVERTER_VERSION_MINOR 3
#define CONVERTER_VERSION_PATCH 29



/*
#define SHOW(...) \
    std::cout << std::setw(29) << #__VA_ARGS__ << " == " << __VA_ARGS__ << '\n'
// usage :: SHOW( std::is_integral_v<float> );
*/

//  Project path is removed from the __FILE__
//  Resulting file-path is relative path from project-root-folder.
#if  CONVERTER_USE_FILEPREFIXMAP == 1
  // the project-prefix-path is removed via compilation directive file-prefix-map
  #define CONVERTER_FILE    __FILE__
#else
  // https://stackoverflow.com/questions/8487986/file-macro-shows-full-path/40947954#40947954
  // the project-prefix-path is skipped by offsetting to length of project-prefix-path
  //#define CONVERTER_FILE   (__FILE__ + CONVERTER_SOURCE_PATH_SIZE)  // gives lot of warnings on windows:clangCL
  #define CONVERTER_FILE   &(__FILE__[CONVERTER_SOURCE_PATH_SIZE])
#endif

// to handle windows back-slash path seperator
#define CONVERTER_PREFERRED_PATH    std::filesystem::path(CONVERTER_FILE).make_preferred().string()


#if FLAG_CONVERTER_debug_log == 1
  #define CONVERTER_DEBUG_LOG(aMessage) { std::cout << aMessage << " :: file:" << CONVERTER_PREFERRED_PATH << ":" << __LINE__ << std::endl; }
  #define CONVERTER_DEBUG_TRY_START try {
  #define CONVERTER_DEBUG_TRY_END   }
  #define CONVERTER_DEBUG_TRY_CATCH(EXCEPTION_TYPE)                                   \
      catch(const EXCEPTION_TYPE& ex) {                                               \
        CONVERTER_DEBUG_LOG( "got-ERROR: " << ex.what() );                            \
        throw ex;                                                                     \
      }
#else
  #define CONVERTER_DEBUG_LOG(aMessage)
  #define CONVERTER_DEBUG_TRY_START
  #define CONVERTER_DEBUG_TRY_END
  #define CONVERTER_DEBUG_TRY_CATCH(EXCEPTION_TYPE)
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
    CONVERTER_VERSION_MAJOR,
    CONVERTER_VERSION_MINOR,
    CONVERTER_VERSION_PATCH
  };
}
