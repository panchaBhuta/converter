
// test default conversions for inbuilt-types


#include <iostream>

#include <converter/converter.h>
#include <specializedTypes/RegexString.h>
#include <specializedTypes/RegexString.hpp>

#include "unittest.h"

using t_strInteger  = specializedTypes::RegexString<"\\d+">;
using t_strEmail    = specializedTypes::RegexString<"[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}">;
using t_strISINcode = specializedTypes::RegexString<"INE\\d{3}A\\d{5}">; // INE045A01017


int main()
{
  int rv = 0;
  try {
    // Create a RegexString that expects only digits
    t_strInteger digit_string("12345"); // This will succeed
    unittest::ExpectEqual(std::string, digit_string.get(), "12345");
    //std::cout << "Digit String: " << digit_string.get() << std::endl;

    // Try to assign a non-digit string, which will throw an exception
    ExpectExceptionMsg(digit_string = "abc", std::runtime_error,
          "Assignment failed: String{abc} does not match the required regex pattern RegexString<\\d+>");

    // Create a RegexString that accepts only email addresses
    t_strEmail email_string("test@example.com"); // This will succeed
    unittest::ExpectEqual(std::string, email_string.get(), "test@example.com");

    // Try to create an invalid email string, which will throw an exception
    ExpectExceptionMsg(t_strEmail invalid_email("invalid-email"), std::runtime_error,
          "String{invalid-email} does not match the required regex pattern RegexString<[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\\.[a-zA-Z]{2,}>");


    t_strISINcode adorWeld{"INE045A01017"};
    unittest::ExpectEqual(std::string, adorWeld.get(), "INE045A01017");

    t_strISINcode isin;  // default constructor : empty value
    unittest::ExpectTrue(isin.get().empty());

    // Try to assign a empty value, which will throw an exception
    ExpectExceptionMsg(adorWeld = isin, std::runtime_error,
          "Assignment failed: other can't be empty, for regex pattern RegexString<INE\\d{3}A\\d{5}>");

    isin = adorWeld;
    unittest::ExpectEqual(t_strISINcode, adorWeld, isin);

    // Try to create an invalid ISIN code, which will throw an exception
    ExpectExceptionMsg(t_strISINcode invalid_isin("not applicable"), std::runtime_error,
          "String{not applicable} does not match the required regex pattern RegexString<INE\\d{3}A\\d{5}>");

  } catch (const std::exception& ex) {
    std::cout << ex.what() << std::endl;
    rv = 1;
  }

  return rv;
}