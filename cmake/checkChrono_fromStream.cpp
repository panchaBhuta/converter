#include <sstream>
#include <string>
#include <cassert>

//  ./manualBuild.sh cmake checkChrono_fromStream -DUSE_CHRONO_FROMSTREAM_1=1

#if    USE_CHRONO_FROMSTREAM_1 > 0  // both 1 and 2
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


#if    USE_CHRONO_FROMSTREAM_1 == 2
    // Parse string into chrono::/date:: year_month_day object (C++20)
    iss >> std::chrono::parse(fmt, ymd);
#else
    datelib::from_stream(iss, fmt, ymd);
#endif

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
