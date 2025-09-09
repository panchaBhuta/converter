#include <algorithm>

//  ./manualBuild.sh cmake check_constexpr_copyN

  template<std::size_t N>
  struct CompTimeStr
  {
    char data[N] {};

    consteval CompTimeStr(const char (&str)[N])
    {
      std::copy_n(str, N, data);
    }

    // Add comparison operators if needed
    consteval bool operator==(const CompTimeStr<N>& other) const
    {
      return std::equal(data, data + N, other.data);
    }

  };


int main(int, char**)
{
  constexpr CompTimeStr test1("Test_Compile_time_Constructor");
  constexpr CompTimeStr test2("Test_Compile_time_Constructor");
  constexpr bool flag = (test1 == test2);
  return 0;
}

