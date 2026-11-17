#include <cerrno>
#include <charconv>
#include <clocale>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <system_error>

namespace
{
constexpr const char* input =
    "3.64519953188247460252840593361941982e-4951";

void print_value(const char* label, long double value)
{
    std::cout << label << " = "
              << std::scientific
              << std::setprecision(std::numeric_limits<long double>::max_digits10)
              << value
              << " ; fpclassify = " << std::fpclassify(value)
              << " ; equals denorm_min = "
              << std::boolalpha
              << (value == std::numeric_limits<long double>::denorm_min())
              << '\n';
}

void print_limits()
{
    using limits = std::numeric_limits<long double>;

    std::cout << "numeric_limits<long double>:\n"
              << "  digits       = " << limits::digits << '\n'
              << "  min_exponent = " << limits::min_exponent << '\n'
              << "  max_exponent = " << limits::max_exponent << '\n'
              << "  has_denorm   = " << static_cast<int>(limits::has_denorm) << '\n';
    print_value("  denorm_min", limits::denorm_min());
}

void test_from_chars()
{
    long double value = 0.0L;
    const char* first = input;
    const char* last = first + std::char_traits<char>::length(first);

    const auto result = std::from_chars(first, last, value, std::chars_format::general);

    std::cout << "\nstd::from_chars:\n"
              << "  ec             = " << static_cast<int>(result.ec)
              << " (" << (result.ec == std::errc{} ? "success" :
                           result.ec == std::errc::result_out_of_range ? "result_out_of_range" :
                           result.ec == std::errc::invalid_argument ? "invalid_argument" : "other")
              << ")\n"
              << "  consumed       = " << (result.ptr - first) << " / "
              << (last - first) << '\n'
              << "  full input     = " << std::boolalpha << (result.ptr == last) << '\n';
    print_value("  result", value);
}

void test_stold()
{
    std::size_t consumed = 0;
    errno = 0;

    std::cout << "\nstd::stold:\n";
    try
    {
        const long double value = std::stold(input, &consumed);
        const int conversion_errno = errno;

        std::cout << "  consumed       = " << consumed << " / "
                  << std::char_traits<char>::length(input) << '\n'
                  << "  errno          = " << conversion_errno << '\n';
        print_value("  result", value);
    }
    catch (const std::exception& error)
    {
        std::cout << "  exception      = " << error.what() << '\n'
                  << "  consumed       = " << consumed << '\n'
                  << "  errno          = " << errno << '\n';
    }
}

void test_stringstream()
{
    std::istringstream stream(input);
    stream.imbue(std::locale::classic());

    long double value = 0.0L;
    stream >> value;

    const bool extraction_succeeded = !stream.fail();
    const bool extraction_failbit = static_cast<bool>(stream.rdstate() & std::ios::failbit);
    stream >> std::ws;
    const bool full_input = extraction_succeeded && stream.eof() && !stream.bad();

    std::cout << "\nstd::stringstream extraction:\n"
              << "  extraction ok  = " << std::boolalpha << extraction_succeeded << '\n'
              << "  full input     = " << full_input << '\n'
              << "  failbit after extraction = " << extraction_failbit << '\n';
    print_value("  result", value);
}
} // namespace

int main()
{
    std::setlocale(LC_NUMERIC, "C");

    std::cout << "Input: " << input << '\n'
              << "C++ standard: " << __cplusplus << '\n';
#ifdef __VERSION__
    std::cout << "Compiler: " << __VERSION__ << '\n';
#endif
#ifdef __GLIBCXX__
    std::cout << "libstdc++ __GLIBCXX__: " << __GLIBCXX__ << '\n';
#endif

    print_limits();
    test_from_chars();
    test_stold();
    test_stringstream();

    return 0; // This is a diagnostic: report each mechanism without masking its result.
}
