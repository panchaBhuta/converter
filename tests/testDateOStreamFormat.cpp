



/*
So I'd be inclined to try exactly that in the real converter.h, then test it in a few situations:

1. std::cout << ymd;
2. std::cout << date_format<...>() << ymd;
3. Multiple dates after one format.
4. Changing the format midway.
5. A different std::ostream.
6. A translation unit that includes converter.h but doesn't use the date functionality.
7. Interaction with other operator<< overloads.

*/

int main()
{
    using namespace std::chrono;

    const year_month_day date1{
        2026y, September, 1d
    };

    const year_month_day date2{
        2026y, September, 2d
    };

    const year_month_day date3{
        2026y, September, 3d
    };


    std::cout
        << converter::date_format<"{:%Y-%m-%d}">()
        << date1 << '\n'
        << date2 << '\n'
        << date3 << '\n';


    std::cout
        << converter::date_format<"{:%d/%m/%Y}">()
        << date1 << '\n'
        << date2 << '\n'
        << date3 << '\n';


    /*
     * The format remains associated with std::cout.
     */
    std::cout
        << date1 << '\n'
        << date2 << '\n';
}

