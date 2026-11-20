#include <charconv>
#include <cfloat>
#include <limits>
#include <string>
#include <system_error>

#define STRINGIFY_IMPL(x) #x
#define STRINGIFY(x) STRINGIFY_IMPL(x)

int main()
{
    std::string input = STRINGIFY(LDBL_TRUE_MIN);

    if (!input.empty() && input.back() == 'L')
        input.pop_back();

    long double value{};

    auto [ptr, ec] = std::from_chars(
        input.data(),
        input.data() + input.size(),
        value,
        std::chars_format::general
    );

    return (
        ec == std::errc{} &&
        ptr == input.data() + input.size() &&
        value == std::numeric_limits<long double>::denorm_min()
    ) ? 0 : 2;   //  returns DENORM_DB_VARIDX  used in testFloatingPointBoundary.cpp
}