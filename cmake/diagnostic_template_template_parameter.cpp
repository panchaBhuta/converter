enum class P
{
    A,
    B
};

template <typename, auto>
struct Generic
{
    static constexpr bool value = true;
};

template <typename T, P V>
    requires (V == P::A)
struct Capability
{
    static constexpr bool value = true;
};

using Test = Generic<int, P::A, Capability>;

static_assert(Test::value);

int main()
{
    return 0;
}
