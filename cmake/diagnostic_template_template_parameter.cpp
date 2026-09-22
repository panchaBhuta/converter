#include <type_traits>
#include <iostream>

enum class P
{
    A,
    B
};


// 1. No constraints
template <typename T, P V>
struct Capability_AllTypesAllEnum
{
    static constexpr bool value = true;
};


// 2. requires constraint on enum parameter
//
template <typename T, P V>
    requires (V == P::A)
struct Capability_AllTypesConstrEnum
{
    static constexpr bool value = true;
};


// 3. Concept constraint on T
template<typename T>
concept ct_arithmetic = std::is_arithmetic_v<T>;

template <ct_arithmetic T, P V>
struct Capability_ArithmTypesAllEnum
{
    static constexpr bool value = true;
};


// 4. Concept constraint on T + requires constraint on enum parameter
template <ct_arithmetic T, P V>
    requires (V == P::A)
struct Capability_ArithmTypesConstrEnum
{
    static constexpr bool value = true;
};


// Consumer 1: no constraints
template <
    typename,
    auto CONV_PROCESS,
    template <typename, decltype(CONV_PROCESS)> class CapabilityTrait
>
struct Consumer_AllTypesAllEnum
{
    static constexpr bool value = true;
};


template <typename, auto CONV_PROCESS>
struct Test_Consumer_AllTypesAllEnum
{
    static constexpr bool capability_AllTypesAllEnum =
        requires{
            typename Consumer_AllTypesAllEnum<int, P::A, Capability_AllTypesAllEnum>;
        };

    static constexpr bool capability_AllTypesConstrEnum =
        requires{
            typename Consumer_AllTypesAllEnum<int, P::A, Capability_AllTypesConstrEnum>;
        };

    static constexpr bool capability_ArithmTypesAllEnum =
        requires{
            typename Consumer_AllTypesAllEnum<int, P::A, Capability_ArithmTypesAllEnum>;
        };

    static constexpr bool capability_ArithmTypesConstrEnum =
        requires{
            typename Consumer_AllTypesAllEnum<int, P::A, Capability_ArithmTypesConstrEnum>;
        };
};


// Consumer 2: requires constraint on CONV_PROCESS
template <
    typename,
    auto CONV_PROCESS,
    template <typename, decltype(CONV_PROCESS)> class CapabilityTrait
>
    requires (CONV_PROCESS == P::A)
struct Consumer_AllTypesConstrEnum
{
    static constexpr bool value = true;
};


template <typename, auto CONV_PROCESS>
struct Test_Consumer_AllTypesConstrEnum
{
    static constexpr bool capability_AllTypesAllEnum =
        requires{
            typename Consumer_AllTypesConstrEnum<int, P::A, Capability_AllTypesAllEnum>;
        };

    static constexpr bool capability_AllTypesConstrEnum =
        requires{
            typename Consumer_AllTypesConstrEnum<int, P::A, Capability_AllTypesConstrEnum>;
        };

    static constexpr bool capability_ArithmTypesAllEnum =
        requires{
            typename Consumer_AllTypesConstrEnum<int, P::A, Capability_ArithmTypesAllEnum>;
        };

    static constexpr bool capability_ArithmTypesConstrEnum =
        requires{
            typename Consumer_AllTypesConstrEnum<int, P::A, Capability_ArithmTypesConstrEnum>;
        };
};


// Consumer 3: constrained T and constrained CapabilityTrait T
template <
    ct_arithmetic,
    auto CONV_PROCESS,
    template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
>
struct Consumer_ArithmTypesAllEnum
{
    static constexpr bool value = true;
};


template <typename, auto CONV_PROCESS>
struct Test_Consumer_ArithmTypesAllEnum
{
    static constexpr bool capability_AllTypesAllEnum =
        requires{
            typename Consumer_ArithmTypesAllEnum<int, P::A, Capability_AllTypesAllEnum>;
        };

    static constexpr bool capability_AllTypesConstrEnum =
        requires{
            typename Consumer_ArithmTypesAllEnum<int, P::A, Capability_AllTypesConstrEnum>;
        };

    static constexpr bool capability_ArithmTypesAllEnum =
        requires{
            typename Consumer_ArithmTypesAllEnum<int, P::A, Capability_ArithmTypesAllEnum>;
        };

    static constexpr bool capability_ArithmTypesConstrEnum =
        requires{
            typename Consumer_ArithmTypesAllEnum<int, P::A, Capability_ArithmTypesConstrEnum>;
        };
};


// Consumer 4: constrained T, constrained CapabilityTrait T,
// and requires constraint on CONV_PROCESS
template <
    ct_arithmetic,
    auto CONV_PROCESS,
    template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
>
    requires (CONV_PROCESS == P::A)
struct Consumer_ArithmTypesConstrEnum
{
    static constexpr bool value = true;
};


template <typename, auto CONV_PROCESS>
struct Test_Consumer_ArithmTypesConstrEnum
{
    static constexpr bool capability_AllTypesAllEnum =
        requires{
            typename Consumer_ArithmTypesConstrEnum<int, P::A, Capability_AllTypesAllEnum>;
        };

    static constexpr bool capability_AllTypesConstrEnum =
        requires{
            typename Consumer_ArithmTypesConstrEnum<int, P::A, Capability_AllTypesConstrEnum>;
        };

    static constexpr bool capability_ArithmTypesAllEnum =
        requires{
            typename Consumer_ArithmTypesConstrEnum<int, P::A, Capability_ArithmTypesAllEnum>;
        };

    static constexpr bool capability_ArithmTypesConstrEnum =
        requires{
            typename Consumer_ArithmTypesConstrEnum<int, P::A, Capability_ArithmTypesConstrEnum>;
        };
};


int main()
{
    std::cout
        << "___________,capability_AllTypesAllEnum,capability_AllTypesConstrEnum,"
        << "capability_ArithmTypesAllEnum,capability_ArithmTypesConstrEnum"
        << std::endl;

    std::cout
        << "Consumer_AllTypesAllEnum,"
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_ArithmTypesConstrEnum
        << std::endl;

    std::cout
        << "Consumer_AllTypesConstrEnum,"
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_ArithmTypesConstrEnum
        << std::endl;

    std::cout
        << "Consumer_ArithmTypesAllEnum,"
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_ArithmTypesConstrEnum
        << std::endl;

    std::cout
        << "Consumer_ArithmTypesConstrEnum,"
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_ArithmTypesConstrEnum
        << std::endl;

    return 0;
}
