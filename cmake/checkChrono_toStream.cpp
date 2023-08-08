#include <sstream>
#include <string>
#include <cassert>

//  ./manualBuild.sh cmake checkChrono_toStream -DUSE_CHRONO_TOSTREAM=1

#if  USE_CHRONO_TOSTREAM == 1
  #include <chrono>
  namespace datelib = std::chrono;
#else
  #include <date/date.h>
  namespace datelib = date;
#endif

std::string FromYMD(const datelib::year_month_day& pYmd, std::string::value_type* fmt)
{
    std::ostringstream oss;

#if  USE_CHRONO_TOSTREAM == 1
    datelib::to_stream(oss, fmt, pYmd);
#else
    using CS = std::chrono::seconds;
    std::string* abbrev  = nullptr;
    std::chrono::seconds* offset_sec = nullptr;
    date::fields<CS> fds{pYmd};
    date::to_stream(oss, fmt, fds, abbrev, offset_sec);
#endif
    return oss.str();
}

int main()
{
    std::string dateExp("2016-12-11");
    datelib::year_month_day ymd = datelib::year(2016)/12/11;
    std::string dateCon = FromYMD(ymd, const_cast<char*> ("%F"));
    assert(dateExp == dateCon);
}
