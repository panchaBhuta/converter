#include <sstream>
#include <string>
#include <cassert>

//  ./manualBuild.sh cmake checkChrono_fromStream -DUSE_CHRONO_FROMSTREAM_1=1

#if    USE_CHRONO_FROMSTREAM_1 == 1
  #include <chrono>
  namespace datelib = std::chrono;
#else  // if  USE_DATE_FROMSTREAM_2 == 1
  #include <date/date.h>
  namespace datelib = date;
#endif

datelib::year_month_day
           ToYMD(const std::string& pStr, std::string::value_type* fmt)
{
    datelib::year_month_day ymd;
    std::istringstream iss(pStr);

    /*
     *  To check if  'std::chrono::from_stream(...)' is supported by <chrono>
     *  As on 25-Mar-2023, this is only supported by chrono formatters in MSVC 19.31*
     *  REFER :::
     *  https://stackoverflow.com/questions/71095317/from-stream-not-a-member-of-stdchrono
     *  https://github.com/HowardHinnant/date
     */
    datelib::from_stream(iss, fmt, ymd);
    return ymd;
}

int main()
{
    std::string date("2016-12-11");
    datelib::year_month_day ymd = ToYMD(date, const_cast<char*> ("%F") );
    assert(ymd == datelib::year(2016)/12/11);
}
