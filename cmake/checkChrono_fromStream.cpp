#include <sstream>
#include <string>
#include <cassert>

#define  e_DATE_NO_LIB  0     //  no date conversion support from any available lib's
#define  e_DATE_CHRONO  1     //  std::chrono
#define  e_DATE_HHDATE  2     //  https://github.com/HowardHinnant/date/

//  ./manualBuild.sh cmake checkChrono_fromStream -DSUPPORTED_DATE_LIB_FOR_FROMSTREAM=e_DATE_CHRONO




#if    SUPPORTED_DATE_LIB_FOR_FROMSTREAM == e_DATE_CHRONO
  #include <chrono>
  namespace datelib = std::chrono;
#elif    SUPPORTED_DATE_LIB_FOR_FROMSTREAM == e_DATE_HHDATE
  #include <date/date.h>
  namespace datelib = date;
#endif

datelib::year_month_day
           ToYMD(const std::string& pStr, std::string::value_type* fmt)
{
    datelib::year_month_day ymd;
    std::istringstream iss(pStr);


    datelib::from_stream(iss, fmt, ymd);

    assert(!iss.fail());
    assert(!iss.bad());
    //assert(ymd == 2016_y/12/11);

    return ymd;
}

int main()
{
    std::string date("2016-12-11");
    datelib::year_month_day ymd = ToYMD(date, const_cast<char*> ("%F") );  // "Y-%m-%d"
    assert(ymd == datelib::year(2016)/12/11);

    return 0;
}
