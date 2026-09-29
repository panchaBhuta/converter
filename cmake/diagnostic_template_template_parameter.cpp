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

// 5. Arithmetic T constraint; V unconstrained
template <ct_arithmetic T, P V>
    requires true
struct Capability_ArithmTypesAllEnum2
{
    static constexpr bool value = true;
};

// 6. Arithmetic T constraint + enum requires constraint
template <typename T, P V>
    requires (ct_arithmetic<T> && V == P::A)
struct Capability_ArithmTypesConstrEnum2
{
    static constexpr bool value = true;
};




/*
 * Brief expalantion of macros    CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING
 *                                CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING_LIMITED
 *                      and       CONSTRAINED_PARTIAL_SPECIALIZATION
 * We are going to kick their compiler corner cases. 😡
 *
 * The particularly maddening part is that we're not dealing with a simple typo:
 *
 * GCC 13.3 rejects the direct constrained template-template matching in the diagnostic.
 * MSVC 19.51 accepts a different subset of those combinations.
 * Then we changed the experiment to primary-template + constrained-partial-specialization, and suddenly GCC gives us 63/63.
 * So the experiment itself changed the semantic question being asked.
 *
 * That is exactly the kind of C++20 template territory where two compilers can make you question your life choices.
*/


#define CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING           0
#define CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING_LIMITED   1
#define CONSTRAINED_PARTIAL_SPECIALIZATION                      2

#define COMMA ,

#if TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING
  #define EVALUATE_CDTTM(TEMPLATE_INSTANCE)   requires { typename TEMPLATE_INSTANCE; }
  #define       EVALUATE(TEMPLATE_INSTANCE)   requires { typename TEMPLATE_INSTANCE; }
#elif  TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING_LIMITED
  #define EVALUATE_CDTTM(TEMPLATE_INSTANCE)   requires { typename TEMPLATE_INSTANCE; }
  #define       EVALUATE(TEMPLATE_INSTANCE)   0
#elif  TEMPLATE_CONSTRAINT == CONSTRAINED_PARTIAL_SPECIALIZATION
  #define EVALUATE_CDTTM(TEMPLATE_INSTANCE)   0
  #define       EVALUATE(TEMPLATE_INSTANCE)   TEMPLATE_INSTANCE::value
#else
  static_assert(false, "undefined compilation diagnostic for TEMPLATE_TEMPLATE_PARAMETER matching");
