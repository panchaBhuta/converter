#include <iostream>
#include <vector>
#include <converter/converter.h>

int main()
{
  std::string strInput("1.23");
  const double valConv = converter::ConvertFromStr<double>::ToVal(strInput);
  const std::string strRoundtripActual = converter::ConvertFromVal<double>::ToStr(valConv);

  std::cout << "strInput='" << strInput << "' -> valConv=" << valConv
            << " (double)  ->  strRoundtripActual='" << strRoundtripActual << "'" << std::endl;
}
