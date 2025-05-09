#pragma once
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <vector>
#include "FileStruct.h"
#include "Mem.h"
namespace sup
{
    class PCB
    {
        typedef void (*pfunc_t)();

    public:
        enum Status
        {
            CREATE,
            EXIT,
            READY,
            BLOCKED,
            RUN
        };
        uint64_t _slice_len;
        uint64_t _TTL;
        uint64_t _lifelong;
        bool _has_io;
        uint64_t _io_start;
        uint64_t _io_end;
        uint64_t _io_time;


        Status _status;
        pid_t _PID;
        std::string _name;
        vector<FileStruct> _file_table; // 管理内存文件
        pfunc_t *_signal_handler_table; //信号处理函数表 TODO
        MemoryStruct *_memory_struct;
        
        std::string getStatStr()
        {
            switch (_status)
            {
            case CREATE:return "CREATE";
            case EXIT: return "EXIT";
            case READY: return "READY";
            case BLOCKED: return "BLOCKED";
            case RUN: return "RUN";
            default:
                return "?";
            }
        }
    };
}