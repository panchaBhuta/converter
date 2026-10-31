#include <iostream>
#include <limits>
#include <charconv>
#include <array>


char* os = nullptr;
char* compiler = nullptr;

template <typename T>
void getLimits(const char* typeName, int order)
{
    std::cout
        << os << ","
        << compiler << ","
        << order << ": " << typeName << ",STRINGSTREAM,";

    std::cout << std::numeric_limits<T>::has_quiet_NaN << ",";
    if constexpr (std::numeric_limits<T>::has_quiet_NaN)
    {
        std::cout <<  std::numeric_limits<T>::quiet_NaN() << ",";
        std::cout << -std::numeric_limits<T>::quiet_NaN() << ",";
    } else
        std::cout << ",,";

    std::cout << std::numeric_limits<T>::has_signaling_NaN << ",";
    if constexpr (std::numeric_limits<T>::has_signaling_NaN)
    {
        std::cout <<  std::numeric_limits<T>::signaling_NaN() << ",";
        std::cout << -std::numeric_limits<T>::signaling_NaN() << ",";
    } else
        std::cout << ",,";

    std::cout << std::numeric_limits<T>::has_infinity << ",";
    if constexpr (std::numeric_limits<T>::has_infinity)
        std::cout <<  std::numeric_limits<T>::infinity() << ","
                  << -std::numeric_limits<T>::infinity();
    else
        std::cout << ",,";

    std::cout << "\n";

    constexpr static int charArraySize = std::numeric_limits<T>::max_digits10 + 10;
    std::array<char, charArraySize > str;
    std::to_chars_result result;
    std::cout
        << os << ","
        << compiler << ","
        << order << ": " << typeName << ",TO_CHAR,";

    std::cout << std::numeric_limits<T>::has_quiet_NaN << ",";
    if constexpr (std::numeric_limits<T>::has_quiet_NaN)
    {
        result = std::to_chars( str.data(), str.data() + str.size(),
                                std::numeric_limits<T>::quiet_NaN());
        std::cout <<  std::string(str.data(), result.ptr) << ",";
        result = std::to_chars( str.data(), str.data() + str.size(),
                               -std::numeric_limits<T>::quiet_NaN());
        std::cout <<  std::string(str.data(), result.ptr) << ",";
    } else
        std::cout << ",,";

    std::cout << std::numeric_limits<T>::has_signaling_NaN << ",";
    if constexpr (std::numeric_limits<T>::has_signaling_NaN)
    {
        result = std::to_chars( str.data(), str.data() + str.size(),
                                std::numeric_limits<T>::signaling_NaN());
        std::cout <<  std::string(str.data(), result.ptr) << ",";
        result = std::to_chars( str.data(), str.data() + str.size(),
                               -std::numeric_limits<T>::signaling_NaN());
        std::cout <<  std::string(str.data(), result.ptr) << ",";
    } else
        std::cout << ",,";

    std::cout << std::numeric_limits<T>::has_infinity << ",";
    if constexpr (std::numeric_limits<T>::has_infinity)
    {
        result = std::to_chars( str.data(), str.data() + str.size(),
                                std::numeric_limits<T>::infinity());
        std::cout <<  std::string(str.data(), result.ptr) << ",";
        result = std::to_chars( str.data(), str.data() + str.size(),
                               -std::numeric_limits<T>::infinity());
        std::cout <<  std::string(str.data(), result.ptr);
    } else
        std::cout << ",";

    std::cout << "\n";
}

#define GETLIMITS(X, Y) getLimits<X>( #X , Y )

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <OS> <compiler>" << std::endl;
        return 1;
    }

    os = argv[1];
    compiler = argv[2];

    std::cout
        << "#OS,compiler,"
        << "floatingpoint-type,conversion-mechanism,"
        << "has_quiet_nan,quiet_NaN(),-quiet_NaN(),"
        << "has_signal_nan,signaling_NaN(),-signaling_NaN(),"
        << "has_infinity,infinity(),-infinity()"
        << "\n";

    GETLIMITS(float , 1);
    GETLIMITS(double , 2);
    GETLIMITS(long double , 3);


    return 0;
}
