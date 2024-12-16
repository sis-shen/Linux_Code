#include "ColorReplace.hpp"
#include <iostream>

using namespace std;

int main()
{
    string str = "[DEBUG]something wrong...DEBUGing";
    string output = ColorReplace::colorReplace(str,"DEBUG",ColorReplace::Color::Red);
    cout<<output<<endl;
    return 0;
}