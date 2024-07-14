#pragma once

#include <vector>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cassert>
#include <iostream>

typedef void (*task_t)();

void task1()
{
    std::cout << "PVZ 刷新日志" << std::endl;
}
void task2()
{
    std::cout<< "PVZ 生成阳光"<<std::endl;
}
void task3()
{
    std::cout<<"PVZ 检测更新" <<std::endl;
}
void task4()
{
    std::cout<<"PVZ 使用能量豆"<<std::endl;
}

void LoadTask(std::vector<task_t>*tasks)
{
    tasks->push_back(task1);
    tasks->push_back(task2);
    tasks->push_back(task3);
    tasks->push_back(task4);

}