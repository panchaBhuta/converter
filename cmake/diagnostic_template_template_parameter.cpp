#include <type_traits>

enum class P
{
    A,
    B
};

template <typename, auto CONV_PROCESS,
                 template <typename, decltype(CONV_PROCESS)> class CapabilityTrait>
struct Generic
{
    static constexpr bool value = true;
};

template <typename T, P V>
    requires (V == P::A)
struct CapabilityAllTypes
{
    static constexpr bool value = true;
};

using TestAllTypes = Generic<int, P::A, CapabilityAllTypes>;


  template<typename T>
  concept ct_arithmetic = std::is_arithmetic_v<T>;  // std::is_integral<T>::value || std::is_floating_point<T>::value

template <ct_arithmetic T, P V>
    requires (V == P::A)
struct CapabilityArithmTypes
{
    static constexpr bool value = true;
};

using TestArithmTypes = Generic<int, P::A, CapabilityArithmTypes>;

static_assert(TestArithmTypes::value);

int main()
{
    return 0;
}
