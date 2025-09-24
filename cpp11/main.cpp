#include <iostream>
using namespace std;

int main()
{
    constexpr int a = 0x5566;
    cout << a << endl;
    cout << (&a) << endl;
    cout << (&a + 1) << endl;
    return 0;
}