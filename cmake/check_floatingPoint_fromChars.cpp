#include <string>
#include <charconv>

//  ./manualBuild.sh cmake check_floatingPoint_fromChars


int main(int, char**)
{
  float result;
  std::string str("1.2345");
  [[maybe_unused]] auto [ptr, ec] = std::from_chars(str.data(), str.data() + str.size(), result);

  return 0;
}

