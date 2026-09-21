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
//
// MSVC 19.51 rejects this constrained template as a template-template
// argument for Generic::CapabilityTrait with C3201.
template <typename T, P V>
    requires (V == P::A)
struct CapabilityAllTypesResEnm
{
    static constexpr bool value = true;
};

// Expected to fail with MSVC 19.51 (C3201).
// using TestAllTypesResEnm =
//     Generic<int, P::A, CapabilityAllTypesResEnm>;


// 3. Concept constraint on T
template<typename T>
concept ct_arithmetic = std::is_arithmetic_v<T>;
// std::is_integral<T>::value || std::is_floating_point<T>::value

template <ct_arithmetic T, P V>
struct CapabilityArithmTypes
{
    static constexpr bool value = true;
};

// Expected to fail with MSVC 19.51 (C3201).
// using TestArithmTypes =
//     Generic<int, P::A, CapabilityArithmTypes>;


// 4. Concept constraint on T + requires constraint on enum parameter
template <ct_arithmetic T, P V>
    requires (V == P::A)
struct CapabilityArithmTypesResEnm
{
    static constexpr bool value = true;
};

// Expected to fail with MSVC 19.51 (C3201).
// using TestArithmTypesResEnm =
//     Generic<int, P::A, CapabilityArithmTypesResEnm>;


// The unconstrained case is expected to compile.
// The remaining assertions are commented out because their corresponding
// template arguments are rejected by MSVC 19.51 before the assertion
// can be evaluated.
static_assert(TestAllTypes::value);

// static_assert(TestAllTypesResEnm::value);
// static_assert(TestArithmTypes::value);
// static_assert(TestArithmTypesResEnm::value);


// 5. Outer requires constraint, unconstrained CapabilityTrait
template <
    typename,
    auto CONV_PROCESS,
    template <typename, decltype(CONV_PROCESS)> class CapabilityTrait
>
    requires (CONV_PROCESS == P::A)
struct SpecificUnconstrainedTrait
{
    static constexpr bool value = true;
};

using TestSpecificUnconstrainedTrait =
    SpecificUnconstrainedTrait<int, P::A, CapabilityAllTypes>;

static_assert(TestSpecificUnconstrainedTrait::value);


// 6. Constrained template-template parameter, no outer requires
template <
    typename,
    auto CONV_PROCESS,
    template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
>
struct SpecificConstrainedTrait
{
    static constexpr bool value = true;
};


// Expected to fail with MSVC 19.51 (C3201) because the
// template-template parameter itself is constrained.
// using TestSpecificConstrainedTrait =
//     SpecificConstrainedTrait<int, P::A, CapabilityArithmTypes>;
//
// static_assert(TestSpecificConstrainedTrait::value);


template <
    typename,
    auto CONV_PROCESS,
    template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
>
    requires (CONV_PROCESS == P::A)
struct Specific
{
    static constexpr bool value = true;
};


using TestSpecificAllTypes = Specific<int, P::A, CapabilityAllTypes>;
using TestSpecificAllTypesResEnm = Specific<int, P::A, CapabilityAllTypesResEnm>;
using TestSpecificArithmTypes = Specific<int, P::A, CapabilityArithmTypes>;
using TestSpecificArithmTypesResEnm = Specific<int, P::A, CapabilityArithmTypesResEnm>;


static_assert(TestSpecificAllTypes::value);
static_assert(TestSpecificAllTypesResEnm::value);
static_assert(TestSpecificArithmTypes::value);
static_assert(TestSpecificArithmTypesResEnm::value);



// 5. Outer requires constraint, unconstrained CapabilityTrait
template <
    typename,
    auto CONV_PROCESS,
    template <typename, decltype(CONV_PROCESS)> class CapabilityTrait
>
    requires (CONV_PROCESS == P::A)
struct SpecificUnconstrainedTrait
{
    static constexpr bool value = true;
};

using TestSpecificUnconstrainedTrait =
    SpecificUnconstrainedTrait<int, P::A, CapabilityAllTypes>;

static_assert(TestSpecificUnconstrainedTrait::value);


// 6. Constrained template-template parameter, no outer requires
template <
    typename,
    auto CONV_PROCESS,
    template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
>
struct SpecificConstrainedTrait
{
    static constexpr bool value = true;
};


using TestSpecificConstrainedTrait =
    SpecificConstrainedTrait<int, P::A, CapabilityArithmTypes>;


static_assert(TestSpecificConstrainedTrait::value);


int main()
{
    return 0;
}
