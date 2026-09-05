/*
 * _dateFormat.h
 *
 * URL:      https://github.com/panchaBhuta/converter
 * Version:  v1.4
 *
 * Copyright (c) 2026-2026 Gautam Dhar
 * All rights reserved.
 *
 * converter is distributed under the BSD 3-Clause license, see LICENSE for details.
 *
 */

#pragma once



#include <algorithm>
#include <chrono>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <utility>

#include <specializedTypes/CompTimeStr.h>
#include <converter/_convertDateWorkaround.h>


namespace converter
{
  namespace detail
  {
    /*
     * One xalloc() slot is used to hold the pointer to the
     * dynamically allocated format string.
     */
    inline int dateFormatIndex()
    {
      static const int index = std::ios_base::xalloc();
      return index;
    }


    /*
     * A second slot records whether the cleanup callback has
     * already been registered for a particular stream.
     */
    inline int dateFormatCallbackIndex()
    {
      static const int index = std::ios_base::xalloc();
      return index;
    }


    /*
     * Retrieve the format currently associated with the stream.
     *
     * If no format has been installed, use the default.
     */
    inline std::string_view getDateFormat(std::ostream& os)
    {
      const void* value = os.pword(dateFormatIndex());

      if (value == nullptr)
        return "{:%F}";

      return *static_cast<const std::string*>(value);
    }


    /*
     * Store a new format in the ostream.
     */
    inline void setDateFormat(
      std::ostream& os,
      std::string format)
    {
      const int index = dateFormatIndex();

      delete static_cast<std::string*>(os.pword(index));

      os.pword(index) =
        new std::string(std::move(format));
    }


    /*
     * Cleanup callback associated with the ostream.
     */
    inline void dateFormatCallback(
      std::ios_base::event event,
      std::ios_base& ios,
      int)
    {
      const int index = dateFormatIndex();

      if (event == std::ios_base::erase_event)
      {
        delete static_cast<std::string*>(ios.pword(index));
        ios.pword(index) = nullptr;
      }
      else if (event == std::ios_base::copyfmt_event)
      {
        /*
         * copyfmt() copies pword() values.

         * Therefore the copied stream would otherwise point
         * to the SAME std::string as the original stream.

         * Make an independent copy.
         */
        if (ios.pword(index) != nullptr)
        {
          ios.pword(index) =
            new std::string(
              *static_cast<std::string*>(ios.pword(index)));
        }
      }
    }


    /*
     * Register the callback once for each stream.
     */
    inline void registerDateFormatCallback(std::ostream& os)
    {
      const int callbackIndex =
        dateFormatCallbackIndex();

      if (os.iword(callbackIndex) == 0)
      {
        os.iword(callbackIndex) = 1;

        os.register_callback(
          dateFormatCallback,
          0);
      }
    }
  }


  /*
   * Persistent date-format stream manipulator.

   * Example:

   *   std::cout
   *     << converter::date_format<"{:%Y-%m-%d}">()
   *     << ymd1
   *     << ymd2
   *     << ymd3;
   */
//  template <auto Format>   -> compile error
//  template <std::size_t N, specializedTypes::CompTimeStr<N> Format>   -> compile error
  template <specializedTypes::CompTimeStr Format>
  struct date_format
  {
    friend std::ostream& operator<<(
      std::ostream& os,
      const date_format&)
    {
      detail::registerDateFormatCallback(os);

      detail::setDateFormat(
        os,
        std::string{Format.view()});

      return os;
    }
  };

  /*
   * Stream insertion operator for std::chrono::year_month_day.
   *
   * Uses the date format associated with the ostream by date_format<>.
   * If no format has been explicitly associated with the stream,
   * "{:%F}" is used.
   */
  inline std::ostream& operator<<(
    std::ostream& os,
    const std::chrono::year_month_day& ymd)
  {

//    std::cerr << "CONVERTER OPERATOR<< CALLED\n";

    const std::string format( converter::detail::getDateFormat(os) );

    /*
     * For this first test we use runtime formatting.
     *
     * The format was originally supplied as a compile-time
     * CompTimeStr, but once installed in the ostream it is
     * stored as a runtime string.
     */
    workaround::DateAdapterT2S<datelibTo::year_month_day>::toStream(os, format, ymd);  // "{:%F}" formats as "{:%Y-%m-%d}"

    return os;
  }

//  using ::operator<<; gives compile error

}

// Make converter::operator<< visible to ordinary operator lookup.
// so that std::chrono::year_month_day can use converter's
// date-formatting stream operator.
// ADL for std::chrono::year_month_day searches std/std::chrono,
// not namespace converter.
using converter::operator<<;