#endif




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
        EVALUATE_CDTTM(Consumer_AllTypesAllEnum<int COMMA P::A COMMA Capability_AllTypesAllEnum>);

    static constexpr bool capability_AllTypesConstrEnum =
        EVALUATE_CDTTM(Consumer_AllTypesAllEnum<int COMMA P::A COMMA Capability_AllTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum =
        EVALUATE_CDTTM(Consumer_AllTypesAllEnum<int COMMA P::A COMMA Capability_ArithmTypesAllEnum>);

    static constexpr bool capability_ArithmTypesConstrEnum =
        EVALUATE_CDTTM(Consumer_AllTypesAllEnum<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum2 =
        EVALUATE_CDTTM(Consumer_AllTypesAllEnum<int COMMA P::A COMMA Capability_ArithmTypesAllEnum2>);

    static constexpr bool capability_ArithmTypesConstrEnum2 =
        EVALUATE_CDTTM(Consumer_AllTypesAllEnum<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum2>);

    static constexpr std::size_t capability_bitMaskResult =
        std::size_t(capability_AllTypesAllEnum) +
        std::size_t(capability_AllTypesConstrEnum) * 2 +
        std::size_t(capability_ArithmTypesAllEnum) * 4 +
        std::size_t(capability_ArithmTypesConstrEnum) * 8 +
        std::size_t(capability_ArithmTypesAllEnum2) * 16 +
        std::size_t(capability_ArithmTypesConstrEnum2) * 32;
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
        EVALUATE_CDTTM(Consumer_AllTypesConstrEnum<int COMMA P::A COMMA Capability_AllTypesAllEnum>);

    static constexpr bool capability_AllTypesConstrEnum =
        EVALUATE_CDTTM(Consumer_AllTypesConstrEnum<int COMMA P::A COMMA Capability_AllTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum =
        EVALUATE_CDTTM(Consumer_AllTypesConstrEnum<int COMMA P::A COMMA Capability_ArithmTypesAllEnum>);

    static constexpr bool capability_ArithmTypesConstrEnum =
        EVALUATE_CDTTM(Consumer_AllTypesConstrEnum<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum2 =
        EVALUATE_CDTTM(Consumer_AllTypesConstrEnum<int COMMA P::A COMMA Capability_ArithmTypesAllEnum2>);

    static constexpr bool capability_ArithmTypesConstrEnum2 =
        EVALUATE_CDTTM(Consumer_AllTypesConstrEnum<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum2>);

    static constexpr std::size_t capability_bitMaskResult =
        std::size_t(capability_AllTypesAllEnum) +
        std::size_t(capability_AllTypesConstrEnum) * 2 +
        std::size_t(capability_ArithmTypesAllEnum) * 4 +
        std::size_t(capability_ArithmTypesConstrEnum) * 8 +
        std::size_t(capability_ArithmTypesAllEnum2) * 16 +
        std::size_t(capability_ArithmTypesConstrEnum2) * 32;
};





// Consumer 3: constrained T and constrained CapabilityTrait T
#if TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING ||        \
    TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING_LIMITED
 // compiles on MSVC but  compilation failure on GCC
  template <
      ct_arithmetic T,
      auto CONV_PROCESS,
      template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
  >
  struct Consumer_ArithmTypesAllEnum
  {
      static constexpr bool value = true;
  };
#elif  TEMPLATE_CONSTRAINT == CONSTRAINED_PARTIAL_SPECIALIZATION
  // compilation work-around on GCC
  template <
      typename,
      auto CONV_PROCESS,
      template <typename, decltype(CONV_PROCESS)> class CapabilityTrait
  >
  struct Consumer_ArithmTypesAllEnum
  {
      static constexpr bool value = false;
  };

  template <
      ct_arithmetic T,
      auto CONV_PROCESS,
      template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
  >
  struct Consumer_ArithmTypesAllEnum<T, CONV_PROCESS, CapabilityTrait>
  {
      static constexpr bool value = true;
  };
#endif


template <typename, auto CONV_PROCESS>
struct Test_Consumer_ArithmTypesAllEnum
{
    static constexpr bool capability_AllTypesAllEnum =
        EVALUATE(Consumer_ArithmTypesAllEnum<int COMMA P::A COMMA Capability_AllTypesAllEnum>);

    static constexpr bool capability_AllTypesConstrEnum =
        EVALUATE(Consumer_ArithmTypesAllEnum<int COMMA P::A COMMA Capability_AllTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum =
        EVALUATE(Consumer_ArithmTypesAllEnum<int COMMA P::A COMMA Capability_ArithmTypesAllEnum>);

    static constexpr bool capability_ArithmTypesConstrEnum =
        EVALUATE(Consumer_ArithmTypesAllEnum<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum2 =
        EVALUATE(Consumer_ArithmTypesAllEnum<int COMMA P::A COMMA Capability_ArithmTypesAllEnum2>);

    static constexpr bool capability_ArithmTypesConstrEnum2 =
        EVALUATE(Consumer_ArithmTypesAllEnum<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum2>);

    static constexpr std::size_t capability_bitMaskResult =
        std::size_t(capability_AllTypesAllEnum) +
        std::size_t(capability_AllTypesConstrEnum) * 2 +
        std::size_t(capability_ArithmTypesAllEnum) * 4 +
        std::size_t(capability_ArithmTypesConstrEnum) * 8 +
        std::size_t(capability_ArithmTypesAllEnum2) * 16 +
        std::size_t(capability_ArithmTypesConstrEnum2) * 32;
};


// Consumer 4: constrained T, constrained CapabilityTrait T,
// and requires constraint on CONV_PROCESS
#if TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING ||        \
    TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING_LIMITED
  // compiles on MSVC but  compilation failure on GCC
  template <
      ct_arithmetic T,
      auto CONV_PROCESS,
      template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
  >
      requires (CONV_PROCESS == P::A)
  struct Consumer_ArithmTypesConstrEnum
  {
      static constexpr bool value = true;
  };
#elif  TEMPLATE_CONSTRAINT == CONSTRAINED_PARTIAL_SPECIALIZATION
  // compilation work-around on GCC
  template <
      typename,
      auto CONV_PROCESS,
      template <typename, decltype(CONV_PROCESS)> class CapabilityTrait
  >
  struct Consumer_ArithmTypesConstrEnum
  {
      static constexpr bool value = false;
  };

  template <
      ct_arithmetic T,
      auto CONV_PROCESS,
      template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
  >
      requires (CONV_PROCESS == P::A)
  struct Consumer_ArithmTypesConstrEnum<T, CONV_PROCESS, CapabilityTrait>
  {
      static constexpr bool value = true;
  };
#endif


template <typename, auto CONV_PROCESS>
struct Test_Consumer_ArithmTypesConstrEnum
{
    static constexpr bool capability_AllTypesAllEnum =
        EVALUATE(Consumer_ArithmTypesConstrEnum<int COMMA P::A COMMA Capability_AllTypesAllEnum>);

    static constexpr bool capability_AllTypesConstrEnum =
        EVALUATE(Consumer_ArithmTypesConstrEnum<int COMMA P::A COMMA Capability_AllTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum =
        EVALUATE(Consumer_ArithmTypesConstrEnum<int COMMA P::A COMMA Capability_ArithmTypesAllEnum>);

    static constexpr bool capability_ArithmTypesConstrEnum =
        EVALUATE(Consumer_ArithmTypesConstrEnum<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum2 =
        EVALUATE(Consumer_ArithmTypesConstrEnum<int COMMA P::A COMMA Capability_ArithmTypesAllEnum2>);

    static constexpr bool capability_ArithmTypesConstrEnum2 =
        EVALUATE(Consumer_ArithmTypesConstrEnum<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum2>);

    static constexpr std::size_t capability_bitMaskResult =
        std::size_t(capability_AllTypesAllEnum) +
        std::size_t(capability_AllTypesConstrEnum) * 2 +
        std::size_t(capability_ArithmTypesAllEnum) * 4 +
        std::size_t(capability_ArithmTypesConstrEnum) * 8 +
        std::size_t(capability_ArithmTypesAllEnum2) * 16 +
        std::size_t(capability_ArithmTypesConstrEnum2) * 32;
};


// Consumer 5: arithmetic T1 and constrained capability T2,
// with no constraint on CONV_PROCESS
#if TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING ||        \
    TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING_LIMITED
  // compiles on MSVC but  compilation failure on GCC
  template <
      ct_arithmetic T,
      auto CONV_PROCESS,
      template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
  >
      requires true
  struct Consumer_ArithmTypesAllEnum2
  {
      static constexpr bool value = true;
  };
#elif TEMPLATE_CONSTRAINT == CONSTRAINED_PARTIAL_SPECIALIZATION
  // compilation work-around on GCC
  template <
      typename,
      auto CONV_PROCESS,
      template <typename, decltype(CONV_PROCESS)> class CapabilityTrait
  >
  struct Consumer_ArithmTypesAllEnum2
  {
      static constexpr bool value = false;
  };

  template <
      ct_arithmetic T,
      auto CONV_PROCESS,
      template <ct_arithmetic, decltype(CONV_PROCESS)> class CapabilityTrait
  >
      requires true
  struct Consumer_ArithmTypesAllEnum2<T, CONV_PROCESS, CapabilityTrait>
  {
      static constexpr bool value = true;
  };
#endif


template <typename, auto CONV_PROCESS>
struct Test_Consumer_ArithmTypesAllEnum2
{
    static constexpr bool capability_AllTypesAllEnum =
        EVALUATE(Consumer_ArithmTypesAllEnum2<int COMMA P::A COMMA Capability_AllTypesAllEnum>);

    static constexpr bool capability_AllTypesConstrEnum =
        EVALUATE(Consumer_ArithmTypesAllEnum2<int COMMA P::A COMMA Capability_AllTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum =
        EVALUATE(Consumer_ArithmTypesAllEnum2<int COMMA P::A COMMA Capability_ArithmTypesAllEnum>);

    static constexpr bool capability_ArithmTypesConstrEnum =
        EVALUATE(Consumer_ArithmTypesAllEnum2<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum2 =
        EVALUATE(Consumer_ArithmTypesAllEnum2<int COMMA P::A COMMA Capability_ArithmTypesAllEnum2>);

    static constexpr bool capability_ArithmTypesConstrEnum2 =
        EVALUATE(Consumer_ArithmTypesAllEnum2<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum2>);

    static constexpr std::size_t capability_bitMaskResult =
        std::size_t(capability_AllTypesAllEnum) +
        std::size_t(capability_AllTypesConstrEnum) * 2 +
        std::size_t(capability_ArithmTypesAllEnum) * 4 +
        std::size_t(capability_ArithmTypesConstrEnum) * 8 +
        std::size_t(capability_ArithmTypesAllEnum2) * 16 +
        std::size_t(capability_ArithmTypesConstrEnum2) * 32;
};

// Consumer 6: arithmetic T1 and constrained capability T2,
// with CONV_PROCESS constrained to P::A
#if TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING ||        \
    TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING_LIMITED
  // compiles on MSVC but  compilation failure on GCC
  template <
      typename T1,
      auto CONV_PROCESS,
      template <ct_arithmetic T2, decltype(CONV_PROCESS)> class CapabilityTrait
  >
      requires (ct_arithmetic<T1> && CONV_PROCESS == P::A)
  struct Consumer_ArithmTypesConstrEnum2
  {
      static constexpr bool value = true;
  };
#elif TEMPLATE_CONSTRAINT == CONSTRAINED_PARTIAL_SPECIALIZATION
  // compilation work-around on GCC
  template <
      typename T1,
      auto CONV_PROCESS,
      template <typename T2, decltype(CONV_PROCESS)> class CapabilityTrait
  >
  struct Consumer_ArithmTypesConstrEnum2
  {
      static constexpr bool value = false;
  };

  template <
      typename T1,
      auto CONV_PROCESS,
      template <ct_arithmetic T2, decltype(CONV_PROCESS)> class CapabilityTrait
  >
      requires (ct_arithmetic<T1> && CONV_PROCESS == P::A)
  struct Consumer_ArithmTypesConstrEnum2<T1, CONV_PROCESS, CapabilityTrait>
  {
      static constexpr bool value = true;
  };
#endif


template <typename, auto CONV_PROCESS>
struct Test_Consumer_ArithmTypesConstrEnum2
{
    static constexpr bool capability_AllTypesAllEnum =
        EVALUATE(Consumer_ArithmTypesConstrEnum2<int COMMA P::A COMMA Capability_AllTypesAllEnum>);

    static constexpr bool capability_AllTypesConstrEnum =
        EVALUATE(Consumer_ArithmTypesConstrEnum2<int COMMA P::A COMMA Capability_AllTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum =
        EVALUATE(Consumer_ArithmTypesConstrEnum2<int COMMA P::A COMMA Capability_ArithmTypesAllEnum>);

    static constexpr bool capability_ArithmTypesConstrEnum =
        EVALUATE(Consumer_ArithmTypesConstrEnum2<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum>);

    static constexpr bool capability_ArithmTypesAllEnum2 =
        EVALUATE(Consumer_ArithmTypesConstrEnum2<int COMMA P::A COMMA Capability_ArithmTypesAllEnum2>);

    static constexpr bool capability_ArithmTypesConstrEnum2 =
        EVALUATE(Consumer_ArithmTypesConstrEnum2<int COMMA P::A COMMA Capability_ArithmTypesConstrEnum2>);

    static constexpr std::size_t capability_bitMaskResult =
        std::size_t(capability_AllTypesAllEnum) +
        std::size_t(capability_AllTypesConstrEnum) * 2 +
        std::size_t(capability_ArithmTypesAllEnum) * 4 +
        std::size_t(capability_ArithmTypesConstrEnum) * 8 +
        std::size_t(capability_ArithmTypesAllEnum2) * 16 +
        std::size_t(capability_ArithmTypesConstrEnum2) * 32;
};


#if TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING
    #define TEMPLATE_CONSTRAINT_STRING "CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING"
#elif TEMPLATE_CONSTRAINT == CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING_LIMITED
    #define TEMPLATE_CONSTRAINT_STRING "CONSTRAINED_DIRECT_TEMPLATE_TEMPLATE_MATCHING_LIMITED"
#elif TEMPLATE_CONSTRAINT == CONSTRAINED_PARTIAL_SPECIALIZATION
    #define TEMPLATE_CONSTRAINT_STRING "CONSTRAINED_PARTIAL_SPECIALIZATION"
#else
    #define TEMPLATE_CONSTRAINT_STRING "UNKNOWN"
#endif


int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr << "Usage: " << argv[0] << " <OS> <compiler>" << std::endl;
        return 1;
    }

    std::cout
        << TEMPLATE_CONSTRAINT_STRING << ",capability_AllTypesAllEnum,capability_AllTypesConstrEnum,"
        << "capability_ArithmTypesAllEnum,capability_ArithmTypesConstrEnum,"
        << "capability_ArithmTypesAllEnum2,capability_ArithmTypesConstrEnum2"
        << std::endl;

    std::cout
        << "Consumer_AllTypesAllEnum,"
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_ArithmTypesConstrEnum << ","
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_ArithmTypesAllEnum2 << ","
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_ArithmTypesConstrEnum2
        << std::endl;

    std::cout
        << "Consumer_AllTypesConstrEnum,"
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_ArithmTypesConstrEnum << ","
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_ArithmTypesAllEnum2 << ","
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_ArithmTypesConstrEnum2
        << std::endl;

    std::cout
        << "Consumer_ArithmTypesAllEnum,"
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_ArithmTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_ArithmTypesAllEnum2 << ","
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_ArithmTypesConstrEnum2
        << std::endl;

    std::cout
        << "Consumer_ArithmTypesConstrEnum,"
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_ArithmTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_ArithmTypesAllEnum2 << ","
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_ArithmTypesConstrEnum2
        << std::endl;

    std::cout
        << "Consumer_ArithmTypesAllEnum2,"
        << Test_Consumer_ArithmTypesAllEnum2<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_ArithmTypesAllEnum2<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesAllEnum2<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_ArithmTypesAllEnum2<int, P::A>::capability_ArithmTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesAllEnum2<int, P::A>::capability_ArithmTypesAllEnum2 << ","
        << Test_Consumer_ArithmTypesAllEnum2<int, P::A>::capability_ArithmTypesConstrEnum2
        << std::endl;

    std::cout
        << "Consumer_ArithmTypesConstrEnum2,"
        << Test_Consumer_ArithmTypesConstrEnum2<int, P::A>::capability_AllTypesAllEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum2<int, P::A>::capability_AllTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum2<int, P::A>::capability_ArithmTypesAllEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum2<int, P::A>::capability_ArithmTypesConstrEnum << ","
        << Test_Consumer_ArithmTypesConstrEnum2<int, P::A>::capability_ArithmTypesAllEnum2 << ","
        << Test_Consumer_ArithmTypesConstrEnum2<int, P::A>::capability_ArithmTypesConstrEnum2
        << std::endl;

    const char* os = argv[1];
    const char* compiler = argv[2];

    /*
    std::cerr
        << "_OS,compiler,TEMPLATE_CONSTRAINT,"
        << "Consumer_AllTypesAllEnum,"
        << "Consumer_AllTypesConstrEnum,"
        << "Consumer_ArithmTypesAllEnum,"
        << "Consumer_ArithmTypesConstrEnum,"
        << "Consumer_ArithmTypesAllEnum2,"
        << "Consumer_ArithmTypesConstrEnum2"
        << std::endl;
    */

    std::cerr
        << os << ","
        << compiler << ","
        << TEMPLATE_CONSTRAINT_STRING << ","
        << Test_Consumer_AllTypesAllEnum<int, P::A>::capability_bitMaskResult << ","
        << Test_Consumer_AllTypesConstrEnum<int, P::A>::capability_bitMaskResult << ","
        << Test_Consumer_ArithmTypesAllEnum<int, P::A>::capability_bitMaskResult << ","
        << Test_Consumer_ArithmTypesConstrEnum<int, P::A>::capability_bitMaskResult << ","
        << Test_Consumer_ArithmTypesAllEnum2<int, P::A>::capability_bitMaskResult << ","
        << Test_Consumer_ArithmTypesConstrEnum2<int, P::A>::capability_bitMaskResult
        << std::endl;

    return 0;
}
