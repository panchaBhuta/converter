#include <cassert>
#include <iostream>
#include <string>

int main()
{
    float value = 11.0f;
    std::string strVal = std::to_string(value);
    std::cout << "value=11.0f result=[" << strVal << "]\n";

    assert(strVal == "11");

    value = 1.2345f;
    strVal = std::to_string(value);
    std::cout << "value=1.2345f result=[" << strVal << "]\n";

    assert(strVal == "1.2345");

    return 0;
}
