#include <charconv>
#include <cfloat>
#include <cmath>
#include <limits>
#include <sstream>
#include <string>
#include <system_error>

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

int main()
{
    const std::string input = [] {
        std::string value = STRINGIFY(LDBL_TRUE_MIN);

        if (!value.empty() && value.back() == 'L')
            value.pop_back();

        return value;
    }();

    const long double expected =
        std::numeric_limits<long double>::denorm_min();

    /*
     * Each method contributes one bit to the return value:
     *
     *   bit 0 (1): std::from_chars
     *   bit 1 (2): std::stod
     *   bit 2 (4): std::istringstream
     *
     * A bit is clear when that method successfully converts the input to
     * the expected value; otherwise it is set.
     */

    // std::from_chars
    long double value_from_chars{};
    const auto [ptr, ec] = std::from_chars(
        input.data(),
        input.data() + input.size(),
        value_from_chars,
        std::chars_format::general
    );

    const std::size_t result_from_chars = (
        ec == std::errc{} &&
        ptr == input.data() + input.size() &&
        value_from_chars == expected
    ) ? 0u : 1u;

    // std::stod
    std::size_t stod_pos{};
    std::size_t result_stod = 1u;

    try
    {
        const double value_stod = std::stod(input, &stod_pos);

        /*
         * std::stod returns double, not long double. Compare after promoting
         * the result to long double; this succeeds only when the double
         * result equals the long-double denormal minimum exactly.
         */
        if (stod_pos == input.size() &&
            static_cast<long double>(value_stod) == expected)
        {
            result_stod = 0u;
        }
    }
    catch (const std::exception&)
    {
        // Conversion failure: leave result_stod set.
    }

    // std::istringstream
    long double value_stream{};
    std::istringstream stream(input);
    stream >> value_stream;

    std::size_t result_stream = (
        !stream.fail() &&
        stream.peek() == std::char_traits<char>::eof() &&
        value_stream == expected
    ) ? 0u : 1u;

    return static_cast<int>(
        result_from_chars +
        2u * result_stod +
        4u * result_stream
    );
}
