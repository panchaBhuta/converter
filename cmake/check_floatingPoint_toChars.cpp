#include <string>
#include <charconv>
#include <array>
#include <limits>
#include <cassert>

//  ./manualBuild.sh cmake check_floatingPoint_toChars


int main(int, char**)
{
  std::array<char, std::numeric_limits<FLOAT_TYPE>::digits +5 > str;  // +5 just to be on the safe side :)
  FLOAT_TYPE value = 1.2345f;
  [[maybe_unused]] auto [pos, ec] = std::to_chars(str.data(), str.data() + str.size(), value);

  return 0;
}

