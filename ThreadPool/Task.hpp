#pragma once
#include <time.h>
#include <stdlib.h>
#include <iostream>

using namespace std;
struct Task
{
public:
    Task()
    {
        x = rand() %100;
        y = rand() %100;
    }

    void operator()()
    {
        _result = x+y;
    }

    string PrintTask()
    {
        string str ="task: "+to_string(x)+" + "+ to_string(y) + " = ? ";
        return str;
    }

public:
    int _result;
    int x;
    int y;
};