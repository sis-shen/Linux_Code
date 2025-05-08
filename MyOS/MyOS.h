#pragma once
#include <iostream>
#include <string>
#include "common.h"
#include "Mem.h"
#include "PCB.h"
#include <list>
using namespace std;

#define OS_MAX_TIME UINT64_MAX

namespace sup{
    class MyOS
    {
        MyOS();
        MyOS(const MyOS&) = delete;
        MyOS operator=(const MyOS&) = delete;
    public:
        static MyOS& getInstance()
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
        void run_one_cycle();
        uint64_t getTime();

    //进程调度
    public:
        int32_t proc_create_pid();
        void proc_creat(const string&proc_name,uint64_t TTL,uint64_t mem_sz,uint64_t io_start,uint64_t io_end);
        void proc_kill(pid_t id);
        void proc_block(pid_t id);
        void proc_wakeup(pid_t id);
        void proc_schedule();
        int32_t _max_pid = 1;
    //内存调度
    public:
        uint64_t npage2addr(uint64_t npage);
        uint64_t get_npage();
        void realse_npage(uint64_t npage,uint64_t size);
        void swapin(PageTable& pt);
        void swapout(PageTable& pt);
    private:
        char* pmem;
        char* pdisk;
        uint64_t _time;
        std::list<PCB> _ready_list;
        std::list<PCB> _blocked_list;
    }; 
}

