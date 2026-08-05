#include <string>
#include <charconv>
#include <array>
#include <limits>
#include <cassert>

//  ./manualBuild.sh cmake check_floatingPoint_toString


int main(int, char**)
{
  std::array<char, std::numeric_limits<float>::digits +5 > str;  // +5 just to be on the safe side :)
  float value = 11.0f;
  std::string strVal = std::to_string(value); // std::to_string(11.0f) -> "11.0000000" is not what is expected "11"
  assert(strVal == "11");

  value = 1.2345f;
  strVal = std::to_string(value);
  assert(strVal == "1.2345");

  return 0;
}

