#include <charconv>
#include <cfloat>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <system_error>

#include <concepts>
#include <iostream>
#include <type_traits>

#include <converter/_workaroundConfig.h>


char* os = nullptr;
char* compiler = nullptr;

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

#define STRINGIFY_IMPL2(y) "-" #y
#define STRINGIFY2(x) STRINGIFY_IMPL2(x)


inline void popLastChar(std::string& str, const char FPtype)
{
  if (!str.empty() && str.back() == FPtype) str.pop_back();
}

inline void removeDoublePrefix(std::string& str)
{
  static const std::string  prefix = "double(";
  static const std::string _prefix = "-double(";
  static const std::string  suffix = "L)";

  // Ensure the string actually starts with prefix and ends with suffix
  if (str.size() >= (prefix.size() + suffix.size()) &&
      str.compare(0, prefix.size(), prefix) == 0 &&
      str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0)
  {
      str = str.substr(prefix.size(), str.size() - prefix.size() - suffix.size());
  } else
  if (str.size() >= (_prefix.size() + suffix.size()) &&
      str.compare(0, _prefix.size(), _prefix) == 0 &&
      str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0)
  {
      str = str.substr(_prefix.size(), str.size() - _prefix.size() - suffix.size());
      str = std::string{"-"} + str;
  }
}


template <typename T>
struct always_false : std::false_type {};  // usually as default for type-traits or in static_assert()

template <std::floating_point T >
size_t diagnostic_S2T_floatingPointBoundry(const std::string& fpType, const std::string& functionName,
                                           const std::string& inputOrg, const T expected )
{
  std::string input{inputOrg};
  if constexpr (std::is_same_v<T, float>)
  {
    popLastChar(input, 'F');
  } else if constexpr (std::is_same_v<T, double>)
  {
    removeDoublePrefix(input);
  } else if constexpr (std::is_same_v<T, long double>)
  {
    popLastChar(input, 'L');
  }

  constexpr size_t flag_FP_from_chars =
        (std::is_same_v<T, float>) ? 1 : 
           (
             (std::is_same_v<T, double>) ? 2 :
                (
                  (std::is_same_v<T, long double>) ? 4 : 0
                )
           );

  /*
   * Each method contributes one bit to the return value:
   *
   *   bit 0 (1): std::from_chars
   *   bit 1 (2): std::stod
   *   bit 2 (4): std::istringstream
   *
   * A bit is set when that method successfully converts the input to
   * the expected value; otherwise it is clear.
   */
  std::size_t result_from_chars = 0u;
  if constexpr (HAS_FLOATINGPOINT_FROM_CHARS & flag_FP_from_chars)
  {
    // std::from_chars
    T value_from_chars{};
    const auto [ptr, ec] = std::from_chars(
        input.data(),
        input.data() + input.size(),
        value_from_chars,
        std::chars_format::general
    );

    result_from_chars = (
        ec == std::errc{} &&
        ptr == (input.data() + input.size()) &&
        value_from_chars == expected
    ) ? 1u : 0u;
  }

  // std::stod
  std::size_t stoFP_pos{};
  std::size_t result_stoFS = 0u;

  try
  {

    T value_stoFS{0};
    if constexpr (std::is_same_v<T, float>) {
      value_stoFS = std::stof(input, &stoFP_pos);  // throws  std::invalid_argument, std::out_of_range
    } else if constexpr (std::is_same_v<T, double>) {
      value_stoFS = std::stod(input, &stoFP_pos);
    } else if constexpr (std::is_same_v<T, long double>) {
      value_stoFS = std::stold(input, &stoFP_pos);
    } else {
      static_assert(always_false<T>::value, "Unsupported type for conversion");
    }

    if (stoFP_pos == input.size() &&
        value_stoFS == expected)
    {
      result_stoFS = 1u;
    }
  }
  catch (const std::exception&)
  {
      // Conversion failure: leave result_stoFS set.
  }

  // std::istringstream
  T value_stream{};
  std::istringstream stream(input);
  stream >> value_stream;

  std::size_t result_stream = (
      !stream.fail() &&
      stream.peek() == std::char_traits<char>::eof() &&
      value_stream == expected
  ) ? 1u : 0u;

            // "#OS,compiler,float-type,"
  std::cout << os << "," << compiler << "," << flag_FP_from_chars << ": " << fpType << ","
            // "functionName,inputOriginal,inputCurated,"
            << functionName << "," << inputOrg << "," << input << ","
            // "flag_fromChars,flag_stoFP,flag_stream\n"
            << result_from_chars << "," << result_stoFS << "," << result_stream << "\n";

  return size_t(
      result_from_chars +
      2u * result_stoFS +
      4u * result_stream
  );
};


//#define GETLIMITS(X, Y) getLimits<X>( #X , Y )

