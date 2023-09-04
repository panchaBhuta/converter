#include <string>
#include <string_view>

//  ./manualBuild.sh cmake check_clang_string -DUSE_CLANG_STRING_WORKAROUND=0

#if USE_CLANG_STRING_WORKAROUND == 1
[[maybe_unused]]static std::u16string clang_string_workaround(const char16_t* a, const char16_t* b) { return {a, b}; }
#endif

std::u16string_view sv{u"test"};

std::u16string print() { return std::u16string(sv); }

int main(int, char**) { std::u16string s = print(); }

