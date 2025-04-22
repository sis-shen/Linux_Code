#pragma once
#include <iostream>
#include <string>
#include "common.h"
#include "Mem.h"
#include "PCB.h"
using namespace std;

namespace sup{
    class MyOS
    {
        MyOS();
        MyOS(const MyOS&) = delete;
        MyOS operator=(const MyOS&) = delete;
    public:
        MyOS& getInstance()
        {
            static MyOS os;
            return os;
        }

        void createproc(const string&proc_name,uint64_t TTL,uint64_t mem_sz,uint64_t io_start,uint64_t io_end);

        void killproc(pid_t pid);
        void psproc();
        void mem();
        void createfile(const string&filename);
        void deletefile(const string&filenmae);
    //内存调度
    public:
        void swapin(PageTable& pt);
        void swapout(PageTable& pt);
    private:
        char* pmem;
        char* pdisk;
    }; 
}