int main(int argc, char* argv[])
{
  if (argc != 3)
  {
      std::cerr << "Usage: " << argv[0] << " <OS> <compiler>" << std::endl;
      return 1;
  }

  os = argv[1];
  compiler = argv[2];


/*
    std::cout
        << "#OS,compiler,float-type,"
        << "functionName,inputOriginal,inputCurated,"
        << "flag_fromChars,flag_stoFP,flag_stream\n";
*/



  size_t consolidated_result{0};

  consolidated_result = diagnostic_S2T_floatingPointBoundry<float>
                            (STRINGIFY(float),       "1: max", STRINGIFY(FLT_MAX),  std::numeric_limits<float>::max());
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<double>
                            (STRINGIFY(double),      "1: max", STRINGIFY(DBL_MAX),  std::numeric_limits<double>::max()) << 3 );
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<long double>
                            (STRINGIFY(long double), "1: max", STRINGIFY(LDBL_MAX), std::numeric_limits<long double>::max()) << 6 );
  std::cerr << std::endl << "testFloatingPointBoundary :: S2T :: std::numeric_limits<T>::max() :: " << consolidated_result << std::endl;
  std::cerr << "CONSOLIDATED_RESULT_max=" << consolidated_result << '\n';

  consolidated_result = 0;
  consolidated_result = diagnostic_S2T_floatingPointBoundry<float>
                            (STRINGIFY(float),       "2: min", STRINGIFY(FLT_MIN),  std::numeric_limits<float>::min());
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<double>
                            (STRINGIFY(double),      "2: min", STRINGIFY(DBL_MIN),  std::numeric_limits<double>::min()) << 3 );
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<long double>
                            (STRINGIFY(long double), "2: min", STRINGIFY(LDBL_MIN), std::numeric_limits<long double>::min()) << 6 );
  std::cerr << std::endl << "testFloatingPointBoundary :: S2T :: std::numeric_limits<T>::min() :: " << consolidated_result << std::endl;
  std::cerr << "CONSOLIDATED_RESULT_min=" << consolidated_result << '\n';

  consolidated_result = 0;
  consolidated_result = diagnostic_S2T_floatingPointBoundry<float>
                            (STRINGIFY(float),       "3: denorm_min", STRINGIFY(FLT_TRUE_MIN),  std::numeric_limits<float>::denorm_min());
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<double>
                            (STRINGIFY(double),      "3: denorm_min", STRINGIFY(DBL_TRUE_MIN),  std::numeric_limits<double>::denorm_min()) << 3 );
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<long double>
                            (STRINGIFY(long double), "3: denorm_min", STRINGIFY(LDBL_TRUE_MIN), std::numeric_limits<long double>::denorm_min()) << 6 );
  std::cerr << std::endl << "testFloatingPointBoundary :: S2T :: std::numeric_limits<T>::denorm_min() :: " << consolidated_result << std::endl;
  std::cerr << "CONSOLIDATED_RESULT_denorm_min=" << consolidated_result << '\n';

  consolidated_result = 0;
  consolidated_result = diagnostic_S2T_floatingPointBoundry<float>
                            (STRINGIFY(float),       "4: epsilon", STRINGIFY(FLT_EPSILON),  std::numeric_limits<float>::epsilon());
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<double>
                            (STRINGIFY(double),      "4: epsilon", STRINGIFY(DBL_EPSILON),  std::numeric_limits<double>::epsilon()) << 3 );
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<long double>
                            (STRINGIFY(long double), "4: epsilon", STRINGIFY(LDBL_EPSILON), std::numeric_limits<long double>::epsilon()) << 6 );
  std::cerr << std::endl << "testFloatingPointBoundary :: S2T :: std::numeric_limits<T>::epsilon() :: " << consolidated_result << std::endl;
  std::cerr << "CONSOLIDATED_RESULT_epsilon=" << consolidated_result << '\n';

  consolidated_result = 0;
  consolidated_result = diagnostic_S2T_floatingPointBoundry<float>
                            (STRINGIFY(float),       "5: lowest", STRINGIFY2(FLT_MAX),  std::numeric_limits<float>::lowest());
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<double>
                            (STRINGIFY(double),      "5: lowest", STRINGIFY2(DBL_MAX),  std::numeric_limits<double>::lowest()) << 3 );
  consolidated_result |= ( diagnostic_S2T_floatingPointBoundry<long double>
                            (STRINGIFY(long double), "5: lowest", STRINGIFY2(LDBL_MAX), std::numeric_limits<long double>::lowest()) << 6 );
  std::cerr << std::endl << "testFloatingPointBoundary :: S2T :: std::numeric_limits<T>::lowest() :: " << consolidated_result << std::endl;
  std::cerr << "CONSOLIDATED_RESULT_low=" << consolidated_result << '\n';

  return 0;
}
