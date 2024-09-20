#include <sstream>
#include <string>
#include <cassert>

//  ./manualBuild.sh cmake checkChrono_toStream -DUSE_CHRONO_TOSTREAM_1=1

#include <chrono>
#if    USE_CHRONO_TOSTREAM_1 == 1
  //#include <chrono>
  #include <format>
  namespace datelib = std::chrono;
#else  // if  USE_DATE_TOSTREAM_2 == 1
  #include <date/date.h>
  namespace datelib = date;
#endif

std::string FromYMD(const datelib::year_month_day& pYmd, std::string::value_type* fmt)
{
    std::ostringstream oss;

#if    USE_CHRONO_TOSTREAM_1 == 1
    //std::chrono::to_stream(oss, fmt, pYmd);
    //oss << std::format(oss.getloc(), STATICALLY_WIDEN(std::string::value_type,"{:%F}"), pYmd);
    //oss << std::format(oss.getloc(), "{:%F}", pYmd);
    std::string fmtV("{:");
    fmtV += fmt;
    fmtV += "}";
    oss << std::vformat(oss.getloc(), fmtV, std::make_format_args(pYmd));
#else // if  USE_DATELIB_TOSTREAM_2 == 1
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
