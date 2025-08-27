/*
 * RegexString.h
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

#include <iostream>
#include <sstream>
#include <string>
#include <regex>
#include <stdexcept> // For std::runtime_error

#include <specializedTypes/CompTimeStr.h>


/*
 * include the file "RegexString.hpp" in only ONE of the the projects ".cpp" file, probably "main.cpp".
 * As the definition below needs to exist in ONE binary file to satisfy ...
 * One Definition Rule (ODR): Defining the static member in a single ".cpp" file
 * ensures compliance with the ODR, preventing linker errors.
 * 
 * 
 *  template<CompTimeStr CTS>
 *  const std::regex RegexString<CTS>::_regex_pattern{CTS.data};
 */

namespace specializedTypes
{
  template<CompTimeStr CTS>   // CTS = const std::string& pattern
  class RegexString
  {
  public:
    // Constructor with string and regex pattern
    explicit RegexString(const std::string& str)
      : _value{""}
    {
      if (!std::regex_match(str, _regex_pattern))
      {
        std::ostringstream oss;
        oss << "String{" << str << "} does not match the required regex pattern RegexString<" << CTS.data << ">";
        throw std::runtime_error(oss.str());
      }
      _value = str;
    }

    // default constructor
    RegexString()
      : _value{""}
    {}
    // copy constructor
    RegexString(const RegexString<CTS>& other) = default;
    // Equality Operator
    bool operator==(const RegexString<CTS>& other) const = default;

    // Constructor with only a string, assumes a default regex (optional)
    //explicit RegexString(const std::string& str) : RegexString(str, ".*") {} // Default regex to accept any string

    // Assignment operator
    RegexString& operator=(const std::string& new_str)
    {
      if (!std::regex_match(new_str, _regex_pattern))
      {
        std::ostringstream oss;
        oss << "Assignment failed: String{" << new_str << "} does not match the required regex pattern RegexString<" << CTS.data << ">";
        throw std::runtime_error(oss.str());
      }
      _value = new_str;
      return *this;
    }

    // Assignment operator
    RegexString& operator=(const RegexString<CTS>& other)
    {
      if (other.get().empty())
      {
        std::ostringstream oss;
        oss << "Assignment failed: other can't be empty, for regex pattern RegexString<" << CTS.data << ">";
        throw std::runtime_error(oss.str());
      }
      _value = other._value;
      return *this;
    }

    // Accessor for the string value
    const std::string& get() const { return _value; }

  private:
    std::string _value;
    const static std::regex _regex_pattern;
  };
}

  /*
    *  https://stackoverflow.com/questions/5195512/namespaces-and-operator-resolution
    *  refer URL^  for   "Namespaces and operator resolution"  for eg: 'operator<<'
  */
  // Definition of the overloaded operator<<
  template<specializedTypes::CompTimeStr CTS>   // CTS = const std::string& pattern
  inline std::ostream& operator<<(
              std::ostream& os,
              const specializedTypes::RegexString<CTS>& obj)
  {
    return (os << obj.get());
  }

namespace specializedTypes
{
  using ::operator<<;
}
