#include <sstream>
#include <string>
#include <cassert>

#define  e_DATE_NO_LIB  0     //  no date conversion support from any available lib's
#define  e_DATE_CHRONO  1     //  std::chrono
#define  e_DATE_HHDATE  2     //  https://github.com/HowardHinnant/date/


//  ./manualBuild.sh cmake checkChrono_toStream -DSUPPORTED_DATE_LIB_FOR_TOSTREAM=e_DATE_CHRONO

#include <chrono>
#if    SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_CHRONO
  //#include <chrono>
  #include <format>
  namespace datelib = std::chrono;
#elif  SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_HHDATE
  #include <date/date.h>
  namespace datelib = date;
#endif

std::string FromYMD(const datelib::year_month_day& pYmd, std::string::value_type* fmt)
{
    std::ostringstream oss;

#if    SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_CHRONO
    //std::chrono::to_stream(oss, fmt, pYmd);

    std::string fmtV("{:");
    fmtV += fmt;
    fmtV += "}";
    oss << std::vformat(oss.getloc(), fmtV, std::make_format_args(pYmd));
        // return std::format(fmtV, today); this only works when fmtV is string literal
        // like "{:%d %B %Y}"  gets converted to "std::format_string<Args...> fmt"
        // Compile-Time Validation: it is a template wrapper,
        // the C++ compiler parses your format string ("{:%d %B %Y}") during compilation.
#elif  SUPPORTED_DATE_LIB_FOR_TOSTREAM == e_DATE_HHDATE
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
