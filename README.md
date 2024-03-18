Converter
=========

[//]: # (<img src="images/RefactoringInProgress.jpg" width="500">)

[//]: # (## Documentation is **NOT** updated to latest changes.)


| **Linux** 🐧![Ubuntu](https://cdn.emojidex.com/emoji/px16/Ubuntu.png "Ubuntu")  | **Mac** ![apple logo](https://cdn.emojidex.com/emoji/px16/apple_logo.png "apple logo") | **Windows** ![windows official](https://cdn.emojidex.com/emoji/px16/windows_official.png "windows official") |
|--------------|---------|-------------|
| [![Linux](https://github.com/panchaBhuta/converter/actions/workflows/linux.yml/badge.svg)](https://github.com/panchaBhuta/converter/actions/workflows/linux.yml) | [![macOS](https://github.com/panchaBhuta/converter/actions/workflows/macos.yml/badge.svg)](https://github.com/panchaBhuta/converter/actions/workflows/macos.yml) | [![Windows](https://github.com/panchaBhuta/converter/actions/workflows/windows.yml/badge.svg)](https://github.com/panchaBhuta/converter/actions/workflows/windows.yml) |

Converter is an easy-to-use C++ `text<->type` conversion library. It supports C++20 (and
later), is header-only and comes with a basic test suite.


Supported Platforms/Compilers
=============================
Converter is implemented using C++20 with the intention of being portable. It's been tested on:


|  🖥️ OS ➡️ <br> 🤖 Compiler ⬇️ | **Ubuntu 22.04** | **macOS-13** | **macOS-12** | **macOS-11** | **Windows 10<br>VS 17 - 2022** | **Windows 10<br>VS 16 - 2019** |
|------------|------------------|--------------|--------------|--------------|-------------------------------|-------------------------------|
| **g++ 13** | ✅ | ❌1 | ✅ | NA | - | - | - |
| **g++ (11,12)** | ✅ (default:11) | ❌1 | ✅ | ✅ | - | - |
| **clang++ 15** | ✅ | - | - | - | - | - |
| **clang++ (**<span style="color:grey">12\*,13\#,14\#</span>**)** | ❌2 | - | - | - | - | - |
| **AppleClang 15** | NA | ✅ | NA | NA | NA | NA |
| **AppleClang 14** | NA | NA | ✅ | NA | NA | NA |
| **msvc 19** | NA | NA | NA | NA | ✅ (default) | NA |
| **clangCL 16** | - | - | - | - | ✅ | - |
| **clangCL 12** | - | - | - | - | - | ✅ |

❌1 : <span style="color:grey">macOS-13</span> : New linker breaks the build. Both Cmake and Linker needs an update for this fix.

❌2 : <span style="color:grey">clang++ 12\*</span> : Last successful run with [Clang 12.0.1](https://github.com/panchaBhuta/converter/actions/runs/6291468380/job/17079862439) and `OS-id-version=Linux-6.2.0-1011-azure`. Newer Ubuntu-image OS-version stopped supporting Clang-12.

❌2 : <span style="color:grey">clang++ 13\#</span> : Last successful run with [Clang 13.0.1](https://github.com/panchaBhuta/converter/actions/runs/6524732682/job/17716666880) and `OS-id-version=Linux-6.2.0-1012-azure`. **\<chorno>** headers stopped compiling in Newer Ubuntu-image OS-version. Refer [ubuntu-latest runners have an incompatible combination of clang and libstdc++](https://github.com/actions/runner-images/issues/8659).

❌2 : <span style="color:grey">clang++ 14\#</span> : Last successful run with [Clang 14.0.0](https://github.com/panchaBhuta/converter/actions/runs/6524732682/job/17716666951) and `OS-id-version=Linux-6.2.0-1012-azure`. **\<chorno>** headers stopped compiling in Newer Ubuntu-image OS-version. Refer [ubuntu-latest runners have an incompatible combination of clang and libstdc++](https://github.com/actions/runner-images/issues/8659).


[//]:  ❌


Installation
============

CMake FetchContent
------------------
Converter may be included in a CMake project using FetchContent. Refer to the
[CMake FetchContent Example Project](examples/cmake-fetchcontent) and in
particular its [CMakeLists.txt](examples/cmake-fetchcontent/CMakeLists.txt).

CMake Add-SubDirectory
----------------------
Converter may be included in a CMake project using Add-SubDirectory. Refer to the
[CMake Add-SubDirectory Example Project](examples/cmake-add-subdirectory) and in
particular its [CMakeLists.txt](examples/cmake-add-subdirectory/CMakeLists.txt).



[//]: # (TODO : add to 'vcpkg' and 'conan' packages)
[//]: # (Converter is also available via)
[//]: # ([vcpkg]\(https://vcpkg.io/en/packages.html\) and)
[//]: # ([conan]\(https://conan.io/center/converter\) )
[//]: # (package managers.)




Supported Data Types for Conversion
===================================
Floating-types, Integer-types, char-types, bool, `std::chrono::year_month_day` are the various types supported.<br>
Special types `ci_string` (i.e case-insensitive string) and `format_year_month_day<format-info>` (date-type with built-in format needed during string-conversion). <br>
Use `T ConvertFromStr<T>::ToVal(const std::string&)` for `std::string` to T conversions.<br>
Use `std::string ConvertFromVal<T>::ToStr(const T&)` for T to `std::string` conversions.<br>
The Default convertion functions, maps to best available converter functions (for a type) provided by _std_ library.

Numeric Floating-Point types
----------------------------
For floating point types, the converter S2T function calls `std::from_chars()` and the converter T2S calls `std::to_chars()`, if the compiler supports `std::*_chars` functions. The table below, is based on test results of [testFloatingPointPrecision.cpp](tests/testFloatingPointPrecision.cpp). <br>

Function Nomenclature:<br>
⚔️ : uses `std::from_chars()` and `std::to_chars()` for data conversion and has better precision. <br>
🛠️ : uses `std::from_string()` and `std::operator<<` workarounds, for data conversion. As complier doesn't support `std::*_chars()` functions. Precision is less compared to above approach. <br>

Precision Nomenclature ( for roundtrip conversions :: **string -> data-type -> string**): <br>
✅ : Good. Minimal precision loss, best of the lot.  <br>
☑️ : Average. Precision loss is non-uniform, loss can be high for some FP values.

|  🖥️ OS ➡️ <br> 🤖 Compiler ⬇️ | **Ubuntu 22.04** | **macOS-13** | **macOS-12** | **macOS-11** | **Windows 10<br>VS 17 - 2022** | **Windows 10<br>VS 16 - 2019** |
|------------|------------------|--------------|--------------|--------------|-------------------------------|-------------------------------|
| **g++ 13** | ⚔️ ✅ | ❌ | ⚔️ ✅ | NA | - | - |
| **g++ (11,12)** | ⚔️ ✅ | ❌ | ⚔️ ✅ | ⚔️ ✅ | - | - |
| **clang++ 15** | ⚔️ ✅ | - | - | - | - | - |
| **clang++ (**<span style="color:grey">12\*,13\#,14\#</span>**)** | ❌ | - | - | - | - | - |
| **AppleClang 15** | NA | 🛠️ ☑️ | ️️NA | NA | NA | NA |
| **AppleClang 14** | NA | NA | 🛠️ ☑️ | NA | NA | NA |
| **msvc 19** | NA | NA | NA | NA | ⚔️ ☑️ | NA |
| **clangCL 16** | - | - | - | - | ⚔️ ☑️ | - |
| **clangCL 12** | - | - | - | - | - | ⚔️ ☑️ |


### Data Conversion Precision

For floating-point numbers, the default decimal precision is determined by the return value of the call to `getDecimalPrecision<T>()`, this value is passed as template parameter to class-template `T2S_Format_StreamDecimalPrecision`.

```cpp
  template<c_floating_point T, int decimalPrecision = getDecimalPrecision<T>()>
  struct T2S_Format_StreamDecimalPrecision;
```

There is loss of data-precision at several places, for e.g:

```cpp
float pi_val = 3.1415926535f;                   // (1)
std::ostringstream oss;

oss << pi_val; std::string pi_str = oss.str();  // (2)

std::istringstream iss(pi_str);
float pi_read;

iss >> pi_read;                                 // (3)
```

Potential data-precision loss can happen at steps (1), (2) and (3).
The precision-loss at (1), where in rvalue-raw is captured to the nearest binary-bits equivalent
in lvalue-variable. This is system dependent.<br>
It's better to write the value of lvalue-variable(pi_val) at a
higher precision digits for float(hence +3). As we need to eliminate the
precision loss happening at steps (2) and (3).
The repeatability or read-write accuracy can only be achieved by using
higher precision of that specified by precision accuracy for float.

Refer [testFloatingPointPrecision.cpp](tests/testFloatingPointPrecision.cpp) for data-loss in **text -> data-type -> text**, conversion cycle.
Refer [testUserDefinedConverter_lowerPrecision.cpp](tests/testUserDefinedConverter_lowerPrecision.cpp),  to see the effect of lowered Decimal-Precision when writing the data and then retriving it back.



Numeric Integer types
----------------------
For integer types, the converter S2T function calls `std::from_chars()` and the converter T2S calls `std::to_chars()`.


Date types
----------
`std::chrono::year_month_day` is used for date-type.

As of writing this code, `std::chrono` is not fully supported by various OS's, in that case `converter` lib, calls [date](https://github.com/HowardHinnant/date) lib for conversions.<br>

Below table shows, the underlying conversion function(i.e between `std::chrono::*_stream()` or `date::*_stream()`) used across different OS's. Different compilers on a particular OS doesn't have any impact on this selection.<br>
For _string -> year_month_day_ conversion, refers to call to ...<br>
`std::chrono::year_month_day ConverterFromStr<std::chrono::year_month_day>::ToVal(const std::string&)`.<br>
For _year_month_day -> string_ conversion, refers to call to ...<br>
`std::string ConverterFromVal<std::chrono::year_month_day>::ToStr(const std::chrono::year_month_day&)`.<br>

⚔️ : `std::chrono::from_stream()` and `std::chrono::to_stream` are preffered.<br>
🛠️ : `date::from_stream()` and `date::to_stream` are workarounds, if `std::chrono::*` functions are not supported.<br>

|  🖥️ OS ➡️ <br> 🪄 conversion type ⬇️ | **Linux** 🐧![Ubuntu](https://cdn.emojidex.com/emoji/px16/Ubuntu.png "Ubuntu")  | **Mac** ![apple logo](https://cdn.emojidex.com/emoji/px16/apple_logo.png "apple logo") | **Windows** ![windows official](https://cdn.emojidex.com/emoji/px16/windows_official.png "windows official") |
|------------|--------------|---------|-------------|
| **string -> year_month_day** | 🛠️ date::from_stream() | 🛠️ date::from_stream() | ⚔️ **std::chrono::from_stream()** |
| **year_month_day -> string** | 🛠️ date::to_stream() | 🛠️ date::to_stream() | 🛠️ date::to_stream() |


The default date format is _"%F"_ (i.e "%Y-%m-%d"). For configuring a different date format refer [testDateConversionFormat.cpp](tests/testDateConversionFormat.cpp).



char(s) and bool types
----------------------
For `char-types`, converter interprets the cell's (first)
byte as a character. For `bool`, the expected integer values are `0` or `1`.


Locale Parsing Formats : String-to-T (i.e S2T)
----------------------------------------------
**converter** by default, uses `std::from_chars()` conversion function(s) when parsing numeric-type values from string. `std::from_chars()` donot use any locale for parsing. Though caveat observed for `macos-13` and `g++-11` combination, where in `std::from_chars()` is dependent on C-locale, refer [testUserDefinedConverter_locale.cpp](tests/testUserDefinedConverter_locale.cpp) for details.<br>
 Note that `std::from_chars()` is not supported by 'AppleClang' compiler, refer [testFloatingPointLocale.cpp](tests/testFloatingPointLocale.cpp) for interplay between user-defined-locale, C-locale and limitations of 'AppleClang' compiler.<br>


To use a particular locale for parsing numeric values, stream conversion can be used as shown below. <br>
Refer [testUserDefinedConverter_locale.cpp](tests/testUserDefinedConverter_locale.cpp) for full details.
```c++
// string literal object with static storage duration
constexpr char de_Loc[] = "de_DE.UTF-8"; // uses comma (,) as decimal separator

template<typename T>
using deLocal_iss = converter::S2T_Format_StreamUserLocale<T, converter::FailureS2Tprocess::THROW_ERROR, char, de_Loc>;

template<converter::c_floating_point T>
using ConvertFromStr_loc = converter::ConvertFromStr<T, deLocal_iss<T> >;
```
<br>

It is possible to configure **converter** to use locale dependent parsing by passing template-parameter <br>
S2T_FORMAT=`converter::S2T_Format_StreamAsIs<T, converter::FailureS2Tprocess::THROW_ERROR, char>`. <br>
Refer [testUserDefinedConverter_locale.cpp](tests/testUserDefinedConverter_locale.cpp).
```c++
template<typename T>
using convertS2T_stream =
    converter::ConvertFromStr<T,
                              converter::S2T_Format_StreamAsIs<T,
                                                               converter::FailureS2Tprocess::THROW_ERROR,
                                                               char>
                             >;
```
<br>

... or configure **converter** to use either classic-locale parsing by setting template-parameter <br>
S2T_FORMAT=`converter::S2T_Format_StreamUseClassicLocale<T, converter::FailureS2Tprocess::THROW_ERROR, char>`. <br>
Refer [testFloatingPointNaN.cpp](tests/testFloatingPointNaN.cpp) and [testIntegerNaN.cpp](tests/testIntegerNaN.cpp).
```c++
template <typename T, converter::FailureS2Tprocess EP>
using _ConvS2T_ISS = converter::ConvertFromStr<T, converter::S2T_Format_StreamUseClassicLocale<T, EP, char>>;
```

Locale Parsing Formats : T-to-String (i.e T2S)
----------------------------------------------
**converter** by default uses, `std::to_chars()` conversion functions when converting numeric-type values to string.

To use a particular locale for converting numeric values, stream conversion can be used as shown below. <br>
Refer [testUserDefinedConverter_locale.cpp](tests/testUserDefinedConverter_locale.cpp) for full details.
```c++
template<typename T>
using deLocal_oss = converter::T2S_Format_StreamUserLocale<char, de_Loc>;

template<typename T>
using combine_deLocal_oss =
    converter::T2S_Format_floating_StreamCombine< T,
                                                  converter::T2S_Format_StreamDecimalPrecision<T>,
                                                  deLocal_oss<T>
                                                >;
```

It is possible to configure **converter** to use locale dependent parsing by changing template-parameter T2S_FORMAT=`T2S_Format_StreamAsIs<char>`. <br>

... or configure **converter** to use either classic-locale parsing by setting template-parameter T2S_FORMAT=`T2S_Format_StreamUseClassicLocale<char>`. Refer [testIntegerNaN.cpp](tests/testIntegerNaN.cpp). <br>
```c++
template <typename T>
using _ConvT2S_OSS = converter::ConvertFromVal<T, converter::T2S_Format_StreamUseClassicLocale<char>>;
```

### WARNING
With floating point types `std::to_string(...)` may yield unexpected results as the number of significant digits in the returned string can be zero, for e.g: `pVal = 1e-09`. The return value may differ significantly from what `std::cout` prints by default. This particular specialization is disabled by default. In case if this is needed, enable it by defining macro `ENABLE_STD_TtoS`. <br> <br>


Custom Data Type Conversion
===========================
A different precision can be specified as below. Refer [testUserDefinedConverter_lowerPrecision.cpp](tests/testUserDefinedConverter_lowerPrecision.cpp) for more details.

```c++
template<converter::c_floating_point T>
constexpr int getLowerDecimalPrecision()
{
  if constexpr (std::is_same_v<T, float>)
    return FLT_DIG-1;
  else
  if constexpr (std::is_same_v<T, double>)
    return DBL_DIG-1;
  else
  if constexpr (std::is_same_v<T, long double>)
    return LDBL_DIG-1;
}
template<converter::c_floating_point T>
using T2S_Format_ldp = converter::T2S_Format_StreamDecimalPrecision<T, getLowerDecimalPrecision<T>() >;

template<converter::c_floating_point T>
using ConvertFromVal_lDP = converter::ConvertFromVal<T, T2S_Format_ldp<T> >;
```



Reading a String with Invalid Numbers (e.g. Empty string) as Numeric Data
=========================================================================
There are four ways to handle error during _string -> type_ conversion. This is achieved by enum `FailureS2Tprocess` and template `OnError`. Refer below.

```c++
  enum FailureS2Tprocess { THROW_ERROR = 0, SIGNAL_NAN = 1, QUIET_NAN = 2, VARIANT_NAN = 3 };

  template<typename T, FailureS2Tprocess PROCESS_ERR>
  struct OnError;
```

Note: NaN means _Not a Number_

For usage examples refer [testFloatingPointNaN.cpp](tests/testFloatingPointNaN.cpp) and [testIntegerNaN.cpp](tests/testIntegerNaN.cpp)

1. THROW_ERROR
--------------
This option throws an exception if one tries to convert non-numeric text to numeric data type, as it basically propagates the underlying conversion routine exceptions to the calling application.<br>
_THROW_ERROR_ is **default** behaviour for integer-types.<br>
Both floating-point and integer type conversions support this option.

2. SIGNAL_NAN
-------------
For floating-point types, with this option, the floating-point variable after conversion will have `std::numeric_limits<T>::signaling_NaN()` as its value.<br>
_SIGNAL_NAN_ is **default** behaviour for floating-point numbers.<br>
This is not applicable for integer types.

3. QUIET_NAN
-------------
For floating-point types, with this option, the floating-point variable after conversion will have `std::numeric_limits<T>::quite_NaN()` as its value.<br>
This is applicable for floating-point numbers.

4. VARIANT_NAN
--------------
Here instead of any type `T` a variant type `std::variant<T, std::string>`.<br>
If the conversion is succes the vriant will of type `T`, with the numeric-value that the string represents.<br>
Else on conversion-error, the variant will of type `std::string`, and value of the input-string that caused the error.<br>
Both floating-point and integer type conversions support this option.


Output String Conversion with Invalid Values/Numbers (e.g. Empty Cells) as input Numeric Data
=============================================================================================
1. For floating-point numbers
-----------------------------
A `NaN` value(both `std::numeric_limits<T>::quiet_NaN()` and `std::numeric_limits<T>::signaling_NaN()`) generally converts to a string  value 'nan'.<br>
Only exception is for _Windows_, where `std::numeric_limits<T>::signaling_NaN()` will to convert to either of string values 'nan' or 'nan(snan)'.

2. For values of type `std::variant<T, std::string>` (instead of type `T`)
--------------------------------------------------------------------------
By default, no convertion takes if data-type has error values, as it basically propagates the underlying conversion routines' exceptions to the calling application.

The reason for this is to ensure data correctness. If one wants to be able
to identify variables with invalid values/numbers(for numeric data types), one can use the below class.

```c++
  template< c_NOT_string T, typename T2S_FORMAT >
  struct ConvertFromVal<std::variant<T, std::string>, T2S_FORMAT>
  {
    inline static std::string
    ToStr(const std::variant<T, std::string>& val);
  };
```

Above Template-specializaton can be used for all types(integer, floating, bool) to write the underlying valid-data(of type `T`) or the string which raised the conversion-error.


Special types
=============
1. date-type `converter::format_year_month_day< const char* dateFMT,  converter::FailureS2Tprocess PROCESS_ERR >`
-----------------------------------------------------------------------------------------------------------------
Use case for this type: For `std::tuple<std::chrono::year_month_day,int,float,...>` conversion read-from string OR write-to string, the default conversion format for type `std::chrono::year_month_day` is "%Y-%m-%d". If date-format "%d-%b-%Y" is needed, then read-from and write-to conversions would need different type-list as indicate in comments of code-block below. Also refer [testTupleConversions.cpp](tests/testTupleConversions.cpp) for working code.

```c++
    using t_tupleRowDD = std::tuple<std::chrono::year_month_day,  // pure data type  : 1/3
                                    std::chrono::year_month_day>;
    ...
    t_tupleRowDD convTupleDD =
          converter::ConvertFromString< converter::ConvertFromStr_toDbY,  // S2T-converter type   :   2/3
                                        std::chrono::year_month_day  >::ToVal(rowDateStrInput);
    std::string convTupleDD_toStr =
          converter::ConvertFromTuple< converter::ConvertFromDbY_toStr,  // T2S-converter type    :   3/3
                                       std::chrono::year_month_day  >::ToStr(convTupleDD);
```

Instead same conversion-list across declarations and conversion is a better approach. See code below for **recommended** approach.  (NOTE :: constexpr char dbY_fmt[] = "%d-%b-%Y";)

```c++
    using t_fmtdbY = converter::format_year_month_day<converter::dbY_fmt, converter::FailureS2Tprocess::THROW_ERROR>;

    #define  TUPLE_TYPE_LIST   t_fmtdbY, std::chrono::year_month_day
    using t_tupleRowDD = std::tuple<TUPLE_TYPE_LIST>;          //    :  same  type list  1/3
    ...
    t_tupleRowDD convTupleDD =
          converter::ConvertFromString< TUPLE_TYPE_LIST >::    //    :  same  type list  2/3
                      ToVal(rowDateStrInput);
    std::string convTupleDD_toStr =
          converter::ConvertFromTuple < TUPLE_TYPE_LIST >::    //    :  same  type list  3/3
                      ToStr(convTupleDD);
```

For usage code, refer [testDateYMD_format_dbY.cpp](tests/testDateYMD_format_dbY.cpp) and [testDateYMD_format_YMD.cpp](tests/testDateYMD_format_YMD.cpp)

2. Case-Insensitive String  `ci_string`
---------------------------------------
`ci_string` type which does a case-insentive comparision. This is based on std::basic_string<...> with a different TypeTraits then that used by `std::string`. Refer [case_insensitive_string.h](include/converter/specializedTypes/case_insensitive_string.h) and [testCIstring.cpp](tests/testCIstring.cpp).


Architecture Components and Overview
====================================

   `ConvertFromStr<T, S2T_FORMAT = S2T_DefaultFormat<T>::type>::ToVal(...)` function(s) convert string to `T` data-type. <br>
   `ConvertFromVal<T, T2S_FORMAT = T2S_DefaultFormat<T>::type>::ToStr(...)` function(s) convert `T` data-type to string. <br>
   There are several specializations of `ConvertFromStr` and `ConvertFromVal`. <br>
   Each specialization is for certain types `T` determined by _`concepts`_ and format type (`S2T_FORMAT` or `T2S_FORMAT`). <br>
   Format types `S2T_FORMAT` and `T2S_FORMAT` (if needed), provide additional information for formatting for types(such as `Date`) or `Locale` specific info/params or precision for floating-point types. <br>
   In addition if multiple conversion speicalization/algorithm is available for a particular type, the alternative specialization can be selected by passing the appropriate `S2T_FORMAT`/`T2S_FORMAT`. User could also define  their own specialization of `ConvertFromStr` or `ConvertFromVal` (NOTE: new class type of `S2T_FORMAT` or `T2S_FORMAT` will also need to be defined). <br>

API Documentation
-----------------
 - [class converter::ConvertFromStr< CH, S2T_Format_WorkAround< CH, PROCESS_ERR > >](doc/converter/converter_ConvertFromStr__CH.md)
 - [class converter::ConvertFromStr< T, S2T_FORMAT_STREAM >](doc/converter/converter_ConvertFromStr__T.md)
 - [class converter::ConvertFromStr< T, S2T_Format_std_CtoT< T, PROCESS_ERR > >](doc/converter/converter_ConvertFromStr__Numeric__S2T_Format_std_CtoT.md)
 - [class converter::ConvertFromStr< T, S2T_Format_std_StoT< T, PROCESS_ERR > >](doc/converter/converter_ConvertFromStr__Numeric__S2T_Format_std_StoT.md)
 - [class converter::ConvertFromStr< bool, S2T_Format_WorkAround< bool, PROCESS_ERR > >](doc/converter/converter_ConvertFromStr__bool.md)
 - [class converter::ConvertFromStr< std::chrono::year_month_day, S2T_FORMAT_YMD >](doc/converter/converter_ConvertFromStr__std_chrono_year_month_day.md)
 - [class converter::ConvertFromString< T_C >](doc/converter/converter_Tuple_helpers.md)
 - [class converter::ConvertFromTuple< T_C >](doc/converter/converter_Tuple_helpers.md)
 - [class converter::ConvertFromVal< CH, T2S_FORMAT_STREAM >](doc/converter/converter_ConvertFromVal__CH.md)
 - [class converter::ConvertFromVal< T, T2S_Format_WorkAround >](doc/converter/converter_ConvertFromVal__T.md)
 - [class converter::ConvertFromVal< T, T2S_Format_std_TtoC >](doc/converter/converter_ConvertFromVal__Numeric__T2S_Format_std_TtoC.md)
 - [class converter::ConvertFromVal< T, T2S_Format_std_TtoS >](converter_ConvertFromVal__Numeric__T2S_Format_std_TtoS.md)
 - [class converter::ConvertFromVal< bool, T2S_Format_WorkAround >](doc/converter/converter_ConvertFromVal__bool.md)
 - [class converter::ConvertFromVal< std::chrono::year_month_day, T2S_FORMAT_YMD >](doc/converter/converter_ConvertFromVal__std_chrono_year_month_day.md)
 - [class converter::format_year_month_day< dateFMT, PROCESS_ERR >](doc/converter/specializedTypes/converter__format_year_month_day.md)
 - [class converter::ConvertFromVal< std::variant< T, std::string >, T2S_FORMAT >](doc/converter/converter_ConvertFromVal__std_variant__T.md)
 - [class converter::t_S2Tconv< T_C >](doc/converter/converter_t_S2Tconv.md)
 - [class converter::t_T2Sconv< T_C >](doc/converter/converter_t_T2Sconv.md)




Technical Details
=================
Converter uses cmake for its tests. Commands to build and execute the test suite:

```bash
mkdir -p build && cd build
cmake -DCONVERTER_BUILD_TESTS=ON .. && cmake --build .
ctest -C unit --output-on-failure && ctest -C perf --verbose
cd -
```

Converter uses [doxygenmd](https://github.com/d99kris/doxygenmd) to generate
its Markdown API documentation:

```bash
doxygenmd include/converter doc
```


License
=======
Converter is distributed under the BSD 3-Clause license. See
[LICENSE](https://github.com/panchaBhuta/converter/blob/master/LICENSE) file.

Contributions
=============
Bugs, PRs, etc are welcome on the GitHub project page
https://github.com/panchaBhuta/converter/tree/master

Keywords
========
c++, c++20, converter, string to type, type to string, single header library.

