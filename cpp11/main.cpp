#include <iostream>
using namespace std;

// int main()
// {
//     constexpr int a = 0x5566;
//     cout << a << endl;
//     cout << (&a) << endl;
//     cout << (&a + 1) << endl;
//     return 0;
// }

//========================
// constexpr int constfunc(int x)
// {
//     return x + 10;
// }

// int main()
// {
//     constexpr int x = constfunc(11);
//     cout << x << endl;
//     int arr[x];     //可以用于声明数组
//     // int arr[constfunc(2)];      //这样子用返回的不是字面量
//     return 0;
// }

class Date{
public:
    constexpr Date(int year, int month, int day)
    :_year(year),_month(month),_day(day)
    {
        // _year++; //C++11必须为空
    }

    constexpr int getYear() const
    {
        return _year;
    }
private:
    int _year;
    int _month;
    int _day;
};

// C++14允许的函数示例
constexpr int factorial(int n)
{
    int res = 1; //允许局部变量
    for (int i = 2; i<= n; ++i) {
        //允许循环
        res*= i;
        res %= (int)(1e9+7);
    }
    return res; //单一return
}


int main()
{
    constexpr Date d0(2025, 8, 21);
    constexpr int y = d0.getYear();
    cout << y <<endl;

    constexpr size_t n = factorial(5);
    cout << n <<endl;
    return 0;
}