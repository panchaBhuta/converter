#include <string>
#include <chrono>

//  ./manualBuild.sh cmake check_three_way_comparison


int main(int, char**)
{
  [[maybe_unused]] bool chk = ((4 <=> 5) == 0);
  std::chrono::year_month_day d1;
  std::chrono::year_month_day d2;
  [[maybe_unused]] bool chkDt = ((d1 <=> d2) == 0);
  return 0;
}

