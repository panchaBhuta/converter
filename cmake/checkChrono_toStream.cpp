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

    std::string fmtV("{:");
    fmtV += fmt;
    fmtV += "}";
    oss << std::vformat(oss.getloc(), fmtV, std::make_format_args(pYmd));
#else // if  USE_DATELIB_TOSTREAM_2 == 1
    //oss << date::format(fmt, pYmd);
    date::to_stream(oss, fmt, pYmd);
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
