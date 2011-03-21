#include <iostream>
#include <ios>
#include <iomanip>
#include <string>

using std::cin;
using std::cout;
using std::streamsize;
using std::setprecision;
using std::endl;

int main()
{
    streamsize prec = cout.precision();
    float pi = 3.1415926535897;
    cout << setprecision(6) << pi << endl;
    
}
