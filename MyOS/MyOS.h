#pragma once
#include <iostream>
#include <string>
#include "common.h"
#include "Mem.h"
#include "PCB.h"
#include <list>
#include <map>
using namespace std;

#define OS_MAX_TIME UINT64_MAX

namespace sup{
    class MyOS
    {
        MyOS()
        {
            set_mem();
            set_disk();
            _fs.set_pdisk(pdisk);
            _page_table.set_mem(pmem);
        };
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
        void set_mem();
        uint64_t npage2addr(uint64_t npage);
        void realse_npage(uint64_t npage,uint64_t size);
        void swapin();
        void swapout();
    //文件系统
        void set_disk();

        void creatfile(const std::string&filename,uint64_t size);
        void deletefile(const std::string&filename);
        void lsfile(const std::string&filename);
        

    private:
        char* pmem;
        char* pdisk;
        PageTable _page_table;
        uint64_t _time;
        FileSystem _fs;
        std::list<PCB> _ready_list;
        std::list<PCB> _blocked_list;
    }; 
}

