#include <type_traits>

enum class P
{
    A,
    B
};

template <
    typename,
    auto CONV_PROCESS,
    template <typename, decltype(CONV_PROCESS)> class CapabilityTrait
>
struct Generic
{
    static constexpr bool value = true;
};


// 1. No constraints
template <typename T, P V>
struct CapabilityAllTypes
{
    static constexpr bool value = true;
};

using TestAllTypes = Generic<int, P::A, CapabilityAllTypes>;


// 2. requires constraint on enum parameter
template <typename T, P V>
    requires (V == P::A)
struct CapabilityAllTypesResEnm
{
    static constexpr bool value = true;
};

using TestAllTypesResEnm =
    Generic<int, P::A, CapabilityAllTypesResEnm>;


// 3. Concept constraint on T
template<typename T>
concept ct_arithmetic = std::is_arithmetic_v<T>;
// std::is_integral<T>::value || std::is_floating_point<T>::value

template <ct_arithmetic T, P V>
struct CapabilityArithmTypes
{
    static constexpr bool value = true;
};

using TestArithmTypes =
    Generic<int, P::A, CapabilityArithmTypes>;


// 4. Concept constraint on T + requires constraint on enum parameter
template <ct_arithmetic T, P V>
    requires (V == P::A)
struct CapabilityArithmTypesResEnm
{
    static constexpr bool value = true;
};

using TestArithmTypesResEnm =
    Generic<int, P::A, CapabilityArithmTypesResEnm>;


static_assert(TestAllTypes::value);
static_assert(TestAllTypesResEnm::value);
static_assert(TestArithmTypes::value);
static_assert(TestArithmTypesResEnm::value);


int main()
{
    return 0;
}
