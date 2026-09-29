#include <iostream>
#include <utility>
#include <vector>

#include <specializedTypes/CompTimeStr.h>

int main()
{
    std::cout << "get_name<short> = ["
              << specializedTypes::get_name<short>() << "]\n";

    std::cout << "get_name<int> = ["
              << specializedTypes::get_name<int>() << "]\n";

    std::cout << "get_name<double> = ["
              << specializedTypes::get_name<double>() << "]\n";

    std::cout << "get_name<std::vector<int>> = ["
              << specializedTypes::get_name<std::vector<int>>() << "]\n";

    std::cout << "get_name<std::vector<std::pair<int, double>>> = ["
              << specializedTypes::get_name<std::vector<std::pair<int, double>>>() << "]\n";

    return 0;
}
