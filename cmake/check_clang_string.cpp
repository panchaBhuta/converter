#include <string>
#include <string_view>

//  ./manualBuild.sh cmake check_clang_string -DSUPPORTED_CLANG_STRING=e_CLANG_STRING_WORKAROUND

/*
 * Failure to compile std::u16string from libstdc++ 12.1 in c++20 mode #55560
 * https://github.com/llvm/llvm-project/issues/55560
 */
#if SUPPORTED_CLANG_STRING == e_CLANG_STRING_WORKAROUND
[[maybe_unused]]static std::u16string clang_string_workaround(const char16_t* a, const char16_t* b) { return {a, b}; }
#endif

std::u16string_view sv{u"test"};

std::u16string print() { return std::u16string(sv); }

int main(int, char**) { std::u16string s = print(); }

